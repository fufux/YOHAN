#pragma once

#include "Base.h"

#include "VolumeModel2.h"

using namespace std;
using namespace yohan;
using namespace base;


class SceneRecorder2
{
	char dir[256];

	char sceneFileName[256];

	char frameFileName[256];

	int indexOfFrame;

	/**/
	FILE* sceneFile;

public:
	/* To open/create a file for the recording */
	SceneRecorder2(char* filename);

	/* to indicate a new frame start, with its absolute time in the simulated world */
	void newFrame(const DATA timeStamp);

	/* */
	void endFrame();

	/* record all the information necessary of the given model (which stands for an object) in the current frame*/
	void record(VolumeModel2* model);

	/* end and flush all on the disk, add an index of shifting timing (for slow replay) */
	void endScene();
};