#include "stdafx.h"

#include "Base.h"

#include <iostream>

using namespace std;

using namespace yohan;
using namespace base;

SceneRecorder::SceneRecorder(char *filename)
{
	strcpy(dir, filename);

	//create the directory if not exist
	createDir(dir);

	//name of the scene file
	strcpy(sceneFileName, dir);
	strcat(sceneFileName, "/scene.xml");

	indexOfFrame = 0;

	// open scene file
	this->sceneFile = fopen(sceneFileName, "w");
	// write the head of video
	fprintf(sceneFile, "<video name=\"test\">\n");
	// close
	fclose(sceneFile);
}

void SceneRecorder::newFrame(const yohan::base::DATA timeStamp)
{
	char tmp[16];

	//decide if create a new sub directory to avoid penalize the performance of file system
	int subDirIndex = indexOfFrame / 256;
	strcpy(frameFileName, dir);
	strcat(frameFileName, "/sub");
	strcat(frameFileName, _itoa(subDirIndex, tmp, 10));
	createDir(frameFileName);

	// dir of the frame
	strcat(frameFileName, "/frame-");
	strcat(frameFileName, _itoa(indexOfFrame, tmp, 10));
	createDir(frameFileName);

	// open scene file
	this->sceneFile = fopen(sceneFileName, "a+");
	// the head tag of a frame
	fprintf(sceneFile, "\t<frame id=\"%d\" timestamp=\"%.4lf\">\n", this->indexOfFrame, timeStamp);
	// do not close until the endFrame()

}

void SceneRecorder::endFrame()
{
	fprintf(sceneFile, "\t</frame>\n");
	fclose(sceneFile);

	indexOfFrame++;
}

void SceneRecorder::record(VolumeModel* model)
{
	model->output(sceneFile, frameFileName, indexOfFrame);
}

void SceneRecorder::endScene()
{
	this->sceneFile = fopen(sceneFileName, "a+");
	// write the end of video
	fprintf(sceneFile, "</video>");
	// close
	fclose(sceneFile);
}