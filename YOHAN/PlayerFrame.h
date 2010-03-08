#pragma once

#include "StdAfx.h"
#include "Player.h"

class PlayerFrame
{
public:
	PlayerFrame(Player* player, FrameInfo info, bool load_volumic=true);
	~PlayerFrame(void);

	// display this frame in the scene
	void display();

	// hide this frame from the scene
	void hide();

	// getters
	core::array<scene::IAnimatedMeshSceneNode*> getNodes();
	s32 getID();
	double getTimestamp();

	// total number of loaded frames
	static s32 totalLoadedFrames;

	// these 3 variables allow us to prevent the same ele or face file from being realoded
	static bool last_was_volumic;
	static core::array<stringc> lastEleFileNames;
	static core::array<stringc> lastFaceFileNames;
	static core::array<scene::SMeshBuffer*> lastBuffers;

private:
	// this is the list of nodes which will be displayed. One node <=> one volume
	core::array<scene::IAnimatedMeshSceneNode*> nodes;

	// this is the list of boundingBoxes which will be displayed. One node <=> one boundingBoxes
	core::array<scene::IMeshSceneNode*> boundingBoxes;

	// this if the files names in which we are getting mesh info for the loading frame
	s32 id;

	double timestamp;

	bool is_volumic;

	Player* player;
};
