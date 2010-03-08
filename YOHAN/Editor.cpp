#include "Editor.h"
#include "Scene.h"


extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
extern scene::ICameraSceneNode* camera[CAMERA_COUNT];


// lets import the tetBuf method
int tetBuf (IMeshBuffer *newBuffer, char *name, char *directory, float volume);



// This is usefull when loading a scene from an xml file. It allows us to know what we are currently loading.
enum XmlNodeType
{
	UNDEFINED,
	SCENENODE,
	FORCEFIELD
};




Editor::Editor(void)
{
	this->debugData = scene::EDS_MESH_WIRE_OVERLAY;
	this->er = new EditorEventReceiver(this);
	this->selectedNodeIndex = -1;
	this->selectedForceField = -1;
	this->is_running = false;
	this->player = NULL;
	this->name = "untitled";
	this->baseDir = device->getFileSystem()->getWorkingDirectory();
	this->lastSimulatedSceneOutDir = "";
}

Editor::~Editor(void)
{
}

void Editor::start()
{
	this->clear();
	this->createGUI();
	device->setEventReceiver(this->er);
	this->is_running = true;
	this->lastSimulatedSceneOutDir = "";
}


void Editor::stop()
{
	this->is_running = false;
	this->clear();
}

void Editor::switchToPlayer(stringc sceneDirToLoad)
{
	if (player != NULL)
	{
		// reset the working directory
		device->getFileSystem()->changeWorkingDirectoryTo( baseDir.c_str() );
		this->save("tmp.xml");
		this->stop();
		player->start();
		if (sceneDirToLoad.size() > 0)
			player->load(sceneDirToLoad + "/scene.xml");
	}
}


bool Editor::isRunning()
{
	return is_running;
}

stringc Editor::getName()
{
	return name;
}

stringc Editor::getLastSimulatedSceneOutDir()
{
	return lastSimulatedSceneOutDir;
}

stringc Editor::getBaseDir()
{
	return baseDir;
}

void Editor::clear(bool clear_gui)
{
	for (u16 i=0; i < nodes.size(); i++)
		nodes[i]->remove();
	nodes.clear();
	meshFiles.clear();
	meshMaterials.clear();
	initialSpeeds.clear();
	forceFields.clear();
	this->selectedNodeIndex = -1;
	this->selectedForceField = -1;
	this->debugData = scene::EDS_MESH_WIRE_OVERLAY;
	this->name = "untitled";

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
		{
			nodes[selectedNodeIndex]->setMaterialFlag(EMF_WIREFRAME, false);
			nodes[selectedNodeIndex]->setDebugDataVisible( debugData );
		}

		this->selectedNodeIndex = this->nodes.binary_search( selectedSceneNode );
		
		selectedSceneNode->setMaterialFlag(EMF_WIREFRAME, true);

		this->createSceneNodeToolBox();
	}
	else
	{
		if (selectedNodeIndex >= 0 && selectedNodeIndex < (s32)nodes.size())
		{
			nodes[selectedNodeIndex]->setMaterialFlag(EMF_WIREFRAME, false);
			nodes[selectedNodeIndex]->setDebugDataVisible( debugData );
		}
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
		core::vector3df pos, rot, sca, ispeed;
		EditorMaterial material;
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
		s = root->getElementFromId(GUI_ID_TOOL_BOX_X_SPEED, true)->getText();
		ispeed.X = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_SPEED, true)->getText();
		ispeed.Y = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_SPEED, true)->getText();
		ispeed.Z = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_MATERIAL_LAMBDA, true)->getText();
		material.lambda = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_MATERIAL_MU, true)->getText();
		material.mu = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_MATERIAL_ALPHA, true)->getText();
		material.alpha = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_MATERIAL_BETA, true)->getText();
		material.beta = (f32)atof(s.c_str());
		s = root->getElementFromId(GUI_ID_TOOL_BOX_MATERIAL_DENSITY, true)->getText();
		material.density = (f32)atof(s.c_str());

		nodes[selectedNodeIndex]->setPosition( pos );
		nodes[selectedNodeIndex]->setRotation( rot );
		nodes[selectedNodeIndex]->setScale( sca );
		meshMaterials[selectedNodeIndex] = material;
		initialSpeeds[selectedNodeIndex] = ispeed;
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


