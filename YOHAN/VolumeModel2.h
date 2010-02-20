#pragma once

#include "Base.h"


using namespace std;

using namespace yohan;
using namespace base;

/* A(3*4) * B(4*4) = RES(3*4) */
void calcul_M34_M44_Mul(DATA** A, DATA** B, DATA** RES);

/* A(3*4) * B(4*1) = RES(3*1) */
void calcul_M34_V4_Mul(DATA** A, DATA* B, DATA* RES);

/* res = A(3*1) .* B(3*1) */
DATA calcul_V3_V3_Dot(DATA* A, DATA* B);

/* res = sum (sum (A[i][j] * A[i][j]) ) */
DATA calcul_Jacobin_M33(DATA** A);

/* A(3*3) + B(3*3) = RES(3*3) */
void calcul_M33_M33_Add(DATA** A, DATA** B, DATA** RES);

/* A(3*3) - B(3*3) = RES(3*3) */
void calcul_M33_M33_Minus(DATA** A, DATA** B, DATA** RES);

/* A(3*3) *= b */
void calcul_M33_const_Mul(DATA** A, DATA b);

struct IndexTetraPoint
{
	int tetIndex;
	int indexOfPoint;
};

struct Point
{
	DATA originalPosition[3];
	DATA currentPosition[3];
	DATA velority[3];
	DATA acceleration[3];

	DATA mass;
	DATA force[3];

	bool isVisible;
	std::list<IndexTetraPoint>* tetIndexList;

};

struct Tetra
{
	int index[4];

	DATA beta[4][4];

	DATA stress[3][3];

	DATA internalForce[4][3];

	DATA vol;
};

struct Face
{
	int index[3];
};

struct Material
{
	DATA lambda;
	DATA mu;
	DATA alpha;
	DATA beta;
	DATA k1;
	DATA k2;
	DATA density;
};

class VolumeModel2
{
	int modelID;

	//tet pool
	std::vector<struct Tetra> *tetPool;
	int oldTetCount;
	char oldTetFileName[256];

	//face pool
	std::vector<struct Face> *facePool;
	int oldFaceCount;
	char oldFaceFileName[256];

	// point pool
	std::vector<struct Point> *pointPool;

	// material
	struct Material material;

	// gravity
	DATA gravity[3];

	// delta Time
	DATA deltaTime;

public:
	VolumeModel2(const char* nodeFile, const char* faceFile, const char* eleFile, DATA speed[], const DATA materialPropriety[]);

	void updateGravity(DATA gravity[]);

	void setDeltaTime(DATA dt);

	void fillForceVector(int pointIndex, DATA force[]);

	void calculatePosition();

	void calculateInternalForce();

	void output(FILE* sceneFile, char* objectFileDir, int frameID);

	// tempral
	void fillForceList(std::list<SceneForce> *fl);


private:
	void calcul_Tet_Beta(struct Tetra *tet);

	void calcul_M33_const_Mul(DATA (*A)[3], DATA b);

	void calcul_M33_M33_Minus(DATA (*A)[3], DATA (*B)[3], DATA (*RES)[3]);

	void calcul_M33_M33_Add(DATA (*A)[3], DATA (*B)[3], DATA (*RES)[3]);

	DATA calcul_Jacobin_M33(DATA (*A)[3]);

	DATA calcul_V3_V3_Dot(DATA* A, DATA* B);

	void calcul_M34_M44_Mul(DATA (*A)[4], DATA (*B)[4], DATA (*RES)[4]);

	void calcul_M34_V4_Mul(DATA (*A)[4], DATA* B, DATA* RES);


};