#include "stdafx.h"

#include "Base.h"

using namespace yohan;
using namespace base;

ForceEvent::ForceEvent(SceneForce sf)
{
	this->sf = sf;
}

int ForceEvent::getEventType()
{
	return MotivationEvent::TYPE_ForceEvent;
}

/* */

ForceFieldEvent::ForceFieldEvent(bool isActive, DATA intensity[], DATA deltaT)
{
	this->isGravityActive = isActive;

	this->gravityValue[0] = intensity[0];
	this->gravityValue[1] = intensity[1];
	this->gravityValue[2] = intensity[2];

	this->deltaTime = deltaT;
}

int ForceFieldEvent::getEventType()
{
	return MotivationEvent::TYPE_ForceFieldEvent;
}

/* */

MotivationReport::MotivationReport(VolumeModel* modelRef)
{
	this->modelRef = modelRef;
}

VolumeModel* MotivationReport::getModel()
{
	return this->modelRef;
}

