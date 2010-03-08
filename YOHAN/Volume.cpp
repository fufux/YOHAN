
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

vector<Tetrahedron*>* Volume::getTetrahedra()
{
	return &tetrahedra;
}


BoundingBox* Volume::getMasterBoundingBox()
{
	return bb;
}


bool Volume::load(std::string nodeFile, std::string eleFile, std::string faceFile, Material material, double* pos, double* rot, double* v)
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
		/* Modified by Ning */
		/*
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
		*/

		struct Surface* surf = new struct Surface();

		fp >> tmp >> surf->pointIndex[0] >> surf->pointIndex[1] >> surf->pointIndex[2] >> tmp;
		
		// ajust indices
		surf->pointIndex[0]--;
		surf->pointIndex[1]--;
		surf->pointIndex[2]--;

		// update points
		for (int j = 0; j < 3; j++)
		{
			points[surf->pointIndex[j]]->setIsSurface(true);

			struct IndexSurfacePoint indexSurf;
			indexSurf.indexOfPoint = j;
			indexSurf.surface = surf;

			points[surf->pointIndex[j]]->getIndexSurface()->push_back(indexSurf);
		}

		// fill facets
		surfaces.push_back(surf);

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
		{
			/* Modified by Ning, for fracture */

			// points[p[j]]->getTetrahedra()->push_back( t );

			struct IndexTetraPoint newIndexTetra;
			newIndexTetra.tet = t;
			newIndexTetra.indexOfPoint = j;
			points[p[j]]->getIndexTetra()->push_back(newIndexTetra);
			/* END -- Modified by Ning, for fracture */
		}
	}

	// close file
	fp.close();
	fp.clear();

	/* Added by Ning, for fracture*/

	/* pre-calculate the face neighbour and the edge neighbour for each tetrahedron */


	/* END -- Added by Ning, for fracture*/

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

	// BoundingBox
	bb = new BoundingBox(NULL, tetrahedra, false);

	for (int i=0; i < (int)points.size(); i++)
	{
		double x[3];
		
		// apply rotation
		x[0] = points[i]->getX()[0] * ( cos(rot[2])*cos(rot[1]) )
			 + points[i]->getX()[1] * ( cos(rot[2])*sin(rot[1])*sin(rot[0])-sin(rot[2])*cos(rot[0]) )
			 + points[i]->getX()[2] * ( cos(rot[2])*sin(rot[1])*cos(rot[0])+sin(rot[2])*sin(rot[0]) );
		x[1] = points[i]->getX()[0] * ( sin(rot[2])*cos(rot[1]) )
			 + points[i]->getX()[1] * ( sin(rot[2])*sin(rot[1])*sin(rot[0])+cos(rot[2])*cos(rot[0]) )
			 + points[i]->getX()[2] * ( sin(rot[2])*sin(rot[1])*cos(rot[0])-cos(rot[2])*sin(rot[0]) );
		x[2] = points[i]->getX()[0] * ( -sin(rot[1]) )
			 + points[i]->getX()[1] * ( cos(rot[1])*sin(rot[0]) )
			 + points[i]->getX()[2] * ( cos(rot[1])*cos(rot[0]) );
		
		points[i]->getX()[0] = x[0];
		points[i]->getX()[1] = x[1];
		points[i]->getX()[2] = x[2];

		// apply position
		points[i]->getX()[0] += pos[0];
		points[i]->getX()[1] += pos[1];
		points[i]->getX()[2] += pos[2];
	}

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

		/*
		size = surfaces.size();
		fp.write ((char*)&size, sizeof(int));

		for (std::vector<struct Surface*>::iterator iter = surfaces.begin(); iter != surfaces.end(); ++iter)
		{
			int* face = (*iter)->pointIndex;
			int f1 = face[0];
			int f2 = face[1];
			int f3 = face[2];
			fp.write ((char*)&f1, sizeof(int));
			fp.write ((char*)&f2, sizeof(int));
			fp.write ((char*)&f3, sizeof(int));
		}
		*/

		/* Modified by Ning, for fracture, debug */
		size = tetrahedra.size() * 4;
		fp.write ((char*)&size, sizeof(int));

		int format[4][3] = {0,1,2,0,2,3,0,3,1,1,3,2};	// 0-1-2, 0-2-3, 0-3-1, 1-3-2 : inverse clock-wise

		for (std::vector<Tetrahedron*>::iterator iter = tetrahedra.begin(); iter != tetrahedra.end(); ++iter)
		{
			Tetrahedron* tet = *iter;
			std::vector<Point*> pl = tet->getPoints();
			
			// 0-1-2, 0-2-3, 0-3-1, 1-3-2
			
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					int pid = pl[format[i][j]]->getID();
					fp.write ((char*)&pid, sizeof(int));
				}
			}
		}


		fp.flush();
		fp.close();
		fp.clear();

		facesChanged = false;
		lastOutputFacesFileName = sstream3.str();
	}
	files.push_back(lastOutputFacesFileName);


	// .bbb
	std::stringstream sstream4;
	sstream4 << dir.c_str() << "/" << id << ".bbb";

	// output
	bb->saveAllToFile(sstream4.str());
	files.push_back(sstream4.str());

	return files;
}


