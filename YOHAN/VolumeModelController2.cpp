#include "VolumeModelController2.h"

VolumeModelController2::VolumeModelController2(const char* nodeFile, const char* faceFile, const char* eleFile, DATA speed[], const DATA materialPropriety[])
{
	this->eventQueue = std::queue<MotivationEvent*>();
	this->reportQueue = std::queue<MotivationReport>();

	this->model = new VolumeModel2(nodeFile, faceFile, eleFile, speed, materialPropriety);

	this->round = 0;
}

void VolumeModelController2::sendEvent(MotivationEvent* e)
{
	this->eventQueue.push(e);
}

void VolumeModelController2::evoluteModel()
{
	this->model->calculateInternalForce();

	while (!this->eventQueue.empty())
	{
		MotivationEvent* e = this->eventQueue.front();
		this->eventQueue.pop();

		if (e->getEventType() == MotivationEvent::TYPE_ForceFieldEvent)
		{
			ForceFieldEvent* ffe = (ForceFieldEvent*)e;

			this->model->updateGravity(ffe->gravityValue);
			this->model->setDeltaTime(ffe->deltaTime);

			delete ffe;
		}
		else if (e->getEventType() == MotivationEvent::TYPE_ForceEvent)
		{
			ForceEvent* fe = (ForceEvent*)e;

			this->model->fillForceVector(fe->sf.vertexID, fe->sf.intensity);

			delete fe;
		}
		else
			fetalError();
	}

	

	// combine the matrix and resolve the linear equation
	// this->model->calculate();
	this->model->calculatePosition();

	// increase round
	++round;

}

VolumeModel2* VolumeModelController2::getModel()
{
	return this->model;
}
