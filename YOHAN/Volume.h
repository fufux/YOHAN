#pragma once

#include "stdafx.h"
#include "Matrix.h"
#include "Point.h"
#include "Tetrahedron.h"
#include "BoundingBox.h"
#include <Eigen/Eigen>
USING_PART_OF_NAMESPACE_EIGEN

using namespace matrix;


class Scene;


struct Material
{
	double lambda, mu, phi, psy, rho;

	/* Added by Ning, for fracture */
	double toughness;
	/* END -- Added by Ning*/
};

/* Added by Ning */
struct Surface
{
	int pointIndex[3];
};
/* END -- Added by Ning*/


//
class Volume
{
public:
	Volume(int id, Scene* scene);
	~Volume(void);

	bool load(std::string nodeFile, std::string eleFile, std::string faceFile, Material material, double* pos, double* rot, double* v);
	vector<std::string> save(std::string dir);

	void evolve(double deltaT);
	void generateMAndForceField();

	// getters
	int getID();
	Material* getMaterial();
	matrix::SymmetricMumpsSquareSparseMatrix* getK();
	matrix::SymmetricMumpsSquareSparseMatrix* getM();
	matrix::SymmetricMumpsSquareSparseMatrix* getC();
	double* getForceField();
	double* getForces();
	vector<Tetrahedron*>* getTetrahedra();
	BoundingBox* getMasterBoundingBox();

	void collisionBidon();

	/* Added by Ning, for fracture */
	int calculFracture();
	int calculFracture2();

	void resetAll();

	/* END -- Added by Ning, for fracture */

private:
	int id;

	Scene* scene;

	BoundingBox* bb;

	// this is the list of all points in this volume. Note this is redundent data.
	vector<Point*> points;

	// this is the list of all tetrahedra in this volume
	vector<Tetrahedron*> tetrahedra;

	// this is the list of all facets in this volume
	vector<struct Surface*> surfaces; // vector of int[3]

	// big matrices needed in the equation
	matrix::SymmetricMumpsSquareSparseMatrix* K;
	matrix::SymmetricMumpsSquareSparseMatrix* M;
	matrix::SymmetricMumpsSquareSparseMatrix* C;

	void generateK();
	void generateC(); 
	void generateXU();
	void generateV();
	void generateF();

	// update all positions and velocities of all points in this volume
	void updateVolume(double deltaT);

	// big vectors needed in the equation
	double* xu;
	double* v;
	double* f;

	// material properties
	Material material;

	// intensity of force field on x,y & z
	double* forceField;

	// intensity of forces on x,y & z. Use it like that : force i == {forces[i], forces[i+1], forces[i+2]}
	double* forces;

	// these attributes are needed to prevent creation of same files
	bool facesChanged;
	bool tetrahedraChanged;
	std::string lastOutputFacesFileName;
	std::string lastOutputTetrahedraFileName;

	/* Added by Ning, for fracture */
	Point* replicaPointWithoutRemesh(Point* orginal, Matrix<double, 3, 1>& nvector, int replicaPointIndex);
	Point* replicaPointWithRemesh(Point* orginal, Matrix<double, 3, 1>& nvector, int replicaPointIndex);
	Point* replicaPointWithRemesh2(Point* orginal, Matrix<double, 3, 1>& nvector);

	int oldOrder;

	void calculTensileAndCompressiveOfTetrahedron();
	void calculTensileAndCompressOfPoint();
	int reassign();

	/* END -- Added by Ning, for fracture */
};
