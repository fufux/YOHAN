#pragma once

#include "stdafx.h"
#include <Eigen/Eigen>
USING_PART_OF_NAMESPACE_EIGEN
#include "Point.h"

class Volume;

class Tetrahedron
{
public:
	Tetrahedron(int id, Volume* volume, vector<Point*> points);
	// To construct a fake tetrahedron representing y=0 plan with id=-1
	Tetrahedron();
	~Tetrahedron(void);

	vector<Point*>& getPoints();
	Matrix3d& getBeta();
	vector<Matrix3d>& getCoreJacobian();
	double getMass();
	int getID();
	Volume* getVolume();
	Vector3d& getN(int i);
	double getVol();

	void computeCoreJacobian();
	void computeBeta();

	bool hasPointInCommonWith(Tetrahedron* other);

	/* Added by Ning, for fracture */
	void setStress(Matrix3d& stress);
	void setQ(Matrix3d& q);
	void retrieveEigenOfStress(Vector3d& eigenValue, Matrix3d& eigenVector);
	void setTensileForce(int index, Matrix<double, 3, 1>& force);
	void setCompressiveForce(int index, Matrix<double, 3, 1>& force);
	Matrix3d& getQ();
	Matrix<double, 3, 1>& getTensileForce(int index);
	Matrix<double, 3, 1>& getCompressiveForce(int index);

	void remesh(Point* orginal, Point* replica, Matrix<double, 3, 1>& nvector, vector<Point*>& pointList);
	void remesh2(Point* orginal, Matrix<double, 3, 1>& nvector, vector<Point*>& pointList);
	/* END -- Added by Ning, for fracture */

private:
	int id;

	Volume* volume;
	
	// The volume of the Tetrahedron
	double vol;

	// list of the four points making this tetrahedron
	vector<Point*> points; // array of size 4
	Vector3d n[4]; // ni
	void computeN();

	// this is the beta in equation (1) of main article
	Matrix3d beta; // 3x3

	/* "The core of (2), consisting of inner and outer normal products,
	does not change as the mesh deforms and can be cached
	for each element." (see main article)
	*/
	vector<Matrix3d> coreJacobian; // 16x3x3

	// mass of this tetrahedron
	double mass;

	/* Added by Ning, for fracture */

	//stress
	Matrix3d stress;

	//Q
	Matrix3d Q;

	//tensile Force
	Matrix<double, 3, 1> tensileForce[4];

	//compressive Force
	Matrix<double, 3, 1> compressiveForce[4];

	
	void remeshByFaceWithTwoNewPoints(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5);
	void selfRemeshByFaceWithTwoNewPoints_Positive(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* replica);
	void selfRemeshByFaceWithTwoNewPoints_Negative(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* replica);
	void remeshByFaceWithOneNewPoint(Point* p1, Point* p2, Point* p3, Point* p4);
	void selfRemeshByFaceWithOneNewPoint_Positive(Point* p1, Point* p2, Point* p3, Point* p4, Point* replica);
	void selfRemeshByFaceWithOneNewPoint_Negative(Point* p1, Point* p2, Point* p3, Point* p4, Point* replica);
	void remeshByEdgeWithOneNewPoint(Point* p2, Point* p3, Point* p4);
	Tetrahedron* faceNeighour(Point* a, Point* b, Point* c);
	void edgeNeighour(Point* a, Point* b, Tetrahedron* faceNeighbourA, Tetrahedron* faceNeighbourB, std::vector<Tetrahedron*>& commonAB);
	
	Point* getFacePoint(Point* a, Point* b, Point* c);
	bool isPositiveRemesh(Point* p1, Point* p2, Matrix<double, 3, 1>& nvector);


	/* END -- Added by Ning, for fracture */
};
