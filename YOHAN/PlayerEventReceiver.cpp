#include "Player.h"
#include "PlayerEventReceiver.h"


extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
extern scene::ICameraSceneNode* camera[CAMERA_COUNT];

PlayerEventReceiver::PlayerEventReceiver(Player* player)
{
	this->player = player;
	this->image_play = driver->getTexture("play.png");
	this->image_pause = driver->getTexture("pause.png");
	setActiveCamera( camera[0] );
}

PlayerEventReceiver::~PlayerEventReceiver(void)
{
}

bool PlayerEventReceiver::OnEvent(const SEvent &event)
{
	if (!device || !env)
		return false;

	gui::IGUIElement* root = env->getRootGUIElement();
	core::stringc s = "";

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
				if (root->getElementFromId(GUI_ID_PLAYER_STATIC_TEXT_CAMERA_FPS, true))
					root->getElementFromId(GUI_ID_PLAYER_STATIC_TEXT_CAMERA_FPS, true)->remove();
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
		case KEY_SPACE:
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
				case GUI_ID_PLAYER_OPEN_VIDEO: // File -> Open scene
					env->addFileOpenDialog(L"Please select a video file to open");
					break;
				case GUI_ID_SWITCH_TO_EDITOR: // File -> Switch to editor
					player->switchToEditor();
					break;
				case GUI_ID_PLAYER_QUIT: // File -> Quit
					device->closeDevice();
					break;
				case GUI_ID_PLAYER_DEBUG_OFF: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem()+1, false);
					menu->setItemChecked(menu->getSelectedItem()+2, false);
					menu->setItemChecked(menu->getSelectedItem()+3, false);
					menu->setItemChecked(menu->getSelectedItem()+4, false);
					menu->setItemChecked(menu->getSelectedItem()+5, false);
					menu->setItemChecked(menu->getSelectedItem()+6, false);
					player->setDebugDataVisible(scene::EDS_OFF);
					break;
				case GUI_ID_PLAYER_DEBUG_BOUNDING_BOX: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					player->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(player->isDebugDataVisible()^scene::EDS_BBOX));
					break;
				case GUI_ID_PLAYER_DEBUG_NORMALS: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					player->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(player->isDebugDataVisible()^scene::EDS_NORMALS));
					break;
				case GUI_ID_PLAYER_DEBUG_SKELETON: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					player->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(player->isDebugDataVisible()^scene::EDS_SKELETON));
					break;
				case GUI_ID_PLAYER_DEBUG_WIRE_OVERLAY: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					player->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(player->isDebugDataVisible()^scene::EDS_MESH_WIRE_OVERLAY));
					break;
				case GUI_ID_PLAYER_DEBUG_HALF_TRANSPARENT: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					player->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(player->isDebugDataVisible()^scene::EDS_HALF_TRANSPARENCY));
					break;
				case GUI_ID_PLAYER_DEBUG_BUFFERS_BOUNDING_BOXES: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem(), !menu->isItemChecked(menu->getSelectedItem()));
					player->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(player->isDebugDataVisible()^scene::EDS_BBOX_BUFFERS));
					break;
				case GUI_ID_PLAYER_DEBUG_ALL: // View -> Debug Information
					menu->setItemChecked(menu->getSelectedItem()-1, true);
					menu->setItemChecked(menu->getSelectedItem()-2, true);
					menu->setItemChecked(menu->getSelectedItem()-3, true);
					menu->setItemChecked(menu->getSelectedItem()-4, true);
					menu->setItemChecked(menu->getSelectedItem()-5, true);
					menu->setItemChecked(menu->getSelectedItem()-6, true);
					player->setDebugDataVisible(scene::EDS_FULL);
					break;
				case GUI_ID_PLAYER_ABOUT: // Help->About
					showHelp();
					break;

				case GUI_ID_PLAYER_CAMERA_MAYA:
					setActiveCamera(camera[0]);
					if (root->getElementFromId(GUI_ID_PLAYER_STATIC_TEXT_CAMERA_FPS, true))
						root->getElementFromId(GUI_ID_PLAYER_STATIC_TEXT_CAMERA_FPS, true)->remove();
					break;
				case GUI_ID_PLAYER_CAMERA_FIRST_PERSON:
					setActiveCamera(camera[1]);
					env->addStaticText(L"Press Escape to go back to maya-style camera mode.", core::rect<s32>(20,60,220,75), true, true, 0, GUI_ID_PLAYER_STATIC_TEXT_CAMERA_FPS);
					break;

				}
			break;
			}

		case EGET_FILE_SELECTED:
			{
				// load the model file, selected in the file open dialog
				IGUIFileOpenDialog* dialog =
					(IGUIFileOpenDialog*)event.GUIEvent.Caller;
				player->load(core::stringc(dialog->getFileName()).c_str());
			}
			break;

		case EGET_SCROLL_BAR_CHANGED:
			break;

		case EGET_COMBO_BOX_CHANGED:
			break;

		case EGET_BUTTON_CLICKED:
			switch(id)
			{
			case GUI_ID_PLAYER_NEXT_FRAME:
				player->displayNextFrame();
				break;
			case GUI_ID_PLAYER_PREVIOUS_FRAME:
				player->displayPreviousFrame();
				break;
			case GUI_ID_PLAYER_PLAY:
				if (!player->is_playing)
				{
					player->play();
					IGUIButton* b = (IGUIButton*)root->getElementFromId(GUI_ID_PLAYER_PLAY, true);
					b->setImage(image_pause);
					b->setToolTipText(L"Pause");
				}
				else
				{
					player->pause();
					IGUIButton* b = (IGUIButton*)root->getElementFromId(GUI_ID_PLAYER_PLAY, true);
					b->setImage(image_play);
					b->setToolTipText(L"Play");
				}
				break;
			case GUI_ID_PLAYER_OPEN_VIDEO_BUTTON:
				env->addFileOpenDialog(L"Please select a video file to open");
				break;
			case GUI_ID_PLAYER_HELP_BUTTON:
				showHelp();
				break;
			case GUI_ID_PLAYER_FRAME_NUMBER_BUTTON:
				s = root->getElementFromId(GUI_ID_PLAYER_FRAME_NUMBER, true)->getText();
				player->displayFrameById( (s32)atoi(s.c_str()) );
				break;
			}

			break;
		default:
			break;
		}
	}

	return false;
}



void PlayerEventReceiver::showHelp()
{
	stringw message = L"";
	message += L"This video player allows you to load....\n\r";
	message += L"Bla bla.";

	env->addMessageBox(
		CAPTION_HELP, message.c_str());
}

/*
Toggle between various cameras
*/
void PlayerEventReceiver::setActiveCamera(scene::ICameraSceneNode* newActive)
{
	scene::ICameraSceneNode * active = smgr->getActiveCamera();
	active->setInputReceiverEnabled(false);

	newActive->setInputReceiverEnabled(true);
	smgr->setActiveCamera(newActive);
}