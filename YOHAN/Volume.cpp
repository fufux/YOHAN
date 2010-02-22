
#include "Volume.h"
#include "Scene.h"

Volume::Volume(int id, Scene* scene)
{
	this->id = id;
	this->scene = scene;
	facesChanged = true;
	tetrahedraChanged = true;
}

Volume::~Volume(void)
{
}


int Volume::getID()
{
	return id;
}
Material* Volume::getMaterial()
{
	return &material;
}
matrix::SymmetricMumpsSquareSparseMatrix* Volume::getK()
{
	return K;
}
matrix::SymmetricMumpsSquareSparseMatrix* Volume::getM()
{
	return M;
}
matrix::SymmetricMumpsSquareSparseMatrix* Volume::getC()
{
	return C;
}
double* Volume::getForceField()
{
	return forceField;
}
double* Volume::getForces()
{
	return forces;
}


bool Volume::load(std::string nodeFile, std::string eleFile, std::string faceFile, Material material, double* v)
{
	this->material = material;
	int n;		//number of nodes/tetrahedra/faces
	int tmp;	//for those values not important

	/* file .node */

	ifstream fp (nodeFile.c_str(), ios::in);
	if (!fp || !fp.good())
	{
		cout << "Can not open node file: " << nodeFile << endl;
		return false;
	}

	// get number
	fp >> n >> tmp >> tmp >> tmp;

	// the point list
	for (int i = 0; i < n && fp.good(); i++)
	{
		double x[3];

		// current position
		fp >> tmp >> x[0] >> x[1] >> x[2];

		Point* p = new Point(i, (double*)x, v, (double*)x);
		points.push_back(p);
	}

	// close file
	fp.close();
	fp.clear();


	/* file .face */

	fp.open(faceFile.c_str(), ios::in);
	if (!fp || !fp.good())
	{
		cout << "Can not open face file: " << faceFile << endl;
		return false;
	}

	// get number
	fp >> n >> tmp;

	// the point visible list
	for (int i = 0; i < n && fp.good(); i++)
	{
		int* p = new int[3];
		fp >> tmp >> p[0] >> p[1] >> p[2] >> tmp;

		// ajust indices
		p[0]--;p[1]--;p[2]--;

		// update points
		points[p[0]]->setIsSurface(true);
		points[p[1]]->setIsSurface(true);
		points[p[2]]->setIsSurface(true);

		// fill facets
		facets.push_back(p);
	}

	// close file
	fp.close();
	fp.clear();


	/* file .ele */

	fp.open(eleFile.c_str(), ios::in);
	if (!fp || !fp.good())
	{
		cout << "Can not open ele file: " << eleFile << endl;
		return false;
	}

	// get number
	fp >> n >> tmp >> tmp;

	// the tet list
	for (int i = 0; i < n && fp.good(); i++)
	{
		int id, p[4];
		fp >> id >> p[0] >> p[1] >> p[2] >> p[3];

		// coherent index
		id--;
		p[0]--;p[1]--;p[2]--;p[3]--;

		// Tetrahedron instance
		vector<Point*> vp;
		for (int j=0; j<4; j++)
			vp.push_back( points[p[j]] );
		Tetrahedron* t = new Tetrahedron(id, this, vp);
		tetrahedra.push_back( t );
		for (int j=0; j<4; j++)
			points[p[j]]->getTetrahedra()->push_back( t );
	}

	// close file
	fp.close();
	fp.clear();

	// the matrix
	int order = points.size() * 3;
	this->K = new matrix::SymmetricMumpsSquareSparseMatrix(order);
	this->M = new matrix::SymmetricMumpsSquareSparseMatrix(order);
	this->C = new matrix::SymmetricMumpsSquareSparseMatrix(order);

	// the vectors
	this->xu = new double[order];
	this->v = new double[order];
	this->f = new double[order];
	this->forceField = new double[order];
	this->forces = new double[order];
	for (int i=0; i < order; i++)
		forces[i] = 0;

	return true;
}


