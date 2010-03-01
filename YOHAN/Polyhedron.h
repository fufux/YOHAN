#pragma once

#include "Tetrahedron.h"
#include "Face.h"

class Polyhedron
{
private:
	vector<Face*>* faces;
	vector<Tetrahedron*> parents;
	Vector3d center;
	double volume;

public:
	Polyhedron(void);
	Polyhedron(vector<Face*>* faces, Tetrahedron* t1, Tetrahedron* t2);
	~Polyhedron(void);

	// Returns the faces
	vector<Face*>* getFaces();

	// Returns faces which comes from designated tetrahedron
	vector<Face*>* getByOwner(int ind);

	// Calculates the volume of the polyhedron
	double calcVolume();

	// Calculates the direction of the collision response
	Vector3d* calcDir(int owner);

	// Calculates coordinates of the mass center of the polyhedron
	void calcCenter();
	Vector3d* getCenter();

	// Returns vertices of the designated tetrahedron
	vector<Point*>* getByParent(int parent);

	// Calculates barycentric coefficients for each point
	vector<double>* calcBarCoeff(vector<Point*>* pts);

	// Calculates the collision response force
	void collisionForces(double kerr, double kdmp, double kfrc);
};
