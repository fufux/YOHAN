#include "Editor.h"

extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
extern scene::ICameraSceneNode* camera[CAMERA_COUNT];


// lets import the tetBuf method
int tetBuf (IMeshBuffer *newBuffer, char *name);



// This is usefull when loading a scene from an xml file. It allows us to know what we are currently loading.
enum XmlNodeType
{
	UNDEFINED,
	SCENENODE,
	FORCEFIELD
};




Editor::Editor(void)
{
	this->er = new EditorEventReceiver(this);
	this->selectedNodeIndex = -1;
	this->selectedForceField = -1;
	this->debugData = scene::EDS_OFF;
	this->is_running = false;
	this->player = NULL;
	this->name = "untitled";
	this->baseDir = device->getFileSystem()->getWorkingDirectory();
}

Editor::~Editor(void)
{
}

void Editor::start()
{
	env->getRootGUIElement()->remove();
	this->createGUI();
	device->setEventReceiver(this->er);
	this->clear();
	this->is_running = true;
}


void Editor::stop()
{
	this->is_running = false;
	this->clear();
}

void Editor::switchToPlayer()
{
	if (player != NULL)
	{
		this->stop();
		player->start();
	}
}


void Editor::clear()
{
	for (u16 i=0; i < nodes.size(); i++)
		nodes[i]->remove();
	nodes.clear();
	meshFiles.clear();
	densities.clear();
	forceFields.clear();
	this->selectedNodeIndex = -1;
	this->selectedForceField = -1;
	this->debugData = scene::EDS_OFF;
}


void Editor::setPlayer(Player* player)
{
	this->player = player;
}


void Editor::selectNode()
{
	scene::ICameraSceneNode * activeCam = smgr->getActiveCamera();
	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();

	// All intersections are done with a ray cast out from the camera to a distance of 10000.
	core::line3d<f32> ray = collMan->getRayFromScreenCoordinates( device->getCursorControl()->getPosition(), activeCam );
	ray.end = ray.start + ray.getVector().normalize() * 10000.0f;

	// Tracks the current intersection point with the level or a mesh
	core::vector3df intersection;
	// Used to show which triangle has been hit
	core::triangle3df hitTriangle;

	// This call is all you need to perform ray/triangle collision on every scene node
	// that has a triangle selector.  It finds the nearest
	// collision point/triangle, and returns the scene node containing that point.
	scene::IMeshSceneNode * selectedSceneNode = (IMeshSceneNode*)
		collMan->getSceneNodeAndCollisionPointFromRay(
				ray,
				intersection,
				hitTriangle);

	if (selectedSceneNode)
	{
		if (selectedNodeIndex >= 0 && selectedNodeIndex < (s32)nodes.size())
			nodes[selectedNodeIndex]->setDebugDataVisible( debugData );

		this->selectedNodeIndex = this->nodes.binary_search( selectedSceneNode );
		selectedSceneNode->setDebugDataVisible( scene::EDS_MESH_WIRE_OVERLAY );

		this->createSceneNodeToolBox();
	}
	else
	{
		if (selectedNodeIndex >= 0 && selectedNodeIndex < (s32)nodes.size())
			nodes[selectedNodeIndex]->setDebugDataVisible( debugData );
		this->selectedNodeIndex = -1;

		this->removeSceneNodeToolBox();
	}

}


void Editor::setDebugDataVisible(scene::E_DEBUG_SCENE_TYPE state)
{
	for (u16 i=0; i < nodes.size(); i++)
	{
		nodes[i]->setDebugDataVisible(state);
	}
	this->debugData = state;
}

s32 Editor::isDebugDataVisible()
{
	return debugData;
}


void Editor::setPositionRotationScaleOfSelectedNode()
{
	if (selectedNodeIndex >= 0 && selectedNodeIndex < (s32)nodes.size())
	{
		gui::IGUIElement* root = env->getRootGUIElement();
		core::vector3df pos, rot, sca;
		f32 density;
		core::stringc s;

		s = root->getElementFromId(GUI_ID_TOOL_BOX_X_POSITION, true)->getText();
		pos.X = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_POSITION, true)->getText();
		pos.Y = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_POSITION, true)->getText();
		pos.Z = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_X_ROTATION, true)->getText();
		rot.X = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_ROTATION, true)->getText();
		rot.Y = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_ROTATION, true)->getText();
		rot.Z = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_X_SCALE, true)->getText();
		sca.X = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_SCALE, true)->getText();
		sca.Y = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_SCALE, true)->getText();
		sca.Z = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_DENSITY, true)->getText();
		density = (f32)atof(s.c_str());

		nodes[selectedNodeIndex]->setPosition( pos );
		nodes[selectedNodeIndex]->setRotation( rot );
		nodes[selectedNodeIndex]->setScale( sca );
		densities[selectedNodeIndex] = density;
	}
}