void Volume::generateK()
{
	K->clear();
	Matrix3d dx,jac;
	Matrix3d& f = Matrix3d();
	Matrix3d& q = Matrix3d();
	Matrix3d& qt = Matrix3d();
	Matrix3d& core = Matrix3d();
	int ri,ci,ind;
	vector<Point*> pts;
	double *x1,*x2,*x3,*x4;
	double q00,q01,q02,q10,q11,q12,q20,q21,q22;
	double core00,core01,core02,core10,core11,core12,core20,core21,core22;
	double jac00,jac01,jac02,jac10,jac11,jac12,jac20,jac21,jac22;
	double q_core00,q_core01,q_core02,q_core10,q_core11,q_core12,q_core20,q_core21,q_core22;

	// indices
	int t,i,j;

	for(t=0;t<(int)tetrahedra.size();t++){
		pts = tetrahedra[t]->getPoints();

		// compute Q
		x1 = pts[0]->getX();
		x2 = pts[1]->getX();
		x3 = pts[2]->getX();
		x4 = pts[3]->getX();
		dx(0,0) = x2[0]-x1[0];
		dx(1,0) = x2[1]-x1[1];
		dx(2,0) = x2[2]-x1[2];
		dx(0,1) = x3[0]-x1[0];
		dx(1,1) = x3[1]-x1[1];
		dx(2,1) = x3[2]-x1[2];
		dx(0,2) = x4[0]-x1[0];
		dx(1,2) = x4[1]-x1[1];
		dx(2,2) = x4[2]-x1[2];

		f = dx * tetrahedra[t]->getBeta();
		util::polarDecomposition(f, q);
		q00 = q(0,0); q01 = q(0,1); q02 = q(0,2);
		q10 = q(1,0); q11 = q(1,1); q12 = q(1,2);
		q20 = q(2,0); q21 = q(2,1); q22 = q(2,2);

		/* Added by Ning, for fracture */

		// F~ = Q' * F
		Matrix3d fp = q.transpose() * f;
		// e~ = 0.5 * (F~ + F~') - I
		Matrix3d epsilon = 0.5 * (fp + fp.transpose()) - Matrix3d::Identity();
		// sigma~ = lambda * trace(e~) * I + 2 * mu * e~
		Matrix3d sigma = material.lambda * epsilon.trace() * Matrix3d::Identity() + 2 * material.mu * epsilon;

		// copy sigma as stress
		tetrahedra[t]->setStress(sigma);

		// copy Q
		tetrahedra[t]->setQ(q);

		/* END -- Added by Ning, for fracture */


                for (i = 0; i < 4; i++)
		{
			ri = pts[i]->getID() * 3 + 1;	// +1 is to conform the index in the matrix
			ind = pts[i]->getID() * 3;
			for (j = 0; j < 4; j++)
			{
				ci = pts[j]->getID() * 3 + 1;	// +1 is to conform the index in the matrix
				
				// Compute Jij
				core = tetrahedra[t]->getCoreJacobian()[i*4+j];
				core00 = core(0,0); core01 = core(0,1); core02 = core(0,2);
				core10 = core(1,0); core11 = core(1,1); core12 = core(1,2);
				core20 = core(2,0); core21 = core(2,1); core22 = core(2,2);

				//jac = q * core * q.transpose();
				q_core00 = q00*core00+q01*core10+q02*core20;
				q_core01 = q00*core01+q01*core11+q02*core21;
				q_core02 = q00*core02+q01*core12+q02*core22;

				q_core10 = q10*core00+q11*core10+q12*core20;
				q_core11 = q10*core01+q11*core11+q12*core21;
				q_core12 = q10*core02+q11*core12+q12*core22;

				q_core20 = q20*core00+q21*core10+q22*core20;
				q_core21 = q20*core01+q21*core11+q22*core21;
				q_core22 = q20*core02+q21*core12+q22*core22;

				jac00 = q_core00*q00+q_core01*q01+q_core02*q02;
				jac01 = q_core00*q10+q_core01*q11+q_core02*q12;
				jac02 = q_core00*q20+q_core01*q21+q_core02*q22;

				jac10 = q_core10*q00+q_core11*q01+q_core12*q02;
				jac11 = q_core10*q10+q_core11*q11+q_core12*q12;
				jac12 = q_core10*q20+q_core11*q21+q_core12*q22;

				jac20 = q_core20*q00+q_core21*q01+q_core22*q02;
				jac21 = q_core20*q10+q_core21*q11+q_core22*q12;
				jac22 = q_core20*q20+q_core21*q21+q_core22*q22;


				// 9 items for stiffness matrix
				K->addAndSetValue(ri, ci, jac00);
				K->addAndSetValue(ri, ci + 1, jac01);
				K->addAndSetValue(ri, ci + 2, jac02);
				K->addAndSetValue(ri + 1, ci, jac10);
				K->addAndSetValue(ri + 1, ci + 1, jac11);
				K->addAndSetValue(ri + 1, ci + 2, jac12);
				K->addAndSetValue(ri + 2, ci, jac20);
				K->addAndSetValue(ri + 2, ci + 1, jac21);
				K->addAndSetValue(ri + 2, ci + 2, jac22);

			}
		}

	}

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
		/*points[i]->getU()[0] = points[i]->getX()[0];
		points[i]->getU()[1] = points[i]->getX()[1];
		points[i]->getU()[2] = points[i]->getX()[2];*/
		points[i]->getX()[0] += f[3*i] * deltaT;
		points[i]->getX()[1] += f[3*i+1] * deltaT;
		points[i]->getX()[2] += f[3*i+2] * deltaT;
	}
}



