#include "StdAfx.h"
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
				case GUI_ID_OPEN_SCENE: // File -> Open scene
					opening = OPENING_SCENE;
					env->addFileOpenDialog(L"Please select a scene file to open");
					break;
				case GUI_ID_SAVE_SCENE: // File -> Save scene
					editor->askForFileName();
					break;
				case GUI_ID_OPEN_MODEL: // File -> Open Model
					opening = OPENING_MODEL;
					env->addFileOpenDialog(L"Please select a model file to open");
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
					break;
				case GUI_ID_CAMERA_FIRST_PERSON:
					setActiveCamera(camera[1]);
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

			// control skin transparency
			if (id == GUI_ID_SKIN_TRANSPARENCY_SCROLL_BAR)
			{
				const s32 pos = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
				{
					video::SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
					col.setAlpha(pos);
					env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
				}
			}
			break;

		case EGET_COMBO_BOX_CHANGED:
			break;

		case EGET_BUTTON_CLICKED:
			switch(id)
			{
			case GUI_ID_TOOL_BOX_SET_BUTTON:
				editor->setPositionRotationScaleOfSelectedNode();
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
			case GUI_ID_ASK_FILENAME_CANCEL_BUTTON:
				if (root->getElementFromId(GUI_ID_ASK_FILENAME_WINDOW, true))
					root->getElementFromId(GUI_ID_ASK_FILENAME_WINDOW, true)->remove();
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