void Editor::moveSelectedNode(irr::core::vector3df vec)
{
	if (selectedNodeIndex >= 0 && selectedNodeIndex < (s32)nodes.size())
	{
		vector3df pos = nodes[selectedNodeIndex]->getPosition() + vec;
		//setPositionOfSelectedNode( pos );
	}
}



void Editor::setForceField()
{
	if (forceFields.size() > 0)
	{
		gui::IGUIElement* root = env->getRootGUIElement();
		core::vector3df val;
		core::stringc s;

		s = root->getElementFromId(GUI_ID_FORCE_FIELD_TOOL_BOX_X_INTENSITY, true)->getText();
		val.X = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_FORCE_FIELD_TOOL_BOX_Y_INTENSITY, true)->getText();
		val.Y = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_FORCE_FIELD_TOOL_BOX_Z_INTENSITY, true)->getText();
		val.Z = (f32)atof(s.c_str());

		forceFields[0] = val;

		removeForceFieldToolBox();
	}
}

core::array<IMeshSceneNode*> Editor::getAllSceneNodes()
{
	return nodes;
}

vector3df Editor::getForceField()
{
	if (forceFields.size() > 0)
		return forceFields[0];
	else
		return vector3df(0,0,0);
}


void Editor::add3DModel(stringc filename)
{
	IMesh* m = smgr->getMesh( filename.c_str() );
	if (!m)
	{
		// model could not be loaded
		env->addMessageBox(
			CAPTION_ERROR, L"The model could not be loaded. " \
			L"Maybe it is not a supported file format.");
		return;
	}

	IMeshSceneNode* node = smgr->addMeshSceneNode( m );
	if (!node)
		return;
	
	// set default material properties
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->setDebugDataVisible(scene::EDS_OFF);

	// Now create a triangle selector for it.
	scene::ITriangleSelector* selector = smgr->createTriangleSelector(node->getMesh(), node);
	node->setTriangleSelector(selector);
	selector->drop(); // We're done with this selector, so drop it now.


	// select the newly loaded scene node
	nodes.push_back( node );
	meshFiles.push_back( filename.c_str() );
	densities.push_back( 50.0f );

	// unselect current selected node if exists
	if (selectedNodeIndex >= 0 && selectedNodeIndex < (s32)nodes.size())
			nodes[selectedNodeIndex]->setDebugDataVisible( debugData );

	// select the node we've just loaded
	selectedNodeIndex = nodes.size() - 1;
	this->createSceneNodeToolBox();
	node->setDebugDataVisible( scene::EDS_MESH_WIRE_OVERLAY );
}


void Editor::addForceField()
{
	forceFields.push_back( vector3df(0,0,0) );
}


void Editor::remove3DModel()
{
	if (selectedNodeIndex >= 0 && selectedNodeIndex < (s32)nodes.size())
	{
		nodes[selectedNodeIndex]->remove();
		nodes.erase(selectedNodeIndex);
		meshFiles.erase(selectedNodeIndex);
		densities.erase(selectedNodeIndex);
		selectedNodeIndex = -1;
	}
}


void Editor::removeForceField()
{
	if (selectedForceField >= 0 && selectedForceField < (s32)forceFields.size())
	{
		forceFields.erase(selectedForceField);
	}
}



void Editor::askForFileName()
{
	// create the toolbox window
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(300,200,500,350),
		true, L"File name", 0, GUI_ID_ASK_FILENAME_WINDOW);

	env->addStaticText(L"File name:", core::rect<s32>(20,20,40,40), false, false, wnd);
	env->addEditBox(L"myscene.xml", core::rect<s32>(42,20,200,36), true, wnd, GUI_ID_ASK_FILENAME_NAME);

	env->addButton(core::rect<s32>(60,50,100,70), wnd, GUI_ID_ASK_FILENAME_OK_BUTTON, L"Ok");
	env->addButton(core::rect<s32>(120,50,160,70), wnd, GUI_ID_ASK_FILENAME_CANCEL_BUTTON, L"Cancel");
}


