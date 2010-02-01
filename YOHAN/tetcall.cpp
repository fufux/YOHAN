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

int main_(int argc, char *argv[])
{

	/************************************************
	 *         Cube definition using Irrlicht       *
	 ************************************************/

	SMeshBuffer* buffer = new SMeshBuffer();

	// Create indices
	const u16 u[36] = {   0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6, 
            7,3,0,   7,6,3,   9,5,2,   9,8,5,   0,11,10,   0,10,7};

	buffer->Indices.set_used(36);

	for (u32 i=0; i<36; ++i)
		buffer->Indices[i] = u[i];


	// Create vertices
	video::SColor clr1(0,255,100,100);
	video::SColor clr2(0,100,100,255);

	buffer->Vertices.reallocate(12);

	buffer->Vertices.push_back(video::S3DVertex(0,0,0, -1,-1,-1, clr1, 0, 1));
	buffer->Vertices.push_back(video::S3DVertex(30,0,0,  1,-1,-1, clr2, 1, 1));
	buffer->Vertices.push_back(video::S3DVertex(30,30,0,  1, 1,-1, clr1, 1, 0));
	buffer->Vertices.push_back(video::S3DVertex(0,30,0, -1, 1,-1, clr2, 0, 0));
	buffer->Vertices.push_back(video::S3DVertex(30,0,30,  1,-1, 1, clr1, 0, 1));
	buffer->Vertices.push_back(video::S3DVertex(30,30,30,  1, 1, 1, clr2, 0, 0));
	buffer->Vertices.push_back(video::S3DVertex(0,30,30, -1, 1, 1, clr1, 1, 0));
	buffer->Vertices.push_back(video::S3DVertex(0,0,30, -1,-1, 1, clr2, 1, 1));
	buffer->Vertices.push_back(video::S3DVertex(0,30,30, -1, 1, 1, clr1, 0, 1));
	buffer->Vertices.push_back(video::S3DVertex(0,30,0, -1, 1,-1, clr2, 1, 1));
	buffer->Vertices.push_back(video::S3DVertex(30,0,30,  1,-1, 1, clr1, 1, 0));
	buffer->Vertices.push_back(video::S3DVertex(30,0,0,  1,-1,-1, clr2, 0, 0));
	
	
	SMesh* mesh = new SMesh;
	mesh->addMeshBuffer(buffer);
	buffer->drop();


	/************************************************
	 *        Adapting the cube data structure      *
	 *           so that TetGen can use it          *
	 ************************************************/
/*
	tetgenio in, out;
	tetgenio::facet *f;
	tetgenio::polygon *p;
	int i;
	Vertices vertices;

	in.firstnumber = 1;
	in.numberofpoints = buffer->Vertices.size();
	in.pointlist = new REAL[in.numberofpoints * 3];
	for (i = 0; i < in.numberofpoints; i++) {
		in.pointlist[i] = buffer->Vertices.
		in.pointlist[i+1] =
		in.pointlist[i+2] =
	}
*/
	/************************************************
	 *       Processing the cube using TeTgen       *
	 ************************************************/






	/************************************************
	 *                 Example code                 *
	 ************************************************/

  tetgenio in, out;
  tetgenio::facet *f;
  tetgenio::polygon *p;
  int i;

  // All indices start from 1.
  in.firstnumber = 1;

  in.numberofpoints = 8;
  in.pointlist = new REAL[in.numberofpoints * 3];
  in.pointlist[0]  = 0;  // node 1.
  in.pointlist[1]  = 0;
  in.pointlist[2]  = 0;
  in.pointlist[3]  = 2;  // node 2.
  in.pointlist[4]  = 0;
  in.pointlist[5]  = 0;
  in.pointlist[6]  = 2;  // node 3.
  in.pointlist[7]  = 2;
  in.pointlist[8]  = 0;
  in.pointlist[9]  = 0;  // node 4.
  in.pointlist[10] = 2;
  in.pointlist[11] = 0;
  // Set node 5, 6, 7, 8.
  for (i = 4; i < 8; i++) {
    in.pointlist[i * 3]     = in.pointlist[(i - 4) * 3];
    in.pointlist[i * 3 + 1] = in.pointlist[(i - 4) * 3 + 1];
    in.pointlist[i * 3 + 2] = 12;
  }

  in.numberoffacets = 6;
  in.facetlist = new tetgenio::facet[in.numberoffacets];
  in.facetmarkerlist = new int[in.numberoffacets];

  // Facet 1. The leftmost facet.
  f = &in.facetlist[0];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 1;
  p->vertexlist[1] = 2;
  p->vertexlist[2] = 3;
  p->vertexlist[3] = 4;
  
  // Facet 2. The rightmost facet.
  f = &in.facetlist[1];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 5;
  p->vertexlist[1] = 6;
  p->vertexlist[2] = 7;
  p->vertexlist[3] = 8;

  // Facet 3. The bottom facet.
  f = &in.facetlist[2];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 1;
  p->vertexlist[1] = 5;
  p->vertexlist[2] = 6;
  p->vertexlist[3] = 2;

  // Facet 4. The back facet.
  f = &in.facetlist[3];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 2;
  p->vertexlist[1] = 6;
  p->vertexlist[2] = 7;
  p->vertexlist[3] = 3;

  // Facet 5. The top facet.
  f = &in.facetlist[4];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 3;
  p->vertexlist[1] = 7;
  p->vertexlist[2] = 8;
  p->vertexlist[3] = 4;

  // Facet 6. The front facet.
  f = &in.facetlist[5];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 4;
  p->vertexlist[1] = 8;
  p->vertexlist[2] = 5;
  p->vertexlist[3] = 1;


  // Set 'in.facetmarkerlist'

  in.facetmarkerlist[0] = -1;
  in.facetmarkerlist[1] = -2;
  in.facetmarkerlist[2] = 0;
  in.facetmarkerlist[3] = 0;
  in.facetmarkerlist[4] = 0;
  in.facetmarkerlist[5] = 0;


  // Output the PLC to files 'cube.node' and 'cube.poly'.
  in.save_nodes("output/cube");
  in.save_poly("output/cube");

  // Tetrahedralize the PLC. Switches are chosen to read a PLC (p),
  //   do quality mesh generation (q) with a specified quality bound
  //   (1.414), and apply a maximum volume constraint (a0.1).

  tetrahedralize("pq1.414a0.1", &in, &out);

  // Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.
  out.save_nodes("output/barout");
  out.save_elements("output/barout");
  out.save_faces("output/barout");

  return 0;
}