void Volume::collisionBidon()
{
	for (int i=0; i<(int)points.size(); i++)
	{
		if (points[i]->getX()[1] < 0) {
			forces[3*i+1] = -points[i]->getMass()*points[i]->getX()[1]/(scene->getDeltaT()*scene->getDeltaT());
		}
	}
}

int Volume::calculFracture()
{
	/* Each Tetrahedron */
	for (int t = 0; t < (int)tetrahedra.size(); t++)
	{
		/* Eigen Value and Vector */
		Matrix3d eigenVectors;
		Matrix<double, 3, 1> eigenValues;

		tetrahedra[t]->retrieveEigenOfStress(eigenValues, eigenVectors);

		/* Tensile Component and Compressive Component */
		Matrix3d tensileComp = Matrix3d::Zero();
		Matrix3d compressiveComp = Matrix3d::Zero();
		for (int i = 0; i < 3; i++)
		{
			double tensileCoef = eigenValues(i,0) > 0 ? eigenValues(i,0) : 0;	// max(vi,0)
			double compressiveCoef = eigenValues(i,0) < 0 ? eigenValues(i,0) : 0;	// min(vi,0)

			
			Matrix<double, 3, 1> eigenVector;
			eigenVector(0,0) = eigenVectors(0,i);	// column
			eigenVector(1,0) = eigenVectors(1,i);
			eigenVector(2,0) = eigenVectors(2,i);

			Matrix3d ma_n = util::calcul_M33_MA(eigenVector);

			tensileComp += ma_n * tensileCoef;
			compressiveComp += ma_n * compressiveCoef;
		}

		/* Tensile Force and Compressive Force */
		for (int i = 0; i < 4; i++)
		{
			Matrix<double, 3, 1> tensileForce = tetrahedra[t]->getQ() * tensileComp * tetrahedra[t]->getN(i);
			Matrix<double, 3, 1> compressiveForce = tetrahedra[t]->getQ() * compressiveComp * tetrahedra[t]->getN(i);

			tetrahedra[t]->setCompressiveForce(i, compressiveForce);
			tetrahedra[t]->setTensileForce(i, tensileForce);
		}
	}

	/* Each point */
	int fractureCount = 0;
	int oldPointSize = (int)points.size();
	int oldTetSize = (int)tetrahedra.size();

	for (int k = 0; k < oldPointSize; k++)
	{
		Matrix<double, 3, 1> tforce = Matrix<double, 3, 1>::Zero();
		Matrix<double, 3, 1> cforce = Matrix<double, 3, 1>::Zero();
		Matrix3d separationTensor = Matrix3d::Zero();

		Point* point = points[k];

		std::vector<IndexTetraPoint> *myTetraIndex = point->getIndexTetra();

		for (int i = 0; i < (int)myTetraIndex->size(); i++)
		{
			int pointIndex = myTetraIndex->at(i).indexOfPoint;
			Tetrahedron* tet = myTetraIndex->at(i).tet;

			// should be read-only
			Matrix<double, 3, 1> pointTF = tet->getTensileForce(pointIndex);
			Matrix<double, 3, 1> pointCF = tet->getCompressiveForce(pointIndex);

			// accumulate f+ and f-
			tforce += pointTF;
			cforce += pointCF;

			// c += m(f+(ell)) - m(f-(ell))
			separationTensor += util::calcul_M33_MA(pointTF) - util::calcul_M33_MA(pointCF);
		}

		// c-= m(f+) - m(f-)	// whether * 1/2 it depends
		separationTensor -= util::calcul_M33_MA(tforce) - util::calcul_M33_MA(cforce);

		// v+, the greatest positive eigenValue of c
		Matrix<double, 3, 1> eigenValue; 
		Matrix3d eigenVector;
		util::retrieveEigen(separationTensor, eigenValue, eigenVector);

		int vindex = 0;
		for (int i = 0; i < 3; i++)
		{
			if (eigenValue(i, 0) > eigenValue(vindex, 0))
				vindex = i;
		}

		if (eigenValue(vindex, 0) > 0 && eigenValue(vindex, 0) > material.toughness)
		{
			/* A fracture is happened */

			// normal vector
			Matrix<double, 3, 1> nvector;
			nvector(0,0) = eigenVector(0, vindex);
			nvector(1,0) = eigenVector(1, vindex);
			nvector(2,0) = eigenVector(2, vindex);
			nvector.normalize();

			/*
			// replica without re-mesh
						
			int pointIndex = points.size();	//int pointIndex = pointPool->size() + newPointList.size();

			Point* replica = replicaPointWithoutRemesh(point, nvector, pointIndex);

			if (replica->getIndexTetra()->size() > 0)
			{
				points.push_back(replica);//newPointList.push_back(replica);

				tetrahedraChanged = true;
				facesChanged = true;

				fractureCount++;
			}
			else
			{
				// the replica is not useful
				delete replica;
			}
			*/
			
			// replica with remesh
			int pointIndex = points.size();
			//Point* replica = replicaPointWithRemesh(point, nvector, pointIndex);
			Point* replica = replicaPointWithRemesh2(point, nvector);
			if (replica->getIndexTetra()->size() > 0)
			{
				points.push_back(replica);

				tetrahedraChanged = true;
				facesChanged = true;

				fractureCount++;
			}
			else
			{
				// must not reach here
				util::log("FETAL ERROR: Volume::replica with remesh");
			}

			// end the remesh process to avoid some crush increasing tets
			if (tetrahedra.size() - oldTetSize > 100)
				return fractureCount;
		}

	}

	return fractureCount;
}

