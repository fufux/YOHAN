// YOHAN.cpp définit le point d'entrée pour l'application console.

#include "stdafx.h"
#include "irrlicht/XEffects/Source/XEffects.h"
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
EffectHandler* effect;
scene::ICameraSceneNode* camera[CAMERA_COUNT];

/*
This is the main method.
*/
int main(int argc, _TCHAR* argv[])
{
	FILE *stdoutToStdStream = freopen( "cout.txt", "w", stdout );

	// initialize random number generator
	srand((unsigned int)time(NULL)); 

	device = createDevice( video::EDT_OPENGL, dimension2d<u32>(800, 600), 32, false, true);
	
	if (!device)
		return 1;

	device->setResizable(true);
	device->setWindowCaption(L"YOHAN");

	/*
	Get a pointer to the VideoDriver, the SceneManager and the graphical
	user interface environment.
	*/
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	env = device->getGUIEnvironment();
	smgr->setAmbientLight(SColorf(0.2f,0.2f,0.2f));

	//driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

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
	Add XEffects
	*/
	smgr->setShadowColor(video::SColor(150,0,0,0));
	effect = new EffectHandler(device, driver->getScreenSize(), true, true, true);
	effect->setAmbientColor(SColor(255, 32, 32, 32));
	effect->setClearColour(SColor(255,100,101,140));

	// Add some post processing effects, a very subtle bloom here.
	/*const stringc shaderExt = (driver->getDriverType() == EDT_DIRECT3D9) ? ".hlsl" : ".glsl";

	effect->addPostProcessingEffectFromFile(core::stringc("irrlicht/XEffects/Bin/shaders/BrightPass") + shaderExt);
	effect->addPostProcessingEffectFromFile(core::stringc("irrlicht/XEffects/Bin/shaders/BlurHP") + shaderExt);
	effect->addPostProcessingEffectFromFile(core::stringc("irrlicht/XEffects/Bin/shaders/BlurVP") + shaderExt);
	effect->addPostProcessingEffectFromFile(core::stringc("irrlicht/XEffects/Bin/shaders/BloomP") + shaderExt);*/

	effect->addShadowLight(SShadowLight(2048, vector3df(0, 40.0f, -100.0f), vector3df(0, 0, 0), 
		SColor(0, 255, 255, 175), 80.0f, 4000.0f, 70.0f * DEGTORAD));


	/*
	Allow us to load files directly frome this folder without giving the explicit path
	*/
	device->getFileSystem()->addFileArchive("../YOHAN/irrlicht/media/", true, true, EFAT_FOLDER);

	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
		"terrain-heightmap.bmp",
		0,
		-1,
		vector3df(-128*60.0f, 0.0f, -128*60.0f),
		vector3df(0,0,0),
		vector3df(60.f, 1.0f, 60.f),
		SColor(255,255,255,255),
		5);
	terrain->setMaterialFlag(video::EMF_WIREFRAME, true);
	terrain->setMaterialFlag(video::EMF_LIGHTING, false);
	terrain->setMaterialTexture(0,
			driver->getTexture("detailmap2.jpg"));
	terrain->scaleTexture(200.0f);

	// create skydomes
	core::array<scene::ISceneNode*> skydomes;
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	scene::ISceneNode* skydome = smgr->addSkyDomeSceneNode(driver->getTexture("skydome1.jpg"),16,8,1.0f,2.0f);
	skydome->setVisible(false);
	skydomes.push_back(skydome);
	skydome = smgr->addSkyDomeSceneNode(driver->getTexture("skydome2.jpg"),16,8,1.0f,2.0f);
	skydome->setVisible(false);
	skydomes.push_back(skydome);
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	/*
	Create the editor, GUI, etc.
	*/
	Editor* editor = new Editor();
	Player* player = new Player(terrain, skydomes);
	editor->setPlayer( player );
	player->setEditor( editor );

	// by default, we start the editor
	editor->start();
	editor->load("tmp.xml");

	/*
	Ok, now we have set up the scene, lets draw everything: We run the
	device in a while() loop, until the device does not want to run any
	more. This would be when the user closes the window or presses ALT+F4
	(or whatever keycode closes a window).
	*/
	effect->setActiveSceneManager(smgr);
	video::SMaterial m2d = driver->getMaterial2D();
	core::dimension2du screenSize = driver->getScreenSize();

  	while(device->run())
	{
		if (device->isWindowActive())
		{
			/////////////////////////////////////
			driver->beginScene(true, true, SColor(255,100,101,140));

			player->run(); // this will return imediately if player is not currently runing

			//smgr->drawAll();
			effect->update();
			
			driver->setMaterial(m2d);
			env->drawAll();

			driver->endScene();
			/////////////////////////////////////


			if (driver->getScreenSize() != screenSize)
			{
				screenSize = driver->getScreenSize();
				effect->setScreenRenderTargetResolution(driver->getScreenSize());
			}

			stringw caption = L"";
			if (editor->isRunning())
			{
				caption += L"Editor - [";
				caption += editor->getName();
				caption += L"]";
			}
			else if (player->isRunning())
			{
				caption += L"Player";
			}
			device->setWindowCaption( caption.c_str() );

		}
		else
			device->yield();

	}

	device->getFileSystem()->changeWorkingDirectoryTo( editor->getBaseDir().c_str() );
	if (editor->isRunning())
		editor->save("tmp.xml");

	editor->clear();
	player->clear();
	device->drop();

	return 0;
}