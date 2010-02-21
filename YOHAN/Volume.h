#pragma once

#include "stdafx.h"
#include "Matrix.h"
#include "Point.h"
#include "Tetrahedron.h"


struct Material
{
	double lambda, mu, phi, psy, rho;
};


//
class Volume
{
public:
	Volume(int id);
	~Volume(void);

	bool load(std::string nodeFile, std::string eleFile, std::string faceFile, Material material, double* v);
	bool save(std::string nodeFile, std::string eleFile, std::string faceFile);

	// getters
	int getID();
	Material* getMaterial();
	matrix::SymmetricMumpsSquareSparseMatrix* getK();
	matrix::SymmetricMumpsSquareSparseMatrix* getM();
	matrix::SymmetricMumpsSquareSparseMatrix* getC();
	double* getForceField();
	vector<double> getForces();

private:
	int id;

	// this is the list of all points in this volume. Note this is redundent data.
	vector<Point*> points;

	// this is the list of all tetrahedra in this volume
	vector<Tetrahedron*> tetrahedra;

	// big matrices needed in the equation
	matrix::SymmetricMumpsSquareSparseMatrix* K;
	matrix::SymmetricMumpsSquareSparseMatrix* M;
	matrix::SymmetricMumpsSquareSparseMatrix* C;

	// material properties
	Material material;

	// intensity of force field on x,y & z
	double forceField[3];

	// intensity of forces on x,y & z. Use it like that : force i == {forces[i], forces[i+1], forces[i+2]}
	vector<double> forces;

	bool facesChanged;
	bool tetrahedraChanged;
};