Point* Volume::replicaPointWithRemesh2(Point* orginal, Matrix<double, 3, 1>& nvector)
{
	// remesh buf
	std::vector<IndexTetraPoint> remeshBuf = *orginal->getIndexTetra();

	for (std::vector<IndexTetraPoint>::iterator iter = remeshBuf.begin(); iter != remeshBuf.end(); ++iter)
	{
		iter->tet->remesh2(orginal, nvector, this->points);
	}

	// reassign
	return replicaPointWithoutRemesh(orginal, nvector, points.size());
}	

Point* Volume::replicaPointWithRemesh(Point* orginal, Matrix<double, 3, 1>& nvector, int replicaPointIndex)
{
	// copy material position, current position , velority;
	Point* replica = new Point(replicaPointIndex, orginal->getX(), orginal->getV(), orginal->getU(), true);

	// reassign
	std::vector<IndexTetraPoint> *myTetraIndex = orginal->getIndexTetra();

	// remesh buf
	std::vector<IndexTetraPoint> remeshBuf;

	double* thisPosition = orginal->getX();

	for (std::vector<IndexTetraPoint>::iterator iter = orginal->getIndexTetra()->begin(); iter != orginal->getIndexTetra()->end();)
	{
		int pointIndex = iter->indexOfPoint;
		Tetrahedron* tet = iter->tet;

		/* for those poi																													nts which is not this point (fracture), could calculate its cos() to determine whether the 
			tetraedre is (on + side, on - side, intersect)
		*/
		int state = 0;
		
		for (int i = 0; i < 4; i++)
		{
			if (i == pointIndex)		// skip "this point"
				continue;

			double* thatPosition = tet->getPoints().at(i)->getX();

			// calcul the vector directs from this point (end) to that point (head)
			Matrix<double, 3, 1> vec;
			vec(0,0) = thatPosition[0] - thisPosition[0];
			vec(1,0) = thatPosition[1] - thisPosition[1];
			vec(2,0) = thatPosition[2] - thisPosition[2];

			// calcul the cos of two vector
			vec.normalize();
			double cosValue = vec.dot(nvector);

			// accumulate the number of point on the same side
			if (cosValue > 0)
				state += 1;
			else
				state -= 1;
		}

		if (state == 3)
		{
			// belongs to q+
			// do not remove from the current list
			++iter;
		}
		else if (state == -3)	// 
		{
			// belongs to q-
			if (orginal->getIndexTetra()->size() > 1)
			{
				// visability
				orginal->setIsSurface(true);			

				/* re-assign volumic */
				struct IndexTetraPoint newTetPointIndex;
				newTetPointIndex.tet = tet;
				newTetPointIndex.indexOfPoint = pointIndex;

				replica->getIndexTetra()->push_back(newTetPointIndex);
				tet->getPoints()[pointIndex] = replica;

				// remove from the current tet list
				iter = orginal->getIndexTetra()->erase(iter);
			}
			else
			{
				++iter;
			}
		}
		else
		{
			// intersect, should remesh
			IndexTetraPoint index = *iter;
			remeshBuf.push_back(index);
			
			++iter;
		}
	}

	//remesh
	int oldTetSize = tetrahedra.size();
	for (std::vector<IndexTetraPoint>::iterator iter = remeshBuf.begin(); iter != remeshBuf.end(); ++iter)
	{
		iter->tet->remesh(orginal, replica, nvector, this->points);

		if (tetrahedra.size() - oldTetSize > 100)
			break;
	}

	return replica;
}

