#include "PlayerFrame.h"
#include "Base.h" // because we need to know the type DATA

extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
extern scene::ICameraSceneNode* camera[CAMERA_COUNT];


// initialize frame counter
s32 PlayerFrame::totalLoadedFrames = 0;
core::array<stringc> PlayerFrame::lastEleFileNames;
core::array<stringc> PlayerFrame::lastFaceFileNames;
core::array<scene::SMeshBuffer*> PlayerFrame::lastBuffers;


PlayerFrame::PlayerFrame(FrameInfo info)
{
	// increase total number of loaded frames
	PlayerFrame::totalLoadedFrames++;
	device->getLogger()->log((stringw("Loading frame number ")+stringw(info.id)+L". Total of frames loaded : "+stringw(PlayerFrame::totalLoadedFrames)).c_str());

	core::array<stringc> _lastEleFileNames;
	core::array<stringc> _lastFaceFileNames;
	core::array<scene::SMeshBuffer*> _lastBuffers;

	// initialize some variables
	this->id = info.id;

	for (u32 o=0; o < info.nodefiles.size(); o++)
	{
		SMeshBuffer* buffer = new SMeshBuffer();
		IMeshSceneNode* node = NULL;
		stringc nodeFileName = info.nodefiles[o];
		stringc faceFileName = info.facefiles[o];
		stringc eleFileName = info.elefiles[o];

		// LOADING
		ifstream innode; // innode is like cin
		ifstream inele; // inele is like cin

		int nb_of_points, nb_of_tetrahedra;
		int p1, p2, p3, p4;
		yohan::base::DATA x, y, z;

		// -----------------------------------------------------------------------
		// - POINTS --------------------------------------------------------------
		// -----------------------------------------------------------------------
		innode.open(nodeFileName.c_str(), ios::in | ios::binary); // opens the nodes file
		if (!innode || !innode.good())
		{
			device->getLogger()->log(( stringc("ERROR: This node file could not be opened : ") + nodeFileName ).c_str());
			buffer->drop();
			continue;
		}

		// first line data : innode >> nb_of_points >> dim >> nb_of_attr >> boundary_marker;
		innode.read(reinterpret_cast < char * > (&nb_of_points), sizeof(int));

		// we should have at least one tetrahedra (4 points) and each point should have 3 coordinates
		if (nb_of_points > 65535 || nb_of_points < 4)
		{
			device->getLogger()->log("ERROR: a node file should not contain more than 65535 points and less than 4 points.");
			buffer->drop();
			continue;
		}
		device->getLogger()->log((stringw("Loading ")+stringw(nb_of_points)+L" points from "+stringw(nodeFileName.c_str())+L"...").c_str());

		// default color
		video::SColor clr(255,100,100,200);

		// lets add the vertices to the buffer
		buffer->Vertices.reallocate( nb_of_points );

		// this is one line : innode >> index >> x >> y >> z;
		innode.read(reinterpret_cast < char * > (&x), sizeof(yohan::base::DATA));
		innode.read(reinterpret_cast < char * > (&y), sizeof(yohan::base::DATA));
		innode.read(reinterpret_cast < char * > (&z), sizeof(yohan::base::DATA));
		while (!innode.eof() && innode.good())// && (int)buffer->Vertices.size() < nb_of_points)
		{
			buffer->Vertices.push_back(video::S3DVertex((f32)x, (f32)y, (f32)z, 0,0,0, clr, 0,0));
			// this is one line : innode >> index >> x >> y >> z;
			innode.read(reinterpret_cast < char * > (&x), sizeof(yohan::base::DATA));
			innode.read(reinterpret_cast < char * > (&y), sizeof(yohan::base::DATA));
			innode.read(reinterpret_cast < char * > (&z), sizeof(yohan::base::DATA));
		}

		innode.close();
		innode.clear();
		// -----------------------------------------------------------------------

		// lets check if verticies have been added well
		if (buffer->Vertices.size() != nb_of_points)
		{
			device->getLogger()->log("ERROR: the node file does not seem to be valid.");
			buffer->drop();
			continue;
		}


		// -----------------------------------------------------------------------
		// - TETRAHEDRAS ---------------------------------------------------------
		// -----------------------------------------------------------------------
		// at first we check if the ele file has not been already opened
		s32 eleLoadedIndex = -1;
		for (u32 e=0; e < PlayerFrame::lastEleFileNames.size(); e++)
		{
			if (PlayerFrame::lastEleFileNames[e] == eleFileName)
			{
				eleLoadedIndex = e;
				device->getLogger()->log("The ele file is already in memory. We do not need to reload it.");
				break;
			}
		}

		// used to know if an error occured in the while loop
		bool error = false;

		// if is not in memory, load it from file
		if (eleLoadedIndex == -1)
		{
			inele.open(eleFileName.c_str(), ios::in | ios::binary); // opens the ele file
			if (!inele || !inele.good())
			{
				device->getLogger()->log(( stringc("ERROR: This ele file could not be opened : ") + eleFileName ).c_str());
				buffer->drop();
				continue;
			}

			// first line data : inele >> nb_of_tetrahedra >> dim >> nb_of_attr;
			inele.read(reinterpret_cast < char * > (&nb_of_tetrahedra), sizeof(int));
			device->getLogger()->log((stringw("Loading ")+stringw(nb_of_tetrahedra)+L" tetrahedras from "+stringw(eleFileName.c_str())+L"...").c_str());

			// we should have at least one tetrahedra and each tetrahedra should have 4 points
			if (nb_of_tetrahedra < 1)
			{
				buffer->drop();
				continue;
			}

			// lets add the indices to the buffer
			buffer->Indices.set_used( (u32)(3 * 4 * nb_of_tetrahedra) );
			u32 i = 0;

			// this is one line : inele >> index >> p1 >> p2 >> p3 >> p4;
			inele.read(reinterpret_cast < char * > (&p1), sizeof(int));
			inele.read(reinterpret_cast < char * > (&p2), sizeof(int));
			inele.read(reinterpret_cast < char * > (&p3), sizeof(int));
			inele.read(reinterpret_cast < char * > (&p4), sizeof(int));
			while (!inele.eof() && inele.good())// && i < 3 * 4 * nb_of_tetrahedra - 12)
			{
				// check if we are not out of bounds
				if (i > (u32)(3 * 4 * nb_of_tetrahedra - 12) || p1 > nb_of_points || p2 > nb_of_points || p3 > nb_of_points || p4 > nb_of_points)
				{
					device->getLogger()->log("ERROR: the ele file does not seem to be valid. ");
					buffer->drop();
					error = true;
					break;
				}

				// add 4 polygons per tetrahedra. Not optimized !
				s32 ajust_index = 0;
				buffer->Indices[(u32)(i+0)] = (u32)(p1 + ajust_index);
				buffer->Indices[(u32)(i+1)] = (u32)(p3 + ajust_index);
				buffer->Indices[(u32)(i+2)] = (u32)(p2 + ajust_index);
				core::triangle3df t4(
					buffer->Vertices[(u32)(p1 + ajust_index)].Pos,
					buffer->Vertices[(u32)(p3 + ajust_index)].Pos,
					buffer->Vertices[(u32)(p2 + ajust_index)].Pos);
				i += 3;

				buffer->Indices[(u32)(i+0)] = (u32)(p1 + ajust_index);
				buffer->Indices[(u32)(i+1)] = (u32)(p2 + ajust_index);
				buffer->Indices[(u32)(i+2)] = (u32)(p4 + ajust_index);
				core::triangle3df t3(
					buffer->Vertices[(u32)(p1 + ajust_index)].Pos,
					buffer->Vertices[(u32)(p2 + ajust_index)].Pos,
					buffer->Vertices[(u32)(p4 + ajust_index)].Pos);
				i += 3;

				buffer->Indices[(u32)(i+0)] = (u32)(p3 + ajust_index);
				buffer->Indices[(u32)(i+1)] = (u32)(p4 + ajust_index);
				buffer->Indices[(u32)(i+2)] = (u32)(p2 + ajust_index);
				core::triangle3df t1(
					buffer->Vertices[(u32)(p3 + ajust_index)].Pos,
					buffer->Vertices[(u32)(p4 + ajust_index)].Pos,
					buffer->Vertices[(u32)(p2 + ajust_index)].Pos);
				i += 3;

				buffer->Indices[(u32)(i+0)] = (u32)(p1 + ajust_index);
				buffer->Indices[(u32)(i+1)] = (u32)(p4 + ajust_index);
				buffer->Indices[(u32)(i+2)] = (u32)(p3 + ajust_index);
				core::triangle3df t2(
					buffer->Vertices[(u32)(p1 + ajust_index)].Pos,
					buffer->Vertices[(u32)(p4 + ajust_index)].Pos,
					buffer->Vertices[(u32)(p3 + ajust_index)].Pos);
				i += 3;

				buffer->Vertices[(u32)(p1 + ajust_index)].Normal = t1.getNormal();
				buffer->Vertices[(u32)(p2 + ajust_index)].Normal = t2.getNormal();
				buffer->Vertices[(u32)(p3 + ajust_index)].Normal = t3.getNormal();
				buffer->Vertices[(u32)(p4 + ajust_index)].Normal = t4.getNormal();

				// this is one line : inele >> index >> p1 >> p2 >> p3 >> p4;
				inele.read(reinterpret_cast < char * > (&p1), sizeof(int));
				inele.read(reinterpret_cast < char * > (&p2), sizeof(int));
				inele.read(reinterpret_cast < char * > (&p3), sizeof(int));
				inele.read(reinterpret_cast < char * > (&p4), sizeof(int));
			}

			inele.close();
			inele.clear();
		}
		else // we do not need to reload from the file !
		{
			buffer->Indices = PlayerFrame::lastBuffers[eleLoadedIndex]->Indices;
		}
		// -----------------------------------------------------------------------


		if (error)
			continue;

		// lets recalculate the bounding box and create the mesh
		for (u32 j=0; j < buffer->Vertices.size(); ++j)
			buffer->BoundingBox.addInternalPoint(buffer->Vertices[j].Pos);

		SMesh* mesh = new SMesh;
		mesh->addMeshBuffer(buffer);
		mesh->recalculateBoundingBox();

		// here we go, lets create the node that will owns the mesh data
		node = smgr->addMeshSceneNode( mesh );
		mesh->drop();
		if (!node)
		{
			node = NULL;
			continue;
		}
		node->setVisible(false);
		//node->setMaterialFlag(video::EMF_WIREFRAME, true);

		this->nodes.push_back( node );

		// updating last
		_lastEleFileNames.push_back( eleFileName );
		_lastFaceFileNames.push_back( faceFileName );
		_lastBuffers.push_back( buffer );
	}

	// clean history and update it
	PlayerFrame::lastEleFileNames = _lastEleFileNames;
	PlayerFrame::lastFaceFileNames = _lastFaceFileNames;
	for (u32 e=0; e < lastBuffers.size(); e++)
	{
		if (_lastBuffers.binary_search(PlayerFrame::lastBuffers[e]) == -1)
		{
			PlayerFrame::lastBuffers[e]->drop();
		}
	}
	PlayerFrame::lastBuffers = _lastBuffers;


	// that's it !
	device->getLogger()->log("Loaded.");
}



PlayerFrame::~PlayerFrame(void)
{
	for (u32 i=0; i < nodes.size(); i++)
		nodes[i]->remove();
}

void PlayerFrame::display()
{
	for (u32 i=0; i < nodes.size(); i++)
		nodes[i]->setVisible(true);
}

void PlayerFrame::hide()
{
	for (u32 i=0; i < nodes.size(); i++)
		nodes[i]->setVisible(false);
}

core::array<scene::IMeshSceneNode*> PlayerFrame::getNodes()
{
	return nodes;
}

s32 PlayerFrame::getId()
{
	return id;
}