vector<std::string> Volume::save(std::string dir)
{
	// return
	vector<std::string> files;
	int size, pid;

	// .bnode
	std::stringstream sstream;
	sstream << dir.c_str() << "/" << id << ".bnode";

	// output
	ofstream fp (sstream.str().c_str(), ios::out | ios::binary);

	size = points.size();
	fp.write ((char*)&size, sizeof(int));

	for (std::vector<Point*>::iterator iter = points.begin(); iter != points.end(); ++iter)
	{
		Point* point = *iter;
		fp.write ((char*)point->getX(), sizeof(double) * 3);
	}
	fp.flush();
	fp.close();
	fp.clear();

	files.push_back(sstream.str());

	if (tetrahedraChanged)
	{
		// .bele
		std::stringstream sstream2;
		sstream2 << dir.c_str() << "/" << id << ".bele";

		// output
		fp.open(sstream2.str().c_str(), ios::out | ios::binary);

		size = tetrahedra.size();
		fp.write ((char*)&size, sizeof(int));

		for (std::vector<Tetrahedron*>::iterator iter = tetrahedra.begin(); iter != tetrahedra.end(); ++iter)
		{
			Tetrahedron* tet = *iter;
			for (int j=0; j<4; j++)
			{
				pid = tet->getPoints()[j]->getID();
				fp.write ((char*)&pid, sizeof(int));
			}
		}
		fp.flush();
		fp.close();
		fp.clear();

		tetrahedraChanged = false;
		lastOutputTetrahedraFileName = sstream2.str();
	}
	files.push_back(lastOutputTetrahedraFileName);


	if (facesChanged)
	{
		// .bface
		std::stringstream sstream3;
		sstream3 << dir.c_str() << "/" << id << ".bface";

		// output
		fp.open(sstream3.str().c_str(), ios::out | ios::binary);

		size = facets.size();
		fp.write ((char*)&size, sizeof(int));

		for (std::vector<int*>::iterator iter = facets.begin(); iter != facets.end(); ++iter)
		{
			int* face = *iter;
			int f1 = face[0];
			int f2 = face[1];
			int f3 = face[2];
			fp.write ((char*)&f1, sizeof(int));
			fp.write ((char*)&f2, sizeof(int));
			fp.write ((char*)&f3, sizeof(int));
		}
		fp.flush();
		fp.close();
		fp.clear();

		facesChanged = false;
		lastOutputFacesFileName = sstream3.str();
	}
	files.push_back(lastOutputFacesFileName);

	return files;
}


void Volume::generateK()
{
	K->clear();
	double** q = new double*[3];
	double** dx = new double*[3];
	double** jac = new double*[3];
	double** tmp = new double*[3];
	for(int i=0;i<3;i++){
		q[i] = new double[3];
		dx[i] = new double[3];
		jac[i] = new double[3];
		tmp[i] = new double[3];
	}
	for(int t=0;t<(int)tetrahedra.size();t++){
		vector<Point*>& pts = tetrahedra[t]->getPoints();

		// compute Q
		double* x1 = pts[0]->getX();
		double* x2 = pts[1]->getX();
		double* x3 = pts[2]->getX();
		double* x4 = pts[3]->getX();
		dx[0][0] = x2[0]-x1[0];
		dx[1][0] = x2[1]-x1[1];
		dx[2][0] = x2[2]-x1[2];
		dx[0][1] = x3[0]-x1[0];
		dx[1][1] = x3[1]-x1[1];
		dx[2][1] = x3[2]-x1[2];
		dx[0][2] = x4[0]-x1[0];
		dx[1][2] = x4[1]-x1[1];
		dx[2][2] = x4[2]-x1[2];

		util::matrixProd(tmp, dx, tetrahedra[t]->getBeta());

		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				//q[i][j] = 0;
				q[i][j] = tmp[i][j]/(10);
			}
			//q[i][i]=1;
		}

		for (int i = 0; i < 4; i++)
		{
			int ri = pts[i]->getID() * 3 + 1;	// +1 is to conform the index in the matrix
			for (int j = 0; j < 4; j++)
			{
				int ci = pts[j]->getID() * 3 + 1;	// +1 is to conform the index in the matrix
				
				// Compute Jij
				util::matrixProd(tmp, q, tetrahedra[t]->getCoreJacobian()[i*4+j]);
				util::matrixProdTrans(jac, tmp, q);

				// 9 items for stiffness matrix
				K->addAndSetValue(ri, ci, jac[0][0]);
				K->addAndSetValue(ri, ci + 1, jac[0][1]);
				K->addAndSetValue(ri, ci + 2, jac[0][2]);
				K->addAndSetValue(ri + 1, ci, jac[1][0]);
				K->addAndSetValue(ri + 1, ci + 1, jac[1][1]);
				K->addAndSetValue(ri + 1, ci + 2, jac[1][2]);
				K->addAndSetValue(ri + 2, ci, jac[2][0]);
				K->addAndSetValue(ri + 2, ci + 1, jac[2][1]);
				K->addAndSetValue(ri + 2, ci + 2, jac[2][2]);

			}
		}
	}

	// Desallocation
	for(int ii=0;ii<3;ii++){
		delete []q[ii];
		delete []dx[ii];
		delete []tmp[ii];
		delete []jac[ii];
	}
	delete [] q;
	delete [] dx;
	delete [] tmp;
	delete [] jac;
}

