#include "StdAfx.h"
#include "Editor.h"

extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;




// This is usefull when loading a sccene from an xml file. It allows us to know what we are currently loading.
enum XmlNodeType
{
	UNDEFINED,
	SCENENODE,
	FORCEFIELD
};




Editor::Editor(void)
{
	this->createGUI();
	this->er = new EditorEventReceiver(this);
	device->setEventReceiver(this->er);
}

Editor::~Editor(void)
{
}


void Editor::setDebugDataVisible(scene::E_DEBUG_SCENE_TYPE state)
{
	for (u16 i=0; i < nodes.size(); i++)
	{
		nodes[i]->setDebugDataVisible(state);
	}
}

s32 Editor::isDebugDataVisible()
{
	if (nodes.size() > 0)
		return nodes[0]->isDebugDataVisible();
	else
		return 0;
}


void Editor::setPositionOfSelectedNode(irr::core::vector3df pos)
{
	if (selectedNodeIndex >= 0 && selectedNodeIndex < nodes.size())
	{
		nodes[selectedNodeIndex]->setPosition( pos );
	}
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
	nodes.push_back( node );

	// select the newly loaded scene node
	selectedNodeIndex = nodes.size() - 1;

	// set default material properties
	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->setDebugDataVisible(scene::EDS_OFF);
}


void Editor::addForceField()
{
	forceFields.push_back( vector3df(0,0,0) );
}


void Editor::remove3DModel()
{
	if (selectedNodeIndex >= 0 && selectedNodeIndex < nodes.size())
	{
		nodes.erase(selectedNodeIndex);
		meshFiles.erase(selectedNodeIndex);
	}
}


void Editor::removeForceField()
{
	if (selectedForceField >= 0 && selectedForceField < forceFields.size())
	{
		forceFields.erase(selectedForceField);
	}
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
	submenu->addItem(L"Open Model File...", GUI_ID_OPEN_MODEL);
	submenu->addSeparator();
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
	bar->addButton(GUI_ID_OPEN_DIALOG_BUTTON, 0, L"Open a model",image, 0, false, true);

	image = driver->getTexture("help.png");
	bar->addButton(GUI_ID_HELP_BUTTON, 0, L"Open Help", image, 0, false, true);
}


void Editor::createSceneNodeToolBox()
{
	if (selectedNodeIndex < 0 || selectedNodeIndex >= nodes.size())
		return;

	// remove tool box if already there
	IGUIElement* root = env->getRootGUIElement();
	IGUIElement* e = root->getElementFromId(GUI_ID_SCENE_NODE_TOOL_BOX, true);
	if (e)
		e->remove();

	// create the toolbox window
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(600,45,800,480),
		false, L"Toolset", 0, GUI_ID_SCENE_NODE_TOOL_BOX);

	// create tab control and tabs
	IGUITabControl* tab = env->addTabControl(
		core::rect<s32>(2,20,800-602,480-7), wnd, true, true);

	IGUITab* t1 = tab->addTab(L"Config");

	// add some edit boxes and a button to tab one
	env->addStaticText(L"Position:",
			core::rect<s32>(10,20,150,45), false, false, t1);
	env->addStaticText(L"X:", core::rect<s32>(22,48,40,66), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getPosition().X ).c_str(), core::rect<s32>(40,46,130,66), true, t1, GUI_ID_TOOL_BOX_X_POSITION);
	env->addStaticText(L"Y:", core::rect<s32>(22,82,40,66), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getPosition().Y ).c_str(), core::rect<s32>(40,76,130,96), true, t1, GUI_ID_TOOL_BOX_Y_POSITION);
	env->addStaticText(L"Z:", core::rect<s32>(22,108,40,66), false, false, t1);
	env->addEditBox(stringw( nodes[selectedNodeIndex]->getPosition().Z ).c_str(), core::rect<s32>(40,106,130,126), true, t1, GUI_ID_TOOL_BOX_Z_POSITION);

	env->addButton(core::rect<s32>(10,134,85,165), t1, GUI_ID_TOOL_BOX_SET_BUTTON, L"Set");

}



void Editor::createForceFieldToolBox()
{
}




bool Editor::load(irr::core::stringc filename)
{
	IReadFile* file = device->getFileSystem()->createAndOpenFile( filename );
	if (!file)
		return false;
	IXMLReader* xml = device->getFileSystem()->createXMLReader( file );
	if (!xml)
		return false;

	// parse the file until end reached
	/*
	<scenenode filename="mesh.x">
		<position x="32.8" y=".." z=".." />
		<rotation x="32.8" y=".." z=".." />
		<scale x="32.8" y=".." z=".." />
	</scenenode>
	<forcefield>
		<intensity x="32.8" y=".." z=".." />
	</forcefield>
	*/
	while(xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				XmlNodeType currentType = UNDEFINED;

				if (stringw("scenenode") == xml->getNodeName())
				{
					currentType = SCENENODE;
					IMeshSceneNode* node = smgr->addMeshSceneNode(smgr->getMesh(xml->getAttributeValue(L"filename")));
					if (!node)
						return false;
					nodes.push_back(node);
					meshFiles.push_back(xml->getAttributeValue(L"filename"));
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

	return true;
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

	// Save scene nodes
	/*
	<scenenode filename="mesh.x">
		<position x="32.8" y=".." z=".." />
		<rotation x="32.8" y=".." z=".." />
		<scale x="32.8" y=".." z=".." />
	</scenenode>
	*/
	for (u16 i=0; i < nodes.size(); i++)
	{
		xml->writeElement(L"scenenode", false, L"filename", meshFiles[i].c_str());

		xml->writeElement(L"position", true,
			L"x", stringw( nodes[i]->getPosition().X ).c_str(),
			L"y", stringw( nodes[i]->getPosition().Y ).c_str(),
			L"z", stringw( nodes[i]->getPosition().Z ).c_str());

		xml->writeElement(L"rotation", true,
			L"x", stringw( nodes[i]->getRotation().X ).c_str(),
			L"y", stringw( nodes[i]->getRotation().Y ).c_str(),
			L"z", stringw( nodes[i]->getRotation().Z ).c_str());

		xml->writeElement(L"scale", true,
			L"x", stringw( nodes[i]->getScale().X ).c_str(),
			L"y", stringw( nodes[i]->getScale().Y ).c_str(),
			L"z", stringw( nodes[i]->getScale().Z ).c_str());

		xml->writeClosingTag(L"scenenode");
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

		xml->writeElement(L"intensity", true,
			L"x", stringw( forceFields[i].X ).c_str(),
			L"y", stringw( forceFields[i].Y ).c_str(),
			L"z", stringw( forceFields[i].Z ).c_str());

		xml->writeClosingTag(L"forcefield");
	}

	xml->drop();

	return true;
}
