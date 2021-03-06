#include "Polyhedron.h"
#include "Volume.h"

Polyhedron::Polyhedron(void)
{
}

Polyhedron::Polyhedron(vector<Face*>* faces, Tetrahedron* t1, Tetrahedron* t2)
{
	this->faces = faces;
	parents.push_back(t1);
	parents.push_back(t2);
	volume = this->calcVolume();
	calcCenter();
}

Polyhedron::~Polyhedron(void)
{
}



vector<Face*>* Polyhedron::getByOwner(int ind)
{
	vector<Face*>* resul = new vector<Face*>();
	for(int i=0; i<(int)faces->size(); i++){
		if((*faces)[i]->getOwner()==ind){
			resul->push_back((*faces)[i]);
		}
	}
	return resul;
}

double Polyhedron::calcVolume()
{
	double resul;
	resul = 0;
	Vector3d q = *(*(*faces)[0])[0];
	Vector3d tmp1;
	Vector3d tmp2;
	// We parse the polyhedron in tetrahedron and then compute the volume of each tetrahedron
	for(int i=0; i<(int)faces->size(); i++){
		for(int j=2; j<(*faces)[i]->size(); j++){
			tmp1 = *(*(*faces)[i])[j]-*(*(*faces)[i])[0];
			tmp2 = *(*(*faces)[i])[j-1]-*(*(*faces)[i])[0];
			tmp1 = tmp1.cross(tmp2);
			tmp2 = q-*(*(*faces)[i])[0];
			resul -= tmp1.dot(tmp2);
		}
	}
	return resul/6;
}

Vector3d* Polyhedron::calcDir(int owner)
{
	Vector3d* resul; 
	vector<Face*>* facets;
	Vector3d tmp;
	Vector3d dir;
	// Test if this is collision with y=0 plan
	if(parents[0]->getID()<0 || parents[1]->getID()<0){
		if(owner<0)resul = new Vector3d(0,-1,0);
		else resul = new Vector3d(0,1,0);
	// Two tetrahedron collision
	}else{
		resul = new Vector3d(0,0,0);
		if(this->parents[0]->getID()==owner){
			facets = this->getByOwner(parents[1]->getID());
		}else{
			facets = this->getByOwner(parents[0]->getID());
		}
		for(int i=0;i<(int)facets->size();i++){
			for(int j=2;j<(int)(*facets)[i]->size();j++){
				tmp = *(*(*facets)[i])[j] - *(*(*facets)[i])[0];
				dir = *(*(*facets)[i])[j-1] - *(*(*facets)[i])[0];
				dir = tmp.cross(dir);
				*resul = *resul + dir;
			}
		}
		delete facets;
		resul->normalize();
	}
	return resul;
}

void Polyhedron::calcCenter()
{
	center = Vector3d(0,0,0);
	Vector3d q = *(*(*faces)[0])[0];
	Vector3d tmp1;
	Vector3d tmp2;
	Vector3d pt;
	double vol;
	for(int i=0; i<(int)faces->size(); i++){
		for(int j=2; j<(*faces)[i]->size(); j++){
			tmp1 = *(*(*faces)[i])[j]-*(*(*faces)[i])[0];
			tmp2 = *(*(*faces)[i])[j-1]-*(*(*faces)[i])[0];
			tmp1 = tmp1.cross(tmp2);
			tmp2 = q-*(*(*faces)[i])[0];
			vol = abs(tmp1.dot(tmp2))/6;
			pt = vol * (q + *(*(*faces)[i])[0] + *(*(*faces)[i])[j-1] + *(*(*faces)[i])[j]);
			center = center + pt;
		}
	}
	vol = volume;
	center = center/(4*vol);
}

Vector3d* Polyhedron::getCenter()
{
	return &center;
}

vector<Point*>* Polyhedron::getByParent(int parent)
{
	if(parents[0]->getID()==parent)
		return &parents[0]->getPoints();
	else
		return &parents[1]->getPoints();
}

