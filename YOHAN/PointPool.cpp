#include "stdafx.h"

#include "Base.h"

#include <cstdio>

using namespace std;

using namespace yohan;
using namespace base;

/*************/
/* PointPool */
/*************/

PointPool::PointPool(const char *nodeFile, const char* faceFile, DATA speed[])
{
	int n;		//number of nodes
	int nf;		//number of faces
	int tmp;	//for those values not important

	// allocation
	pointList = std::vector<DATA*>();
	tetrahedronIndexList = std::vector<std::list<int*>*>();
	visiblePointList = std::vector<bool>();

	/* file .node */

	//FILE *fp = fopen(nodeFile, "r");
	ifstream fp (nodeFile, ios::in);
	//if (fp == NULL)
	if (!fp || !fp.good())
	{
		printf("Can not open node file: %s", nodeFile);
		fetalError();
	}

	// get number
	//fscanf(fp, "%d%d%d%d", &n, &tmp, &tmp, &tmp);
	fp >> n >> tmp >> tmp >> tmp;

	// the point list
	for (int i = 0; i < n; i++)
	{
		DATA* pointData = new DATA[PointPool::POINT_ARGU_NUM];

		// current position
		//fscanf(fp, "%d%lf%lf%lf", &tmp, pointData, pointData + 1, pointData + 2);
		fp >> tmp >> pointData[0] >> pointData[1] >> pointData[2];	// Y est vertical

		// temporal, only for test
		
		//pointData[0] += 1;
		//pointData[1] += 1;
		//pointData[2] += 1;
		

		// velority
		pointData[3] = speed[0];
		pointData[4] = speed[1];
		pointData[5] = speed[2];

		//orginal position
		pointData[6] = pointData[0];
		pointData[7] = pointData[1];
		pointData[8] = pointData[2];

		//acceleration
		pointData[9] = 0;
		pointData[10] = 0;
		pointData[11] = 0;

		//stress
		pointData[12] = 0;
		pointData[13] = 0;
		pointData[14] = 0;
		pointData[15] = 0;
		pointData[16] = 0;
		pointData[17] = 0;

		pointList.push_back(pointData);
		visiblePointList.push_back(false);
		tetrahedronIndexList.push_back(new std::list<int*>());
	}

	// close file
	//fclose(fp);
	fp.close();
	fp.clear();


	/* file .face */

	//fp = fopen(faceFile, "r");
	ifstream fp2 (faceFile, ios::in);
	//if (fp == NULL)
	if (!fp2 || !fp2.good())
	{
		printf("Can not open face file: %s", faceFile);
		fetalError();
	}

	// get number
	//fscanf(fp, "%d%d", &nf, &tmp);
	fp2 >> nf >> tmp;

	// the point visible list
	for (int i = 0; i < nf; i++)
	{
		int pa, pb, pc;
		//fscanf(fp, "%d%d%d%d%d", &tmp, &pa, &pb, &pc, &tmp);
		fp2 >> tmp >> pa >> pb >> pc >> tmp;

		// should - 1 : index
		visiblePointList[pa - 1] = true;
		visiblePointList[pb - 1] = true;
		visiblePointList[pc - 1] = true;
	}

	// close file
	//fclose(fp);
	fp2.close();
	fp2.clear();

}

void PointPool::updateTetrahedraIndex(int pointIndex, int tetIndex, int indexOfPointIndex)
{
	int* element = new int[2];
	element[0] = tetIndex;
	element[1] = indexOfPointIndex;

	tetrahedronIndexList[pointIndex]->push_back(element);
}

int PointPool::getPointCount()
{
	return this->pointList.size();
}

DATA* PointPool::getPointInfo(int index)
{
	return this->pointList[index];
}

std::list<int*>* PointPool::getPointTetIndexList(int index)
{
	return this->tetrahedronIndexList[index];
}

void PointPool::fillVector(DATA* V, DATA* XU)
{
	int i = 0;
	for (std::vector<DATA*>::iterator iter = pointList.begin(); iter != pointList.end(); ++iter)
	{
		DATA* pointData = *iter;

		// velority
		V[i] = pointData[3];
		V[i + 1] = pointData[4];
		V[i + 2] = pointData[5];

		// deplacement
		XU[i] = pointData[0] - pointData[6];
		XU[i + 1] = pointData[1] - pointData[7];
		XU[i + 2] = pointData[2] - pointData[8];

		i += 3;
	}
}

void PointPool::fillVector2(DATA* X, DATA* V, DATA* A)
{
	int i = 0;
	for (std::vector<DATA*>::iterator iter = pointList.begin(); iter != pointList.end(); ++iter)
	{
		DATA* pointData = *iter;

		// position
		X[i] = pointData[0];
		X[i + 1] = pointData[1];
		X[i + 2] = pointData[2];

		// velority
		V[i] = pointData[3];
		V[i + 1] = pointData[4];
		V[i + 2] = pointData[5];

		// acceleration
		A[i] = pointData[9];
		A[i + 1] = pointData[10];
		A[i + 2] = pointData[11];

		i += 3;
	}
}

void PointPool::feedBackVector(DATA* V, DATA deltaTime)
{
	int i = 0;
	for (std::vector<DATA*>::iterator iter = pointList.begin(); iter != pointList.end(); ++iter)
	{
		DATA* pointData = *iter;

		// acceleration, only for debug
		pointData[9] = (V[i] - pointData[3]) / deltaTime;
		pointData[10] = (V[i + 1] - pointData[4]) / deltaTime;
		pointData[11] = (V[i + 2] - pointData[5]) / deltaTime;

		// velority
		pointData[3] = V[i];
		pointData[4] = V[i + 1];
		pointData[5] = V[i + 2];

		// deplacement
		pointData[0] += pointData[3] * deltaTime;
		pointData[1] += pointData[4] * deltaTime;
		pointData[2] += pointData[5] * deltaTime;

		i += 3;
	}
}

