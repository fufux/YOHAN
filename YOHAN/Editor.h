#pragma once

#include "StdAfx.h"
#include "EditorEventReceiver.h"
#include "Player.h"



struct EditorMaterial
{
	f32 lambda,mu,alpha,beta,density;
};


class Editor
{
public:
	Editor(void);
	~Editor(void);

	// call this to launch/stop all the editor
	void start();
	void stop();
	void switchToPlayer(stringc sceneDirToLoad="");
	// call this to remove all nodes of the scene editor
	void clear(bool clear_gui=true);

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
	bool add3DModel(core::stringc filename);

	// add a force field to the scene
	void addForceField();

	// remove selected node from the scene. See selectedNodeIndex
	void remove3DModel();

	// remove selected forcefield from the scene. See selectedForceField
	void removeForceField();

	// allow to change visibility of debug data
	void setDebugDataVisible(scene::E_DEBUG_SCENE_TYPE state);
	s32 isDebugDataVisible();


	// This method tetrahedralize the current scene and save the result in the given file and directory.
	bool tetrahedralizeScene(stringc outTetrahedralizedFile, stringc outDir, f32 tetrahedraDensity);
	// This method simulate the given volumic scene and save the result in a given directory
	bool simulateScene(stringc tetrahedralizedSceneFile, stringc simulatedSceneOutDir, s32 nbFrame, f32 deltaT);

	// This simply calls tetrahedralizeScene and simulateScene with right arguments
	void quickTetAndSimulate();
	
	// allow to change the position of the selected scene node
	void setPositionRotationScaleOfSelectedNode();
	void moveSelectedNode(vector3df vec);

	// allow to change the intensity of the force field
	void setForceField();
	void createForceFieldToolBox();

	// this method allows the user to select a node
	void selectNode();

	void setPlayer(Player* player);

	// is the editor running ?
	bool isRunning();
	stringc getName();

	// usefull to switch & load to player
	stringc getLastSimulatedSceneOutDir();
	stringc getBaseDir();

private:
	// name of the scene
	stringc name;
	stringc baseDir;

	// is the user currently using the player ? (it could be the editor)
	bool is_running;

	// the event receiver that handles all controls in the editor
	EditorEventReceiver* er;

	// this is the list of all models in the scene
	core::array<IMeshSceneNode*> nodes;
	IMesh* getMeshWithAbsoluteCoordinates(IMeshSceneNode* node);

	// and this is the list of meshe files name that correspond to the nodes (index must correspond with previous list)
	core::array<stringc> meshFiles;
	core::array<EditorMaterial> meshMaterials; // and here are the materials of each object
	core::array<vector3df> initialSpeeds;

	// this is the list of all force fields in the scene
	core::array<vector3df> forceFields;

	// store (current) selected node/forcefield (if defined)
	s32 selectedNodeIndex;
	s32 selectedForceField;
	video::ITexture* selectedObjectTexture;
	video::ITexture* defaultObjectTexture;

	// what kind of debug are we displaying currently ?
	scene::E_DEBUG_SCENE_TYPE debugData;

	// these methods allow to create / initialize GUI elements
	void createGUI();
	void createSceneNodeToolBox();
	void removeSceneNodeToolBox();
	void removeForceFieldToolBox();

	// this allows us to access the player in order to start it
	Player* player;

	// usefull to switch & load to player
	stringc lastSimulatedSceneOutDir;
	
};