vector<double>* Polyhedron::calcBarCoeff(vector<Point*>* pts)
{
	Matrix4d A = Matrix4d::Ones();
	Vector4d b = Vector4d(center[0], center[1], center[2], 1);
	double* pt;
	for(int i=0; i<4;i++){
		pt = (*pts)[i]->getX();
		A(0,i) = pt[0];
		A(1,i) = pt[1];
		A(2,i) = pt[2];
	}
	A = A.inverse();
	b = A*b;
	vector<double>* coeffBar = new vector<double>();
	coeffBar->push_back(b[0]);
	coeffBar->push_back(b[1]);
	coeffBar->push_back(b[2]);
	coeffBar->push_back(b[3]);
	return coeffBar;
	//vector<double>* coeffBar = new vector<double>();
	//double sum = 0;
	//double distance;
	//Vector3d pt;
	//for(int i=0;i<(int)pts->size();i++){
	//	pt[0] = (*pts)[i]->getX()[0];
	//	pt[1] = (*pts)[i]->getX()[1];
	//	pt[2] = (*pts)[i]->getX()[2];
	//	pt = pt - center;
	//	distance = pt.norm();
	//	coeffBar->push_back(distance);
	//	sum+=distance;
	//}
	//for(int i=0;i<(int)coeffBar->size();i++){
	//	(*coeffBar)[i]/=sum;
	//}
	//return coeffBar;
}


