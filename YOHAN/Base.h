#ifndef _STD_VECTOR_
#define _STD_VECTOR_
#include <vector>
#endif

#ifndef _STD_LIST_
#define _STD_LIST_
#include <list>
#endif

#ifndef _STD_QUEUE_
#define _STD_QUEUE_
#include <queue>
#endif

#ifndef _STD_MAP_
#define _STD_MAP_
#include <map>
#endif

#define _YOHAN_BASE_

namespace yohan
{
	namespace base
	{
		/**********************/
		/*    Configuration   */
		/**********************/


		/* 
		As we are not sure it is float or double that would be more adaptive to our project, on mesure of
		the compromise between the calculation precision and the calculation instruction quantity, we use DATA
		as the definition of a floating number everywhere.
		*/
		typedef double DATA;

		const int MATERIAL_PROPERTY_NUM = 5;



		/*******************************/
		/*    Cross-free declaration   */
		/*******************************/
		class VolumeModel;
		class SquareSparseMatrix;
		class SceneRecorder;



		/**********************/
		/*    Data Structure  */
		/**********************/

		/* This struct is to represent the scene initial force */
		typedef struct SceneForce
		{
			int vmeshID;		// to indicate on which object this force will applicate
			int vertexID;		// on which point
			DATA intensity[3];	// the intensity of 3 directions
		}SceneForce;

		/*
		This class is used to cache all the points that represent a volume model.
		It will carry on some features used to increase the performance of the calculation.
		*/
		class PointPool
		{
			/* The constant which indicates how many parameters are used to discribe a point's behavior */
			static const int POINT_ARGU_NUM = 13;

			/* 
			The global information of each point.
			As this cordination could be interfered by the scene setting, we need a transformation when generating these
			cordinations.
			DATA[0:2] -- current postion
			DATA[3:5] -- velotity
			DATA[6:8] -- original position

			DATA[9:12] -- color

			The size of this array is decided by POINT_ARGU_NUM
			*/
			std::vector<DATA*> pointList;

			/* A list to indicate which points (by its index in the pointPool) are contacted by the "air"*/
			std::vector<bool> visiblePointList;


			/* 
			Index of tetrahedron, which allows to find the tetrahedrons correspondant with a given point.
			The order of the tetrahedron in the list is implictly the same as the order in the "pointList".
			"std::list<int>" signifies the tetrahedron list of the given point, which order is not important
			*/
			std::vector<std::list<int>*> tetrahedronIndexList;

			/**/
			char nodeFileName[256];


		public:
			PointPool(const char* nodeFile, const char* faceFile, DATA speed[]);

			void updateTetrahedraIndex(int pointIndex, int tetIndex);

			int getPointCount();

			DATA* getPointInfo(int index);

			void fillVector(DATA* V, DATA* XU);

			void feedBackVector(DATA* V, DATA deltaTime);

			void showInfo(int round);

			// temporal

			void resolveConflit();

			// -- end temporal

			char* output(char* dir, int modelID);

		};


		/* 
		The basic class involved in our system. 
		Generally it contains the information of its 4 points. In our system, it is costly to create an instance 
		for a point, and it will increase the calculation performation if we present them by some matrix.
		*/
		class Tetrahedron
		{
			static const int TET_PARA_NUM = 9;

			/* the index (replica) */
			int id;

			/* The index of its 4 points, which ordre implicts the <i,j,m,p> */
			int pointIndex[4];

			/* The reference of the PointPool which belongs to the whole volume model */
			PointPool* pointPoolRef;

			/* The reference of the correspendant volume model */
			VolumeModel* volumeModelRef;

			/* The reference of the constants */
			DATA* constants;

			/* The mass */
			DATA mass;
			

			/* The stiffness matrix for this element */
			//DATA stiffnessMatrix[12][12];

			/* The mass matrix for this element */
			//DATA massMatrix[12][12];

			/* The externe force vector for this element */
			//DATA forceVector[12];

		public:
			Tetrahedron(int id, const int pointIndex[], DATA constants[], PointPool* pointPoolRef, VolumeModel* volumeModelRef);

			void fillMatrix(SquareSparseMatrix* K, SquareSparseMatrix* M, DATA* F, DATA gravity[]);

			int* getPointIndex();

			//temporal
			void fillForceList(std::list<SceneForce> *fl, DATA deltaTime);

			//-- end temporal

		};

		class FacePool
		{
			std::vector<int*> faceList;

			int oldFaceCount;

			char oldOutputFileName[256];

		public:
			FacePool(const char* faceFile);

			char* output(char* dir, int modelID);
		};

