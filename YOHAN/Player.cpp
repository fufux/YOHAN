#include "Player.h"
#include "Editor.h"


extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
extern scene::ICameraSceneNode* camera[CAMERA_COUNT];



Player::Player(void)
{
	this->er = new PlayerEventReceiver(this);
	this->currFrame = NULL;
	this->sceneFile = "";
	this->is_playing = false;
	this->currentFrame = -1;
	this->is_running = false;
	this->editor = NULL;
}

Player::~Player(void)
{
}

void Player::start()
{
	device->setEventReceiver(this->er);
	this->clear();
	this->createGUI();
	this->is_running = true;
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
	}
}

bool Player::isRunning()
{
	return is_running;
}

void Player::clear()
{
	for (u16 i=0; i < frames.size(); i++)
		delete frames[i];
	this->frames.clear();
	framesFileNames.clear();
	if (currFrame) delete currFrame;
	this->currFrame = NULL;
	this->sceneFile = "";
	this->currentFrame = -1;
	this->is_playing = false;

	env->getRootGUIElement()->remove();
}


void Player::setEditor(Editor* editor)
{
	this->editor = editor;
}


void Player::displayNextFrame()
{
	if (framesFileNames.size() == 0)
		return;

	s32 nextId = 0;
	if (currFrame && currFrame != NULL)
	{
		nextId = currFrame->getId()+1;
		delete currFrame;
	}
	if (nextId >= (s32)framesFileNames.size())
		nextId = 0;

	currFrame = new PlayerFrame(framesFileNames[nextId].id, framesFileNames[nextId].nodefile, framesFileNames[nextId].elefile);
	if (!(currFrame->getNode()) || currFrame->getNode() == NULL)
	{
		delete currFrame;
		currentFrame = -1;
		currFrame = NULL;
		return;
	}
	currentFrame = currFrame->getId();
	currFrame->display();
	setDebugDataVisible( this->debugData );
	updateFrameNumber();
}


void Player::displayPreviousFrame()
{
	if (framesFileNames.size() == 0)
		return;

	s32 nextId = 0;
	if (currFrame && currFrame != NULL)
	{
		nextId = currFrame->getId()-1;
		delete currFrame;
	}
	if (nextId < 0)
		nextId = (s32)framesFileNames.size() - 1;

	currFrame = new PlayerFrame(framesFileNames[nextId].id, framesFileNames[nextId].nodefile, framesFileNames[nextId].elefile);
	if (!(currFrame->getNode()) || currFrame->getNode() == NULL)
	{
		delete currFrame;
		currentFrame = -1;
		currFrame = NULL;
		return;
	}
	currentFrame = currFrame->getId();
	currFrame->display();
	setDebugDataVisible( this->debugData );
	updateFrameNumber();
}


void Player::displayFrameById(s32 id)
{
	if (framesFileNames.size() == 0 || id < 0 || id >= (s32)framesFileNames.size())
	{
		updateFrameNumber();
		return;
	}

	if (currFrame && currFrame != NULL)
		delete currFrame;
	
	currFrame = new PlayerFrame(framesFileNames[id].id, framesFileNames[id].nodefile, framesFileNames[id].elefile);
	if (!(currFrame->getNode()) || currFrame->getNode() == NULL)
	{
		delete currFrame;
		currentFrame = -1;
		currFrame = NULL;
		return;
	}
	currentFrame = currFrame->getId();
	currFrame->display();
	setDebugDataVisible( this->debugData );
	updateFrameNumber();
}



void Player::playNextFrame()
{
	if (framesFileNames.size() == 0)
		return;

	++currentFrame;
	if (currentFrame == (s32)frames.size())
		currentFrame = 0;
	if (currentFrame >= 0 && currentFrame < (s32)frames.size())
	{
		if (currentFrame > 0)
			frames[currentFrame-1]->hide();
		frames[currentFrame]->display();
	}
	else
	{
		--currentFrame;
	}
	setDebugDataVisible( this->debugData );
	updateFrameNumber();
}


void Player::updateFrameNumber()
{
	if (env->getRootGUIElement()->getElementFromId(GUI_ID_PLAYER_FRAME_NUMBER, true))
	{
		IGUIEditBox* box = (IGUIEditBox*)(env->getRootGUIElement()->getElementFromId(GUI_ID_PLAYER_FRAME_NUMBER, true));
		box->setText( stringw(this->currentFrame).c_str() );
	}
}


// load the video xml file and stor its info in framesFileNames
bool Player::load(irr::core::stringc filename)
{
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
	int plop = 0;// TEMP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
					}
				}

				if (stringw("frame") == xml->getNodeName())
				{
					FrameInfo fi;
					fi.id = xml->getAttributeValueAsInt(L"id");
					fi.id = plop++; // TEMP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					fi.nodefile = xml->getAttributeValue(L"nodefile");
					fi.elefile = xml->getAttributeValue(L"elefile");
					framesFileNames.push_back( fi );
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

	return is_valid_file;
}


