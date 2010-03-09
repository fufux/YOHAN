#include "irrlicht/XEffects/Source/XEffects.h"
#include "Player.h"
#include "PlayerFrame.h"
#include "Editor.h"


extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
extern EffectHandler* effect;
extern scene::ICameraSceneNode* camera[CAMERA_COUNT];



Player::Player(scene::ITerrainSceneNode* terrain, core::array<scene::ISceneNode*> skydomes)
{
	this->debugData = scene::EDS_MESH_WIRE_OVERLAY;
	this->er = new PlayerEventReceiver(this);
	this->currFrame = NULL;
	this->sceneFile = "";
	this->is_playing = false;
	this->currentFrame = -1;
	this->is_running = false;
	this->editor = NULL;
	this->name = "untitled";
	this->baseDir = device->getFileSystem()->getWorkingDirectory();
	this->frames_size = 0;
	this->improve_rendering = false;
	this->defaultObjectTexture = driver->getTexture("irrlicht/media/wall.bmp");
	this->noneObjectTexture = driver->getTexture("irrlicht/media/red.bmp");
	this->terrain = terrain;
	this->skydomes = skydomes;
}

Player::~Player(void)
{
}

void Player::start()
{
	this->clear();
	this->createGUI();
	device->setEventReceiver(this->er);
	this->is_running = true;
	effect->setAmbientColor(SColor(255, 32, 32, 32));
}

void Player::stop()
{
	this->is_running = false;
	this->clear();
}

void Player::switchToEditor()
{
	if (editor != NULL)
	{
		this->stop();
		editor->start();
		// reset the working directory
		device->getFileSystem()->changeWorkingDirectoryTo( baseDir.c_str() );
		editor->load("tmp.xml");
	}
}

bool Player::isRunning()
{
	return is_running;
}

void Player::clear(bool clear_gui)
{
	/*for (u32 i=0; i < frames_size; i++)
		delete frames[i];
	this->frames.clear();*/
	improveRendering(false);
	framesFileNames.clear();
	if (currFrame) delete currFrame;
	this->currFrame = NULL;
	this->sceneFile = "";
	this->currentFrame = -1;
	this->is_playing = false;
	this->playSpeed = 100;
	this->accumulatedDeltaT = 0;
	this->frames_size = 0;
	this->improve_rendering = false;

	// clear GUI
	if (clear_gui)
	{
		device->clearSystemMessages();
		device->setEventReceiver(NULL);
		device->clearSystemMessages();
		core::list<IGUIElement*> children = env->getRootGUIElement()->getChildren();
		core::list<IGUIElement*>::Iterator it;
		for (it = children.begin(); it != children.end(); ++it)
		{
			IGUIElement* e = (IGUIElement*)(*it);
			if (e->getID() > GUI_ID_ref && e->getID() < GUI_ID_ref + 1000)
				e->remove();
		}
	}
}


void Player::setEditor(Editor* editor)
{
	this->editor = editor;
}


void Player::displayNextFrame()
{
	this->is_playing = false;
	
	s32 nextId = currentFrame+1;
	if (nextId < 0)
		nextId = (s32)framesFileNames.size() - 1;
	if (nextId >= (s32)framesFileNames.size())
		nextId = 0;

	displayFrameById(nextId);
}


void Player::displayPreviousFrame()
{
	this->is_playing = false;

	s32 nextId = currentFrame-1;
	if (nextId < 0)
		nextId = (s32)framesFileNames.size() - 1;
	if (nextId >= (s32)framesFileNames.size())
		nextId = 0;

	displayFrameById(nextId);
}


void Player::displayFrameById(s32 id, bool volumic)
{
	if (framesFileNames.size() == 0 || id < 0 || id >= (s32)framesFileNames.size())
	{
		updateFrameNumber();
		return;
	}

	if (currFrame && currFrame != NULL)
		delete currFrame;

	currFrame = new PlayerFrame(this, framesFileNames[id], volumic);
	if (currFrame->getNodes().size() == 0)
	{
		delete currFrame;
		currentFrame = -1;
		currFrame = NULL;
		return;
	}
	currentFrame = currFrame->getID();
	if (volumic)
		accumulatedDeltaT = currFrame->getTimestamp();
	currFrame->display();
	setDebugDataVisible( this->debugData, volumic );
	updateFrameNumber();
}