Point* Volume::replicaPointWithoutRemesh(Point* orginal, Matrix<double, 3, 1>& nvector, int replicaPointIndex)
{
	// copy material position, current position , velority;
	Point* replica = new Point(replicaPointIndex, orginal->getX(), orginal->getV(), orginal->getU(), true);

	bool shouldCreateSurface = false;

	// reassign
	std::vector<IndexTetraPoint> *myTetraIndex = orginal->getIndexTetra();

	double* thisPosition = orginal->getX();

	for (std::vector<IndexTetraPoint>::iterator iter = orginal->getIndexTetra()->begin(); iter != orginal->getIndexTetra()->end();)
	{
		int pointIndex = iter->indexOfPoint;
		Tetrahedron* tet = iter->tet;

		/* for those points which is not this point (fracture), could calculate its cos() to determine whether the 
			tetraedre is (on + side, on - side, intersect)
		*/
		int state = 0;
		
		for (int i = 0; i < 4; i++)
		{
			if (i == pointIndex)		// skip "this point"
				continue;

			double* thatPosition = tet->getPoints().at(i)->getX();

			// calcul the vector directs from this point (end) to that point (head)
			Matrix<double, 3, 1> vec;
			vec(0,0) = thatPosition[0] - thisPosition[0];
			vec(1,0) = thatPosition[1] - thisPosition[1];
			vec(2,0) = thatPosition[2] - thisPosition[2];

			// calcul the cos of two vector
			vec.normalize();
			double cosValue = vec.dot(nvector);

			// accumulate the number of point on the same side
			if (cosValue > 0)
				state += 1;
			else
				state -= 1;
		}

		if (state == 3)
		{
			// belongs to q+
			// do not remove from the current list
			++iter;
		}
		else if ((state == -3 && orginal->getIndexTetra()->size() > 1) || (state > -3 && replica->getIndexTetra()->size() == 0 && orginal->getIndexTetra()->size() > 1))
		{
			// belongs to q-

			// visability
			orginal->setIsSurface(true);			

			/* re-assign volumic */
			struct IndexTetraPoint newTetPointIndex;
			newTetPointIndex.tet = tet;
			newTetPointIndex.indexOfPoint = pointIndex;

			replica->getIndexTetra()->push_back(newTetPointIndex);
			tet->getPoints()[pointIndex] = replica;

			// remove from the current tet list
			iter = orginal->getIndexTetra()->erase(iter);

			shouldCreateSurface = true;
		}
		else
		{
			// intersect, should remesh
			++iter;
		}
	}

	/*
	if (shouldCreateSurface)
	{
		// create new surface
		vector<struct IndexSurfacePoint>* mySurfIndexVec = orginal->getIndexSurface();
		for (int k = 0; k < (int)mySurfIndexVec->size(); k++)
		{
			struct Surface* newSurf = new struct Surface[1];
			struct Surface* oldSurf = mySurfIndexVec->at(k).surface;
			int oldPointIndex = mySurfIndexVec->at(k).indexOfPoint;

			// copy and change index				
			newSurf->pointIndex[0] = oldSurf->pointIndex[0];
			newSurf->pointIndex[1] = oldSurf->pointIndex[1];
			newSurf->pointIndex[2] = oldSurf->pointIndex[2];

			newSurf->pointIndex[oldPointIndex] = replicaPointIndex;

			// add new surface
			surfaces.push_back(newSurf);

			// update index of surface
			struct IndexSurfacePoint newSurfPointIndex;
			newSurfPointIndex.surface = newSurf;
			newSurfPointIndex.indexOfPoint = oldPointIndex;

			replica->getIndexSurface()->push_back(newSurfPointIndex);
		}
	}
	*/
	
	return replica;
}

void Volume::resetAll()
{
	if (oldOrder != points.size() * 3)
	{
		int order = points.size() * 3;

		//matrix
		K->changeOrder(order);
		M->changeOrder(order);
		C->changeOrder(order);

		//vector
		delete v;
		delete forceField;
		delete forces;
		delete xu;
		delete f;
		

		xu = new double[order];
		f = new double[order];
		v = new double[order];
		forceField = new double[order];
		forces = new double[order];

		memset((void*)xu, 0, order * sizeof(double));
		memset((void*)f, 0, order * sizeof(double));
		memset((void*)v, 0, order * sizeof(double));
		memset((void*)forceField, 0, order * sizeof(double));
		memset((void*)forces, 0, order * sizeof(double));

		// recalculate M
		generateMAndForceField();

		oldOrder = order;		
	}
}