void Polyhedron::collisionForces(double kerr, double kdmp, double kfrc)
{
	double k;

	double vol = this->calcVolume();
	//cout << vol << endl;
	double* f1;
	if(this->parents[0]->getID()!=-1)
		f1 = this->parents[0]->getVolume()->getForces();
	double* f2;
	if(this->parents[1]->getID()!=-1)
		f2 = this->parents[1]->getVolume()->getForces();
	Vector3d fA;
	Vector3d ferrA;
	Vector3d fdmpA;
	Vector3d ffrcA;
	Vector3d fB;
	Vector3d ferrB;
	Vector3d fdmpB;
	Vector3d ffrcB;

	// Calcul des directions des forces
	fA = *(this->calcDir(parents[0]->getID()));
	fB= -fA;

	// Two tetrahedron collision
	if(parents[0]->getID()>=0 && parents[1]->getID()>=0){
		// Ferr calcul
		ferrA = kerr * vol * fA;
		ferrB = - ferrA;

		// Fdmp calcul
		vector<Point*>* ptsA = getByParent( parents[0]->getID());
		vector<double>* bA = calcBarCoeff(ptsA);
		vector<Point*>* ptsB = getByParent( parents[1]->getID());
		vector<double>* bB = calcBarCoeff(ptsB);
		Vector3d v = Vector3d::Zero();
		for(int i=0;i<3;i++){
			for(int j=0;j<4;j++){
				v[i] += (*bA)[j]*(*ptsA)[j]->getV()[i]-(*bB)[j]*(*ptsB)[j]->getV()[i];
			}
		}
		k = v.dot(fA);
		fdmpA = -kdmp * vol *k * fA;
		fdmpB = -fdmpA;

		// Ffrc calcul
		ffrcA = Vector3d::Zero();
		k = -kfrc*(ferrA.norm());
		for(int j=0;j<4;j++){
			v[0] = (*ptsA)[j]->getV()[0];
			v[1] = (*ptsA)[j]->getV()[1];
			v[2] = (*ptsA)[j]->getV()[2];
			ffrcA = ffrcA + (*bA)[j] * (v - v.dot(fA)*fA);
		}
		ffrcA =  k * ffrcA;

		ffrcB = Vector3d::Zero();
		k = -kfrc*(ferrB.norm());
		for(int j=0;j<4;j++){
			v[0] = (*ptsB)[j]->getV()[0];
			v[1] = (*ptsB)[j]->getV()[1];
			v[2] = (*ptsB)[j]->getV()[2];
			ffrcB = ffrcB + (*bB)[j] * (v - v.dot(fB)*fB);
		}
		ffrcB =  k * ffrcB;

		// Sum all forces and dispatch it between the four vertices
		fA = ferrA + fdmpA + ffrcA;
		fB = ferrB + fdmpB + ffrcB;
		int indA;
		int indB;
		for(int i=0;i<4;i++){
			indA = 3*parents[0]->getPoints()[i]->getID();
			indB = 3*parents[1]->getPoints()[i]->getID();
			f1[indA]   += (*bA)[i]*fA[0];
			f1[indA+1] += (*bA)[i]*fA[1];
			f1[indA+2] += (*bA)[i]*fA[2];
			f2[indB]   += (*bB)[i]*fB[0];
			f2[indB+1] += (*bB)[i]*fB[1];
			f2[indB+2] += (*bB)[i]*fB[2];
		}

	// One Tetrahedron and y=O Collision
	}else{
		if(parents[0]->getID()<0){
			// Ferr calcul
			ferrB = this->parents[1]->getVolume()->getMaterial()->rho *kerr * vol * fB;

			// Fdmp calcul
			vector<Point*>* ptsB = getByParent( parents[1]->getID());
			vector<double>* bB = calcBarCoeff(ptsB);
			Vector3d v = Vector3d::Zero();
			// Only the velocity relative to y axe
			for(int j=0;j<4;j++){
				v[1] += (*bB)[j]*(*ptsB)[j]->getV()[1];
			}
			k = v.dot(fB);
			fdmpB = -this->parents[1]->getVolume()->getMaterial()->rho * kdmp * vol * k * fB;

			// Ffrc calcul
			ffrcB = Vector3d::Zero();
			k = -kfrc*(ferrB.norm());
			for(int j=0;j<4;j++){
				v[0] = (*ptsB)[j]->getV()[0];
				v[1] = (*ptsB)[j]->getV()[1];
				v[2] = (*ptsB)[j]->getV()[2];
				ffrcB = ffrcB + (*bB)[j] * (v - v.dot(fB)*fB);
			}
			ffrcB = k * ffrcB;

			// Sum all forces and dispatch it between the four vertices
			fB = ferrB + fdmpB + ffrcB;
			int indB;
			for(int i=0;i<4;i++){
				indB = 3*parents[1]->getPoints()[i]->getID();
				f2[indB]   += (*bB)[i]*fB[0];
				f2[indB+1] += (*bB)[i]*fB[1];
				f2[indB+2] += (*bB)[i]*fB[2];
			}

		}else{
			// Ferr calcul
			ferrA = this->parents[0]->getVolume()->getMaterial()->rho *kerr * vol * fA;

			// Fdmp calcul
			vector<Point*>* ptsA = getByParent( parents[0]->getID());
			vector<double>* bA = calcBarCoeff(ptsA);
			Vector3d v = Vector3d::Zero();
			// Only the velocity relative to y axe
			for(int j=0;j<4;j++){
				v[1] += (*bA)[j]*(*ptsA)[j]->getV()[1];
			}
			k = v.dot(fA);
			fdmpA = -this->parents[0]->getVolume()->getMaterial()->rho * kdmp * vol * k * fA;

			// Ffrc calcul
			ffrcA = Vector3d::Zero();
			k = -kfrc*(ferrA.norm());
			for(int j=0;j<4;j++){
				v[0] = (*ptsA)[j]->getV()[0];
				v[1] = (*ptsA)[j]->getV()[1];
				v[2] = (*ptsA)[j]->getV()[2];
				ffrcA = ffrcA + (*bA)[j] * (v - v.dot(fA)*fA);
			}
			ffrcA = k * ffrcA;

			// Sum all forces and dispatch it between the four vertices
			fA = ferrA + fdmpA + ffrcA;
			int indA;
			for(int i=0;i<4;i++){
				indA = 3*parents[0]->getPoints()[i]->getID();
				f1[indA]   += (*bA)[i]*fA[0];
				f1[indA+1] += (*bA)[i]*fA[1];
				f1[indA+2] += (*bA)[i]*fA[2];
			}
		}
	}	
}
