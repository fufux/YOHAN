#include "Polyhedron.h"
#include <iostream>
#include "stdafx.h"


int _main(int argc, char* argv[])
{
	FILE *stdoutToStdStream = freopen( "cout.txt", "w", stdout );
	
	Tetrahedron tetrah = Tetrahedron();

	// Point creation
	Vector3d p0(0,0,0);
	cout <<"point creation: "; 
	if(p0[0]==0 && p0[1]==0 && p0[2]==0){
		cout << "OK" <<"\n";
	}else{
		cout <<"not OK" <<"\n";
	}
	Vector3d p1(1,0,0);
	Vector3d p2(0,1,0);
	Vector3d p3(0,0,1);
	
	//Facet creation
	vector<Vector3d*>* vertices = new vector<Vector3d*>();
	vertices->push_back(&p0);
	vertices->push_back(&p1);
	vertices->push_back(&p2);
	int owner = 0;
	Face* f0 = new Face(vertices, owner);
	cout <<"facet creation: ";
	if((f0->size())==(vertices->size()) && (f0->getOwner())==owner){
		cout << "OK" <<"\n";
	}else{
		cout <<"not OK" <<"\n";
	}
	// Polyhedron creation
	vertices = new vector<Vector3d*>();
	vertices->push_back(&p0);
	vertices->push_back(&p3);
	vertices->push_back(&p1);
	Face* f1 = new Face(vertices, 1);
	vertices = new vector<Vector3d*>();
	vertices->push_back(&p1);
	vertices->push_back(&p3);
	vertices->push_back(&p2);
	Face* f2 = new Face(vertices, 1);
	vertices = new vector<Vector3d*>();
	vertices->push_back(&p2);
	vertices->push_back(&p3);
	vertices->push_back(&p0);
	Face* f3 = new Face(vertices, 1);
	vector<Face*>* faces = new vector<Face*>();
	faces->push_back(f0);
	faces->push_back(f1);
	faces->push_back(f2);
	faces->push_back(f3);
	Polyhedron* p = new Polyhedron(faces,&tetrah,&tetrah);
	double vol = p->calcVolume();
	cout << "polyhedron volume: " << vol << "\n";
	Vector3d* center = p->getCenter();
	cout << "polyhedron center: (" << (*center)[0] << "," << (*center)[0] << "," << (*center)[0] << ")\n";
	Vector3d* dir = p->calcDir(0);
	cout << "polyhedron dir for 0: (" << (*dir)[0] << "," << (*dir)[0] << "," << (*dir)[0] << ")\n";
	dir = p->calcDir(1);
	cout << "polyhedron dir for 1: (" << (*dir)[0] << "," << (*dir)[0] << "," << (*dir)[0] << ")\n";
	//// cube creation
	//Vector3d c0(0,0,0);
	//Vector3d c1(1,0,0);
	//Vector3d c2(1,1,0);
	//Vector3d c3(0,1,0);
	//Vector3d c4(0,0,1);
	//Vector3d c5(1,0,1);
	//Vector3d c6(1,1,1);
	//Vector3d c7(0,1,1);
	//vertices = new std::vector<Vector3d>();
	//vertices->push_back(c0);
	//vertices->push_back(c1);
	//vertices->push_back(c2);
	//vertices->push_back(c3);
	//Face* cf0 = new Face(vertices, 1);
	//vertices = new std::vector<Vector3d>();
	//vertices->push_back(c0);
	//vertices->push_back(c4);
	//vertices->push_back(c5);
	//vertices->push_back(c1);
	//Face* cf1 = new Face(vertices, 1);
	//vertices = new std::vector<Vector3d>();
	//vertices->push_back(c1);
	//vertices->push_back(c5);
	//vertices->push_back(c6);
	//vertices->push_back(c2);
	//Face* cf2 = new Face(vertices, 1);
	//vertices = new std::vector<Vector3d>();
	//vertices->push_back(c2);
	//vertices->push_back(c6);
	//vertices->push_back(c7);
	//vertices->push_back(c3);
	//Face* cf3 = new Face(vertices, 1);
	//vertices = new std::vector<Vector3d>();
	//vertices->push_back(c3);
	//vertices->push_back(c7);
	//vertices->push_back(c4);
	//vertices->push_back(c0);
	//Face* cf4 = new Face(vertices, 1);
	//vertices = new std::vector<Vector3d>();
	//vertices->push_back(c7);
	//vertices->push_back(c6);
	//vertices->push_back(c5);
	//vertices->push_back(c4);
	//Face* cf5 = new Face(vertices, 1);
	//faces = vector<Face>();
	//faces->push_back(*cf0);
	//faces->push_back(*cf1);
	//faces->push_back(*cf2);
	//faces->push_back(*cf3);
	//faces->push_back(*cf4);
	//faces->push_back(*cf5);
	//Polyhedron* cube = new Polyhedron(faces,1,0);
	//vol = cube->calcVolume();
	//cout << "cube volume: " << vol << "\n";
	//center = cube->calcCenter();
	//cout << "cube center: (" << center->getX() << "," << center->getY() << "," << center->getZ() << ")\n";


	fclose(stdoutToStdStream);
	return 0;
}