void Player::playNextFrame(double deltaT)
{
	
	if (deltaT == 0)// || frames_size == 0)
		return;

	// deltaT is in milliseconds
	accumulatedDeltaT += deltaT / 1000.0;

	s32 old_currentFrame = currentFrame;
	double min = 100000000;

	if (old_currentFrame >= 0 && old_currentFrame < (s32)frames_size)
	{
		for (u32 i=old_currentFrame; i < frames_size && i < (u32)old_currentFrame + 1000; i++)
		{
			if (abs(framesFileNames[i].timestamp - accumulatedDeltaT) < min)
			{
				min = abs(framesFileNames[i].timestamp - accumulatedDeltaT);
				currentFrame = i;
			}
		}
		for (u32 i=0; i < frames_size && i < 1000; i++)
		{
			if (abs(framesFileNames[i].timestamp - accumulatedDeltaT) < min)
			{
				min = abs(framesFileNames[i].timestamp - accumulatedDeltaT);
				currentFrame = i;
			}
		}
	}
	else
	{
		currentFrame = 0;
	}

	// display the new frame
	/*if (old_currentFrame >= 0 && old_currentFrame < (s32)frames_size)
		frames[old_currentFrame]->hide();
	if (currentFrame >= 0 && currentFrame < (s32)frames_size)
		frames[currentFrame]->display();*/

	displayFrameById(currentFrame, false);

	// force looping
	if (currentFrame == frames_size-1)
		accumulatedDeltaT = 0;

	setDebugDataVisible( this->debugData );
	updateFrameNumber();
}


void Player::updateFrameNumber()
{
	// update Frame number in the edit box
	if (env->getRootGUIElement()->getElementFromId(GUI_ID_PLAYER_FRAME_NUMBER, true))
	{
		IGUIEditBox* box = (IGUIEditBox*)(env->getRootGUIElement()->getElementFromId(GUI_ID_PLAYER_FRAME_NUMBER, true));
		box->setText( stringw(this->currentFrame).c_str() );
	}

	// update play/pause button
	if (env->getRootGUIElement()->getElementFromId(GUI_ID_PLAYER_PLAY, true))
	{
		if (is_playing)
		{
			IGUIButton* b = (IGUIButton*)env->getRootGUIElement()->getElementFromId(GUI_ID_PLAYER_PLAY, true);
			b->setImage(er->image_pause);
			b->setToolTipText(L"Pause");
		}
		else
		{
			IGUIButton* b = (IGUIButton*)env->getRootGUIElement()->getElementFromId(GUI_ID_PLAYER_PLAY, true);
			b->setImage(er->image_play);
			b->setToolTipText(L"Play");
		}
	}
}


