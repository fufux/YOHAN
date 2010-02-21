
#include "Volume.h"

Volume::Volume(int id)
{
	this->id = id;
	facesChanged = false;
	tetrahedraChanged = false;
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
	return (double*)forceField;
}
vector<double> Volume::getForces()
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
		int pa, pb, pc;
		fp >> tmp >> pa >> pb >> pc >> tmp;

		// should - 1 : index
		points[pa-1]->setIsSurface(true);
		points[pb-1]->setIsSurface(true);
		points[pc-1]->setIsSurface(true);
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
		Tetrahedron* t = new Tetrahedron(id, vp);
	}

	// close file
	fp.close();
	fp.clear();

	return true;
}


bool Volume::save(std::string dir)
{
	// .bnode
	std::stringstream sstream;
	sstream << dir.c_str() << "/" << id << ".bnode";

	// output
	ofstream fp (sstream.str().c_str(), ios::out | ios::binary);

	int size = points.size();
	fp.write ((char*)&size, sizeof(int));

	for (std::vector<Point*>::iterator iter = points.begin(); iter != points.end(); ++iter)
	{
		Point* point = *iter;
		fp.write ((char*)point->getX(), sizeof(DATA) * 3);
	}
	fp.flush();
	fp.close();
	fp.clear();

	if (!tetrahedraChanged)
	{
		// TODO
	}
	else
	{
		// .bele
		sstream = "";
		sstream << dir.c_str() << "/" << id << ".bele";

		// output
		fp.open(sstream.str().c_str(), ios::out | ios::binary);

		fp.write ((char*)&(tetrahedra.size()), sizeof(int));

		for (std::vector<Tetrahedron*>::iterator iter = tetrahedra.begin(); iter != tetrahedra.end(); ++iter)
		{
			Tetrahedron* tet = *iter;
			for (int j=0; j<4; j++)
				fp.write ((char*)tet->getPoints()[j]->getID(), sizeof(int));
		}
		fp.flush();
		fp.close();
		fp.clear();

		return true;
	}
}