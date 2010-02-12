#include "stdafx.h"

#include "Base.h"

using namespace yohan;
using namespace base;


int main(int argc, _TCHAR* argv[])
{
	char* sceneConfigFile = "F:/doc/ENSEEIHT/3annee/projet_long/model/test/sceneexample3.xml";//argv[0]
	char* sceneRecordFile = "titi";//argv[1]

	//load the scene configuration file
	SceneController* sc = new SceneController(sceneConfigFile);

	//start to simulate, and try to record all the process into the given file
	sc->simulate(sceneRecordFile);

	return 0;
}