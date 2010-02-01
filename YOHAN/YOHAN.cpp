// YOHAN.cpp définit le point d'entrée pour l'application console.

#include "stdafx.h"
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

/*
To be able to use the Irrlicht.DLL file, we need to link with the Irrlicht.lib.
We could set this option in the project settings, but to make it easy, we use a
pragma comment lib for VisualStudio. On Windows platforms, we have to get rid
of the console window, which pops up when starting a program with main(). This
is done by the second pragma. We could also use the WinMain method, though
losing platform independence then.
*/
#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif


/*
We need a few global variables which will be usefull in many classes and functions.
These variables are declared here.
If you want to use these variables in another .cpp file, you must re-decalre them and
add "extern" key word at the begining of each declaration. See below.

here:
IrrlichtDevice *Device;
in another .cpp file:
extern IrrlichtDevice *Device;
*/
IrrlichtDevice* device;
IVideoDriver* driver;
ISceneManager* smgr;
IGUIEnvironment* env;
scene::ICameraSceneNode* camera[1]; /* One camera should be enought, but who knows... */

/*
This is the main method.
*/
int main(int argc, _TCHAR* argv[])
{

	device = createDevice( video::EDT_DIRECT3D9, dimension2d<u32>(640, 480), 32,
			false, false, false, 0);

	if (!device)
		return 1;

	device->setWindowCaption(L"YOHAN");

	/*
	Get a pointer to the VideoDriver, the SceneManager and the graphical
	user interface environment.
	*/
	driver = device->getVideoDriver();
	mgr = device->getSceneManager();
	env = device->getGUIEnvironment();

	/*
	Create the main camera.
	*/
	camera[0] = smgr->addCameraSceneNode();

	/*
	Ok, now we have set up the scene, lets draw everything: We run the
	device in a while() loop, until the device does not want to run any
	more. This would be when the user closes the window or presses ALT+F4
	(or whatever keycode closes a window).
	*/
	while(device->run())
	{
		driver->beginScene(true, true, SColor(255,100,101,140));

		smgr->drawAll();
		env->drawAll();

		driver->endScene();
	}

	device->drop();

	return 0;
}

