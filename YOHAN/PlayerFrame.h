#pragma once

#include "StdAfx.h"
#include "Player.h"

class PlayerFrame
{
public:
	PlayerFrame(FrameInfo info, bool load_volumic=true);
	~PlayerFrame(void);

	// display this frame in the scene
	void display();

	// hide this frame from the scene
	void hide();

	// getters
	core::array<scene::IMeshSceneNode*> getNodes();
	s32 getId();

	// total number of loaded frames
	static s32 totalLoadedFrames;

	// these 3 variables allow us to prevent the same ele or face file from being realoded
	static bool last_was_volumic;
	static core::array<stringc> lastEleFileNames;
	static core::array<stringc> lastFaceFileNames;
	static core::array<scene::SMeshBuffer*> lastBuffers;

private:
	// this is the main node in which everything is displayed
	core::array<scene::IMeshSceneNode*> nodes;

	// this if the files names in which we are getting mesh info for the loading frame
	s32 id;
};
