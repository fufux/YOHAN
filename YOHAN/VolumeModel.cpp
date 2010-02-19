#include "stdafx.h"

#include "Base.h"
#include "Matrix.h"

#include <cstdio>

using namespace std;

using namespace yohan;
using namespace base;
using namespace matrix;

int VolumeModel::ID_COUNT = 0;

VolumeModel::VolumeModel(const char* nodeFile, const char* faceFile, const char* eleFile, DATA speed[], const DATA materialPropriety[])
{
	// id
	id = VolumeModel::ID_COUNT;
	VolumeModel::ID_COUNT++;

	// defensive copy

	/*
			0 - Lambda
			1 - u
			2 - alpha
			3 - beta
			4 - density
	*/
	for (int i = 0; i < VolumeModel::VM_MET_NUM; i++)
		this->materialPropriety[i] = materialPropriety[i];

	/* 
			0 - A1 = u / (1 - u)
			1 - A2 = (1 - 2u) / (2 - 2u)
			2 - A3 = E * (1 - u) / ((1 + u)(1 - 2u))
			3 - A3 / 6
			4 - density
			5 - density / 60
			6 - density / 120
			7 - alpha	(C = alpha * K + beta * M)
			8 - beta
	*/

	// E = lamda * (1 + u) * (1 - 2u) / u
	DATA E = materialPropriety[0] * (1 + materialPropriety[1]) * (1 - 2 * materialPropriety[1]) / materialPropriety[1];
	constants[0] = materialPropriety[1] / (1 - materialPropriety[1]);
	constants[1] = (1 - 2 * materialPropriety[1]) / (2 - 2 * materialPropriety[1]);
	constants[2] = E * (1 - materialPropriety[1]) / ((1 + materialPropriety[1]) * (1 - 2 * materialPropriety[1]));
	constants[3] = constants[2] / 6;
	constants[4] = materialPropriety[4];
	constants[5] = constants[4] / 60;
	constants[6] = constants[4] / 120;
	constants[7] = materialPropriety[2];
	constants[8] = materialPropriety[3];


	this->pointPool = new PointPool(nodeFile, faceFile, speed);
	this->facePool = new FacePool(faceFile);
	this->tetPool = new TetrahedronPool(eleFile, this->constants, this->pointPool, this);

	// the matrix
	int order = this->pointPool->getPointCount() * 3;
	this->K = new SymmetricMumpsSquareSparseMatrix2(order);
	this->M = new SymmetricMumpsSquareSparseMatrix2(order);
	this->C = new SymmetricMumpsSquareSparseMatrix2(order);

	// the vector
	this->vecSize = order;
	this->F = new DATA[vecSize];
	this->X = new DATA[vecSize];
	this->V = new DATA[vecSize];
	this->XU = new DATA[vecSize];

	this->A = new DATA[vecSize];
	this->TMP = new DATA[vecSize];

	memset((void*)this->F, 0, vecSize * sizeof(DATA));
	memset((void*)this->X, 0, vecSize * sizeof(DATA));
	memset((void*)this->V, 0, vecSize * sizeof(DATA));
	memset((void*)this->XU, 0, vecSize * sizeof(DATA));

	memset((void*)this->A, 0, vecSize * sizeof(DATA));
	memset((void*)this->TMP, 0, vecSize * sizeof(DATA));
}

void VolumeModel::fillMatrix(DATA gravity[])
{
	this->tetPool->fillMatrix(K, M, C, F, gravity);

	/* for debug */
	K->show("d:\\qq\\k.txt");
	M->show("d:\\qq\\m.txt");
	C->show("d:\\qq\\c.txt");
}

void VolumeModel::setDeltaTime(DATA t)
{
	this->deltaTime = t;
}

void VolumeModel::fillForceVector(int pointIndex, DATA force[])
{
	this->F[pointIndex * 3] += force[0];
	this->F[pointIndex * 3 + 1] += force[1];
	this->F[pointIndex * 3 + 2] += force[2];
}