void Volume::generateC()
{
	C->clear();
	C->calcul_AXplusBY(this->material.phi, M, this->material.psy, K);
}

void Volume::generateMAndForceField()
{
	M->clear();
	double m;
	double* _f = scene->getForceField();
	for(int i=0;i<(int)points.size();i++){
		m = points[i]->getMass();
		M->addAndSetValue(3*i+1,3*i+1,m);
		M->addAndSetValue(3*i+2,3*i+2,m);
		M->addAndSetValue(3*i+3,3*i+3,m);
		forceField[3*i] = _f[0]*m;
		forceField[3*i+1] = _f[1]*m;
		forceField[3*i+2] = _f[2]*m;
	}
}

void Volume::generateXU()
{
	double* x;
	double* u;
	for(int i=0;i<(int)points.size();i++){
		x = points[i]->getX();
		u = points[i]->getU();
		xu[3*i] = x[0] - u[0];
		xu[3*i+1] = x[1] - u[1];
		xu[3*i+2] = x[2] - u[2];
	}
}

void Volume::generateV()
{
	double* _v;
	for(int i=0;i<(int)points.size();i++){
		_v = points[i]->getV();
		v[3*i] = _v[0];
		v[3*i+1] = _v[1];
		v[3*i+2] = _v[2];
	}
}

void Volume::generateF()
{
	for(int i=0;i<(int)points.size()*3;i++){
		f[i] = forces[i] + forceField[i];
	}
}



void Volume::evolve(double deltaT)
{
	int order = points.size() * 3;

	generateK();
	generateC();
	generateXU();
	generateV();
	generateF();

	// delta_t * C = C_
	C->calcul_AX(deltaT);

	// C_ + M = C__
	C->calcul_plusAX(M, 1);

	// C__ + delta_t * delta_t * K = C___
	C->calcul_plusAX(K, deltaT * deltaT);

	// F_ = deltaTime * F
	calcul_vec_AX(f, order, deltaT);

	// F__ = F_ + M * V
	M->calcul_PlusMatrixVec(v, f);

	// F___ = F__ - deltaTime * K * XU
	K->calcul_MinusMatrixVec(xu, f, deltaT);

	// resolve C___ ? = F___,
	solveLinearEquation(C, f);

	updateVolume(deltaT);
}


void Volume::updateVolume(double deltaT)
{
	int order = points.size() * 3;

	// reset forces
	for (int i=0; i < order; i++)
		forces[i] = 0;

	// update points
	for (int i=0; i < (int)points.size(); i++) {
		// update velocity
		points[i]->getV()[0] = f[3*i];
		points[i]->getV()[1] = f[3*i+1];
		points[i]->getV()[2] = f[3*i+2];
		// update position
		points[i]->getX()[0] += f[3*i] * deltaT;
		points[i]->getX()[1] += f[3*i+1] * deltaT;
		points[i]->getX()[2] += f[3*i+2] * deltaT;
	}
}



void Volume::collisionBidon()
{
	for (int i=0; i<points.size(); i++)
	{
		if (points[i]->getX()[1] < 0) {
			forces[3*i+1] = -points[i]->getMass()*points[i]->getX()[1]*100;
		}
	}
}