void Editor::createGUI()
{
	// create menu
	gui::IGUIContextMenu* menu = env->addMenu();
	menu->addItem(L"File", -1, true, true);
	menu->addItem(L"View", -1, true, true);
	menu->addItem(L"Camera", -1, true, true);
	menu->addItem(L"Help", -1, true, true);

	gui::IGUIContextMenu* submenu;
	submenu = menu->getSubMenu(0);
	submenu->addItem(L"Open scene...", GUI_ID_OPEN_SCENE);
	submenu->addItem(L"Save scene...", GUI_ID_SAVE_SCENE);
	submenu->addItem(L"Tetrahedralize scene...", GUI_ID_TETRAHEDRALIZE_SCENE);
	submenu->addSeparator();
	submenu->addItem(L"Open Model File...", GUI_ID_OPEN_MODEL);
	submenu->addSeparator();
	submenu->addItem(L"Switch to player", GUI_ID_SWITCH_TO_PLAYER);
	submenu->addItem(L"Quit", GUI_ID_QUIT);

	submenu = menu->getSubMenu(1);
	submenu->addItem(L"toggle model debug information", GUI_ID_TOGGLE_DEBUG_INFO, true, true);

	submenu = submenu->getSubMenu(0);
	submenu->addItem(L"Off", GUI_ID_DEBUG_OFF);
	submenu->addItem(L"Bounding Box", GUI_ID_DEBUG_BOUNDING_BOX);
	submenu->addItem(L"Normals", GUI_ID_DEBUG_NORMALS);
	submenu->addItem(L"Skeleton", GUI_ID_DEBUG_SKELETON);
	submenu->addItem(L"Wire overlay", GUI_ID_DEBUG_WIRE_OVERLAY);
	submenu->addItem(L"Half-Transparent", GUI_ID_DEBUG_HALF_TRANSPARENT);
	submenu->addItem(L"Buffers bounding boxes", GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES);
	submenu->addItem(L"All", GUI_ID_DEBUG_ALL);

	submenu = menu->getSubMenu(2);
	submenu->addItem(L"Maya Style", GUI_ID_CAMERA_MAYA);
	submenu->addItem(L"First Person", GUI_ID_CAMERA_FIRST_PERSON);

	submenu = menu->getSubMenu(3);
	submenu->addItem(L"About", GUI_ID_ABOUT);

	/*
	Below the menu we want a toolbar, onto which we can place colored
	buttons and important looking stuff like a senseless combobox.
	*/

	// create toolbar

	gui::IGUIToolBar* bar = env->addToolBar();

	video::ITexture* image = driver->getTexture("open.png");
	bar->addButton(GUI_ID_OPEN_DIALOG_BUTTON, 0, L"Open a model", image, 0, false, true);

	image = driver->getTexture("zip.png");
	bar->addButton(GUI_ID_FORCE_FIELD_BUTTON, 0, L"Edit force field", image, 0, false, true);

	image = driver->getTexture("help.png");
	bar->addButton(GUI_ID_HELP_BUTTON, 0, L"Open Help", image, 0, false, true);
}


