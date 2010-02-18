#pragma once

#include <math.h>
#include <vector>
#include <list>


/**

A Class representing a point or a vector depending of the case

*/
class CollisionPoint
{
private:
	double x;
	double y;
	double z;


public:
	// Constructs a Point/Vector with (0,0,0) as coordinates
	CollisionPoint(void);
	// Constructs a Point/Vector with given coordinates
	CollisionPoint(double _x, double _y,double _z);
	// Constructs a Point/Vector with given coordinates
	CollisionPoint(std::vector<double>*);
	// Constructs a Point/Vector with given coordinates
	CollisionPoint(double*);
	// Destructs a Point/Vector
	~CollisionPoint(void);
	// Returns x
	double getX();
	// Returns y
	double getY();
	// Returns z
	double getZ();
	// Calculates the distance between two Points
	static double distance(CollisionPoint*,CollisionPoint*);
	// Adds the coordinates of one point to another
	void add(CollisionPoint*);
	// Subtracts the coordinates of one point to another
	void minus(CollisionPoint*);
	// Multiplys the coordinates of one point to another
	void mul(double);
	// Calculates the volume define by the four points
	static double volOp(CollisionPoint*,CollisionPoint*,CollisionPoint*,CollisionPoint*);
	// Calculates the cross product (a1-a2)x(b1-b2)
	static CollisionPoint* vect(CollisionPoint* a1, CollisionPoint* a2, CollisionPoint* b1, CollisionPoint* b2);
	// Calculates the dot product of two "points"
	static double scal(CollisionPoint*,CollisionPoint*);
	// Calcultes the determinant x1 y1 z1
	//                           x2 y2 z2
	//                           x3 y3 z3
	static double det3(CollisionPoint*,CollisionPoint*,CollisionPoint*);
	// Calcultes the determinant 1 x1 y1 z1
	//                           1 x2 y2 z2
	//                           1 x3 y3 z3
	//                           1 x4 y4 z4
	static double det4(CollisionPoint*,CollisionPoint*,CollisionPoint*,CollisionPoint*);
	// Calculates the norm of the point
	double norm();
};