		class TetrahedronPool
		{
			/* 
			The list of tetrahedron, using STL::vector, for the reason that the quantity of tetrahedron is changing and
			we need eventually access the element by index
			*/
			std::vector<Tetrahedron*> tetList;

			DATA* constants;

			int oldTetCount;

			char oldOutputFileName[256];

			/* some index to accelerate the collision detection */

			/* undecided yet */

		public:
			TetrahedronPool(const char* eleFile, DATA constants[], PointPool* pointPoolRef, VolumeModel* volumeModelRef);

			void addTetrahedron(Tetrahedron tet);

			void fillMatrix(SquareSparseMatrix* K, SquareSparseMatrix* M, SquareSparseMatrix* C, DATA* F, DATA gravity[]);

			Tetrahedron* getTetrahedron(int index);

			char* output(char* dir, int modelID);

			
			//temporal
			void fillForceList(std::list<SceneForce> *fl, DATA deltaTime);

			//-- end temporal
		};


		/* 
		This class represents an object with a discription of volume.
		It is the core data structure in our system.
		*/
		class VolumeModel
		{
			/* The constant which indicates how many material-sensible parameter are used for a tetrahedron */
		public :
			static const int TET_PARA_NUM = 20;

			static const int VM_MET_NUM = 5;

		private:
			static int ID_COUNT;

		private:

			int id;

			/* */
			PointPool* pointPool;

			/* */
			FacePool* facePool;

			/* */
			TetrahedronPool* tetPool;

			/* The stiffness matrix */
			SquareSparseMatrix* K;

			/* The mass matrix */
			SquareSparseMatrix* M;

			/* The damping matrix */
			SquareSparseMatrix* C;

			/* */
			DATA deltaTime;

			/* Time passed (accumulated) */
			DATA currentTime;

			/**********/
			/* VECTOR */
			/**********/

			int vecSize;

			/* The force matrix (vector) */
			DATA* F;

			/* The vitesse matrix (vector) */
			DATA* V;

			/* The position matrix (vector) */
			DATA* X;

			/* The deplacement matrix (vector), always = X - U */
			DATA* XU;

			/*
			0 - Lambda
			1 - u
			2 - alpha
			3 - beta
			4 - density
			*/
			DATA materialPropriety[VM_MET_NUM];

			/* 
			0 - A1 = u / (1 - u)
			1 - A2 = (1 - 2u) / (2 - 2u)
			2 - A3 = (1 - u) / ((1 + u)(1 - 2u))
			3 - A3 / 6
			4 - density
			5 - density / 60
			6 - density / 120
			7 - alpha	(C = alpha * K + beta * M)
			8 - beta
			*/
			DATA constants[TET_PARA_NUM];

		public:
			VolumeModel(const char* nodeFile, const char* faceFile, const char* eleFile, DATA speed[], const DATA materialPropriety[]);

			void fillForceVector(int pointIndex, DATA force[]);

			void fillMatrix(DATA gravity[]);

			void setDeltaTime(DATA t);

			void calculate();

			void fillVector();

			void feedBackVector();

			void generateFrame(int round);

			DATA* getPoint(int index);

			Tetrahedron* getTetrahedron(int index);

			void output(FILE* sceneFile, char* objectFileDir, int frameID);

			// temporelle function
			void fillForceList(std::list<SceneForce> *fl);

			void resolveConflit();

			// -- end temporelle function


		private:
			
		};

		

		class MotivationEvent
		{
		public :
			virtual int getEventType() = 0;

			static const int TYPE_ForceEvent = 1;
			static const int TYPE_ForceFieldEvent = 2;
		};

		class ForceEvent: public MotivationEvent
		{
		public:
			SceneForce sf;

			ForceEvent(SceneForce sf);

			int getEventType();
		};

		class ForceFieldEvent : public MotivationEvent
		{
		public:
			/* indicate whether in the current step the gravity is actived or not*/
			bool isGravityActive;

			/* the value of gravity if the option MotivationEvent::isGravityActive is available*/
			DATA gravityValue[3];

			/* The timestep for the current round */
			DATA deltaTime;

			ForceFieldEvent(bool isActive, DATA intensity[], DATA deltaT);

			int getEventType();
		};

		class MotivationReport
		{
			VolumeModel* modelRef;

		public:
			MotivationReport(VolumeModel* modelRef);

			VolumeModel* getModel();

			/* need to be refined by Yohann */
		};

