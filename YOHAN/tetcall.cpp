///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// tetcall.cxx                                                               //
//                                                                           //
// An example of how to call TetGen from another program by using the data   //
// type "tetgenio" and function "tetrahedralize()" of TetGen libaray.        //
//                                                                           //
// In order to run this example, you need the library of TetGen, you can get //
// the source code as well as the user's manul of TetGen from:               //
//                                                                           //
//            http://tetgen.berlios.de/index.html                            //
//                                                                           //
// Section 2 of the user's manual contains the information of how to compile //
// TetGen into a libaray.                                                    //
//                                                                           //
// The geometry used in this example (illustrated in Section 3.3 .1, Figure  //
// 12 of the user's manual) is a rectangluar bar consists of 8 points and 6  //
// facets (which are all rectangles). In additional, there are two boundary  //
// markers defined on its facets.                                            //
//                                                                           //
// This code illustrates the following basic steps:                          //
//   - at first create an input object "in", and set data of the geometry    //
//     into it.                                                              //
//   - then call function "tetrahedralize()" to create a quality mesh of the //
//     geometry with output in another object "out".                         //
// In addition, It outputs the geometry in the object "in" into two files    //
// (barin.node and barin.poly), and outputs the mesh in the object "out"     //
// into three files (barout.node, barout.ele, and barout.face).  These files //
// can be visualized by TetView.                                             //
//                                                                           //
// To compile this code into an executable program, do the following steps:  //
//   - compile TetGen into a library named "libtet.a" (see Section 2.1 of    //
//     the user's manula for compiling);                                     //
//   - Save this file into the same directory in which you have the files    //
//     "tetgen.h" and "libtet.a";                                            //
//   - compile it using the following command:                               //
//                                                                           //
//     g++ -o test tetcall.cxx -L./ -ltet                                    //
//                                                                           //
//     which will result an executable program named "test".                 //
//                                                                           //
// Please send your quesions, comments to Hang Si <si@wias-berlin.de>        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// main()  Create and refine a mesh using TetGen library.                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "tetgen.h" // Defined tetgenio, tetrahedralize().
#include "stdafx.h"
#include <fstream>
#include <iostream>
#include "string.h"
#include <exception>
using namespace std;

extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int tetrahedralizeFile (char *fileToOpen)
{

	tetgenio in, out; // Input and output data for tetgen //
	tetgenio::facet *f; // Auxiliary variable for the facets array setting //
	tetgenio::polygon *p; // Auxiliary variable for the facets array setting //
	int n; // Number of vertices
	int pointCpt, auxCpt; // Auxiliary variables //
	IMeshBuffer *newBuffer; // Buffer receiving the irrlicht data //
	video::S3DVertex *vertices; // Vertices of the mesh //
	u16 *indices; // Indices of the vertices of each polygon //
	int *hashMap; // Hash map for the indices of the vertices of each polygon //
	bool isAlreadyThere; // True if the point has already been added, false otherwise //
	REAL *tempPointList; // Temporary list of points //
	bool *toWrite;  // toWrite[i] is true if the i-th vertex is to add to the input of tetgen //
					// false otherwise //
	char loadPath[42] = "irrlicht/media/";
	char tetgenLoadPath[42] = "output/";
	char tetgenSavePath[42] = "output/";
	char extension[] = ".3ds";
	char name[] = "bigcube";

		// Creating the paths used //
	strcat_s(loadPath, name);
	strcat_s(loadPath, extension);
	strcat_s(tetgenLoadPath, name);
	strcat_s(tetgenLoadPath, "in");
	strcat_s(tetgenSavePath, name);
	strcat_s(tetgenSavePath, "out");

		// Trying to load the file //
	newBuffer = smgr->getMesh(loadPath)->getMesh(0)->getMeshBuffer(0);

	/************************************************
	 *         Cube definition using Irrlicht       *
	 ************************************************/

/**************************************************************

	SMeshBuffer* buffer = new SMeshBuffer();

	// Create indices
	const u16 u[36] = {   0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6, 
            7,3,0,   7,8,9,   9,5,2,   3,8,5,   0,11,4,   0,10,7};

	buffer->Indices.set_used(36);

	for (u32 i=0; i<36; ++i)
		buffer->Indices[i] = u[i];


	// Create vertices
	video::SColor clr1(0,255,100,100);
	video::SColor clr2(0,100,100,255);

	buffer->Vertices.reallocate(8);

	buffer->Vertices.push_back(video::S3DVertex(0,0,0, -1,-1,-1, clr1, 0, 1));  //  0
	buffer->Vertices.push_back(video::S3DVertex(30,0,0,  1,-1,-1, clr2, 1, 1));  //  1
	buffer->Vertices.push_back(video::S3DVertex(30,30,0,  1, 1,-1, clr1, 1, 0));  //  2
	buffer->Vertices.push_back(video::S3DVertex(0,30,0, -1, 1,-1, clr2, 0, 0));  //  3
	buffer->Vertices.push_back(video::S3DVertex(30,0,30,  1,-1, 1, clr1, 0, 1));  //  4
	buffer->Vertices.push_back(video::S3DVertex(30,30,30,  1, 1, 1, clr2, 0, 0));  //  5
	buffer->Vertices.push_back(video::S3DVertex(0,30,30, -1, 1, 1, clr1, 1, 0));  //  6
	buffer->Vertices.push_back(video::S3DVertex(0,0,30, -1,-1, 1, clr2, 1, 1));  //  7
	buffer->Vertices.push_back(video::S3DVertex(0,30,30, -1, 1, 1, clr1, 0, 1));  //  8  6
	buffer->Vertices.push_back(video::S3DVertex(0,30,0, -1, 1,-1, clr2, 1, 1));  //  9  3
	buffer->Vertices.push_back(video::S3DVertex(30,0,30,  1,-1, 1, clr1, 1, 0));  //  10  4
	buffer->Vertices.push_back(video::S3DVertex(30,0,0,  1,-1,-1, clr2, 0, 0));  //  11  1
	
	
	SMesh* mesh = new SMesh;
	mesh->addMeshBuffer(buffer);
	buffer->drop();

**************************************************************/


	/************************************************
	 *       Adapting the mesh's data structure     *
	 *           so that TetGen can use it          *
	 ************************************************/

		// Making sure there is a buffer in the mesh //
	//n = mesh->getMeshBufferCount();

	//if (n == 0) {
	//	return -1;
	//}

		// Acquiring the data from the irrlicht mesh //
	//newBuffer = mesh->getMeshBuffer(0);
	vertices = (video::S3DVertex *) newBuffer->getVertices();
	indices = newBuffer->getIndices();

		// Creating the temporary point list and initializing the auxiliary variables //
	n = newBuffer->getVertexCount();
	hashMap = new int[n];
	tempPointList = new REAL[n * 3];
	toWrite = new bool[n];
	pointCpt = 0;

		// Loop for redundant vertices removal //
	for (int i = 0; i < n; i++) {
		isAlreadyThere = false;
			// Checking if some points appear more than once //
			// thus violating the PLC condition //
		for (int j = 0; j < pointCpt; j++) {
			if (vertices[i].Pos.X == tempPointList[3*j] &&
				vertices[i].Pos.Y == tempPointList[3*j+1] &&
				vertices[i].Pos.Z == tempPointList[3*j+2]) {
						// Updating the auxiliary variables if it is the case //
					hashMap[i] = j;
					isAlreadyThere = true;
					toWrite[i] = false;
					//break;
			}
		}
		if (!isAlreadyThere) {
				// Adding the point if it's not already there //
			tempPointList[3*pointCpt]   = (REAL) vertices[i].Pos.X;
			tempPointList[3*pointCpt+1] = (REAL) vertices[i].Pos.Y;
			tempPointList[3*pointCpt+2] = (REAL) vertices[i].Pos.Z;
				// Updating the auxiliary variables if it is the case //
			hashMap[i] = pointCpt;
			toWrite[i] = true;
			pointCpt++;
		}
	}

		// Finally (!) creating the array of vertices (and related attributes) //
		// for tetgen's input // 
	in.firstnumber = 1;
	in.numberofpoints = pointCpt;
	in.pointlist = new REAL[in.numberofpoints * 3];
	auxCpt = 0;

		// Setting the array of vertices for tetgen's input //
	for (int i = 0; i < n; i++) {
		if (toWrite[i]) {
			in.pointlist[auxCpt] = (REAL) tempPointList[3*hashMap[i]];
			in.pointlist[auxCpt+1] = (REAL) tempPointList[3*hashMap[i]+1];
			in.pointlist[auxCpt+2] = (REAL) tempPointList[3*hashMap[i]+2];
			auxCpt += 3;
		}
	}

		// Creating the array of facets for tetgen's input //
		// We chose one facet per polygon //
	in.numberoffacets = newBuffer->getIndexCount()/3;
	in.facetlist = new tetgenio::facet[in.numberoffacets];
	in.facetmarkerlist = new int[in.numberoffacets];

		// Setting the array of facets for tetgen's input //
	for (int i = 0; i < (int) newBuffer->getIndexCount()/3; i++) {
		f = &in.facetlist[i];
		f->numberofpolygons = 1;
		f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
		f->numberofholes = 0;
		f->holelist = NULL;
		p = &f->polygonlist[0];
		p->numberofvertices = 3;
		p->vertexlist = new int[p->numberofvertices];
		p->vertexlist[0] = hashMap[indices[3*i]]+1;
		p->vertexlist[1] = hashMap[indices[3*i+1]]+1;
		p->vertexlist[2] = hashMap[indices[3*i+2]]+1;
			// Boundary markers //
		in.facetmarkerlist[i] = 1;
	}

	/************************************************
	 *      Processing the object using TeTgen       *
	 ************************************************/

	// Output the PLC to files '<object's name>in.node' and '<object's name>in.poly'.
	in.save_nodes(tetgenLoadPath);
	in.save_poly(tetgenLoadPath);

	// Tetrahedralize the PLC. Switches are chosen to read a PLC (p),
	//   do quality mesh generation (q) with a specified quality bound
	//   (1.414), and apply a maximum volume constraint (a0.1).

	//tetrahedralize("pq1.414a0.1", &in, &out);
	tetrahedralize("pq1.414a0.01", &in, &out);

	// Output mesh to files '<object's name>out.node', '<object's name>out.ele' and '<object's name>out.face'.
	out.save_nodes(tetgenSavePath);
	out.save_elements(tetgenSavePath);
	out.save_faces(tetgenSavePath);

	return 0;
}
