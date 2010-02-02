#pragma once

#include <irrlicht.h>
#include <iostream>
#include "EditorEventReceiver.h"

using namespace irr;
using namespace gui;



class Editor
{
public:
	Editor(void);
	~Editor(void);


	// load scene from a scene file
	bool load(core::stringc filename);

	// save scene in a scene file
	bool save(core::stringc filename);

	// load a 3D model from a mesh file (3ds, X, etc.) and add it to the scene
	void add3DModel(core::stringc filename);

	// add a force field to the scene
	void addForceField();

	// remove selected node from the scene. See selectedNodeIndex
	void remove3DModel();

	// remove selected forcefield from the scene. See selectedForceField
	void removeForceField();

	// allow to change visibility of debug data
	void setDebugDataVisible(scene::E_DEBUG_SCENE_TYPE state);
	s32 isDebugDataVisible();


	// allow to change the position of the selected scene node
	void setPositionOfSelectedNode(vector3df pos);

private:
	// the event receiver that handles all controls in the editor
	EditorEventReceiver* er;

	core::array<IMeshSceneNode*> nodes;
	core::array<stringw> meshFiles;
	core::array<vector3df> forceFields;

	// store (current) selected node/forcefield (if defined)
	u16 selectedNodeIndex;
	u16 selectedForceField;

	void createGUI();
	void createSceneNodeToolBox();
	void createForceFieldToolBox();
	
};
