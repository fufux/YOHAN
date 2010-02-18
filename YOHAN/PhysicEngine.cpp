#include "stdafx.h"

#include "Base.h"

using namespace yohan;
using namespace base;


int _main(int argc, _TCHAR* argv[])
{
	char* sceneConfigFile = "../YOHAN/test/sceneexample2.xml";//argv[0]
	char* sceneRecordFile = "../YOHAN/test/output";//argv[1]

	//load the scene configuration file
	SceneController* sc = new SceneController(sceneConfigFile);

	//start to simulate, and try to record all the process into the given file
	sc->simulate(sceneRecordFile);

	return 0;
}