#pragma once

#include "stdafx.h"
#include "Volume.h"
#include "Util.h"
#include "Polyhedron.h"
#include "BoundingBox.h"

class Scene
{
public:
	Scene(void);
	~Scene(void);

	bool load(std::string tetrahedralizedSceneFile);
	bool simulate(std::string simulatedSceneOutDir, double deltaT, int nbSteps, bool fracture, bool selfcollision);

	double* getForceField();
	double getDeltaT();
	void planCollisionResponse(vector<Tetrahedron*>* tets);
	void CollisionResponse(vector<vector<Tetrahedron*>>* tets);

	// simulation options
	bool simulate_fracture;
	bool simulate_selfcollision;

	std::string simulatedSceneOutDir;

private:
	double deltaT;
	double currentTime; // accumulated time from begining
	double forceField[3];
	double kerr;
	double kdmp;
	double kfrc;

	Tetrahedron* plan;

	// current step number
	int stepNumber;

	// list of all volumes in the scene
	vector<Volume*> volumes;

	void handleCollisions();

	void saveStep(std::string filename);
};
