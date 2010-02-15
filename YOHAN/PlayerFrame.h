#pragma once

#include "StdAfx.h"

class PlayerFrame
{
public:
	PlayerFrame(s32 id, stringc nodeFile, stringc eleFile);
	~PlayerFrame(void);

	// display this frame in the scene
	void display();

	// hide this frame from the scene
	void hide();

	// getters
	scene::IMeshSceneNode* getNode();
	s32 getId();

	// total number of loaded frames
	static s32 totalLoadedFrames;

	// these 2 variables allow us to prevent the same ele file from being realoded
	static stringc lastEleFileName;
	static scene::SMeshBuffer* lastBuf;

private:
	// this is the main node in which everything is displayed
	scene::IMeshSceneNode* node;

	// this is the meshBuffer in the scene
	scene::SMeshBuffer* buffer;

	// this if the files names in which we are getting mesh info for the loading frame
	s32 id;
	stringc nodeFile;
	stringc eleFile;
};
