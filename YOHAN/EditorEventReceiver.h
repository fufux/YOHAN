#pragma once


// here define some captions used by the GUI
#define CAPTION_ERROR L"Error"
#define CAPTION_HELP L"Help"


class Editor;

// Values used to identify individual GUI elements
enum
{
	GUI_ID_SCENE_NODE_TOOL_BOX,
	GUI_ID_TOOL_BOX_X_POSITION,
	GUI_ID_TOOL_BOX_Y_POSITION,
	GUI_ID_TOOL_BOX_Z_POSITION,
	GUI_ID_TOOL_BOX_SET_BUTTON,

	GUI_ID_OPEN_MODEL,

	GUI_ID_TOGGLE_DEBUG_INFO,

	GUI_ID_DEBUG_OFF,
	GUI_ID_DEBUG_BOUNDING_BOX,
	GUI_ID_DEBUG_NORMALS,
	GUI_ID_DEBUG_SKELETON,
	GUI_ID_DEBUG_WIRE_OVERLAY,
	GUI_ID_DEBUG_HALF_TRANSPARENT,
	GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES,
	GUI_ID_DEBUG_ALL,

	GUI_ID_CAMERA_MAYA,
	GUI_ID_CAMERA_FIRST_PERSON,

	GUI_ID_OPEN_DIALOG_BUTTON,
	GUI_ID_HELP_BUTTON,

	GUI_ID_SKIN_TRANSPARENCY_SCROLL_BAR,
	GUI_ID_ABOUT,
	GUI_ID_QUIT
};


class EditorEventReceiver : public IEventReceiver
{
public:
	EditorEventReceiver(Editor* editor);
	~EditorEventReceiver(void);
	
	bool OnEvent(const SEvent& event);

private:
	// reference to the main editor object (which is the parent of this EditorEventReceiver)
	Editor* editor;

	void showHelp();

	void setActiveCamera(scene::ICameraSceneNode* newActive);

};
