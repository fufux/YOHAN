#include <Eigen/Eigen>
#include "Util.h"
#include "Point.h"

// import most common Eigen types 
USING_PART_OF_NAMESPACE_EIGEN
using namespace util;


void util::log(const char* message)
{
	OutputDebugStringA(message);
	OutputDebugStringA("\n");
}

void util::log(const std::string message)
{
	util::log(message.c_str());
}

std::string util::ws2s(const std::wstring& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	char* buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0);
	std::string r(buf);
	delete[] buf;
	return r;
}

double util::norm(double** x)
{
	double norm = 0;
	double max;
	for( int i=0;i<3;i++){
		max = 0;
		for(int j=0;j<3;j++){
			max = abs(x[i][j])>max?abs(x[i][j]):max;
		}
		norm += max;
	}
	return norm;
}

double util::normMinus(double** x, double** y)
{
	double norm = 0;
	double max;
	for( int i=0;i<3;i++){
		max = 0;
		for(int j=0;j<3;j++){
			max = abs(x[i][j]-y[i][j])>max?abs(x[i][j]-y[i][j]):max;
		}
		norm += max;
	}
	return norm;
}

void util::plan(vector<Point*>* vertices, int i1, int i2, int i3, Vector3d* n, double* cst)
{
	Vector3d a = Vector3d((*vertices)[i1]->getX()[0], (*vertices)[i1]->getX()[1], (*vertices)[i1]->getX()[2]);
	Vector3d b = Vector3d((*vertices)[i2]->getX()[0], (*vertices)[i2]->getX()[1], (*vertices)[i2]->getX()[2]);
	Vector3d c = Vector3d((*vertices)[i3]->getX()[0], (*vertices)[i3]->getX()[1], (*vertices)[i3]->getX()[2]);
	*n = (b - a).cross(c - a);
	*cst = -n->dot(a);
}

vector<Vector3d*>* util::intersect(vector<Vector3d*>* vertices, Vector3d* alpha, double alphaC, Point* pt)
{
	vector<Vector3d*>* resul = new vector<Vector3d*>();
	Vector3d* s = (*vertices)[vertices->size()-1];
	Vector3d* p = (*vertices)[0];
	Vector3d* v;
	Vector3d in(pt->getX()[0], pt->getX()[1], pt->getX()[2]);
	bool as;
	bool ap;
	for(int i=1; i<(int)vertices->size(); i++){
		// Look if the point is "inside" or "outside"
		as = (alpha->dot(*s)+alphaC)*(alpha->dot(in)+alphaC)<0;
		ap = (alpha->dot(*p)+alphaC)*(alpha->dot(in)+alphaC)<0;
		if(!as && !ap)
			resul->push_back(p);
		else if(!as && ap){
			v = intersectLinePlan(s, p, alpha, alphaC);
		}else if(as && ap){
			// reject
		}else{
			// as && !ap
			v = intersectLinePlan(s, p, alpha, alphaC);
			resul->push_back(p);
		}
		s = p;
		p = (*vertices)[i];
	}
	return resul;
}

Vector3d* util::intersectLinePlan(Vector3d* s, Vector3d* p, Vector3d* alpha, double alphaC)
{
	double k = -(alpha->dot(*s)+alphaC)/(((*p)-(*s)).dot(*alpha));
	return new Vector3d(s->x()+k*(p->x()-s->x()), s->y()+k*(p->y()-s->y()), s->z()+k*(p->z()-s->z()));
}