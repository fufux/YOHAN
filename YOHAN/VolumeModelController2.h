#pragma once

#include "VolumeModel2.h"

class VolumeModelController2
{
	std::queue<MotivationEvent*> eventQueue;

	std::queue<MotivationReport> reportQueue;

	VolumeModel2* model;

	int round;

public:
	/* called definitely by <yohan::base::SceneController> */
	void sendEvent(MotivationEvent* e);

	/* */
	void evoluteModel();

	/* called definitely by <yohan::base::SceneController> */
	MotivationReport retrieveReport();

	/* */
	VolumeModelController2(const char* nodeFile, const char* faceFile, const char* eleFile, DATA speed[], const DATA materialPropriety[]);

	VolumeModel2* getModel();

};