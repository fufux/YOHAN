#include "stdafx.h"

#include "Base.h"
#include "Matrix.h"

#include <cstdio>

using namespace std;

using namespace yohan;
using namespace base;

/*************/
/* Tetrahedron */
/*************/

Tetrahedron::Tetrahedron(int id, const int pointIndex[], yohan::base::DATA constants[], yohan::base::PointPool *pointPoolRef, yohan::base::VolumeModel *volumeModelRef)
{
	this->id = id;

	this->pointIndex[0] = pointIndex[0];
	this->pointIndex[1] = pointIndex[1];
	this->pointIndex[2] = pointIndex[2];
	this->pointIndex[3] = pointIndex[3];

	this->constants = constants;

	this->pointPoolRef = pointPoolRef;
	this->pointPoolRef->updateTetrahedraIndex(this->pointIndex[0], id, 0);
	this->pointPoolRef->updateTetrahedraIndex(this->pointIndex[1], id, 1);
	this->pointPoolRef->updateTetrahedraIndex(this->pointIndex[2], id, 2);
	this->pointPoolRef->updateTetrahedraIndex(this->pointIndex[3], id, 3);

	this->volumeModelRef = volumeModelRef;
}

void Tetrahedron::fillMatrix(SquareSparseMatrix* K, SquareSparseMatrix* M, DATA* F, DATA gravity[])
{
	// point information
	DATA* p0 = this->pointPoolRef->getPointInfo(this->pointIndex[0]);
	DATA* p1 = this->pointPoolRef->getPointInfo(this->pointIndex[1]);
	DATA* p2 = this->pointPoolRef->getPointInfo(this->pointIndex[2]);
	DATA* p3 = this->pointPoolRef->getPointInfo(this->pointIndex[3]);

	// ai,bi,ci,di
	DATA a[4], b[4], c[4], d[4];

	// 6V
	DATA vol6;
	
	// i-0 j-1 m-2 p-3

	// ai = xj*ym*zp - xj*yp*zm - xm*yj*zp + xm*yp*zj + xp*yj*zm - xp*ym*zj
	a[0] = p1[0] * p2[1] * p3[2] - 
		 p1[0] * p3[1] * p2[2] - 
		 p2[0] * p1[1] * p3[2] + 
		 p2[0] * p3[1] * p1[2] + 
		 p3[0] * p1[1] * p2[2] - 
		 p3[0] * p2[1] * p1[2];

	a[1] = p2[0] * p3[1] * p0[2] - 
		 p2[0] * p0[1] * p3[2] - 
		 p3[0] * p2[1] * p0[2] + 
		 p3[0] * p0[1] * p2[2] + 
		 p0[0] * p2[1] * p3[2] - 
		 p0[0] * p3[1] * p2[2];

	a[2] = p3[0] * p0[1] * p1[2] - 
		 p3[0] * p1[1] * p0[2] - 
		 p0[0] * p3[1] * p1[2] + 
		 p0[0] * p1[1] * p3[2] + 
		 p1[0] * p3[1] * p0[2] - 
		 p1[0] * p0[1] * p3[2];

	a[3] = p0[0] * p1[1] * p2[2] - 
		 p0[0] * p2[1] * p1[2] - 
		 p1[0] * p0[1] * p2[2] + 
		 p1[0] * p2[1] * p0[2] + 
		 p2[0] * p0[1] * p1[2] - 
		 p2[0] * p1[1] * p0[2];

	// i-0 j-1 m-2 p-3

	// bi = -ym*zp + yp*zm + yj*zp - yp*zj - yj*zm + ym*zj
	b[0] = - p2[1] * p3[2] + p3[1] * p2[2] +
		   p1[1] * p3[2] - p3[1] * p1[2] -
		   p1[1] * p2[2] - p2[1] * p1[2];

	b[1] = - p3[1] * p0[2] + p0[1] * p3[2] +
		   p2[1] * p0[2] - p0[1] * p2[2] -
		   p2[1] * p3[2] - p3[1] * p2[2];

	b[2] = - p0[1] * p1[2] + p1[1] * p0[2] +
		   p3[1] * p1[2] - p1[1] * p3[2] -
		   p3[1] * p0[2] - p0[1] * p3[2];

	b[3] = - p1[1] * p2[2] + p2[1] * p1[2] +
		   p0[1] * p2[2] - p2[1] * p0[2] -
		   p0[1] * p1[2] - p1[1] * p0[2];

	// i-0 j-1 m-2 p-3

	// ci = xm*zp - xp*zm - xj*zp + xp*zj + xj*zm - xm*zj
	c[0] = p2[0] * p3[2] - p3[0] * p2[2] -
		 p1[0] * p3[2] + p3[0] * p1[2] +
		 p1[0] * p2[2] - p2[0] * p1[2];

	c[1] = p3[0] * p0[2] - p0[0] * p3[2] -
		 p2[0] * p0[2] + p0[0] * p2[2] +
		 p2[0] * p3[2] - p3[0] * p2[2];

	c[2] = p0[0] * p1[2] - p1[0] * p0[2] -
		 p3[0] * p1[2] + p1[0] * p3[2] +
		 p3[0] * p0[2] - p0[0] * p3[2];

	c[3] = p1[0] * p2[2] - p2[0] * p1[2] -
		 p0[0] * p2[2] + p2[0] * p0[2] +
		 p0[0] * p1[2] - p1[0] * p0[2];

	// i-0 j-1 m-2 p-3

	// di = -xm*yp + xp*ym + xj*yp - xp*yj - xj*ym + xm*yj
	d[0] = - p2[0] * p3[1] + p3[0] * p2[1] +
		   p1[0] * p3[1] - p3[0] * p1[1] -
		   p1[0] * p2[1] + p2[0] * p1[1];

	d[1] = - p3[0] * p0[1] + p0[0] * p3[1] +
		   p2[0] * p0[1] - p0[0] * p2[1] -
		   p2[0] * p3[1] + p3[0] * p2[1];

	d[2] = - p0[0] * p1[1] + p1[0] * p0[1] +
		   p3[0] * p1[1] - p1[0] * p3[1] -
		   p3[0] * p0[1] + p0[0] * p3[1];

	d[3] = - p1[0] * p2[1] + p2[0] * p1[1] +
		   p0[0] * p2[1] - p2[0] * p0[1] -
		   p0[0] * p1[1] + p1[0] * p0[1];


	// i-0 j-1 m-2 p-3

	/* 6V = xj*ym*zp - xj*zm*yp - xm*yj*zp + xm*zj*yp + 
			xp*yj*zm - xp*zj*ym - xi*ym*zp + xi*zm*yp + 
			xm*yi*zp - xm*zi*yp - xp*yi*zm + xp*zi*ym + 
			xi*yj*zp - xi*zj*yp - xj*yi*zp + xj*zi*yp + 
			xp*yi*zj - xp*zi*yj - xi*yj*zm + xi*zj*ym + 
			xj*yi*zm - xj*zi*ym - xm*yi*zj + xm*zi*yj 
	*/
	vol6 = p1[0] * p2[1] * p3[2] - p1[0] * p2[2] * p3[1] + p2[0] * p1[1] * p3[2] + p2[0] * p1[2] * p3[1] +
		   p3[0] * p1[1] * p2[2] - p3[0] * p1[2] * p2[1] - p0[0] * p2[1] * p3[2] + p0[0] * p2[2] * p3[1] +
		   p2[0] * p0[1] * p3[2] - p2[0] * p0[2] * p3[1] - p3[0] * p0[1] * p2[2] + p3[0] * p0[2] * p2[1] +
		   p0[0] * p1[1] * p3[2] - p0[0] * p1[2] * p3[1] - p1[0] * p0[1] * p3[2] + p1[0] * p0[2] * p3[1] +
		   p3[0] * p0[1] * p1[2] - p3[0] * p0[2] * p1[1] - p0[0] * p1[1] * p2[2] + p0[0] * p1[2] * p2[1] +
		   p1[0] * p0[1] * p2[2] - p1[0] * p0[2] * p2[1] - p2[0] * p0[1] * p1[2] + p2[0] * p0[2] * p1[1];

	if (vol6 < 0)
		vol6 = -vol6;

	/********************/
	/* Stiffness Matrix */
	/********************/

	// coff = A3 / 36V = (A3 / 6) / 6V
	DATA coff = this->constants[3] / vol6;

	/* r-s: 
	0-0 0-1 0-2 0-3 
	    1-1 1-2 1-3 
	        2-2 2-3 
	            3-3 
	*/
	
	// BrBs, CrCs ...
	DATA BrBs[16], CrCs[16], DrDs[16], BrCs[16], BrDs[16], CrDs[16], CrBs[16], DrBs[16], DrCs[16];
	int k = 0;
	
	for (int r = 0; r < 4; ++r)
	{
		for (int s = 0; s < 4; ++s)
		{
			BrBs[k] = b[r] * b[s];
			CrCs[k] = c[r] * c[s];
			DrDs[k] = d[r] * d[s];
			BrCs[k] = b[r] * c[s];
			BrDs[k] = b[r] * d[s];
			CrDs[k] = c[r] * d[s];
			CrBs[k] = c[r] * b[s];
			DrBs[k] = d[r] * b[s];
			DrCs[k] = d[r] * c[s];

			++k;
		}
	}

	// 16 * 9 items

	/* r-s: 
	0-0 0-1 0-2 0-3 
	    1-1 1-2 1-3 
	        2-2 2-3 
	            3-3 
	*/

	k = 0;
	for (int i = 0; i < 4; i++)
	{
		int ri = this->pointIndex[i] * 3 + 1;	// +1 is to conform the index in the matrix
		for (int j = 0; j < 4; j++)
		{
			int ci = this->pointIndex[j] * 3 + 1;	// +1 is to conform the index in the matrix

			// 9 items for stiffness matrix
			K->addAndSetValue(ri, ci, coff * (BrBs[k] + constants[1] * (CrCs[k] + DrDs[k])));
			K->addAndSetValue(ri, ci + 1, coff * (constants[0] * BrCs[k] + constants[1] * CrBs[k]));
			K->addAndSetValue(ri, ci + 2, coff * (constants[0] * BrDs[k] + constants[1] * DrBs[k]));
			K->addAndSetValue(ri + 1, ci, coff * (constants[1] * BrCs[k] + constants[0] * CrBs[k]));
			K->addAndSetValue(ri + 1, ci + 1, coff * (CrCs[k] + constants[1] * (DrDs[k] + BrBs[k])));
			K->addAndSetValue(ri + 1, ci + 2, coff * (constants[0] * CrDs[k] + constants[1] * DrCs[k]));
			K->addAndSetValue(ri + 2, ci, coff * (constants[1] * BrDs[k] + constants[0] * DrBs[k]));
			K->addAndSetValue(ri + 2, ci + 1, coff * (constants[1] * CrDs[k] + constants[0] * DrCs[k]));
			K->addAndSetValue(ri + 2, ci + 2, coff * (DrDs[k] + constants[1] * (BrBs[k] + CrCs[k])));

			++k;	

			coff = -coff;
		}

		coff = -coff;
	}

	/********************/
	/* Mass Matrix      */
	/********************/

	DATA m60 = vol6 * constants[5], m120 = vol6 * constants[6];
	
	for (int i = 0; i < 4; i++)
	{
		int ri = this->pointIndex[i] * 3 + 1;	// +1 is to conform the index in the matrix
		for (int j = 0; j < 4; j++)
		{
			int ci = this->pointIndex[j] * 3 + 1;	// +1 is to conform the index in the matrix
			
			if (i == j)
			{
				M->addAndSetValue(ri, ci, m60);
				M->addAndSetValue(ri + 1, ci + 1, m60);
				M->addAndSetValue(ri + 2, ci + 2, m60);
			}
			else
			{
				M->addAndSetValue(ri, ci, m120);
				M->addAndSetValue(ri + 1, ci + 1, m120);
				M->addAndSetValue(ri + 2, ci + 2, m120);
			}
		}
	}

	/* Force Matrix */
	

	DATA m = vol6 / 6 * this->constants[4];	// mass = V * density

	this->mass = m;

	DATA fx = gravity[0] * m / 4, fy = gravity[1] * m / 4, fz = gravity[2] * m / 4;

	// the volumic force
	F[this->pointIndex[0] * 3] += fx;
	F[this->pointIndex[1] * 3] += fx;
	F[this->pointIndex[2] * 3] += fx;
	F[this->pointIndex[3] * 3] += fx;

	F[this->pointIndex[0] * 3 + 1] += fy;
	F[this->pointIndex[1] * 3 + 1] += fy;
	F[this->pointIndex[2] * 3 + 1] += fy;
	F[this->pointIndex[3] * 3 + 1] += fy;

	/* could add a constraint : if z = 0 and v(z) = 0, a force will support it */
	
	//if (!(p0[2] < 1E-20 && p0[2] > -1E-20 && p0[5] < 1E-20 && p0[5] > -1E-20))
		F[this->pointIndex[0] * 3 + 2] += fz;
	//if (!(p1[2] < 1E-20 && p1[2] > -1E-20 && p1[5] < 1E-20 && p1[5] > -1E-20))
		F[this->pointIndex[1] * 3 + 2] += fz;
	//if (!(p2[2] < 1E-20 && p2[2] > -1E-20 && p2[5] < 1E-20 && p2[5] > -1E-20))
		F[this->pointIndex[2] * 3 + 2] += fz;
	//if (!(p3[2] < 1E-20 && p3[2] > -1E-20 && p3[5] < 1E-20 && p3[5] > -1E-20))
		F[this->pointIndex[3] * 3 + 2] += fz;

	/* end constant */
	
	// no constraint
	/*
	F[this->pointIndex[0] * 3 + 2] += fz;
	F[this->pointIndex[1] * 3 + 2] += fz;
	F[this->pointIndex[2] * 3 + 2] += fz;
	F[this->pointIndex[3] * 3 + 2] += fz;
	*/

	/********************/
	/* Stress Matrix */
	/********************/

	/*	0-2
		3-5
		6-8
		9-11
		12-14
		15-17
	*/
	DATA scoff = constants[2] / vol6;			// A3 / 6V
	for (int i = 0; i < 4; i++)
	{
		stress[i][0] = scoff * b[i];					// bi
		stress[i][1] = scoff * constants[0] * c[i];		// A1 * ci
		stress[i][2] = scoff * constants[0] * d[i];		// A1 * di
		stress[i][3] = scoff * constants[0] * b[i];		// A1 * bi
		stress[i][4] = scoff * c[i];					// ci
		stress[i][5] = stress[i][2];			// A1 * di
		stress[i][6] = stress[i][3];			// A1 * bi
		stress[i][7] = stress[i][1];			// A1 * ci
		stress[i][8] = scoff * d[i];					// di
		stress[i][9] = 0;
		stress[i][10] = scoff * constants[1] * d[i];	// A2 * di
		stress[i][11] = scoff * constants[1] * c[i];	// A2 * ci
		stress[i][12] = stress[i][10];			// A2 * di
		stress[i][13] = 0;
		stress[i][14] = scoff * constants[1] * b[i];	// A2 * bi
		stress[i][15] = stress[i][11];			// A2 * ci
		stress[i][16] = stress[i][14];			// A2 * bi
		stress[i][17] = 0;

		scoff = -scoff;
	}

}