void Editor::createSceneNodeToolBox()
{
	if (selectedNodeIndex < 0 || selectedNodeIndex >= (s32)nodes.size())
		return;

	// remove tool box if already there
	removeSceneNodeToolBox();

	// create the toolbox window
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(600,45,800,480),
		false, L"Toolset", 0, GUI_ID_SCENE_NODE_TOOL_BOX);

	// create tab control and tabs
	IGUITabControl* tab = env->addTabControl(
		core::rect<s32>(2,20,800-602,480-7), wnd, true, true);

	IGUITab* t1 = tab->addTab(L"Config");

	// add some edit boxes and a button to tab one
	s32 x = 10;
	s32 y = 20;
	env->addStaticText(L"Position:", core::rect<s32>(x,y,x+140,y+16), false, false, t1);
	x = 22; y = y+16;
	env->addStaticText(L"X:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getPosition().X ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_X_POSITION);
	env->addButton(core::rect<s32>(x+122,y+1,x+142,y+10), t1, GUI_ID_TOOL_BOX_INCREASE_POSITION_X, L"+");
	env->addButton(core::rect<s32>(x+122,y+10,x+142,y+19), t1, GUI_ID_TOOL_BOX_DECREASE_POSITION_X, L"-");
	x = 22; y = y+19;
	env->addStaticText(L"Y:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getPosition().Y ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_Y_POSITION);
	env->addButton(core::rect<s32>(x+122,y+1,x+142,y+10), t1, GUI_ID_TOOL_BOX_INCREASE_POSITION_Y, L"+");
	env->addButton(core::rect<s32>(x+122,y+10,x+142,y+19), t1, GUI_ID_TOOL_BOX_DECREASE_POSITION_Y, L"-");
	x = 22; y = y+19;
	env->addStaticText(L"Z:", core::rect<s32>(x,y+2,x+316,y+18), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getPosition().Z ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_Z_POSITION);
	env->addButton(core::rect<s32>(x+122,y+1,x+142,y+10), t1, GUI_ID_TOOL_BOX_INCREASE_POSITION_Z, L"+");
	env->addButton(core::rect<s32>(x+122,y+10,x+142,y+19), t1, GUI_ID_TOOL_BOX_DECREASE_POSITION_Z, L"-");
	x = 22; y = y+19;

	x = 10;
	y += 10;
	env->addStaticText(L"Rotation:", core::rect<s32>(x,y,x+140,y+16), false, false, t1);
	x = 22; y = y+16;
	env->addStaticText(L"X:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getRotation().X ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_X_ROTATION);
	env->addButton(core::rect<s32>(x+122,y+1,x+142,y+10), t1, GUI_ID_TOOL_BOX_INCREASE_ROTATION_X, L"+");
	env->addButton(core::rect<s32>(x+122,y+10,x+142,y+19), t1, GUI_ID_TOOL_BOX_DECREASE_ROTATION_X, L"-");
	x = 22; y = y+19;
	env->addStaticText(L"Y:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getRotation().Y ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_Y_ROTATION);
	env->addButton(core::rect<s32>(x+122,y+1,x+142,y+10), t1, GUI_ID_TOOL_BOX_INCREASE_ROTATION_Y, L"+");
	env->addButton(core::rect<s32>(x+122,y+10,x+142,y+19), t1, GUI_ID_TOOL_BOX_DECREASE_ROTATION_Y, L"-");
	x = 22; y = y+19;
	env->addStaticText(L"Z:", core::rect<s32>(x,y+2,x+316,y+18), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getRotation().Z ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_Z_ROTATION);
	env->addButton(core::rect<s32>(x+122,y+1,x+142,y+10), t1, GUI_ID_TOOL_BOX_INCREASE_ROTATION_Z, L"+");
	env->addButton(core::rect<s32>(x+122,y+10,x+142,y+19), t1, GUI_ID_TOOL_BOX_DECREASE_ROTATION_Z, L"-");
	x = 22; y = y+19;

	x = 10;
	y += 10;
	env->addStaticText(L"Scale:", core::rect<s32>(x,y,x+140,y+16), false, false, t1);
	x = 22; y = y+16;
	env->addStaticText(L"X:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getScale().X ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_X_SCALE);
	env->addButton(core::rect<s32>(x+122,y+1,x+142,y+10), t1, GUI_ID_TOOL_BOX_INCREASE_SCALE_X, L"+");
	env->addButton(core::rect<s32>(x+122,y+10,x+142,y+19), t1, GUI_ID_TOOL_BOX_DECREASE_SCALE_X, L"-");
	x = 22; y = y+19;
	env->addStaticText(L"Y:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getScale().Y ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_Y_SCALE);
	env->addButton(core::rect<s32>(x+122,y+1,x+142,y+10), t1, GUI_ID_TOOL_BOX_INCREASE_SCALE_Y, L"+");
	env->addButton(core::rect<s32>(x+122,y+10,x+142,y+19), t1, GUI_ID_TOOL_BOX_DECREASE_SCALE_Y, L"-");
	x = 22; y = y+19;
	env->addStaticText(L"Z:", core::rect<s32>(x,y+2,x+316,y+18), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getScale().Z ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_Z_SCALE);
	env->addButton(core::rect<s32>(x+122,y+1,x+142,y+10), t1, GUI_ID_TOOL_BOX_INCREASE_SCALE_Z, L"+");
	env->addButton(core::rect<s32>(x+122,y+10,x+142,y+19), t1, GUI_ID_TOOL_BOX_DECREASE_SCALE_Z, L"-");
	x = 22; y = y+19;

	x = 10;
	y += 10;
	env->addStaticText(L"Density:", core::rect<s32>(x,y,x+30,y+16), false, false, t1);
	env->addEditBox(stringw( densities[selectedNodeIndex] ).c_str(), core::rect<s32>(x+32,y+2,x+120,y+18), true, t1, GUI_ID_TOOL_BOX_DENSITY);
	
	x = 10;
	y += 50;
	env->addButton(core::rect<s32>(x,y,x+80,y+20), t1, GUI_ID_TOOL_BOX_SET_BUTTON, L"Set");
	env->addButton(core::rect<s32>(x+90,y,x+170,y+20), t1, GUI_ID_TOOL_BOX_DELETE_BUTTON, L"Delete");

}


void Editor::removeSceneNodeToolBox()
{
	// remove tool box if already there
	IGUIElement* root = env->getRootGUIElement();
	IGUIElement* e = root->getElementFromId(GUI_ID_SCENE_NODE_TOOL_BOX, true);
	if (e)
		e->remove();
}



void Editor::createForceFieldToolBox()
{
	if (forceFields.size() == 0)
		forceFields.push_back(vector3df(0,0,0));

	// remove tool box if already there
	removeForceFieldToolBox();

	// create the toolbox window
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(600,45,800,480),
		false, L"Toolset", 0, GUI_ID_FORCE_FIELD_TOOL_BOX);

	// create tab control and tabs
	IGUITabControl* tab = env->addTabControl(
		core::rect<s32>(2,20,800-602,480-7), wnd, true, true);

	IGUITab* t1 = tab->addTab(L"Config");

	// add some edit boxes and a button to tab one
	s32 x = 10;
	s32 y = 20;
	env->addStaticText(L"Intensity:", core::rect<s32>(x,y,x+140,y+16), false, false, t1);
	x = 22; y = y+16;
	env->addStaticText(L"X:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t1);
	env->addEditBox(stringw( forceFields[0].X ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_FORCE_FIELD_TOOL_BOX_X_INTENSITY);
	x = 22; y = y+19;
	env->addStaticText(L"Y:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t1);
	env->addEditBox(stringw( forceFields[0].Y ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_FORCE_FIELD_TOOL_BOX_Y_INTENSITY);
	x = 22; y = y+19;
	env->addStaticText(L"Z:", core::rect<s32>(x,y+2,x+316,y+18), false, false, t1);
	env->addEditBox(stringw( forceFields[0].Z ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t1, GUI_ID_FORCE_FIELD_TOOL_BOX_Z_INTENSITY);
	x = 22; y = y+19;

	x = 10;
	y += 10;
	env->addButton(core::rect<s32>(x,y,x+80,y+40), t1, GUI_ID_FORCE_FIELD_TOOL_BOX_SET_BUTTON, L"Set");
}



void Editor::removeForceFieldToolBox()
{
	// remove tool box if already there
	IGUIElement* root = env->getRootGUIElement();
	IGUIElement* e = root->getElementFromId(GUI_ID_FORCE_FIELD_TOOL_BOX, true);
	if (e)
		e->remove();
}




bool Editor::load(irr::core::stringc filename)
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
	<scenenode filename="mesh.x">
		<position x="32.8" y=".." z=".." />
		<rotation x="32.8" y=".." z=".." />
		<scale x="32.8" y=".." z=".." />
		<density value="50.0" />
	</scenenode>
	<forcefield>
		<intensity x="32.8" y=".." z=".." />
	</forcefield>
	<force>
		<scenenodeindex value="94"/>
		<vertexindex value="8"/>
		<intensity x="32.8" y=".." z=".." />
	</force>
	*/
	XmlNodeType currentType = UNDEFINED;
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
					if (stringw("scene") != xml->getNodeName())
					{
						env->addMessageBox(
							CAPTION_ERROR, L"This is not a valid scene file !");
						return false;
					}
					else
					{
						is_valid_file = true;
						// clean current scene
						this->clear();
					}
				}

				if (stringw("scenenode") == xml->getNodeName())
				{
					currentType = SCENENODE;
					add3DModel(xml->getAttributeValue(L"filename"));
				}
				else if (stringw("forcefield") == xml->getNodeName())
				{
					currentType = FORCEFIELD;
				}
				else if (stringw("position") == xml->getNodeName())
				{
					if (currentType == SCENENODE)
					{
						nodes.getLast()->setPosition(vector3df(
							xml->getAttributeValueAsFloat(L"x"),
							xml->getAttributeValueAsFloat(L"y"),
							xml->getAttributeValueAsFloat(L"z")));
						this->createSceneNodeToolBox();
					}
				}
				else if (stringw("rotation") == xml->getNodeName())
				{
					if (currentType == SCENENODE)
					{
						nodes.getLast()->setRotation(vector3df(
							xml->getAttributeValueAsFloat(L"x"),
							xml->getAttributeValueAsFloat(L"y"),
							xml->getAttributeValueAsFloat(L"z")));
						this->createSceneNodeToolBox();
					}
				}
				else if (stringw("scale") == xml->getNodeName())
				{
					if (currentType == SCENENODE)
					{
						nodes.getLast()->setScale(vector3df(
							xml->getAttributeValueAsFloat(L"x"),
							xml->getAttributeValueAsFloat(L"y"),
							xml->getAttributeValueAsFloat(L"z")));
						this->createSceneNodeToolBox();
					}
				}
				else if (stringw("density") == xml->getNodeName())
				{
					if (currentType == SCENENODE)
					{
						densities[nodes.size() - 1] = xml->getAttributeValueAsFloat(L"value");
						this->createSceneNodeToolBox();
					}
				}
				else if (stringw("intensity") == xml->getNodeName())
				{
					if (currentType == FORCEFIELD)
					{
						forceFields.push_back(vector3df(
							xml->getAttributeValueAsFloat(L"x"),
							xml->getAttributeValueAsFloat(L"y"),
							xml->getAttributeValueAsFloat(L"z")));
					}
				}
			}
			break;
		default:
			break;
		}
	}

	xml->drop();

	if (!is_valid_file)
	{
		env->addMessageBox(CAPTION_ERROR, L"This is not a valid scene file !");
	}

	this->name = device->getFileSystem()->getFileBasename( filename );

	return is_valid_file;
}

bool Editor::save(irr::core::stringc filename)
{
	// Create / open the file and get ready to write XML
	IWriteFile* file = device->getFileSystem()->createAndWriteFile( filename );
	if (!file)
		return false;
	IXMLWriter* xml = device->getFileSystem()->createXMLWriter( file );
	if (!xml)
		return false;

	xml->writeXMLHeader();
	xml->writeElement(L"scene", false);
	xml->writeLineBreak();

	// Save scene nodes
	/*
	<scenenode filename="mesh.x">
		<position x="32.8" y=".." z=".." />
		<rotation x="32.8" y=".." z=".." />
		<scale x="32.8" y=".." z=".." />
		<density value="50.0" />
	</scenenode>
	*/
	for (u16 i=0; i < nodes.size(); i++)
	{
		xml->writeElement(L"scenenode", false, L"filename", meshFiles[i].c_str());
		xml->writeLineBreak();

		xml->writeElement(L"position", true,
			L"x", stringw( nodes[i]->getPosition().X ).c_str(),
			L"y", stringw( nodes[i]->getPosition().Y ).c_str(),
			L"z", stringw( nodes[i]->getPosition().Z ).c_str());
		xml->writeLineBreak();

		xml->writeElement(L"rotation", true,
			L"x", stringw( nodes[i]->getRotation().X ).c_str(),
			L"y", stringw( nodes[i]->getRotation().Y ).c_str(),
			L"z", stringw( nodes[i]->getRotation().Z ).c_str());
		xml->writeLineBreak();

		xml->writeElement(L"scale", true,
			L"x", stringw( nodes[i]->getScale().X ).c_str(),
			L"y", stringw( nodes[i]->getScale().Y ).c_str(),
			L"z", stringw( nodes[i]->getScale().Z ).c_str());
		xml->writeLineBreak();

		xml->writeElement(L"density", true,
			L"value", stringw( densities[i] ).c_str());
		xml->writeLineBreak();

		xml->writeClosingTag(L"scenenode");
		xml->writeLineBreak();
	}


	xml->writeLineBreak();

	// Save force fields
	/*
	<forcefield>
		<intensity x="32.8" y=".." z=".." />
	</forcefield>
	*/
	for (u16 i=0; i < forceFields.size(); i++)
	{
		xml->writeElement(L"forcefield");
		xml->writeLineBreak();

		xml->writeElement(L"intensity", true,
			L"x", stringw( forceFields[i].X ).c_str(),
			L"y", stringw( forceFields[i].Y ).c_str(),
			L"z", stringw( forceFields[i].Z ).c_str());
		xml->writeLineBreak();

		xml->writeClosingTag(L"forcefield");
		xml->writeLineBreak();
	}

	xml->writeClosingTag(L"scene");
	xml->writeLineBreak();

	xml->drop();
	file->drop();

	this->name = device->getFileSystem()->getFileBasename( filename );

	return true;
}


bool Editor::tetScene()
{
	IGUIWindow* wnd = env->addMessageBox(L"Processing...", L"Tetraheadralizing... Please wait.", true, 0);
	driver->beginScene(true, true, SColor(255,100,101,140));
	env->drawAll();
	driver->endScene();

	device->getFileSystem()->changeWorkingDirectoryTo( baseDir.c_str() );

	// Create / open the file and get ready to write XML
	stringc filename = "output/";
	filename += device->getFileSystem()->getFileBasename( name, false );
	filename += "_volumic.xml";
	IWriteFile* file = device->getFileSystem()->createAndWriteFile( filename );
	if (!file)
		return false;
	IXMLWriter* xml = device->getFileSystem()->createXMLWriter( file );
	if (!xml)
		return false;

	xml->writeXMLHeader();
	xml->writeElement(L"scene", false, L"name", stringw(name.c_str()).c_str());
	xml->writeLineBreak();

	for (u16 i=0; i < nodes.size(); i++)
	{
		IMeshBuffer* buf = nodes[i]->getMesh()->getMeshBuffer(0);
		stringc n =  device->getFileSystem()->getFileBasename( name, false );
		n += stringc(i);
		n = n.trim();
		device->getLogger()->log(n.c_str());
		tetBuf(buf, (char*)n.c_str());

		xml->writeElement(L"volumicmesh", false, L"id", stringw(i).c_str());
		xml->writeLineBreak();

		stringw nodefile = L"output/";
		nodefile += stringw(n.c_str());
		nodefile += L"out.node";
		xml->writeElement(L"nodefile", true, L"file", nodefile.c_str());
		xml->writeLineBreak();

		stringw elefile = L"output/";
		elefile += stringw(n.c_str());
		elefile += L"out.ele";
		xml->writeElement(L"elefile", true, L"file", elefile.c_str());
		xml->writeLineBreak();

		stringw facefile = L"output/";
		facefile += stringw(n.c_str());
		facefile += L"out.face";
		xml->writeElement(L"facefile", true, L"file", facefile.c_str());
		xml->writeLineBreak();

		xml->writeElement(L"initialspeed", true, L"x", L"0", L"y", L"0", L"z", L"0");
		xml->writeLineBreak();
		xml->writeElement(L"materialproperties", true,
			L"Lambda", L"0.419", L"Mu", L"0.578", L"Alpha", L"1.04", L"Beta", L"1.44", L"Density", L"2595");
		xml->writeLineBreak();
		xml->writeClosingTag(L"volumicmesh");
		xml->writeLineBreak();
	}

	for (u16 i=0; i < forceFields.size(); i++)
	{
		xml->writeElement(L"forcefield");
		xml->writeLineBreak();

		xml->writeElement(L"intensity", true,
			L"x", stringw( forceFields[i].X ).c_str(),
			L"y", stringw( forceFields[i].Y ).c_str(),
			L"z", stringw( forceFields[i].Z ).c_str());
		xml->writeLineBreak();

		xml->writeClosingTag(L"forcefield");
		xml->writeLineBreak();
	}

	xml->writeClosingTag(L"scene");
	xml->writeLineBreak();

	xml->drop();
	file->drop();

	wnd->remove();

	return true;
}
