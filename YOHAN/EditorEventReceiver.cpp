#include "Editor.h"
#include "EditorEventReceiver.h"


extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
extern scene::ICameraSceneNode* camera[CAMERA_COUNT];


EditorEventReceiver::EditorEventReceiver(Editor* editor)
{
	this->editor = editor;
	setActiveCamera( camera[0] );
	this->oldMousePos = position2di(0,0);
}

EditorEventReceiver::~EditorEventReceiver(void)
{
}

bool EditorEventReceiver::OnEvent(const SEvent &event)
{
	if (!device || !env)
		return false;

	gui::IGUIElement* root = env->getRootGUIElement();
	core::stringc s = "";

	// Mouse
	if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		scene::ICameraSceneNode * activeCam = smgr->getActiveCamera();

		/*if (event.MouseInput.Control || event.MouseInput.Shift)
			activeCam->setInputReceiverEnabled( false );
		else
			activeCam->setInputReceiverEnabled( true );*/


		if (event.MouseInput.isLeftPressed() && event.MouseInput.Control)
		{
			editor->selectNode();
		}
		else if (event.MouseInput.Shift && event.MouseInput.Event == EMIE_MOUSE_MOVED)
		{
			position2di mousePos;
			mousePos.X = event.MouseInput.X;
            mousePos.Y = event.MouseInput.Y; 
			if ((oldMousePos - mousePos).getLength() > 10)
				oldMousePos = mousePos;
			editor->moveSelectedNode(vector3df(
				(mousePos.X - oldMousePos.X) * cos(activeCam->getRotation().Y) + (mousePos.Y - oldMousePos.Y) * sin(activeCam->getRotation().Y),
				0,
				(mousePos.X - oldMousePos.X) * sin(activeCam->getRotation().Y) + (mousePos.Y - oldMousePos.Y) * cos(activeCam->getRotation().Y)));
			oldMousePos = mousePos;
		}
	}


	// Key pressed once
	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.PressedDown == false)
	{
		switch(event.KeyInput.Key)
		{
		// press escape to go back to maya view
		case KEY_ESCAPE:
			{
				setActiveCamera(camera[0]);
				if (root->getElementFromId(GUI_ID_STATIC_TEXT_CAMERA_FPS, true))
					root->getElementFromId(GUI_ID_STATIC_TEXT_CAMERA_FPS, true)->remove();
				break;
			}
		case KEY_KEY_M:
			device->minimizeWindow();
			break;
		}
	}

	// Key pressed
	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.PressedDown == true)
	{
		switch(event.KeyInput.Key)
		{
		case KEY_UP:
			break;
		}
	}

	// GUI
	if (event.EventType == EET_GUI_EVENT)
	{
		s32 id = event.GUIEvent.Caller->getID();

		switch(event.GUIEvent.EventType)
		{
		case EGET_MENU_ITEM_SELECTED:
			{
				// a menu item was clicked

				IGUIContextMenu* menu = (IGUIContextMenu*)event.GUIEvent.Caller;
				s32 id = menu->getItemCommandId(menu->getSelectedItem());

				switch(id)
				{
				case GUI_ID_NEW_SCENE: // File -> New
					editor->stop();
					editor->start();
					break;
				case GUI_ID_OPEN_SCENE: // File -> Open scene
					opening = OPENING_SCENE;
					env->addFileOpenDialog(L"Please select a scene file to open");
					break;
				case GUI_ID_SAVE_SCENE: // File -> Save scene
					editor->askForFileName();
					break;
				case GUI_ID_TETRAHEDRALIZE_AND_SIMULATE_SCENE: // File -> Tetrahedralize scene
					askForParameters();
					break;
				case GUI_ID_OPEN_MODEL: // File -> Open Model
					opening = OPENING_MODEL;
					env->addFileOpenDialog(L"Please select a model file to open");
					break;
				case GUI_ID_SWITCH_TO_PLAYER: // File -> Switch to player
					editor->switchToPlayer();
					break;
				case GUI_ID_QUIT: // File -> Quit
					device->closeDevice();
					break;
				case GUI_ID_DEBUG_OFF: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem()+1, false);
					menu->setItemChecked(menu->getSelectedItem()+2, false);
					menu->setItemChecked(menu->getSelectedItem()+3, false);
					menu->setItemChecked(menu->getSelectedItem()+4, false);
					menu->setItemChecked(menu->getSelectedItem()+5, false);
					menu->setItemChecked(menu->getSelectedItem()+6, false);
					editor->setDebugDataVisible(scene::EDS_OFF);
					break;
				case GUI_ID_DEBUG_BOUNDING_BOX: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					editor->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(editor->isDebugDataVisible()^scene::EDS_BBOX));
					break;
				case GUI_ID_DEBUG_NORMALS: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					editor->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(editor->isDebugDataVisible()^scene::EDS_NORMALS));
					break;
				case GUI_ID_DEBUG_SKELETON: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					editor->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(editor->isDebugDataVisible()^scene::EDS_SKELETON));
					break;
				case GUI_ID_DEBUG_WIRE_OVERLAY: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					editor->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(editor->isDebugDataVisible()^scene::EDS_MESH_WIRE_OVERLAY));
					break;
				case GUI_ID_DEBUG_HALF_TRANSPARENT: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					editor->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(editor->isDebugDataVisible()^scene::EDS_HALF_TRANSPARENCY));
					break;
				case GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					editor->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(editor->isDebugDataVisible()^scene::EDS_BBOX_BUFFERS));
					break;
				case GUI_ID_DEBUG_ALL: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem()-1, true);
					menu->setItemChecked(menu->getSelectedItem()-2, true);
					menu->setItemChecked(menu->getSelectedItem()-3, true);
					menu->setItemChecked(menu->getSelectedItem()-4, true);
					menu->setItemChecked(menu->getSelectedItem()-5, true);
					menu->setItemChecked(menu->getSelectedItem()-6, true);
					editor->setDebugDataVisible(scene::EDS_FULL);
					break;
				case GUI_ID_ABOUT: // Help->About
					showHelp();
					break;

				case GUI_ID_CAMERA_MAYA:
					setActiveCamera(camera[0]);
					if (root->getElementFromId(GUI_ID_STATIC_TEXT_CAMERA_FPS, true))
						root->getElementFromId(GUI_ID_STATIC_TEXT_CAMERA_FPS, true)->remove();
					break;
				case GUI_ID_CAMERA_FIRST_PERSON:
					setActiveCamera(camera[1]);
					env->addStaticText(L"Press Escape to go back to maya-style camera mode.", core::rect<s32>(20,60,220,75), true, true, 0, GUI_ID_STATIC_TEXT_CAMERA_FPS);
					break;

				}
			break;
			}

		case EGET_FILE_SELECTED:
			{
				// load the model file, selected in the file open dialog
				IGUIFileOpenDialog* dialog =
					(IGUIFileOpenDialog*)event.GUIEvent.Caller;
				if (opening == OPENING_MODEL)
					editor->add3DModel(core::stringc(dialog->getFileName()).c_str());
				else if (opening == OPENING_SCENE)
					editor->load(core::stringc(dialog->getFileName()).c_str());
			}
			break;

		case EGET_SCROLL_BAR_CHANGED:
			break;

		case EGET_COMBO_BOX_CHANGED:
			break;

		case EGET_BUTTON_CLICKED:
			switch(id)
			{
			case GUI_ID_TOOL_BOX_SET_BUTTON:
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DELETE_BUTTON:
				showConfirmDeleteNode();
				break;
			case GUI_ID_FORCE_FIELD_TOOL_BOX_SET_BUTTON:
				editor->setForceField();
				break;
			case GUI_ID_ASK_FILENAME_OK_BUTTON:
				s = root->getElementFromId(GUI_ID_ASK_FILENAME_NAME, true)->getText();
				if (s.size() == 0)
					s = "untitled.xml";
				editor->save(s.c_str());
				if (root->getElementFromId(GUI_ID_ASK_FILENAME_WINDOW, true))
					root->getElementFromId(GUI_ID_ASK_FILENAME_WINDOW, true)->remove();
				break;
			case GUI_ID_CONFIRM_DELETE_BUTTON:
				editor->remove3DModel();
				if (root->getElementFromId(GUI_ID_CONFIRM_DELETE_WINDOW, true))
					root->getElementFromId(GUI_ID_CONFIRM_DELETE_WINDOW, true)->remove();
				break;
			case GUI_ID_ASK_FILENAME_CANCEL_BUTTON:
				if (root->getElementFromId(GUI_ID_ASK_FILENAME_WINDOW, true))
					root->getElementFromId(GUI_ID_ASK_FILENAME_WINDOW, true)->remove();
				break;
			case GUI_ID_CANCEL_DELETE_BUTTON:
				if (root->getElementFromId(GUI_ID_CONFIRM_DELETE_WINDOW, true))
					root->getElementFromId(GUI_ID_CONFIRM_DELETE_WINDOW, true)->remove();
				break;
			case GUI_ID_ASK_PARAMETERS_GO_BUTTON:
				editor->quickTetAndSimulate();
				break;
			case GUI_ID_ASK_PARAMETERS_CANCEL_BUTTON:
				if (root->getElementFromId(GUI_ID_ASK_PARAMETERS_WINDOW, true))
					root->getElementFromId(GUI_ID_ASK_PARAMETERS_WINDOW, true)->remove();
				break;
			case GUI_ID_ASK_SWITCH_YES_BUTTON:
				if (root->getElementFromId(GUI_ID_ASK_SWITCH_WINDOW, true))
					root->getElementFromId(GUI_ID_ASK_SWITCH_WINDOW, true)->remove();
				editor->switchToPlayer(editor->getLastSimulatedSceneOutDir());;
				break;
			case GUI_ID_ASK_SWITCH_NO_BUTTON:
				if (root->getElementFromId(GUI_ID_ASK_SWITCH_WINDOW, true))
					root->getElementFromId(GUI_ID_ASK_SWITCH_WINDOW, true)->remove();
				break;
			case GUI_ID_OPEN_DIALOG_BUTTON:
				opening = OPENING_MODEL;
				env->addFileOpenDialog(L"Please select a model file to open");
				break;
			case GUI_ID_FORCE_FIELD_BUTTON:
				editor->createForceFieldToolBox();
				break;
			case GUI_ID_HELP_BUTTON:
				showHelp();
				break;
			case GUI_ID_TOOL_BOX_INCREASE_POSITION_X:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_X_POSITION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_X_POSITION, true)->setText(stringw( 1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DECREASE_POSITION_X:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_X_POSITION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_X_POSITION, true)->setText(stringw( -1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_INCREASE_POSITION_Y:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_POSITION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Y_POSITION, true)->setText(stringw( 1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DECREASE_POSITION_Y:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_POSITION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Y_POSITION, true)->setText(stringw( -1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_INCREASE_POSITION_Z:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_POSITION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Z_POSITION, true)->setText(stringw( 1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DECREASE_POSITION_Z:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_POSITION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Z_POSITION, true)->setText(stringw( -1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;

			case GUI_ID_TOOL_BOX_INCREASE_ROTATION_X:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_X_ROTATION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_X_ROTATION, true)->setText(stringw( 1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DECREASE_ROTATION_X:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_X_ROTATION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_X_ROTATION, true)->setText(stringw( -1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_INCREASE_ROTATION_Y:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_ROTATION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Y_ROTATION, true)->setText(stringw( 1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DECREASE_ROTATION_Y:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_ROTATION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Y_ROTATION, true)->setText(stringw( -1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_INCREASE_ROTATION_Z:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_ROTATION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Z_ROTATION, true)->setText(stringw( 1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DECREASE_ROTATION_Z:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_ROTATION, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Z_ROTATION, true)->setText(stringw( -1.0f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;

			case GUI_ID_TOOL_BOX_INCREASE_SCALE_X:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_X_SCALE, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_X_SCALE, true)->setText(stringw( 0.01f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DECREASE_SCALE_X:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_X_SCALE, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_X_SCALE, true)->setText(stringw( -0.01f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_INCREASE_SCALE_Y:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_SCALE, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Y_SCALE, true)->setText(stringw( 0.01f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DECREASE_SCALE_Y:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Y_SCALE, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Y_SCALE, true)->setText(stringw( -0.01f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_INCREASE_SCALE_Z:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_SCALE, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Z_SCALE, true)->setText(stringw( 0.01f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;
			case GUI_ID_TOOL_BOX_DECREASE_SCALE_Z:
				s = root->getElementFromId(GUI_ID_TOOL_BOX_Z_SCALE, true)->getText();
				root->getElementFromId(GUI_ID_TOOL_BOX_Z_SCALE, true)->setText(stringw( -0.01f + (f32)atof(s.c_str()) ).c_str());
				editor->setPositionRotationScaleOfSelectedNode();
				break;

			}

			break;
		default:
			break;
		}
	}

	return false;
}


/*
Toggle between various cameras
*/
void EditorEventReceiver::setActiveCamera(scene::ICameraSceneNode* newActive)
{
	scene::ICameraSceneNode * active = smgr->getActiveCamera();
	active->setInputReceiverEnabled(false);

	newActive->setInputReceiverEnabled(true);
	smgr->setActiveCamera(newActive);
}


void EditorEventReceiver::showHelp()
{
	stringw message = L"";
	message += L"This scene editor allows you to load meshes, place them in the scene, add forces, initial speed values, etc.\n\r";
	message += L"Once you have setted up the scene, you can start the simulation. You can save/load a scene whenever you want.";

	env->addMessageBox(
		CAPTION_HELP, message.c_str());
}


void EditorEventReceiver::showConfirmDeleteNode()
{
	// create the window
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(driver->getScreenSize().Width/2-100,200,driver->getScreenSize().Width/2+100,350),
		true, L"Warning", 0, GUI_ID_CONFIRM_DELETE_WINDOW);

	env->addStaticText(L"Are you sure you want to delete this object ?", core::rect<s32>(20,20,180,80), false, true, wnd);

	env->addButton(core::rect<s32>(60,90,100,110), wnd, GUI_ID_CONFIRM_DELETE_BUTTON, L"Delete");
	env->addButton(core::rect<s32>(120,90,160,110), wnd, GUI_ID_CANCEL_DELETE_BUTTON, L"Cancel");
}


void EditorEventReceiver::askForParameters(bool previous_was_bad)
{
	if (editor->getAllSceneNodes().size() == 0)
	{
		env->addMessageBox(L"Warning", L"There is nothing in the scene...");
	}
	else
	{
		// create the window
		IGUIWindow* wnd = env->addWindow(core::rect<s32>(driver->getScreenSize().Width/2-200,100,driver->getScreenSize().Width/2+200,380),
			true, L"Parameters", 0, GUI_ID_ASK_PARAMETERS_WINDOW);

		s32 y=20;
		if (editor->getName() == "untitled")
		{
			env->addStaticText(L"WARNING: this scene has never been saved and thus is named [untitled]. The simulation output will overwrite all files in \"simulated/untitled\" !", core::rect<s32>(20,y,380,y+30), false, true, wnd);
			y += 30;
		}

		env->addStaticText(L"WARNING: If you press \"GO!\", all player data will be unloaded. You will need to reload a video to watch it.", core::rect<s32>(20,y,380,y+30), false, true, wnd);
		
		y += 30;
		if (previous_was_bad)
			env->addStaticText(L"WRONG PARAMETERS, please try again. Choose the parameters for tetrahedrization and simulation.", core::rect<s32>(20,y,380,y+30), false, true, wnd);
		else
			env->addStaticText(L"Please, choose the parameters for tetrahedrization and simulation.", core::rect<s32>(20,y,380,y+30), false, true, wnd);

		y += 30;
		env->addStaticText(L"How many maximum tetrahedras do you want in each object of the scene ? (min: 10, max: 1,000,000)", core::rect<s32>(20,y,380,y+20), false, true, wnd);
		env->addEditBox(L"5000", core::rect<s32>(40,y+20,80,y+36), true, wnd, GUI_ID_ASK_PARAMETERS_NBTET);
		y += 40;
		env->addStaticText(L"How many frames do you want to simulate ? (min: 1)", core::rect<s32>(20,y,380,y+20), false, true, wnd);
		env->addEditBox(L"200", core::rect<s32>(40,y+20,80,y+36), true, wnd, GUI_ID_ASK_PARAMETERS_NBFRAME);
		y += 40;
		env->addStaticText(L"What do you want as Delta t ? (strict-min: 0)", core::rect<s32>(20,y,380,y+20), false, true, wnd);
		env->addEditBox(L"0.01", core::rect<s32>(40,y+20,80,y+36), true, wnd, GUI_ID_ASK_PARAMETERS_DELTAT);
		y += 50;
		env->addButton(core::rect<s32>(60,y,100,y+16), wnd, GUI_ID_ASK_PARAMETERS_GO_BUTTON, L"GO!");
		env->addButton(core::rect<s32>(320,y,360,y+16), wnd, GUI_ID_ASK_PARAMETERS_CANCEL_BUTTON, L"Cancel");
	}
}


void EditorEventReceiver::askForSwitch()
{
	// create the window
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(driver->getScreenSize().Width/2-100,200,driver->getScreenSize().Width/2+100,350),
		true, L"Success", 0, GUI_ID_ASK_SWITCH_WINDOW);

	env->addStaticText(L"Simulation finished well.\r\nDo you want to switch to the player and open the video now ?", core::rect<s32>(20,20,180,80), false, true, wnd);

	env->addButton(core::rect<s32>(60,90,100,110), wnd, GUI_ID_ASK_SWITCH_YES_BUTTON, L"Yes");
	env->addButton(core::rect<s32>(120,90,160,110), wnd, GUI_ID_ASK_SWITCH_NO_BUTTON, L"No");
}