int* Tetrahedron::getPointIndex()
{
	return this->pointIndex;
}

void Tetrahedron::fillStressVector(int indexOfPointIndex, DATA* pointData, DATA* res)
{
	DATA dx = pointData[0] - pointData[6];
	DATA dy = pointData[1] - pointData[7];
	DATA dz = pointData[2] - pointData[8];

	DATA* s = stress[indexOfPointIndex];

	// possible to reduce 3 items because they are all 0 (9, 13, 17)
	res[0] = s[0] * dx + s[1] * dy + s[2] * dz;
	res[1] = s[3] * dx + s[4] * dy + s[5] * dz;
	res[2] = s[6] * dx + s[7] * dy + s[8] * dz;
	res[3] = s[9] * dx + s[10] * dy + s[11] * dz;
	res[4] = s[12] * dx + s[13] * dy + s[14] * dz;
	res[5] = s[15] * dx + s[16] * dy + s[17] * dz;
}


//temporal
void Tetrahedron::fillForceList(std::list<SceneForce> *fl, DATA deltaTime)
{
	for (int i = 0; i < 4; i++)
	{
		int pindex = this->pointIndex[i];
		DATA* pinfo = this->pointPoolRef->getPointInfo(pindex);

		if (pinfo[1] < 0)	// y < 0
		{
			SceneForce sf;

			sf.vmeshID = 0;	// only one object in the scene
			sf.vertexID = pindex;

			//F = m * -v / deltaTime 
			//+ penality (assume that z = v * dt + 0.5 * (F/m-g) * dt2, F = ((z - v * t) / 0.5 / dt2 + g) * m)
			
			DATA m = this->mass / 4;
			//DATA penalityForce = ((-pinfo[2] - pinfo[5] * deltaTime) * 2 / (deltaTime * deltaTime) + 9.8) * m * 10 ;	// coeffient
			
			sf.intensity[0] = m * -pinfo[3] / deltaTime;
			sf.intensity[1] = m * -pinfo[4] / deltaTime;
			sf.intensity[2] = m * -pinfo[5] / deltaTime;

			//add into list
			fl->push_back(sf);

			//show debug
			printf("Force: %d - %32.20f , %32.20f ,%32.20f \n", pindex, sf.intensity[0], sf.intensity[1], sf.intensity[2]);
		}		
	}
}

//-- end temporal