// load the video xml file and stor its info in framesFileNames
bool Player::load(irr::core::stringc filename)
{
	// reset the working directory
	device->getFileSystem()->changeWorkingDirectoryTo( baseDir.c_str() );

	IReadFile* file = device->getFileSystem()->createAndOpenFile( filename );
	if (!file)
	{
		device->getLogger()->log("Could not open the file.");
		return false;
	}
	IXMLReader* xml = device->getFileSystem()->createXMLReader( file );
	if (!xml)
		return false;

	// parse the file until end reached
	/*
	<video name="video01">
		<frame nodefile="mesh.node" elefile="mesh.ele" id="3" />
	</video>
	*/
	bool firstLoop = true;
	bool is_valid_file = false;

	while(xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				// the element should be "scene"
				if (firstLoop)
				{
					firstLoop = false;
					if (stringw("video") != xml->getNodeName())
					{
						env->addMessageBox(
							CAPTION_ERROR, L"This is not a valid video file !");
						return false;
					}
					else
					{
						is_valid_file = true;
						// clean current scene
						this->clear();
						this->createGUI();
						device->setEventReceiver(this->er);
						// get number of frames and reallocate arrays
						u32 nbf = xml->getAttributeValueAsInt(L"frames");
						//frames.reallocate( nbf );
						framesFileNames.reallocate( nbf );
					}
				}

				if (stringw("frame") == xml->getNodeName())
				{
					FrameInfo fi;
					fi.id = xml->getAttributeValueAsInt(L"id");
					fi.timestamp = xml->getAttributeValueAsFloat(L"timestamp");
					framesFileNames.push_back( fi );
				}
				else if (stringw("object") == xml->getNodeName())
				{
					framesFileNames.getLast().nodefiles.push_back( xml->getAttributeValueSafe(L"nodefile") );
					framesFileNames.getLast().facefiles.push_back( xml->getAttributeValueSafe(L"facefile") );
					framesFileNames.getLast().elefiles.push_back( xml->getAttributeValueSafe(L"elefile") );
					framesFileNames.getLast().bbfiles.push_back( xml->getAttributeValueSafe(L"bbfile") );
				}
			}
		default:
			break;
		}
	}

	xml->drop();

	if (!is_valid_file)
	{
		env->addMessageBox(CAPTION_ERROR, L"This is not a valid video file !");
	}
	else
	{
		for (u32 i=0; i < framesFileNames.size()-1; i++)
			framesFileNames[i].bbfiles = framesFileNames[i+1].bbfiles;
		frames_size = framesFileNames.size();
	}

	return is_valid_file;
}


// this calls load and then pre-loads all frames, but only with surfacic meshes
/*bool Player::loadAll()
{
	u16 size = framesFileNames.size();
	for (u16 i=0; i < size; i++)
	{
		frames.push_back(new PlayerFrame(this, framesFileNames[i], false));
		if (frames.getLast()->getNodes().size() == 0)
		{
			delete frames.getLast();
			frames.erase(frames.size()-1);
		}
	}

	frames_size = frames.size();

	return true;
}*/


