// YOHAN.cpp définit le point d'entrée pour l'application console.

#include "stdafx.h"
#include "Editor.h"
#include "Player.h"

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

extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
...
...
*/
IrrlichtDevice* device;
IVideoDriver* driver;
ISceneManager* smgr;
IGUIEnvironment* env;
scene::ICameraSceneNode* camera[CAMERA_COUNT];

/*
This is the main method.
*/
int __main(int argc, _TCHAR* argv[])
{
	// initialize random number generator
	srand((unsigned int)time(NULL)); 

	device = createDevice( video::EDT_DIRECT3D9, dimension2d<u32>(800, 600), 32,
			false, false, false, 0);

	if (!device)
		return 1;

	device->setWindowCaption(L"YOHAN");

	/*
	Get a pointer to the VideoDriver, the SceneManager and the graphical
	user interface environment.
	*/
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	env = device->getGUIEnvironment();

	// Set GUI to have a low transparency
	for (s32 i=0; i < irr::gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(255);
		env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
	}

	/*
	Create cameras.
	*/
	camera[0] = smgr->addCameraSceneNodeMaya();
	camera[0]->setFarValue(20000.f);
	// Maya cameras reposition themselves relative to their target, so target the location
	// where the mesh scene node is placed.
	camera[0]->setTarget(core::vector3df(0,30,0));

	camera[1] = smgr->addCameraSceneNodeFPS();
	camera[1]->setFarValue(20000.f);
	camera[1]->setPosition(core::vector3df(0,0,-70));
	camera[1]->setTarget(core::vector3df(0,30,0));


	/*
	Allow us to load files directly frome this folder without giving the explicit path
	*/
	device->getFileSystem()->addFileArchive("../YOHAN/irrlicht/media/", true, true, EFAT_FOLDER);

	/*
	Create the editor, GUI, etc.
	*/
	Editor* editor = new Editor();
	Player* player = new Player();
	editor->setPlayer( player );
	player->setEditor( editor );

	// by default, we start the editor
	editor->start();

	/*
	Ok, now we have set up the scene, lets draw everything: We run the
	device in a while() loop, until the device does not want to run any
	more. This would be when the user closes the window or presses ALT+F4
	(or whatever keycode closes a window).
	*/
	while(device->run())
	{
		driver->beginScene(true, true, SColor(255,100,101,140));

		player->run(); // this will return imediately if player is not currently runing

		smgr->drawAll();
		env->drawAll();

		driver->endScene();
	}

	editor->clear();
	player->clear();
	device->drop();

	return 0;
}

