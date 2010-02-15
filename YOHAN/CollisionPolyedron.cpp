#include "CollisionPolyedron.h"

CollisionPolyedron::CollisionPolyedron(void)
{
}

CollisionPolyedron::~CollisionPolyedron(void)
{
}

CollisionPolyedron::CollisionPolyedron(std::vector<CollisionFace>* facets, int p1, int p2)
{
	faces = facets;
	parents[0] = p1;
	parents[1] = p2;
}

std::vector<CollisionFace>* CollisionPolyedron::getFaces()
{
	return this->faces;
}

std::vector<CollisionFace>* CollisionPolyedron::getByOwner(int ind)
{
	std::vector<CollisionFace>* resul = new std::vector<CollisionFace>();
	std::vector<CollisionFace>* faces = this->getFaces();
	for(int i=0; i<(int)(*faces).size(); i++){
		if((*faces)[i].getOwner()==ind){
			resul->push_back((*faces)[i]);
		}
	}
	return resul;
}

double CollisionPolyedron::calcVolume()
{
	double resul;
	if (this->volume!=0){
		resul = this->volume;
	}else{
		resul = 0;
		std::vector<CollisionFace>* faces = this->getFaces();
		CollisionPoint& q = (*faces)[0][0];
		for(int i=0; i<(int)(*faces).size(); i++){
			for(int j=2; j<(*faces)[i].size(); j++){
				resul += CollisionPoint::volOp(&q, &(*faces)[i][0], &(*faces)[i][j], &(*faces)[i][j-1]);
			}
		}
	}
	volume = resul;
	return resul;
}

CollisionPoint* CollisionPolyedron::calcDir(int owner)
{
	CollisionPoint* resul;
	std::vector<CollisionFace>* faces;
	CollisionPoint* dir;
	// Test if this is collision with y=0 plan
	if(parents[0]<0 || parents[1]<0){
		if(owner<0)resul = new CollisionPoint(0,-1,0);
		else resul = new CollisionPoint(0,1,0);
	// Two tetrahedron collision
	}else{
		resul = new CollisionPoint();
		if(this->parents[0]==owner){
			faces = this->getByOwner(parents[1]);
		}else{
			faces = this->getByOwner(parents[0]);
		}
		for(int i=0;i<(int)faces->size();i++){
			for(int j=0;j<(int)faces[i].size();j++){
				dir = CollisionPoint::vect(&(*faces)[i][j], &(*faces)[i][0], &(*faces)[i][j-1], &(*faces)[i][0]);
				resul->add(dir);
				delete dir;
			}
		}
		delete faces;
		resul->mul(1/(resul->norm()));
	}
	return resul;
}

CollisionPoint* CollisionPolyedron::calcCenter()
{
	CollisionPoint* resul = new CollisionPoint();
	std::vector<CollisionFace>* faces = this->getFaces();
	CollisionPoint& q = (*faces)[0][0];
	CollisionPoint* pt;
	double vol;
	for(int i=0; i<(int)(*faces).size(); i++){
		for(int j=2; j<(*faces)[i].size(); j++){
			vol = CollisionPoint::det4(&q, &(*faces)[i][0], &(*faces)[i][j-1], &(*faces)[i][j]);
			pt = new CollisionPoint();
			pt->add(&q);
			pt->add(&(*faces)[i][0]);
			pt->add(&(*faces)[i][j-1]);
			pt->add(&(*faces)[i][j]);
			pt->mul(vol);
			resul->add(pt);
			delete pt;
		}
	}
	resul->mul(1/(5*this->calcVolume()));
	return resul;
}

std::vector<yohan::base::DATA*>* CollisionPolyedron::getByParent(yohan::base::VolumeModel* model, int parent)
{
	std::vector<yohan::base::DATA*>* resul = new std::vector<yohan::base::DATA*>();
	if(parent>=0){
		int* ptsInds = model->getTetrahedron(parent)->getPointIndex();
		for(int i=0;i<4;i++){
			resul->push_back(model->getPoint(ptsInds[i]));
		}
	}else{

		yohan::base::DATA tmp[13];
		for(int j=0;j<13;j++){
				tmp[j] = 0;
		}
		yohan::base::DATA* pt;
		for(int i=0;i<4;i++){
			pt = new yohan::base::DATA[13];
			memcpy((void*)pt, (void*)tmp, 13);
			resul->push_back(pt);
		}
	}
	return resul;
}

std::vector<double>* CollisionPolyedron::calcBarCoeff(std::vector<yohan::base::DATA*>* pts)
{
	std::vector<double>* coeffBar = new std::vector<double>();
	double sum = 0;
	double distance;
	CollisionPoint* center = this->calcCenter();
	CollisionPoint* pt;
	for(int i=0;i<(int)pts->size();i++){
		pt = new CollisionPoint((*pts)[i]);
		distance = CollisionPoint::distance(center, pt);
		delete pt;
		coeffBar->push_back(distance);
		sum+=distance;
	}
	for(int i=0;i<(int)coeffBar->size();i++){
		(*coeffBar)[i]/=sum;
	}
	return coeffBar;
}