bool Editor::add3DModel(stringc filename)
{
	if (!device->getFileSystem()->existFile( filename.c_str() ))
	{
		// model doesn't exist
		env->addMessageBox(
			CAPTION_ERROR, (stringw("The surface mesh could not be loaded. The file ")+stringw(filename.c_str())+L" does not exist.").c_str());
		return false;
	}
	IMesh* m = smgr->getMesh( filename.c_str() );
	if (!m)
	{
		// model could not be loaded
		env->addMessageBox(
			CAPTION_ERROR, L"The surface mesh could not be loaded. " \
			L"Maybe it is not a supported file format.");
		return false;
	}

	IMeshSceneNode* node = smgr->addMeshSceneNode( m );
	if (!node)
		return false;
	
	// set default material properties
	node->setMaterialFlag(video::EMF_LIGHTING, true);
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->setDebugDataVisible(debugData);

	// Now create a triangle selector for it.
	scene::ITriangleSelector* selector = smgr->createTriangleSelector(node->getMesh(), node);
	node->setTriangleSelector(selector);
	selector->drop(); // We're done with this selector, so drop it now.


	for (int i=0; i < nodes.size(); i++)
		cout << nodes[i] << endl;
	cout <<endl;
	// select the newly loaded scene node
	nodes.push_back( node );
	for (int i=0; i < nodes.size(); i++)
			cout << nodes[i] << endl;
	cout <<endl<<endl;
	meshFiles.push_back( filename.c_str() );

	// set default values for material and initial speed
	EditorMaterial material;
	material.lambda = 0.419f;
	material.mu = 0.578f;
	material.alpha = 1.04f;
	material.beta = 1.44f;
	material.density = 2595.0f;
	meshMaterials.push_back( material );
	vector3df initialSpeed = vector3df(0,0,0);
	initialSpeeds.push_back( initialSpeed );

	// unselect current selected node if exists
	if (selectedNodeIndex >= 0 && selectedNodeIndex < (s32)nodes.size())
	{
		nodes[selectedNodeIndex]->setMaterialFlag(EMF_WIREFRAME, false);
		nodes[selectedNodeIndex]->setDebugDataVisible( debugData );
	}

	// select the node we've just loaded
	selectedNodeIndex = nodes.size() - 1;
	this->createSceneNodeToolBox();
	node->setMaterialFlag(EMF_WIREFRAME, true);
	node->setDebugDataVisible( debugData );

	return true;
}


void Editor::addForceField()
{
	forceFields.push_back( vector3df(0,0,0) );
}


void Editor::remove3DModel()
{
	if (selectedNodeIndex >= 0 && selectedNodeIndex < (s32)nodes.size())
	{
		core::array<IMeshSceneNode*> _nodes;
		core::array<stringc> _meshFiles;
		core::array<EditorMaterial> _meshMaterials;
		core::array<vector3df> _initialSpeeds;

		for (int i=0; i < nodes.size(); i++) {
			if (i != selectedNodeIndex)
				_nodes.push_back(nodes[i]);
		}
		for (int i=0; i < meshFiles.size(); i++) {
			if (i != selectedNodeIndex)
				_meshFiles.push_back(meshFiles[i]);
		}
		for (int i=0; i < meshMaterials.size(); i++) {
			if (i != selectedNodeIndex)
				_meshMaterials.push_back(meshMaterials[i]);
		}
		for (int i=0; i < initialSpeeds.size(); i++) {
			if (i != selectedNodeIndex)
				_initialSpeeds.push_back(initialSpeeds[i]);
		}

		nodes[selectedNodeIndex]->remove();
		/*for (int i=0; i < nodes.size(); i++)
			cout << nodes[i] << endl;
		cout <<endl<<endl;*/
		nodes = _nodes;
		/*for (int i=0; i < nodes.size(); i++)
			cout << nodes[i] << endl;
		for (int i=0; i < meshFiles.size(); i++)
			cout << meshFiles[i].c_str() << endl;
		cout <<endl<<endl;*/
		meshFiles = _meshFiles;
		/*for (int i=0; i < meshFiles.size(); i++)
			cout << meshFiles[i].c_str() << endl;*/
		meshMaterials = _meshMaterials;
		initialSpeeds = _initialSpeeds;
		selectedNodeIndex = -1;
		removeSceneNodeToolBox();
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
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(driver->getScreenSize().Width/2-150,200,driver->getScreenSize().Width/2+150,280),
		true, L"File name", 0, GUI_ID_ASK_FILENAME_WINDOW);

	env->addStaticText(L"File name:", core::rect<s32>(20,20,60,40), false, false, wnd);
	env->addEditBox(L"myscene.xml", core::rect<s32>(62,20,280,36), true, wnd, GUI_ID_ASK_FILENAME_NAME);

	env->addButton(core::rect<s32>(60,50,100,70), wnd, GUI_ID_ASK_FILENAME_OK_BUTTON, L"Ok");
	env->addButton(core::rect<s32>(120,50,160,70), wnd, GUI_ID_ASK_FILENAME_CANCEL_BUTTON, L"Cancel");
}