void Player::createGUI()
{
	gui::IGUIElement* root = env->getRootGUIElement();

	// create menu
	gui::IGUIContextMenu* menu;
	if (root->getElementFromId(GUI_ID_MENU, true))
		((gui::IGUIContextMenu*)root->getElementFromId(GUI_ID_MENU, true))->remove();
		//menu = (gui::IGUIContextMenu*)root->getElementFromId(GUI_ID_MENU, true);

	menu = env->addMenu(0, GUI_ID_MENU);
	
	menu->addItem(L"File", -1, true, true);
	menu->addItem(L"View", -1, true, true);
	menu->addItem(L"Camera", -1, true, true);
	menu->addItem(L"Help", -1, true, true);

	gui::IGUIContextMenu* submenu;
	submenu = menu->getSubMenu(0);
	submenu->addItem(L"Open animation...", GUI_ID_PLAYER_OPEN_VIDEO);
	submenu->addSeparator();
	submenu->addItem(L"Switch to editor", GUI_ID_SWITCH_TO_EDITOR);
	submenu->addItem(L"Quit", GUI_ID_PLAYER_QUIT);

	submenu = menu->getSubMenu(1);
	submenu->addItem(L"toggle model debug information", GUI_ID_PLAYER_TOGGLE_DEBUG_INFO, true, true);
	submenu->addItem(L"Don't Improve Rendering", GUI_ID_PLAYER_IMPROVE_RENDERING_NONE);
	submenu->addItem(L"Improve Rendering 1", GUI_ID_PLAYER_IMPROVE_RENDERING_1);
	submenu->addItem(L"Improve Rendering 2", GUI_ID_PLAYER_IMPROVE_RENDERING_2);

	submenu = submenu->getSubMenu(0);
	submenu->addItem(L"Off", GUI_ID_PLAYER_DEBUG_OFF, true, false, (isDebugDataVisible() == scene::EDS_OFF));
	submenu->addItem(L"Bounding Box", GUI_ID_PLAYER_DEBUG_BOUNDING_BOX, true, false, (isDebugDataVisible() == scene::EDS_BBOX));
	submenu->addItem(L"Normals", GUI_ID_PLAYER_DEBUG_NORMALS, true, false, (isDebugDataVisible() == scene::EDS_NORMALS));
	submenu->addItem(L"Wire overlay", GUI_ID_PLAYER_DEBUG_WIRE_OVERLAY, true, false, (isDebugDataVisible() == scene::EDS_MESH_WIRE_OVERLAY));
	submenu->addItem(L"Half-Transparent", GUI_ID_PLAYER_DEBUG_HALF_TRANSPARENT, true, false, (isDebugDataVisible() == scene::EDS_HALF_TRANSPARENCY));
	submenu->addItem(L"Buffers bounding boxes", GUI_ID_PLAYER_DEBUG_BUFFERS_BOUNDING_BOXES, true, false, (isDebugDataVisible() == scene::EDS_BBOX_BUFFERS));
	submenu->addItem(L"All", GUI_ID_PLAYER_DEBUG_ALL, true, false, (isDebugDataVisible() == scene::EDS_FULL));

	submenu = menu->getSubMenu(2);
	submenu->addItem(L"Maya Style", GUI_ID_PLAYER_CAMERA_MAYA);
	submenu->addItem(L"First Person", GUI_ID_PLAYER_CAMERA_FIRST_PERSON);

	submenu = menu->getSubMenu(3);
	submenu->addItem(L"About", GUI_ID_PLAYER_ABOUT);

	/*
	Below the menu we want a toolbar, onto which we can place colored
	buttons and important looking stuff like a senseless combobox.
	*/

	// create toolbar

	gui::IGUIToolBar* bar;
	if (root->getElementFromId(GUI_ID_TOOLBAR, true))
		((gui::IGUIToolBar*)root->getElementFromId(GUI_ID_TOOLBAR, true))->remove();
	
	bar = env->addToolBar(0, GUI_ID_TOOLBAR);

	video::ITexture* image = driver->getTexture("open.png");
	bar->addButton(GUI_ID_PLAYER_OPEN_VIDEO_BUTTON, 0, L"Open animation", image, 0, false, true);

	image = driver->getTexture("previous.png");
	bar->addButton(GUI_ID_PLAYER_PREVIOUS_FRAME, 0, L"Previous Frame", image, 0, false, true);

	image = driver->getTexture("next.png");
	bar->addButton(GUI_ID_PLAYER_NEXT_FRAME, 0, L"Next Frame", image, 0, false, true);

	image = er->image_play;
	bar->addButton(GUI_ID_PLAYER_PLAY, 0, L"Play", image, 0, false, true);

	image = driver->getTexture("help.png");
	bar->addButton(GUI_ID_PLAYER_HELP_BUTTON, 0, L"Open Help", image, 0, false, true);

	env->addEditBox(stringw( -1 ).c_str(), core::rect<s32>(200,4,300,24), true, bar, GUI_ID_PLAYER_FRAME_NUMBER);
	image = driver->getTexture("goto.png");
	IGUIButton* b = bar->addButton(GUI_ID_PLAYER_FRAME_NUMBER_BUTTON, 0, L"Go to this frame", image, 0, false, true);
	b->setRelativePosition( core::rect<s32>(304,4,324,24) );

	// add speed control %
	env->addStaticText(L"Speed Control (100%):",
			core::rect<s32>(350,8,440,24), false, false, bar, GUI_ID_PLAYER_SPEED_TEXT);
	IGUIScrollBar* scrollbar = env->addScrollBar(true,
			core::rect<s32>(445,6,775,22), bar, GUI_ID_PLAYER_SPEED_SCROLLBAR);
	scrollbar->setMax(200);
	scrollbar->setPos(100);
	scrollbar->setLargeStep(5);
	scrollbar->setSmallStep(1);
}


