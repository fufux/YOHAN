#include "stdafx.h"

#include "Base.h"

#include <iostream>

using namespace std;

using namespace yohan;
using namespace base;

FacePool::FacePool(const char *faceFile)
{
	this->oldFaceCount = 0;
	faceList = std::vector<int*>();

	int nf;		//number of face
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

char* FacePool::output(char* dir, int modelID)
{
	if (this->faceList.size() == this->oldFaceCount)
	{
		// in the case that do not need to regenerate
		return this->oldOutputFileName;
	}
	else
	{
		// update
		this->oldFaceCount = this->faceList.size();

		// .bface
		char tmp[16];
		strcpy(oldOutputFileName, dir);
		strcat(oldOutputFileName, "/");
		strcat(oldOutputFileName, _itoa(modelID, tmp, 10));
		strcat(oldOutputFileName, ".bface");

		// output
		FILE* fp = fopen(oldOutputFileName, "a+");

		fwrite(&oldFaceCount, sizeof(int), 1, fp);

		for (std::vector<int*>::iterator iter = faceList.begin(); iter != faceList.end(); ++iter)
		{
			int* faceIndex = *iter;
			fwrite((void*)faceIndex, sizeof(int), 3, fp);
		}
		fflush(fp);
		fclose(fp);

		// return the file name
		return oldOutputFileName;
	}
}