void Editor::createGUI()
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
	submenu->addItem(L"New", GUI_ID_NEW_SCENE);
	submenu->addItem(L"Open scene...", GUI_ID_OPEN_SCENE);
	submenu->addItem(L"Save scene", GUI_ID_SAVE_SCENE);
	submenu->addItem(L"Tetrahedralize & simulate scene", GUI_ID_TETRAHEDRALIZE_AND_SIMULATE_SCENE);
	submenu->addSeparator();
	submenu->addItem(L"Open Surface Mesh File...", GUI_ID_OPEN_MODEL);
	submenu->addSeparator();
	submenu->addItem(L"Switch to player", GUI_ID_SWITCH_TO_PLAYER);
	submenu->addItem(L"Quit", GUI_ID_QUIT);

	submenu = menu->getSubMenu(1);
	submenu->addItem(L"Views", GUI_ID_TOGGLE_DEBUG_INFO, true, true);

	submenu = submenu->getSubMenu(0);
	submenu->addItem(L"Off", GUI_ID_DEBUG_OFF, true, false, (isDebugDataVisible() == scene::EDS_OFF));
	submenu->addItem(L"Bounding Box", GUI_ID_DEBUG_BOUNDING_BOX, true, false, (isDebugDataVisible() == scene::EDS_BBOX));
	submenu->addItem(L"Normals", GUI_ID_DEBUG_NORMALS, true, false, (isDebugDataVisible() == scene::EDS_NORMALS));
	submenu->addItem(L"Wire overlay", GUI_ID_DEBUG_WIRE_OVERLAY, true, false, (isDebugDataVisible() == scene::EDS_MESH_WIRE_OVERLAY));
	submenu->addItem(L"Half-Transparent", GUI_ID_DEBUG_HALF_TRANSPARENT, true, false, (isDebugDataVisible() == scene::EDS_HALF_TRANSPARENCY));
	submenu->addItem(L"Buffers bounding boxes", GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES, true, false, (isDebugDataVisible() == scene::EDS_BBOX_BUFFERS));
	submenu->addItem(L"All", GUI_ID_DEBUG_ALL, true, false, (isDebugDataVisible() == scene::EDS_FULL));

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

	gui::IGUIToolBar* bar;
	if (root->getElementFromId(GUI_ID_TOOLBAR, true))
		((gui::IGUIToolBar*)root->getElementFromId(GUI_ID_TOOLBAR, true))->remove();
		//bar = (gui::IGUIToolBar*)root->getElementFromId(GUI_ID_TOOLBAR, true);
	
	bar = env->addToolBar(0, GUI_ID_TOOLBAR);

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
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(driver->getScreenSize().Width-200,45,driver->getScreenSize().Width,480),
		false, L"Toolset", 0, GUI_ID_SCENE_NODE_TOOL_BOX);

	// create tab control and tabs
	IGUITabControl* tab = env->addTabControl(
		core::rect<s32>(2,20,198,435-37), wnd, true, true);

	env->addButton(core::rect<s32>(20,435-25,100,435-5), wnd, GUI_ID_TOOL_BOX_SET_BUTTON, L"Set");
	env->addButton(core::rect<s32>(110,435-25,190,435-5), wnd, GUI_ID_TOOL_BOX_DELETE_BUTTON, L"Delete");

	// ----------------------------------------------------------------
	IGUITab* t1 = tab->addTab(L"Move & Scale");

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



	// ----------------------------------------------------------------
	IGUITab* t2 = tab->addTab(L"Init");

	// add some edit boxes and a button to tab one
	x = 10;
	y = 20;
	env->addStaticText(L"Initial speed:", core::rect<s32>(x,y,x+140,y+16), false, false, t2);
	x = 22; y = y+16;
	env->addStaticText(L"X:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t2);
	env->addEditBox(stringw( initialSpeeds[selectedNodeIndex].X ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t2, GUI_ID_TOOL_BOX_X_SPEED);
	x = 22; y = y+19;
	env->addStaticText(L"Y:", core::rect<s32>(x,y+2,x+16,y+18), false, false, t2);
	env->addEditBox(stringw( initialSpeeds[selectedNodeIndex].Y ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t2, GUI_ID_TOOL_BOX_Y_SPEED);
	x = 22; y = y+19;
	env->addStaticText(L"Z:", core::rect<s32>(x,y+2,x+316,y+18), false, false, t2);
	env->addEditBox(stringw( initialSpeeds[selectedNodeIndex].Z ).c_str(), core::rect<s32>(x+18,y+2,x+120,y+18), true, t2, GUI_ID_TOOL_BOX_Z_SPEED);
	x = 22; y = y+19;



	// ----------------------------------------------------------------
	IGUITab* t3 = tab->addTab(L"Material");

	// add some edit boxes and a button to tab one
	x = 10;
	y = 20;
	env->addStaticText(L"Initial speed:", core::rect<s32>(x,y,x+140,y+16), false, false, t3);
	x = 22; y = y+16;
	env->addStaticText(L"Lambda:", core::rect<s32>(x,y+2,x+36,y+18), false, false, t3);
	env->addEditBox(stringw( meshMaterials[selectedNodeIndex].lambda ).c_str(), core::rect<s32>(x+38,y+2,x+120,y+18), true, t3, GUI_ID_TOOL_BOX_MATERIAL_LAMBDA);
	x = 22; y = y+19;
	env->addStaticText(L"Mu:", core::rect<s32>(x,y+2,x+36,y+18), false, false, t3);
	env->addEditBox(stringw( meshMaterials[selectedNodeIndex].mu ).c_str(), core::rect<s32>(x+38,y+2,x+120,y+18), true, t3, GUI_ID_TOOL_BOX_MATERIAL_MU);
	x = 22; y = y+19;
	env->addStaticText(L"Alpha:", core::rect<s32>(x,y+2,x+36,y+18), false, false, t3);
	env->addEditBox(stringw( meshMaterials[selectedNodeIndex].alpha ).c_str(), core::rect<s32>(x+38,y+2,x+120,y+18), true, t3, GUI_ID_TOOL_BOX_MATERIAL_ALPHA);
	x = 22; y = y+19;
	env->addStaticText(L"Beta:", core::rect<s32>(x,y+2,x+36,y+18), false, false, t3);
	env->addEditBox(stringw( meshMaterials[selectedNodeIndex].beta ).c_str(), core::rect<s32>(x+38,y+2,x+120,y+18), true, t3, GUI_ID_TOOL_BOX_MATERIAL_BETA);
	x = 22; y = y+19;
	env->addStaticText(L"Density:", core::rect<s32>(x,y+2,x+36,y+18), false, false, t3);
	env->addEditBox(stringw( meshMaterials[selectedNodeIndex].density ).c_str(), core::rect<s32>(x+38,y+2,x+120,y+18), true, t3, GUI_ID_TOOL_BOX_MATERIAL_DENSITY);

}


void Editor::removeSceneNodeToolBox()
{
	// remove tool box if already there
	IGUIElement* e = env->getRootGUIElement()->getElementFromId(GUI_ID_SCENE_NODE_TOOL_BOX, true);
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
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(0,45,200,260),
		false, L"Toolset", 0, GUI_ID_FORCE_FIELD_TOOL_BOX);

	// create tab control and tabs
	IGUITabControl* tab = env->addTabControl(
		core::rect<s32>(2,20,198,215-2), wnd, true, true);

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
	<scene name="plop">
		<volumicmesh id="0">
			<nodefile file="F:/doc/ENSEEIHT/3annee/projet_long/model/test/cubeout.node"/>
			<elefile file="F:/doc/ENSEEIHT/3annee/projet_long/model/test/cubeout.ele" />
			<facefile file="F:/doc/ENSEEIHT/3annee/projet_long/model/test/cubeout.face" />
			<initialspeed x="0" y="0" z="10"/>
			<materialproperties Lambda="0.419" Mu="0.578" Alpha="1.04" Beta="1.44" Density="2595"/>
		</volumicmesh>
		<force>
			<volumicmeshid value="0"/>
			<vertex index="1"/>
			<intensity x="1" y="1" z="1"/>
		</force>
		<force>
			<volumicmeshid value="0"/>
			<vertex index="2"/>
			<intensity x="2" y="3" z="4"/>
		</force>
		<forcefield>
			<intensity x="0" y="0" z="-9.8" />
		</forcefield>
	</scene>
	*/
	XmlNodeType currentType = UNDEFINED;
	bool firstLoop = true;
	bool is_valid_file = false;
	bool add3DModel_success = false;

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
							CAPTION_ERROR, L"This is not a valid scene file!");
						return false;
					}
					else
					{
						is_valid_file = true;
						// clean current scene
						this->clear();
						this->createGUI();
						device->setEventReceiver(this->er);
					}
				}

				if (stringw("scenenode") == xml->getNodeName())
				{
					currentType = SCENENODE;
					add3DModel_success = add3DModel(xml->getAttributeValue(L"filename"));
				}
				else if (stringw("forcefield") == xml->getNodeName())
				{
					currentType = FORCEFIELD;
				}
				else if (stringw("position") == xml->getNodeName())
				{
					if (currentType == SCENENODE && add3DModel_success)
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
					if (currentType == SCENENODE && add3DModel_success)
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
					if (currentType == SCENENODE && add3DModel_success)
					{
						nodes.getLast()->setScale(vector3df(
							xml->getAttributeValueAsFloat(L"x"),
							xml->getAttributeValueAsFloat(L"y"),
							xml->getAttributeValueAsFloat(L"z")));
						this->createSceneNodeToolBox();
					}
				}
				else if (stringw("initialspeed") == xml->getNodeName())
				{
					if (currentType == SCENENODE && add3DModel_success)
					{
						initialSpeeds[nodes.size() - 1] = vector3df(
							xml->getAttributeValueAsFloat(L"x"),
							xml->getAttributeValueAsFloat(L"y"),
							xml->getAttributeValueAsFloat(L"z"));
						this->createSceneNodeToolBox();
					}
				}
				else if (stringw("materialproperties") == xml->getNodeName())
				{
					if (currentType == SCENENODE && add3DModel_success)
					{
						meshMaterials[nodes.size() - 1].lambda = xml->getAttributeValueAsFloat(L"lambda");
						meshMaterials[nodes.size() - 1].mu = xml->getAttributeValueAsFloat(L"mu");
						meshMaterials[nodes.size() - 1].alpha = xml->getAttributeValueAsFloat(L"alpha");
						meshMaterials[nodes.size() - 1].beta = xml->getAttributeValueAsFloat(L"beta");
						meshMaterials[nodes.size() - 1].density = xml->getAttributeValueAsFloat(L"density");
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
		xml->writeElement(L"scenenode", false, L"filename", stringw(meshFiles[i].c_str()).c_str());
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

		xml->writeElement(L"initialspeed", true,
			L"x", stringw( initialSpeeds[i].X ).c_str(),
			L"y", stringw( initialSpeeds[i].Y ).c_str(),
			L"z", stringw( initialSpeeds[i].Z ).c_str());
		xml->writeLineBreak();

		xml->writeElement(L"materialproperties", true,
			L"lambda", stringw( meshMaterials[i].lambda ).c_str(),
			L"mu", stringw( meshMaterials[i].mu ).c_str(),
			L"alpha", stringw( meshMaterials[i].alpha ).c_str(),
			L"beta", stringw( meshMaterials[i].beta ).c_str(),
			L"density", stringw( meshMaterials[i].density ).c_str());
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





void Editor::quickTetAndSimulate()
{
	// reset the working directory
	device->getFileSystem()->changeWorkingDirectoryTo( baseDir.c_str() );

	// get parameters
	gui::IGUIElement* root = env->getRootGUIElement();
	if (!root->getElementFromId(GUI_ID_ASK_PARAMETERS_NBTET, true) ||
		!root->getElementFromId(GUI_ID_ASK_PARAMETERS_NBFRAME, true) ||
		!root->getElementFromId(GUI_ID_ASK_PARAMETERS_DELTAT, true))
		return;

	stringc s = "";
	s = root->getElementFromId(GUI_ID_ASK_PARAMETERS_NBTET, true)->getText();
	f32 tetrahedraDensity = (f32)atof(s.c_str());
	s = root->getElementFromId(GUI_ID_ASK_PARAMETERS_NBFRAME, true)->getText();
	s32 nbFrame = (s32)atoi(s.c_str());
	s = root->getElementFromId(GUI_ID_ASK_PARAMETERS_DELTAT, true)->getText();
	f32 deltaT = (f32)atof(s.c_str());

	// check parameters
	if (nbFrame < 1 || deltaT <= 0 || tetrahedraDensity < 10.0f || tetrahedraDensity > 1000000.0f )
	{
		if (root->getElementFromId(GUI_ID_ASK_PARAMETERS_WINDOW, true))
			root->getElementFromId(GUI_ID_ASK_PARAMETERS_WINDOW, true)->remove();
		er->askForParameters(true);
		return;
	}

	if (root->getElementFromId(GUI_ID_ASK_PARAMETERS_WINDOW, true))
		root->getElementFromId(GUI_ID_ASK_PARAMETERS_WINDOW, true)->remove();

	// files and directories names
	stringc outDirTetrahedralize = "tetrahedralized/";
	outDirTetrahedralize += device->getFileSystem()->getFileBasename( name, false );
	stringc tetrahedralizedSceneFile = device->getFileSystem()->getFileBasename( name, false );
	tetrahedralizedSceneFile += "_volumic.xml";
	stringc simulatedSceneOutDir = "simulated/";
	simulatedSceneOutDir += device->getFileSystem()->getFileBasename( name, false );

	// create the directories if they don't exist
	createDir( "tetrahedralized" );
	createDir( "simulated" );
	createDir( (char*)outDirTetrahedralize.c_str() );
	createDir( (char*)simulatedSceneOutDir.c_str() );

	// display message
	IGUIWindow* wnd = env->addMessageBox(L"Processing...", L"Tetrahedralizing... Please wait.", true, 0);
	driver->beginScene(true, true, SColor(255,100,101,140));
	env->drawAll();
	driver->endScene();
	wnd->remove();

	// clear Player data to win memory
	this->player->clear(false);

	// tetrahedralize...
	if ( tetrahedralizeScene(tetrahedralizedSceneFile, outDirTetrahedralize, tetrahedraDensity) )
	{
		// display message
		wnd = env->addMessageBox(L"Processing...", L"Tetrahedralization successful. Simulating... Please wait.", true, 0);
		driver->beginScene(true, true, SColor(255,100,101,140));
		env->drawAll();
		driver->endScene();
		wnd->remove();

		// simulate...
		if ( simulateScene(outDirTetrahedralize + "/" + tetrahedralizedSceneFile, simulatedSceneOutDir, nbFrame, deltaT) )
		{
			this->lastSimulatedSceneOutDir = simulatedSceneOutDir;
			this->er->askForSwitch();
		}
		else
		{
			env->addMessageBox(CAPTION_ERROR, L"Simulation failed! Aborting.", true);
		}
	}
	else
	{
		env->addMessageBox(CAPTION_ERROR, L"Tetrahedralization failed! Aborting.", true);
	}

	// clear system messages to prevent users input of being handled
	device->clearSystemMessages();
}


bool Editor::simulateScene(stringc tetrahedralizedSceneFile, stringc simulatedSceneOutDir, s32 nbFrame, f32 deltaT)
{
	if (nbFrame < 1 || deltaT <= 0)
		return false;

	std::string tsf = tetrahedralizedSceneFile.c_str();
	std::string ssod = simulatedSceneOutDir.c_str();

	try
	{
		// instanciate scene object
		Scene* scene = new Scene();

		// load the scene
		if (scene->load(tsf))
		{
			device->getLogger()->log(
				(stringc("Call simulate(")+simulatedSceneOutDir+", "+stringc(deltaT)+", "+stringc(nbFrame)+")").c_str());
			
			long tstart, tend, tdif;
			tstart = GetTickCount();
			
			// launch the simulation
			scene->simulate(ssod, deltaT, nbFrame);

			// display time
			tend = GetTickCount();
			tdif = tend - tstart; //will now have the time elapsed since the start of the call
			device->getLogger()->log(
				(stringc("Simulation computed in ")+stringc(tdif)+"ms").c_str());
		}
		else
		{
			device->getLogger()->log("Scene loading for simulation failed!");
			cout << "Scene loading for simulation failed!" << endl;
		}


	}
	catch(...)
	{
		device->getLogger()->log("Exception during the simulation. Find me in Editor.cpp, in the simulateScene method.");
		return false;
	}

	return true;
}


/*
Ths method tetrahedralize the current scene and save the result in the given file and directory.
*/
bool Editor::tetrahedralizeScene(stringc outTetrahedralizedFile, stringc outDir, f32 tetrahedraDensity)
{
	if (tetrahedraDensity < 10.0f || tetrahedraDensity > 1000000.0f)
		return false;

	// Create / open the file and get ready to write XML
	stringc filename = outDir + "/" + outTetrahedralizedFile;

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
		IMesh* mesh = getMeshWithAbsoluteCoordinates( nodes[i] );
		IMeshBuffer* buf = mesh->getMeshBuffer(0);
		stringc n =  device->getFileSystem()->getFileBasename( name, false );
		n += stringc(i);
		n = n.trim();

		if (tetBuf(buf, (char*)n.c_str(), (char*)(outDir+"/").c_str(), (float)mesh->getBoundingBox().getVolume() / tetrahedraDensity ) != 0)
		{
			xml->drop();
			file->drop();
			return false;
		}

		xml->writeElement(L"volumicmesh", false, L"id", stringw(i).c_str());
		xml->writeLineBreak();

		stringw nodefile = stringw(outDir.c_str()) + L"/";
		nodefile += stringw(n.c_str());
		nodefile += L"out.node";
		xml->writeElement(L"nodefile", true, L"file", nodefile.c_str());
		xml->writeLineBreak();

		stringw elefile = stringw(outDir.c_str()) + L"/";
		elefile += stringw(n.c_str());
		elefile += L"out.ele";
		xml->writeElement(L"elefile", true, L"file", elefile.c_str());
		xml->writeLineBreak();

		stringw facefile = stringw(outDir.c_str()) + L"/";
		facefile += stringw(n.c_str());
		facefile += L"out.face";
		xml->writeElement(L"facefile", true, L"file", facefile.c_str());
		xml->writeLineBreak();

		xml->writeElement(L"initialposition", true,
			L"x", stringw( nodes[i]->getPosition().X ).c_str(),
			L"y", stringw( nodes[i]->getPosition().Y ).c_str(),
			L"z", stringw( nodes[i]->getPosition().Z ).c_str());
		xml->writeLineBreak();

		xml->writeElement(L"initialrotation", true,
			L"x", stringw( degToRad(nodes[i]->getRotation().X) ).c_str(),
			L"y", stringw( degToRad(nodes[i]->getRotation().Y) ).c_str(),
			L"z", stringw( degToRad(nodes[i]->getRotation().Z) ).c_str());
		xml->writeLineBreak();

		xml->writeElement(L"initialspeed", true,
			L"x", stringw( initialSpeeds[i].X ).c_str(),
			L"y", stringw( initialSpeeds[i].Y ).c_str(),
			L"z", stringw( initialSpeeds[i].Z ).c_str());
		xml->writeLineBreak();

		xml->writeElement(L"materialproperties", true,
			L"Lambda", stringw( meshMaterials[i].lambda ).c_str(),
			L"Mu", stringw( meshMaterials[i].mu ).c_str(),
			L"Alpha", stringw( meshMaterials[i].alpha ).c_str(),
			L"Beta", stringw( meshMaterials[i].beta ).c_str(),
			L"Density", stringw( meshMaterials[i].density ).c_str());
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

	return true;
}



IMesh* Editor::getMeshWithAbsoluteCoordinates(IMeshSceneNode* node)
{
	SMeshBuffer* newBuf = new SMeshBuffer();
	//SMeshBuffer* tmpBuf = new SMeshBuffer();
	// prepare...
	u32 index_count = 0;
	u32 vertex_count = 0;
	u32 index_offset = 0;
	u32 poly_offset = 0;
	for (u32 k=0; k < node->getMesh()->getMeshBufferCount(); k++)
	{
		IMeshBuffer* buf = node->getMesh()->getMeshBuffer(k);
		index_count += buf->getIndexCount();
		vertex_count += buf->getVertexCount();
	}
	//tmpBuf->Indices.set_used( index_count );
	newBuf->Indices.set_used( index_count );
	//tmpBuf->Vertices.reallocate( vertex_count );
	newBuf->Vertices.reallocate( vertex_count );

	for (u32 k=0; k < node->getMesh()->getMeshBufferCount(); k++)
	{
		IMeshBuffer* buf = node->getMesh()->getMeshBuffer(k);

		for (u32 i=0; i < buf->getIndexCount(); i++)
		{
			//tmpBuf->Indices[i+poly_offset] = buf->getIndices()[i] + index_offset;
			newBuf->Indices[i+poly_offset] = buf->getIndices()[i] + index_offset;
		}
		for (u32 i=0; i < buf->getVertexCount(); i++)
			newBuf->Vertices.push_back( ((video::S3DVertex*)buf->getVertices())[i] );

		index_offset += buf->getVertexCount();
		poly_offset += buf->getIndexCount();
	}


	// just do it
	vector3df pos = node->getPosition();
	vector3df rot = node->getRotation();
	vector3df scale = node->getScale();
	rot.X = degToRad(rot.X);
	rot.Y = degToRad(rot.Y);
	rot.Z = degToRad(rot.Z);

	for (u32 i=0; i < newBuf->Vertices.size(); i++)
	{
		// apply scale
		newBuf->Vertices[i].Pos *= scale;
		/*vector3df p = newBuf->Vertices[i].Pos;
		
		// apply rotation
		p.X = newBuf->Vertices[i].Pos.X * ( cos(rot.Z)*cos(rot.Y) )
			+ newBuf->Vertices[i].Pos.Y * ( cos(rot.Z)*sin(rot.Y)*sin(rot.X)-sin(rot.Z)*cos(rot.X) )
			+ newBuf->Vertices[i].Pos.Z * ( cos(rot.Z)*sin(rot.Y)*cos(rot.X)+sin(rot.Z)*sin(rot.X) );
		p.Y = newBuf->Vertices[i].Pos.X * ( sin(rot.Z)*cos(rot.Y) )
			+ newBuf->Vertices[i].Pos.Y * ( sin(rot.Z)*sin(rot.Y)*sin(rot.X)+cos(rot.Z)*cos(rot.X) )
			+ newBuf->Vertices[i].Pos.Z * ( sin(rot.Z)*sin(rot.Y)*cos(rot.X)-cos(rot.Z)*sin(rot.X) );
		p.Z = newBuf->Vertices[i].Pos.X * ( -sin(rot.Y) )
			+ newBuf->Vertices[i].Pos.Y * ( cos(rot.Y)*sin(rot.X) )
			+ newBuf->Vertices[i].Pos.Z * ( cos(rot.Y)*cos(rot.X) );
		
		newBuf->Vertices[i].Pos = p;

		// apply position
		newBuf->Vertices[i].Pos += pos;*/
	}

	// lets recalculate the bounding box and create the mesh
	for (u32 j=0; j < newBuf->Vertices.size(); ++j)
		newBuf->BoundingBox.addInternalPoint(newBuf->Vertices[j].Pos);

	SMesh* mesh = new SMesh();
	mesh->MeshBuffers.push_back(newBuf);
	mesh->recalculateBoundingBox();
	//IMeshSceneNode* n = smgr->addMeshSceneNode( mesh );
	//n->setMaterialFlag(EMF_WIREFRAME, true);

	return mesh;
}