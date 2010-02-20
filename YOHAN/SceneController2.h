#include "Base.h"

#include "VolumeModelController2.h"
#include "SceneRecord2.h"

class SceneController2
{
	/* The list that contains all the "reference" of VolumeModelController which could independantly evoluate */
	std::vector<VolumeModelController2> vmcList;

	/* */
	SceneRecorder2* recorder;

	/* */
	DATA gravity[3];

	/* */
	std::list<struct SceneForce> forceList;

public:

	/* Create an instance of SceneController from an existing Scene Configuration File */
	/* The cordinate transformation is done in this step */
	SceneController2(char* filename);

	/* start to simulate */
	void simulate(char* filename, DATA delta_t=0.01, int nb_steps=10);

private:
	/* collision detection returns the overlaping volumes */
	//std::vector<CollisionPolyedron>* CollisionDetection();

};