// this calls load and then pre-loads all frames ! Could be veeeeery slow !
bool Player::loadAll(irr::core::stringc filename)
{
	bool success = this->load(filename);

	for (u16 i=0; i < framesFileNames.size(); i++)
	{
		frames.push_back(new PlayerFrame(framesFileNames[i].id, framesFileNames[i].nodefile, framesFileNames[i].elefile));
		if (!(frames.getLast()->getNode()) || frames.getLast()->getNode() == NULL)
		{
			delete frames.getLast();
			frames.erase(frames.size()-1);
		}
	}

	return success;
}


void Player::createGUI()
{
	// create menu
	gui::IGUIContextMenu* menu = env->addMenu();
	menu->addItem(L"File", -1, true, true);
	menu->addItem(L"View", -1, true, true);
	menu->addItem(L"Camera", -1, true, true);
	menu->addItem(L"Help", -1, true, true);

	gui::IGUIContextMenu* submenu;
	submenu = menu->getSubMenu(0);
	submenu->addItem(L"Open video...", GUI_ID_PLAYER_OPEN_VIDEO);
	submenu->addSeparator();
	submenu->addItem(L"Switch to editor", GUI_ID_SWITCH_TO_EDITOR);
	submenu->addItem(L"Quit", GUI_ID_PLAYER_QUIT);

	submenu = menu->getSubMenu(1);
	submenu->addItem(L"toggle model debug information", GUI_ID_PLAYER_TOGGLE_DEBUG_INFO, true, true);

	submenu = submenu->getSubMenu(0);
	submenu->addItem(L"Off", GUI_ID_PLAYER_DEBUG_OFF);
	submenu->addItem(L"Bounding Box", GUI_ID_PLAYER_DEBUG_BOUNDING_BOX);
	submenu->addItem(L"Normals", GUI_ID_PLAYER_DEBUG_NORMALS);
	submenu->addItem(L"Skeleton", GUI_ID_PLAYER_DEBUG_SKELETON);
	submenu->addItem(L"Wire overlay", GUI_ID_PLAYER_DEBUG_WIRE_OVERLAY);
	submenu->addItem(L"Half-Transparent", GUI_ID_PLAYER_DEBUG_HALF_TRANSPARENT);
	submenu->addItem(L"Buffers bounding boxes", GUI_ID_PLAYER_DEBUG_BUFFERS_BOUNDING_BOXES);
	submenu->addItem(L"All", GUI_ID_PLAYER_DEBUG_ALL);

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

	gui::IGUIToolBar* bar = env->addToolBar();

	video::ITexture* image = driver->getTexture("open.png");
	bar->addButton(GUI_ID_PLAYER_OPEN_VIDEO_BUTTON, 0, L"Open a video", image, 0, false, true);

	image = driver->getTexture("previous.png");
	bar->addButton(GUI_ID_PLAYER_PREVIOUS_FRAME, 0, L"Previous Frame", image, 0, false, true);

	image = driver->getTexture("next.png");
	bar->addButton(GUI_ID_PLAYER_NEXT_FRAME, 0, L"Next Frame", image, 0, false, true);

	image = er->image_play;
	bar->addButton(GUI_ID_PLAYER_PLAY, 0, L"Play", image, 0, false, true);

	image = driver->getTexture("help.png");
	bar->addButton(GUI_ID_PLAYER_HELP_BUTTON, 0, L"Open Help", image, 0, false, true);

	env->addEditBox(L"-1", core::rect<s32>(300,4,400,24), true, bar, GUI_ID_PLAYER_FRAME_NUMBER);
	image = driver->getTexture("goto.png");
	IGUIButton* b = bar->addButton(GUI_ID_PLAYER_FRAME_NUMBER_BUTTON, 0, L"Go to this frame", image, 0, false, true);
	b->setRelativePosition( core::rect<s32>(404,4,424,24) );
}


void Player::setDebugDataVisible(scene::E_DEBUG_SCENE_TYPE state)
{
	for (u16 i=0; i < frames.size(); i++)
	{
		frames[i]->getNode()->setDebugDataVisible(state);
	}
	if (currFrame && currFrame->getNode())
		currFrame->getNode()->setDebugDataVisible(state);
	this->debugData = state;
}

s32 Player::isDebugDataVisible()
{
	return debugData;
}


void Player::play()
{
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
		if (device->getTimer()->getTime() - lastTime > 1000/24)
		{
			lastTime = device->getTimer()->getTime();
			this->playNextFrame();
		}
	}
}