void VolumeModel::fillVector()
{
	// make sure that size is correspondant with the quantity of point
	int newOrder = pointPool->getPointCount() * 3;
	if (vecSize < newOrder)
	{
		//delete
		delete[] F;
		delete[] X;
		delete[] V;
		delete[] XU;

		//rellocate
		vecSize = newOrder;
		F = new DATA[vecSize];
		X = new DATA[vecSize];
		V = new DATA[vecSize];
		XU = new DATA[vecSize];
	}

	// clear
	memset((void*)this->F, 0, vecSize * sizeof(DATA));
	memset((void*)this->X, 0, vecSize * sizeof(DATA));
	memset((void*)this->V, 0, vecSize * sizeof(DATA));
	memset((void*)this->XU, 0, vecSize * sizeof(DATA));

	pointPool->fillVector(V, XU);
}

void VolumeModel::fillVector2()
{
	// make sure that size is correspondant with the quantity of point
	int newOrder = pointPool->getPointCount() * 3;
	if (vecSize < newOrder)
	{
		//delete
		delete[] F;
		delete[] X;
		delete[] V;
		delete[] XU;

		delete[] A;
		delete[] TMP;

		//rellocate
		vecSize = newOrder;
		F = new DATA[vecSize];
		X = new DATA[vecSize];
		V = new DATA[vecSize];
		XU = new DATA[vecSize];

		A = new DATA[vecSize];
		TMP = new DATA[vecSize];
	}

	// clear
	memset((void*)this->F, 0, vecSize * sizeof(DATA));
	memset((void*)this->X, 0, vecSize * sizeof(DATA));
	memset((void*)this->V, 0, vecSize * sizeof(DATA));
	memset((void*)this->XU, 0, vecSize * sizeof(DATA));

	memset((void*)this->A, 0, vecSize * sizeof(DATA));
	memset((void*)this->TMP, 0, vecSize * sizeof(DATA));

	pointPool->fillVector2(X, V, A);
}

void VolumeModel::calculate()
{
	// delta_t * C = C_
	C->calcul_AX(deltaTime);

	// C_ + M = C__
	C->calcul_plusAX(M, 1);

	// C__ + delta_t * delta_t * K = C___
	C->calcul_plusAX(K, deltaTime * deltaTime);

	// F_ = deltaTime * F
	calcul_vec_AX(F, vecSize, deltaTime);

	// F__ = F_ + M * V
	M->calcul_PlusMatrixVec(V, F);

	// F___ = F__ - deltaTime * K * XU
	K->calcul_MinusMatrixVec(XU, F, deltaTime);

	// resolve C___ ? = F___,
	solveLinearEquation(C, F);

}

void VolumeModel::calculate2()
{
	// considering the stress
	//pointPool->fillStress(F);

	// constant
	DATA gama = 0.6, beta = (0.5 + gama) * (0.5 + gama) / 4;

	/* */
	DATA a0 = 1 / (beta * deltaTime * deltaTime);
	DATA a1 = gama / (beta * deltaTime);
	DATA a2 = 1 / (beta * deltaTime);
	DATA a3 = 1 / (2 * beta) - 1;
	DATA a4 = gama / beta - 1;
	DATA a5 = deltaTime / 2 * (gama / beta - 2);
	DATA a6 = deltaTime * (1 - gama);
	DATA a7 = gama * deltaTime;

	// New K = K + a0 * M + a1 * C
	K->calcul_plusAX(M, a0);
	K->calcul_plusAX(C, a1);

	// New f = f + M * (a0 * X + a2 * V + a3 * A) + C(a1 * X + a4 * V + a5 * A)
	memcpy((void*)TMP, (void*)X, sizeof(DATA) * vecSize);
	for (int i = 0; i < vecSize; i++)
		TMP[i] = TMP[i] * a0 + V[i] * a2 + A[i] * a3;

	M->calcul_PlusMatrixVec(TMP, F);

	memcpy((void*)TMP, (void*)X, sizeof(DATA) * vecSize);
	for (int i = 0; i < vecSize; i++)
		TMP[i] = TMP[i] * a1 + V[i] * a4 + A[i] * a5;

	C->calcul_PlusMatrixVec(TMP, F);

	// resolve
	solveLinearEquation(K, F);

}

