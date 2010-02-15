#include "stdafx.h"

#include "Base.h"

using namespace yohan;
using namespace base;


/****************************/
/* VolumeModelController    */
/****************************/

VolumeModelController::VolumeModelController(const char* nodeFile, const char* faceFile, const char* eleFile, DATA speed[], const DATA materialPropriety[])
{
	this->eventQueue = std::queue<MotivationEvent*>();
	this->reportQueue = std::queue<MotivationReport>();

	this->model = new VolumeModel(nodeFile, faceFile, eleFile, speed, materialPropriety);

	this->round = 0;
}

void VolumeModelController::evoluteModel()
{
	// vitesse vector and x-u vector
	this->model->fillVector();

	while (!this->eventQueue.empty())
	{
		MotivationEvent* e = this->eventQueue.front();
		this->eventQueue.pop();

		if (e->getEventType() == MotivationEvent::TYPE_ForceFieldEvent)
		{
			ForceFieldEvent* ffe = (ForceFieldEvent*)e;

			// generate M, C, K, F
			this->model->fillMatrix(ffe->gravityValue);
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
	this->model->calculate();

	// the new vitesse are in F, feed them back
	this->model->feedBackVector();

	// generate a frame
	//this->model->generateFrame(round);

	// generate the report
	this->reportQueue.push(MotivationReport(this->model));

	// increase round
	++round;

}

void VolumeModelController::sendEvent(MotivationEvent* e)
{
	this->eventQueue.push(e);
}

MotivationReport VolumeModelController::retrieveReport()
{
	MotivationReport e = this->reportQueue.front();
	this->reportQueue.pop();

	return e;
}