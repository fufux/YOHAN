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

	//FILE* fp = fopen(faceFile, "r");
	ifstream fp (faceFile, ios::in);
	//if (fp == NULL)
	if (!fp || !fp.good())
	{
		printf("Can not open face file: %s", faceFile);
		fetalError();
	}

	// get number
	//fscanf(fp, "%d%d", &nf, &tmp);
	fp >> nf >> tmp;

	// the face list
	for (int i = 0; i < nf && fp.good(); i++)
	{
		int* findex = new int[3];
		//fscanf(fp, "%d%d%d%d%d", &tmp, findex, findex + 1, findex + 2, &tmp);
		fp >> tmp >> findex[0] >> findex[1] >> findex[2] >> tmp;

		faceList.push_back(findex);
	}

	// close file
	//fclose(fp);
	fp.close();
	fp.clear();
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
		/*char tmp[16];
		strcpy(oldOutputFileName, dir);
		strcat(oldOutputFileName, "/");
		strcat(oldOutputFileName, _itoa(modelID, tmp, 10));
		strcat(oldOutputFileName, ".bface");*/
		std::stringstream sstream;
		sstream << dir << "/" << modelID << ".bface";
		strcpy_s(oldOutputFileName, 256, sstream.str().c_str());

		// output
		//FILE* fp = fopen(oldOutputFileName, "a+");
		ofstream fp (oldOutputFileName, ios::out | ios::binary);

		//fwrite(&oldFaceCount, sizeof(int), 1, fp);
		fp.write ((char*)&oldFaceCount, sizeof(int));

		for (std::vector<int*>::iterator iter = faceList.begin(); iter != faceList.end(); ++iter)
		{
			int* faceIndex = *iter;
			//fwrite((void*)faceIndex, sizeof(int), 3, fp);
			fp.write ((char*)faceIndex, sizeof(int) * 3);
		}
		//fflush(fp);
		//fclose(fp);

		// return the file name
		return oldOutputFileName;
	}
}