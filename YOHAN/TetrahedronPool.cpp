#include "stdafx.h"

#include "Base.h"

#include <cstdio>

using namespace std;

using namespace yohan;
using namespace base;

/********************/
/* TetrahedronPool */
/*******************/

TetrahedronPool::TetrahedronPool(const char *eleFile, DATA constants[], PointPool* pointPoolRef, VolumeModel* volumeModelRef)
{
	this->oldTetCount = 0;
	tetList = std::vector<Tetrahedron*>();
	this->constants = constants;

	int n;		//number of tetrahedron
	int tmp;	//for those values not important

	/* file .ele */

	FILE *fp = fopen(eleFile, "r");
	if (fp == NULL)
	{
		printf("Can not open ele file: %s", eleFile);
		fetalError();
	}

	// get number
	fscanf(fp, "%d%d%d", &n, &tmp, &tmp);

	// the tet list
	for (int i = 0; i < n; i++)
	{
		int id, p[4];
		fscanf(fp, "%d%d%d%d%d", &id, p, p + 1, p + 2, p + 3);

		// coherent index
		id--;
		p[0]--;p[1]--;p[2]--;p[3]--;

		// Tetrahedron instance
		Tetrahedron* tp = new Tetrahedron(id, p, constants, pointPoolRef, volumeModelRef);
		tetList.push_back(tp);
	}

	// close file
	fclose(fp);
}

void TetrahedronPool::fillMatrix(SquareSparseMatrix* K, SquareSparseMatrix* M, SquareSparseMatrix* C, DATA* F, DATA gravity[])
{
	// clear for reuse
	K->clear();
	M->clear();
	C->clear();

	for (std::vector<Tetrahedron*>::iterator iter = tetList.begin(); iter != tetList.end(); ++iter)
	{
		Tetrahedron* tet = *iter;
		tet->fillMatrix(K, M, F, gravity);		
	}

	/*
	constants[7] - alpha
	constants[8] - beta
	*/
	C->calcul_AXplusBY(this->constants[7], M, this->constants[8], K);
}

Tetrahedron* TetrahedronPool::getTetrahedron(int index)
{
	return this->tetList[index];
}

char* TetrahedronPool::output(char* dir, int modelID)
{
	if (this->tetList.size() == this->oldTetCount)
	{
		// in the case that do not need to regenerate
		return this->oldOutputFileName;
	}
	else
	{
		// update
		this->oldTetCount = this->tetList.size();

		// .bele
		char tmp[16];
		strcpy(oldOutputFileName, dir);
		strcat(oldOutputFileName, "/");
		strcat(oldOutputFileName, _itoa(modelID, tmp, 10));
		strcat(oldOutputFileName, ".bele");

		// output
		FILE* fp = fopen(oldOutputFileName, "a+");

		fwrite(&oldTetCount, sizeof(int), 1, fp);

		for (std::vector<Tetrahedron*>::iterator iter = tetList.begin(); iter != tetList.end(); ++iter)
		{
			Tetrahedron* tet = *iter;
			fwrite(tet->getPointIndex(), sizeof(int), 4, fp);
		}
		fflush(fp);
		fclose(fp);

		// return the file name
		return oldOutputFileName;
	}
}

//temporal
void TetrahedronPool::fillForceList(std::list<SceneForce> *fl, DATA deltaTime)
{
	for (std::vector<Tetrahedron*>::iterator iter = tetList.begin(); iter != tetList.end(); ++iter)
	{
		Tetrahedron* tet = *iter;
		tet->fillForceList(fl, deltaTime);
	}
}

//-- end temporal
