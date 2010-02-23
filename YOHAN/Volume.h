#pragma once

#include "stdafx.h"
#include "Matrix.h"
#include "Point.h"
#include "Tetrahedron.h"
#include <Eigen/Eigen>
USING_PART_OF_NAMESPACE_EIGEN

using namespace matrix;


class Scene;


struct Material
{
	double lambda, mu, phi, psy, rho;
};


//
class Volume
{
public:
	Volume(int id, Scene* scene);
	~Volume(void);

	bool load(std::string nodeFile, std::string eleFile, std::string faceFile, Material material, double* v);
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

	void collisionBidon();

private:
	int id;

	Scene* scene;

	// this is the list of all points in this volume. Note this is redundent data.
	vector<Point*> points;

	// this is the list of all tetrahedra in this volume
	vector<Tetrahedron*> tetrahedra;

	// this is the list of all facets in this volume
	vector<int*> facets; // vector of int[3]

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
};
