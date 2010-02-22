#pragma once

#include "stdafx.h"
#include "Volume.h"
#include "Util.h"

class Scene
{
public:
	Scene(void);
	~Scene(void);

	bool load(std::string tetrahedralizedSceneFile);
	bool simulate(std::string simulatedSceneOutDir, double deltaT, int nbSteps);

	double* getForceField();
	double getDeltaT();

private:
	double deltaT;
	double currentTime; // accumulated time from begining
	double forceField[3];
	std::string simulatedSceneOutDir;

	// current step number
	int stepNumber;

	// list of all volumes in the scene
	vector<Volume*> volumes;

	void saveStep(std::string filename);
};