void VolumeModel::feedBackVector()
{
	pointPool->feedBackVector(F, deltaTime);
}

void VolumeModel::feedBackVector2()
{
	// constant
	DATA gama = 0.6, beta = (0.5 + gama) * (0.5 + gama) / 4;

	/* */
	DATA a0 = 1 / (beta * deltaTime * deltaTime);
	DATA a1 = gama / (beta * deltaTime);
	DATA a2 = 1 / (beta * deltaTime);
	DATA a3 = 1 / (2 * beta) - 1;
	DATA a4 = gama / beta - 1;
	DATA a5 = deltaTime / 2 * (gama / beta - 2);
	DATA a6 = deltaTime * (1 - gama);
	DATA a7 = gama * deltaTime;

	pointPool->feedBackVector2(F, a0, a2, a3, a6, a7);
}

void VolumeModel::generateFrame(int round)
{
	this->pointPool->showInfo(round);	
}

DATA* VolumeModel::getPoint(int index)
{
	return this->pointPool->getPointInfo(index);
}

Tetrahedron* VolumeModel::getTetrahedron(int index)
{
	return this->tetPool->getTetrahedron(index);
}

void VolumeModel::output(FILE* sceneFile, char* objectFileDir, int frameID)
{
	char* nodeFileName, *faceFileName, *eleFileName;

	// generate the .bnode, .bface and .bele
	nodeFileName = pointPool->output(objectFileDir, id);
	faceFileName = facePool->output(objectFileDir, id);
	eleFileName = tetPool->output(objectFileDir, id);

	// save in the scene file
	fprintf(sceneFile, "\t\t<object nodefile=\"%s\" facefile=\"%s\" elefile=\"%s\" />\n", nodeFileName, faceFileName, eleFileName);
}

void VolumeModel::fracture()
{
	FILE* fp = fopen("d:\\qq\\f.txt","a+");

	int n = pointPool->getPointCount();
	for (int i = 0; i < n; i++)
	{
		DATA* pointData = pointPool->getPointInfo(i);
		std::list<int*>* pointTetIndexList = pointPool->getPointTetIndexList(i);

		DATA sum[6];
		memset((void*)sum, 0, sizeof(DATA) * 6);

		for (std::list<int*>::iterator iter = pointTetIndexList->begin(); iter != pointTetIndexList->end(); ++iter)
		{
			int* indexs = *iter;
			/* 0 - tetIndex, 1 - indexOfPointIndex */

			DATA res[6];
			tetPool->getTetrahedron(indexs[0])->fillStressVector(indexs[1], pointData, res);

			// sum
			sum[0] += res[0];sum[1] += res[1];
			sum[2] += res[2];sum[3] += res[3];
			sum[4] += res[4];sum[5] += res[5];

		}

		// conserve
		pointData[12] = sum[0];
		pointData[13] = sum[1];
		pointData[14] = sum[2];
		pointData[15] = sum[3];
		pointData[16] = sum[4];
		pointData[17] = sum[5];

		//output
		fprintf(fp, "%d -\t%.6lf\t%.6lf\t%.6lf\t%.6lf\t%.6lf\t%.6lf\n", i, sum[0], sum[1], sum[2], sum[3], sum[4], sum[5]);		
	}

	fprintf(fp, "\n==========================================\n");
	fflush(fp);
	fclose(fp);
}

// temporal
void VolumeModel::fillForceList(std::list<SceneForce> *fl)
{
	this->tetPool->fillForceList(fl, deltaTime);
}

void VolumeModel::resolveConflit()
{
	this->pointPool->resolveConflit();
}

int VolumeModel::getPointPoolSize()
{
	return this->pointPool->getNbPoint();
}