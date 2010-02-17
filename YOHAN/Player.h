#pragma once

#include "StdAfx.h"
#include "PlayerEventReceiver.h"

class Editor;
class PlayerFrame;


struct FrameInfo
{
	s32 id;
	core::array<stringc> nodefiles, facefiles, elefiles;
};


class Player
{
public:
	Player(void);
	~Player(void);

	// call this to launch/stop all the player
	void start();
	void stop();
	void switchToEditor();
	// call this to remove all nodes of the player
	void clear();

	// loads a video scene file and get ready to play it
	bool loadAll(stringc filename);
	bool load(stringc filename);

	// allows us to change current frame in step-by-step mode
	void displayNextFrame();
	void displayPreviousFrame();
	void displayFrameById(s32 id);

	// allows us to change current frame in playing mode
	void playNextFrame();

	// handle playying mode controls
	void play();
	void pause();
	void run();

	void setEditor(Editor* editor);

	// allow to change visibility of debug data
	void setDebugDataVisible(scene::E_DEBUG_SCENE_TYPE state);
	s32 isDebugDataVisible();

	// are we playing now ?
	bool is_playing;

	// is the player running ?
	bool isRunning();

	void updateFrameNumber();

	s32 currentFrame; // this allows us to know where we are in playing mode
private:
	// name of the scene
	stringc name;
	stringc baseDir;

	// is the user currently using the player ? (it could be the editor)
	bool is_running;

	// the event receiver that handles all controls in the player
	PlayerEventReceiver* er;

	// this allow us to store all info from the main xml file (the video file)
	core::array<FrameInfo> framesFileNames;

	// this is the list of all frames in the video for playing mode
	core::array<PlayerFrame*> frames;

	// this is the current frame in step-by-step mode
	PlayerFrame* currFrame;

	// this is the file name of the scene (.xml)
	stringc sceneFile;

	// these methods allow to create / initialize GUI elements
	void createGUI();

	// this allows us to access the editor in order to start it
	Editor* editor;

	// what kind of debug are we displaying currently ?
	scene::E_DEBUG_SCENE_TYPE debugData;

	// usefull for playing mode. This is the least time we changed the displayyed frame
	u32 lastTime;
};