void CollisionPolyedron::collisionForces(yohan::base::VolumeModel* model, double kerr, double kdmp, double kfrc, CollisionPoint* f)
{
	double k;
	CollisionPoint* ferrA = new CollisionPoint();
	CollisionPoint* fdmpA = new CollisionPoint();
	CollisionPoint* ffrcA = new CollisionPoint();
	CollisionPoint* ferrB = new CollisionPoint();
	CollisionPoint* fdmpB = new CollisionPoint();
	CollisionPoint* ffrcB = new CollisionPoint();

	// Ferr calcul
	ferrA->add(f);
	k = kerr * this->calcVolume();
	ferrA->mul(k);
	ferrB->minus(ferrA);

	// Two tetrahedron collision
	if(parents[0]>=0 && parents[1]>=0){

		// Fdmp calcul
		std::vector<yohan::base::DATA*>* ptsA = getByParent(model, parents[0]);
		std::vector<double>* bA = calcBarCoeff(ptsA);
		std::vector<yohan::base::DATA*>* ptsB = getByParent(model, parents[1]);
		std::vector<double>* bB = calcBarCoeff(ptsB);
		yohan::base::DATA v[3] = {0,0,0};
		for(int i=0;i<3;i++){
			for(int j=0;j<4;j++){
				v[i] += (*bA)[j]*(*ptsA)[j][3+i]-(*bB)[j]*(*ptsB)[j][3+i];
			}
		}
		CollisionPoint* vp = new CollisionPoint(v[0], v[1], v[2]);
		k = CollisionPoint::scal(vp, f);
		fdmpA->add(f);
		fdmpA->mul(-kdmp*(this->calcVolume())*k);
		fdmpB->minus(fdmpA);

		// Ffrc calcul
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		k = -kfrc*(ferrA->norm());
		for(int j=0;j<4;j++){
			CollisionPoint* vtmp = new CollisionPoint((*ptsA)[j][3],(*ptsA)[j][4],(*ptsA)[j][5]);
			double ktmp = CollisionPoint::scal(vtmp,f);
			double ftmp[3] = {f->getX(), f->getY(), f->getZ()};
			for(int i=0;i<3;i++){				
				v[i] += (*bA)[j]*((*ptsA)[j][3+i]-ktmp*ftmp[i]);
			}
		}
		ffrcA = new CollisionPoint(v[0],v[1],v[2]);
		ffrcA->mul(k);
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		for(int j=0;j<4;j++){
			CollisionPoint* vtmp = new CollisionPoint((*ptsB)[j][3],(*ptsB)[j][4],(*ptsB)[j][5]);
			double ktmp = -CollisionPoint::scal(vtmp,f);
			double ftmp[3] = {-f->getX(), -f->getY(), -f->getZ()};
			for(int i=0;i<3;i++){				
				v[i] += (*bB)[j]*((*ptsB)[j][3+i]-ktmp*ftmp[i]);
			}
		}
		ffrcB = new CollisionPoint(v[0],v[1],v[2]);
		ffrcB->mul(k);
	// One Tetrahedron and y=O Collision
	}else{
		if(parents[0]<0){
			// Fdmp calcul
			std::vector<yohan::base::DATA*>* ptsA = getByParent(model, parents[0]);
			std::vector<double>* bA = calcBarCoeff(ptsA);
			std::vector<yohan::base::DATA*>* ptsB = getByParent(model, parents[1]);
			std::vector<double>* bB = calcBarCoeff(ptsB);
			yohan::base::DATA v[3] = {0,0,0};
			for(int i=0;i<3;i++){
				for(int j=0;j<4;j++){
					v[i] += (*bA)[j]*(*ptsA)[j][3+i]-(*bB)[j]*(*ptsB)[j][3+i];
				}
			}
			CollisionPoint* vp = new CollisionPoint(v[0], v[1], v[2]);
			k = CollisionPoint::scal(vp, f);
			fdmpA->add(f);
			fdmpA->mul(-kdmp*(this->calcVolume())*k);
			fdmpB->minus(fdmpA);

			// Ffrc calcul
			v[0] = 0;
			v[1] = 0;
			v[2] = 0;
			k = -kfrc*(ferrB->norm());
			for(int j=0;j<4;j++){
				CollisionPoint* vtmp = new CollisionPoint((*ptsB)[j][3],(*ptsB)[j][4],(*ptsB)[j][5]);
				double ktmp = -CollisionPoint::scal(vtmp,f);
				double ftmp[3] = {-f->getX(), -f->getY(), -f->getZ()};
				for(int i=0;i<3;i++){				
					v[i] += (*bB)[j]*((*ptsB)[j][3+i]-ktmp*ftmp[i]);
				}
			}
			ffrcB = new CollisionPoint(v[0],v[1],v[2]);
			ffrcB->mul(k);
		}else{
			// Fdmp calcul
			std::vector<yohan::base::DATA*>* ptsA = getByParent(model, parents[0]);
			std::vector<double>* bA = calcBarCoeff(ptsA);
			yohan::base::DATA v[3] = {0,0,0};
			// Only the velocity relative to y axe
			for(int j=0;j<4;j++){
				v[2] += (*bA)[j]*(*ptsA)[j][5];
			}
			CollisionPoint* vp = new CollisionPoint(v[0], v[1], v[2]);
			k = CollisionPoint::scal(vp, f);
			fdmpA->add(f);
			fdmpA->mul(-kdmp*(this->calcVolume())*k);

			// Ffrc calcul
			v[0] = 0;
			v[1] = 0;
			v[2] = 0;
			k = -kfrc*(ferrA->norm());
			for(int j=0;j<4;j++){
				CollisionPoint* vtmp = new CollisionPoint((*ptsA)[j][3],(*ptsA)[j][4],(*ptsA)[j][5]);
				double ktmp = CollisionPoint::scal(vtmp,f);
				double ftmp[3] = {f->getX(), f->getY(), f->getZ()};
				for(int i=0;i<3;i++){				
					v[i] += (*bA)[j]*((*ptsA)[j][3+i]-ktmp*ftmp[i]);
				}
			}
			ffrcA = new CollisionPoint(v[0],v[1],v[2]);
			ffrcA->mul(k);
		}
	}
	
}