void Player::setDebugDataVisible(scene::E_DEBUG_SCENE_TYPE state, bool f)
{
	/*if (currentFrame >= 0 && currentFrame < (s32)frames_size)
	{
		for (u16 j=0; j < frames[currentFrame]->getNodes().size(); j++)
			frames[currentFrame]->getNodes()[j]->setDebugDataVisible(state);
	}*/
	if (currFrame && (!improve_rendering || f))
	{
		for (u16 j=0; j < currFrame->getNodes().size(); j++)
			currFrame->getNodes()[j]->setDebugDataVisible(state);
		this->debugData = state;
	}
	else if (currFrame)
	{
		for (u16 j=0; j < currFrame->getNodes().size(); j++)
			currFrame->getNodes()[j]->setDebugDataVisible(scene::EDS_OFF);
	}
}

s32 Player::isDebugDataVisible()
{
	return debugData;
}

void Player::improveRendering(bool enable, s32 type)
{
	if (enable)
	{
		effect->removeShadowFromNode(terrain);
		effect->addShadowToNode(terrain, EFT_8PCF, ESM_RECEIVE);
		if (type == 0)
		{
			skydomes[0]->setVisible(true);
			skydomes[1]->setVisible(false);
			effect->getShadowLight(0).setPosition(vector3df(0, 500.0f, -50.0f));
			effect->setAmbientColor(SColor(255, 132, 132, 132));
		}
		else if (type == 1)
		{
			skydomes[0]->setVisible(false);
			skydomes[1]->setVisible(true);
			effect->getShadowLight(0).setPosition(vector3df(0, 80.0f, -200.0f));
			effect->setAmbientColor(SColor(255, 32, 32, 32));
		}
	}
	else
	{
		if (currFrame)
		{
			for (u32 i=0; i < currFrame->getNodes().size(); i++)
				effect->removeShadowFromNode(currFrame->getNodes()[i]);
		}
		effect->removeShadowFromNode(terrain);
		setDebugDataVisible(this->debugData);
		for (u32 i=0; i < skydomes.size(); i++)
			skydomes[i]->setVisible(false);
	}
	this->improve_rendering = enable;
	terrain->setMaterialFlag(video::EMF_WIREFRAME, !enable);
}

bool Player::isImproveRendering()
{
	return improve_rendering;
}
video::ITexture* Player::getDefaultObjectTexture()
{
	return defaultObjectTexture;
}
video::ITexture* Player::getNoneObjectTexture()
{
	return noneObjectTexture;
}


void Player::play()
{
	if (framesFileNames.size() == 0)
		return;

	if (currFrame && currFrame != NULL)
	{
		delete currFrame;
		currFrame = NULL;
	}

	/*if (frames_size == 0)
		loadAll();*/

	is_playing = true;
	lastTime = device->getTimer()->getTime();
}

void Player::pause()
{
	is_playing = false;
}

void Player::run()
{
	if (!this->is_running)
		return;

	if (framesFileNames.size() == 0)
		return;

	if (is_playing)
	{
		// we always display 30 images per second. The speed will interact with the frames timestamp
		double fps = 1000.0/30;

		if (device->getTimer()->getTime() - lastTime > fps)
		{
			lastTime = device->getTimer()->getTime();
			this->playNextFrame(fps * ((double)this->playSpeed) / 100);
			if (currFrame && currFrame->getNodes().size() > 0 &&
				currFrame->getNodes()[0]->getMesh()->getMeshBufferCount() > 0 &&
				currFrame->getNodes()[0]->getMesh()->getMeshBuffer(0)->getVertexCount() > 0)
			{
				S3DVertex *v = (S3DVertex*)currFrame->getNodes()[0]->getMesh()->getMeshBuffer(0)->getVertices();
				effect->getShadowLight(0).setTarget(v[0].Pos);
			}
		}
	}
}