		/*
		This class represents a controller of an object. It has a event waiting queue in which the event from the <yohan::base::SceneController> indicates 
		the change of context (application of a force and the evaluation by delta-t). In order to make this controller
		be able to run in both a serialized and a parallel environment, its only job is to handle the events, but lucky the
		waiting queue could be non-thread-safe.
		When this controller has handled all the event in one round, it will generate a report <yohan::base::MotivationReport> to
		the SceneController for that it could then detect the collisions and reponse them. For the same reason of flexibility, the
		VolumeModelController will consider its report as an event and put it into another queue waiting for the SceneController come
		to gather it.
		*/
		class VolumeModelController
		{
			std::queue<MotivationEvent*> eventQueue;

			std::queue<MotivationReport> reportQueue;

			VolumeModel* model;

			int round;

		public:
			/* called definitely by <yohan::base::SceneController> */
			void sendEvent(MotivationEvent* e);

			/* */
			void evoluteModel();

			/* called definitely by <yohan::base::SceneController> */
			MotivationReport retrieveReport();

			/* */
			VolumeModelController(const char* nodeFile, const char* faceFile, const char* eleFile, DATA speed[], const DATA materialPropriety[]);

		};

		
		/* The top class of the physic engine, which is dedicated to simulate the whole scene */
		class SceneController
		{
			/* The list that contains all the "reference" of VolumeModelController which could independantly evoluate */
			std::vector<VolumeModelController> vmcList;

			/* */
			SceneRecorder* recorder;

			/* */
			DATA gravity[3];

			/* */
			std::list<struct SceneForce> forceList;

		public:

			/* Create an instance of SceneController from an existing Scene Configuration File */
			/* The cordinate transformation is done in this step */
			SceneController(char* filename);

			/* start to simulate */
			void simulate(char* filename);

		private:
			/* collision detection functions should be write here */

		};

		/* This class is used to record the scene with the most great possibility to gain some performance.*/
		/* As the output could be a file (our prototype) or other IO devices, this class should be rather named DiskSceneRecorder */
		class SceneRecorder
		{
			char dir[256];

			char sceneFileName[256];

			char frameFileName[256];

			int indexInFrame;

			int indexOfFrame;

			/**/
			FILE* sceneFile;

		public:
			/* To open/create a file for the recording */
			SceneRecorder(char* filename);

			/* to indicate a new frame start, with its absolute time in the simulated world */
			void newFrame(const DATA timeStamp);

			/* */
			void endFrame();

			/* record all the information necessary of the given model (which stands for an object) in the current frame*/
			void record(VolumeModel* model);

			/* end and flush all on the disk, add an index of shifting timing (for slow replay) */
			void endScene();
		};

		/*
		Abstract class to define the sparse matrix:
		As we have several form to store the sparse matrix and we are not so sure about the performance on the
		combination <format, solver>, we make this kind of matrix abstract to adapt the further optimisation
		*/ 
		class SquareSparseMatrix
		{
		public:
			static const int TYPE_SymmetricMumpsSquareSparse = 1;
			static const int TYPE_SymmetricIntelSquareSparse = 2;
			static const int TYPE_SymmetricMumpsSquareSparse2 = 3;

		protected:
			virtual ~SquareSparseMatrix() = 0;

		public:
			/* sm(i,j) = value */
			inline virtual void setValue(int i, int j, DATA value) = 0;

			/* sm(i,j) += value */
			inline virtual void addAndSetValue(int i, int j, DATA value) = 0;

			inline virtual int getType() = 0;

			inline virtual int getSize() = 0;

			inline virtual int getOrder() = 0;

			inline virtual DATA getValue(int i, int j) = 0;

			/* clear all the datas for reuse without reallocate */
			virtual void clear() = 0;

			virtual void changeOrder(int newOrder) = 0;

			/* this(i,j) = alpha * x(i,j) + beta * y(i,j), and we use the smaller one as the iterator */
			virtual void calcul_AXplusBY(DATA alpha, SquareSparseMatrix* X, DATA beta, SquareSparseMatrix* Y) = 0;

			/* this(i,j) = this(i,j) * alpha */
			virtual void calcul_AX(DATA alpha) = 0;

			/* this(i,j) = this(i,j) + alpha * X */
			virtual void calcul_plusAX(SquareSparseMatrix* X, DATA alpha) = 0;

			/* RES = RES + M * VEC */
			/* res(i) += M(i,1) * vec(1) + M(i,2) * vec(2) + ... */
			virtual void calcul_PlusMatrixVec(DATA* vec, DATA* res) = 0;

			/* RES = RES - alpha * M * VEC */
			/* res(i) -= (M(i,1) * vec(1) + M(i,2) * vec(2) + ...) * alpha */
			virtual void calcul_MinusMatrixVec(DATA* vec, DATA* res, DATA alpha) = 0;
		};

		
		/**********************/
		/*    Utilities       */
		/**********************/

		void fetalError();

		void createDir(const char* dirName);
		
	}
}
