#include "stdafx.h"

#include "Base.h"

#include <iostream>
#include <dir.h>

using namespace std;

using namespace yohan;
using namespace base;

SceneRecorder::SceneRecorder(const char *filename)
{
	strcpy(dir, filename);

	//create the directory if not exist

	//name of the scene file
	strcpy(sceneFileName, dir);
	strcat(sceneFileName, "/scene.xml");

	//dir of the frame files
	strcpy(frameFileName, dir);

	indexOfFrame = 0;

}

void SceneRecorder::newFrame(const yohan::base::DATA timeStamp)
{
	this->indexInFrame = 0;

	//decide if create a new sub directory to avoid penalize the performance of file system
	int subDirIndex = indexOfFrame / 256;
	strcat(frameFileName, "/sub");
	strcat(frameFileName, itoa(subDirIndex));

	// name of the frame
	strcat(frameFileName, "/frame");
	strcat(frameFileName, itoa(indexOfFrame));

}