void PointPool::feedBackVector2(DATA* X, DATA a0, DATA a2, DATA a3, DATA a6, DATA a7)
{
	int i = 0;
	for (std::vector<DATA*>::iterator iter = pointList.begin(); iter != pointList.end(); ++iter)
	{
		DATA* pointData = *iter;

		// save old acceleration
		DATA oldAcc[3];
		oldAcc[0] = pointData[9];
		oldAcc[1] = pointData[10];
		oldAcc[2] = pointData[11];

		// acceleration
		pointData[9] = a0 * (X[i] - pointData[0]) - a2 * pointData[3] - a3 * oldAcc[0];
		pointData[10] = a0 * (X[i + 1] - pointData[1]) - a2 * pointData[4] - a3 * oldAcc[1];
		pointData[11] = a0 * (X[i + 2] - pointData[2]) - a2 * pointData[5] - a3 * oldAcc[2];
		
		// velority
		pointData[3] += a6 * oldAcc[0] + a7 * pointData[9];
		pointData[4] += a6 * oldAcc[1] + a7 * pointData[10];
		pointData[5] += a6 * oldAcc[2] + a7 * pointData[11];		

		//position
		pointData[0] = X[i];
		pointData[1] = X[i + 1];
		pointData[2] = X[i + 2];

		i += 3;
	}
}

void PointPool::showInfo(int round)
{
	/*
	int i = 0;
	for (std::vector<DATA*>::iterator iter = pointList.begin(); iter != pointList.end(); ++iter)
	{
		DATA* pointData = *iter;
		printf("%d - %32.26lf , %32.26lf , %32.26lf : %32.26lf , %32.26lf , %32.26lf \n", i, pointData[0], pointData[1], pointData[2], pointData[3], pointData[4], pointData[5]);
		++i;
	}
	*/

	/*
	DATA* pointData = pointList[0];
	printf("%d - %32.26lf , %32.26lf , %32.26lf : %32.26lf , %32.26lf , %32.26lf \n", 0, pointData[0], pointData[1], pointData[2], pointData[3], pointData[4], pointData[5]);
	*/

	/*
	// append .xml
	FILE* fpXML = fopen("SceneTest.xml", "a+");
	fprintf(fpXML, "<frame id=\"%d\" nodefile=\"test-%d.node\" elefile=\"test.ele\" facefile=\"test.face\" timestamp=\"0.0\"/>\n", round, round);
	fclose(fpXML);

	// update .node
	char nodeFileName[64];
	sprintf(nodeFileName, "node\\test-%d.node", round);
	
	FILE* fpNode = fopen(nodeFileName, "a+");
	fprintf(fpNode, "%d\t3\t0\t0\n", pointList.size());	//head

	int i = 1;
	for (std::vector<DATA*>::iterator iter = pointList.begin(); iter != pointList.end(); ++iter)
	{
		DATA* pointData = *iter;
		fprintf(fpNode, "%d\t%32.26lf\t%32.26lf\t%32.26lf\n", i, pointData[0], pointData[1], pointData[2]);
		++i;
	}

	fclose(fpNode);
	*/


}

void PointPool::resolveConflit()
{
	for (std::vector<DATA*>::iterator iter = pointList.begin(); iter != pointList.end(); ++iter)
	{
		DATA* pointData = *iter;

		//if y < 0, y = 0, v(y) = 0
		if (pointData[1] < 0)
		{
			pointData[1] = 0;
			pointData[4] = 0;
		}
	}
}

char* PointPool::output(char* dir, int modelID)
{
	// .bnode
	//char tmp[16];
	std::stringstream sstream;
	sstream << dir << "/" << modelID << ".bnode";
	strcpy_s(nodeFileName, 256, sstream.str().c_str());
	/*strcpy_s(nodeFileName, 256, dir);
	strcat_s(nodeFileName, 256, "/");
	strcat_s(nodeFileName, 256, _itoa_s(modelID, tmp, 10));
	strcat_s(nodeFileName, 256, ".bnode");*/

	// output
	//FILE* fp = fopen(nodeFileName, "a+");
	ofstream fp (nodeFileName, ios::out | ios::binary);


	int size = pointList.size();
	fp.write ((char*)&size, sizeof(int));
	//fwrite(&size, sizeof(int), 1, fp);

	for (std::vector<DATA*>::iterator iter = pointList.begin(); iter != pointList.end(); ++iter)
	{
		DATA* pointData = *iter;
		//fwrite(pointData, sizeof(DATA), 3, fp);
		fp.write ((char*)pointData, sizeof(DATA) * 3);
	}
	fp.flush();
	fp.close();
	fp.clear();
	//fflush(fp);
	//fclose(fp);

	// return the file name
	return nodeFileName;
	
}

void PointPool::fracture(yohan::base::DATA limit)
{
	
}

int PointPool::getNbPoint()
{
	return this->pointList.size();
}

void PointPool::fillStress(DATA* F)
{
	int i = 0;
	for (std::vector<DATA*>::iterator iter = pointList.begin(); iter != pointList.end(); ++iter)
	{
		DATA* pointData = *iter;

		F[i] += pointData[12];
		F[i + 1] += pointData[13];
		F[i + 2] += pointData[14];

		i += 3;
	}
}