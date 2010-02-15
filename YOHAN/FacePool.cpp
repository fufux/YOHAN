#include "stdafx.h"

#include "Base.h"

#include <iostream>

using namespace std;

using namespace yohan;
using namespace base;

FacePool::FacePool(const char *faceFile)
{
	faceList = std::vector<int*>();

	int n;		//number of face
	int tmp;	//for those values not important

	/* file .face */

	FILE* fp = fopen(faceFile, "r");
	if (fp == NULL)
	{
		printf("Can not open face file: %s", faceFile);
		fetalError();
	}

	// get number
	fscanf(fp, "%d%d", &nf, &tmp);

	// the face list
	for (int i = 0; i < nf; i++)
	{
		int* findex = new int[3];
		fscanf(fp, "%d%d%d%d%d", &tmp, findex, findex + 1, findex + 2, &tmp);

		faceList.push_back(findex);
	}

	// close file
	fclose(fp);
}