#include "CollisionTest.h"
#include <iostream>
#include "stdafx.h"

using namespace std;

CollisionTest::CollisionTest(void)
{
}

CollisionTest::~CollisionTest(void)
{
}

int _main(int argc, char* argv[])
{
	FILE *stdoutToStdStream = freopen( "cout.txt", "w", stdout );
	// Point creation
	CollisionPoint* p0 = new CollisionPoint();
	cout <<"point creation: "; 
	if(p0->getX()==0 && p0->getY()==0 && p0->getZ()==0){
		cout << "OK" <<"\n";
	}else{
		cout <<"not OK" <<"\n";
	}
	CollisionPoint* p1 = new CollisionPoint(1,0,0);
	CollisionPoint* p2 = new CollisionPoint(0,1,0);
	CollisionPoint* p3 = new CollisionPoint(0,0,1);
	
	//Facet creation
	std::vector<CollisionPoint>* vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*p0);
	vertices->push_back(*p1);
	vertices->push_back(*p2);
	int owner = 0;
	CollisionFace* f0 = new CollisionFace(vertices, owner);
	cout <<"facet creation: ";
	if((f0->size())==(vertices->size()) && (f0->getOwner())==owner){
		cout << "OK" <<"\n";
	}else{
		cout <<"not OK" <<"\n";
	}
	// Polyhedron creation
	vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*p0);
	vertices->push_back(*p3);
	vertices->push_back(*p1);
	CollisionFace* f1 = new CollisionFace(vertices, 1);
	vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*p1);
	vertices->push_back(*p3);
	vertices->push_back(*p2);
	CollisionFace* f2 = new CollisionFace(vertices, 1);
	vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*p2);
	vertices->push_back(*p3);
	vertices->push_back(*p0);
	CollisionFace* f3 = new CollisionFace(vertices, 1);
	std::vector<CollisionFace>* faces = new std::vector<CollisionFace>();
	faces->push_back(*f0);
	faces->push_back(*f1);
	faces->push_back(*f2);
	faces->push_back(*f3);
	CollisionPolyedron* p = new CollisionPolyedron(faces,1,owner);
	double vol = p->calcVolume();
	cout << "polyhedron volume: " << vol << "\n";
	CollisionPoint* center = p->calcCenter();
	cout << "polyhedron center: (" << center->getX() << "," << center->getY() << "," << center->getZ() << ")\n";
	CollisionPoint* dir = p->calcDir(0);
	cout << "polyhedron dir for 0: (" << dir->getX() << "," << dir->getY() << "," << dir->getZ() << ")\n";
	dir = p->calcDir(1);
	cout << "polyedron dir for 1: (" << dir->getX() << "," << dir->getY() << "," << dir->getZ() << ")\n";
	// cube creation
	CollisionPoint* c0 = new CollisionPoint(0,0,0);
	CollisionPoint* c1 = new CollisionPoint(1,0,0);
	CollisionPoint* c2 = new CollisionPoint(1,1,0);
	CollisionPoint* c3 = new CollisionPoint(0,1,0);
	CollisionPoint* c4 = new CollisionPoint(0,0,1);
	CollisionPoint* c5 = new CollisionPoint(1,0,1);
	CollisionPoint* c6 = new CollisionPoint(1,1,1);
	CollisionPoint* c7 = new CollisionPoint(0,1,1);
	vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*c0);
	vertices->push_back(*c1);
	vertices->push_back(*c2);
	vertices->push_back(*c3);
	CollisionFace* cf0 = new CollisionFace(vertices, 1);
	vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*c0);
	vertices->push_back(*c4);
	vertices->push_back(*c5);
	vertices->push_back(*c1);
	CollisionFace* cf1 = new CollisionFace(vertices, 1);
	vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*c1);
	vertices->push_back(*c5);
	vertices->push_back(*c6);
	vertices->push_back(*c2);
	CollisionFace* cf2 = new CollisionFace(vertices, 1);
	vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*c2);
	vertices->push_back(*c6);
	vertices->push_back(*c7);
	vertices->push_back(*c3);
	CollisionFace* cf3 = new CollisionFace(vertices, 1);
	vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*c3);
	vertices->push_back(*c7);
	vertices->push_back(*c4);
	vertices->push_back(*c0);
	CollisionFace* cf4 = new CollisionFace(vertices, 1);
	vertices = new std::vector<CollisionPoint>();
	vertices->push_back(*c7);
	vertices->push_back(*c6);
	vertices->push_back(*c5);
	vertices->push_back(*c4);
	CollisionFace* cf5 = new CollisionFace(vertices, 1);
	faces = new std::vector<CollisionFace>();
	faces->push_back(*cf0);
	faces->push_back(*cf1);
	faces->push_back(*cf2);
	faces->push_back(*cf3);
	faces->push_back(*cf4);
	faces->push_back(*cf5);
	CollisionPolyedron* cube = new CollisionPolyedron(faces,1,0);
	vol = cube->calcVolume();
	cout << "cube volume: " << vol << "\n";
	center = cube->calcCenter();
	cout << "cube center: (" << center->getX() << "," << center->getY() << "," << center->getZ() << ")\n";


	fclose(stdoutToStdStream);
	return 0;
}