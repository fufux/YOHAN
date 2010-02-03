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

	/*
	These methods allow us to get easily all scene info in order to tetrahedralize it.

	getAllSceneNodes() returns an array containing each MeshSceneNode of the scene.
	You might need to get some info from each MeshSceneNode like this :
		MeshSceneNode* myMeshSceneNode = getAllSceneNodes()[i];
		myMeshSceneNode->getMesh(); returns an IMesh*
		myMeshSceneNode->getPosition();
		myMeshSceneNode->getRotation();
		myMeshSceneNode->getScale();

	getForceField() returns a 3D vector containing the intensity of the force field on each coord (X,Y,Z)
	*/
	core::array<IMeshSceneNode*> getAllSceneNodes();
	vector3df getForceField();

	// load scene from a scene file
	bool load(core::stringc filename);

	// save scene in a scene file
	bool save(core::stringc filename);
	void askForFileName();

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
	void setPositionRotationScaleOfSelectedNode();
	void moveSelectedNode(vector3df vec);

	// allow to change the intensity of the force field
	void setForceField();
	void createForceFieldToolBox();

	// this method allows the user to select a node
	void selectNode();

private:
	// the event receiver that handles all controls in the editor
	EditorEventReceiver* er;

	// this is the list of all models in the scene
	core::array<IMeshSceneNode*> nodes;

	// and this is the list of meshe files name that correspond to the nodes (index must correspond with previous list)
	core::array<stringw> meshFiles;

	// this is the list of all force fields in the scene
	core::array<vector3df> forceFields;

	// store (current) selected node/forcefield (if defined)
	s32 selectedNodeIndex;
	s32 selectedForceField;

	// what kind of debug are we displaying currently ?
	scene::E_DEBUG_SCENE_TYPE debugData;

	// these methods allow to create / initialize GUI elements
	void createGUI();
	void createSceneNodeToolBox();
	void removeSceneNodeToolBox();
	void removeForceFieldToolBox();
	
};
