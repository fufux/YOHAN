#include "PlayerFrame.h"

extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
extern scene::ICameraSceneNode* camera[CAMERA_COUNT];


// initialize frame counter
s32 PlayerFrame::totalLoadedFrames = 0;
bool PlayerFrame::last_was_volumic = false;
core::array<stringc> PlayerFrame::lastEleFileNames;
core::array<stringc> PlayerFrame::lastFaceFileNames;
core::array<scene::SMeshBuffer*> PlayerFrame::lastBuffers;


PlayerFrame::PlayerFrame(FrameInfo info, bool load_volumic)
{
	// increase total number of loaded frames
	PlayerFrame::totalLoadedFrames++;
	if (PLAYER_DEBUG || info.id % 50 == 0)
		device->getLogger()->log((stringw("Loading frame number ")+stringw(info.id)+L". Total of frames loaded : "+stringw(PlayerFrame::totalLoadedFrames)).c_str());

	core::array<stringc> _lastEleFileNames;
	core::array<stringc> _lastFaceFileNames;
	core::array<scene::SMeshBuffer*> _lastBuffers;

	// initialize some variables
	this->id = info.id;
	this->timestamp = info.timestamp;

	for (u32 o=0; o < info.nodefiles.size(); o++)
	{
		SMeshBuffer* buffer = new SMeshBuffer();
		IMeshSceneNode* node = NULL;
		stringc nodeFileName = info.nodefiles[o];
		stringc faceFileName = info.facefiles[o];
		stringc eleFileName = info.elefiles[o];
		stringc bbFileName = info.bbfiles[o];

		// LOADING
		ifstream innode;
		ifstream inele;
		ifstream inface;
		ifstream inbb;

		int nb_of_points, nb_of_tetrahedra, nb_of_faces;
		int p1, p2, p3, p4;
		double x, y, z;

		// used to know if an error occured in the while loop
		bool error = false;


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
		if (PLAYER_DEBUG)
			device->getLogger()->log((stringw("Loading ")+stringw(nb_of_points)+L" points from "+stringw(nodeFileName.c_str())+L"...").c_str());

		// default color
		video::SColor clr(255,255,0,0);

		// lets add the vertices to the buffer
		buffer->Vertices.reallocate( nb_of_points );

		// this is one line : innode >> index >> x >> y >> z;
		innode.read(reinterpret_cast < char * > (&x), sizeof(double));
		innode.read(reinterpret_cast < char * > (&y), sizeof(double));
		innode.read(reinterpret_cast < char * > (&z), sizeof(double));
		while (!innode.eof() && innode.good())// && (int)buffer->Vertices.size() < nb_of_points)
		{
			buffer->Vertices.push_back(video::S3DVertex((f32)x, (f32)y, (f32)z, 1,0,0, clr, 0,0));
			// this is one line : innode >> index >> x >> y >> z;
			innode.read(reinterpret_cast < char * > (&x), sizeof(double));
			innode.read(reinterpret_cast < char * > (&y), sizeof(double));
			innode.read(reinterpret_cast < char * > (&z), sizeof(double));
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





		if (load_volumic)
		{
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
					if (PLAYER_DEBUG)
						device->getLogger()->log("The ele file is already in memory. We do not need to reload it.");
					break;
				}
			}

			// if is not in memory, load it from file
			if (!PlayerFrame::last_was_volumic || eleLoadedIndex == -1 || PlayerFrame::lastBuffers[eleLoadedIndex]->Vertices.size() != buffer->Vertices.size())
			{
				if (eleLoadedIndex != -1 && PLAYER_DEBUG)
					device->getLogger()->log((stringc(PlayerFrame::lastBuffers[eleLoadedIndex]->Vertices.size())+
					" This is weird: the ele file which is already in memory don't have the same number of vertices than this one.").c_str());

				inele.open(eleFileName.c_str(), ios::in | ios::binary); // opens the ele file
				if (!inele || !inele.good())
				{
					device->getLogger()->log(( stringc("ERROR: This ele file could not be opened : ") + eleFileName ).c_str());
					buffer->drop();
					continue;
				}

				// first line data : inele >> nb_of_tetrahedra >> dim >> nb_of_attr;
				inele.read(reinterpret_cast < char * > (&nb_of_tetrahedra), sizeof(int));
				if (PLAYER_DEBUG)
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
				buffer->Indices.reallocate(PlayerFrame::lastBuffers[eleLoadedIndex]->Indices.size());
				buffer->Indices = PlayerFrame::lastBuffers[eleLoadedIndex]->Indices;
				for (u32 j=0; j < buffer->Vertices.size(); j++)
					buffer->Vertices[j].Normal = PlayerFrame::lastBuffers[eleLoadedIndex]->Vertices[j].Normal;
			}
			// -----------------------------------------------------------------------
		}
		else // load_volumic == false
		{
			// -----------------------------------------------------------------------
			// - FACES ---------------------------------------------------------------
			// -----------------------------------------------------------------------
			// at first we check if the ele file has not been already opened
			s32 faceLoadedIndex = -1;
			for (u32 e=0; e < PlayerFrame::lastFaceFileNames.size(); e++)
			{
				if (PlayerFrame::lastFaceFileNames[e] == faceFileName)
				{
					faceLoadedIndex = e;
					if (PLAYER_DEBUG)
						device->getLogger()->log("The face file is already in memory. We do not need to reload it.");
					break;
				}
			}
			// if is not in memory, load it from file
			if (PlayerFrame::last_was_volumic || faceLoadedIndex == -1 || PlayerFrame::lastBuffers[faceLoadedIndex]->Vertices.size() != buffer->Vertices.size())
			{
				inface.open(faceFileName.c_str(), ios::in | ios::binary); // opens the face file
				if (!inface || !inface.good())
				{
					device->getLogger()->log(( stringc("ERROR: This face file could not be opened : ") + faceFileName ).c_str());
					buffer->drop();
					continue;
				}

				// first line data : inface >> nb_of_tetrahedra >> dim >> nb_of_attr;
				inface.read(reinterpret_cast < char * > (&nb_of_faces), sizeof(int));
				if (PLAYER_DEBUG)
					device->getLogger()->log((stringw("Loading ")+stringw(nb_of_faces)+L" faces from "+stringw(faceFileName.c_str())+L"...").c_str());

				// we should have at least one face
				if (nb_of_faces < 1)
				{
					buffer->drop();
					continue;
				}

				// lets add the indices to the buffer
				buffer->Indices.set_used( (u32)(3 * nb_of_faces) );
				u32 i = 0;

				// this is one line : inface >> index >> p1 >> p2 >> p3;
				inface.read(reinterpret_cast < char * > (&p1), sizeof(int));
				inface.read(reinterpret_cast < char * > (&p2), sizeof(int));
				inface.read(reinterpret_cast < char * > (&p3), sizeof(int));
				while (!inface.eof() && inface.good())
				{
					// check if we are not out of bounds
					if (i > (u32)(3 * nb_of_faces - 3) || p1 > nb_of_points || p2 > nb_of_points || p3 > nb_of_points)
					{
						device->getLogger()->log("ERROR: the face file does not seem to be valid. ");
						buffer->drop();
						error = true;
						break;
					}

					// add 1 polygon per face.
					s32 ajust_index = 0;
					buffer->Indices[(u32)(i+0)] = (u32)(p1 + ajust_index);
					buffer->Indices[(u32)(i+1)] = (u32)(p3 + ajust_index);
					buffer->Indices[(u32)(i+2)] = (u32)(p2 + ajust_index);
					core::triangle3df t(
						buffer->Vertices[(u32)(p1 + ajust_index)].Pos,
						buffer->Vertices[(u32)(p3 + ajust_index)].Pos,
						buffer->Vertices[(u32)(p2 + ajust_index)].Pos);
					i += 3;

					buffer->Vertices[(u32)(p1 + ajust_index)].Normal = t.getNormal();
					buffer->Vertices[(u32)(p2 + ajust_index)].Normal = t.getNormal();
					buffer->Vertices[(u32)(p3 + ajust_index)].Normal = t.getNormal();

					// this is one line : inface >> index >> p1 >> p2 >> p3;
					inface.read(reinterpret_cast < char * > (&p1), sizeof(int));
					inface.read(reinterpret_cast < char * > (&p2), sizeof(int));
					inface.read(reinterpret_cast < char * > (&p3), sizeof(int));
				}

				inface.close();
				inface.clear();
			}
			else // we do not need to reload from the file !
			{
				buffer->Indices.reallocate(PlayerFrame::lastBuffers[faceLoadedIndex]->Indices.size());
				buffer->Indices = PlayerFrame::lastBuffers[faceLoadedIndex]->Indices;
				for (u32 j=0; j < buffer->Vertices.size(); j++)
					buffer->Vertices[j].Normal = PlayerFrame::lastBuffers[faceLoadedIndex]->Vertices[j].Normal;
			}
			// -----------------------------------------------------------------------
		}

		if (error)
			continue;

		// -----------------------------------------------------------------------
		// - BOUNDINGBOXES -------------------------------------------------------
		// -----------------------------------------------------------------------
		if (bbFileName.size() > 0)
		{
			inbb.open(bbFileName.c_str(), ios::in | ios::binary); // opens the nodes file
			if (!inbb || !inbb.good())
			{
				device->getLogger()->log(( stringc("ERROR: This bb file could not be opened : ") + bbFileName ).c_str());
				buffer->drop();
				continue;
			}

			// default color
			video::SColor clr(255,0,255,0);

			// lets add the vertices to the buffer
			SMeshBuffer* bb_buffer = new SMeshBuffer();
			double x1, y1, z1, x2, y2, z2;
			int bb_offset = 0;

			inbb.read(reinterpret_cast < char * > (&x1), sizeof(double));
			inbb.read(reinterpret_cast < char * > (&y1), sizeof(double));
			inbb.read(reinterpret_cast < char * > (&z1), sizeof(double));
			inbb.read(reinterpret_cast < char * > (&x2), sizeof(double));
			inbb.read(reinterpret_cast < char * > (&y2), sizeof(double));
			inbb.read(reinterpret_cast < char * > (&z2), sizeof(double));
			while (!inbb.eof() && inbb.good())
			{
				bb_buffer->Vertices.push_back(video::S3DVertex((f32)x1, (f32)y1, (f32)z1, -1,-1,-1, clr, 0,0));//0
				bb_buffer->Vertices.push_back(video::S3DVertex((f32)x2, (f32)y1, (f32)z1, 1,-1,-1, clr, 0,0));//1
				bb_buffer->Vertices.push_back(video::S3DVertex((f32)x2, (f32)y1, (f32)z2, 1,-1,1, clr, 0,0));//2
				bb_buffer->Vertices.push_back(video::S3DVertex((f32)x1, (f32)y1, (f32)z2, -1,-1,1, clr, 0,0));//3
				bb_buffer->Vertices.push_back(video::S3DVertex((f32)x1, (f32)y2, (f32)z1, -1,1,-1, clr, 0,0));//4
				bb_buffer->Vertices.push_back(video::S3DVertex((f32)x2, (f32)y2, (f32)z1, 1,1,-1, clr, 0,0));//5
				bb_buffer->Vertices.push_back(video::S3DVertex((f32)x2, (f32)y2, (f32)z2, 1,1,1, clr, 0,0));//6
				bb_buffer->Vertices.push_back(video::S3DVertex((f32)x1, (f32)y2, (f32)z2, -1,1,1, clr, 0,0));//7

				bb_buffer->Indices.push_back((u32)(bb_offset+0));
				bb_buffer->Indices.push_back((u32)(bb_offset+1));
				bb_buffer->Indices.push_back((u32)(bb_offset+2));

				bb_buffer->Indices.push_back((u32)(bb_offset+0));
				bb_buffer->Indices.push_back((u32)(bb_offset+2));
				bb_buffer->Indices.push_back((u32)(bb_offset+3));

				bb_buffer->Indices.push_back((u32)(bb_offset+3));
				bb_buffer->Indices.push_back((u32)(bb_offset+2));
				bb_buffer->Indices.push_back((u32)(bb_offset+6));

				bb_buffer->Indices.push_back((u32)(bb_offset+3));
				bb_buffer->Indices.push_back((u32)(bb_offset+6));
				bb_buffer->Indices.push_back((u32)(bb_offset+7));

				bb_buffer->Indices.push_back((u32)(bb_offset+7));
				bb_buffer->Indices.push_back((u32)(bb_offset+6));
				bb_buffer->Indices.push_back((u32)(bb_offset+5));

				bb_buffer->Indices.push_back((u32)(bb_offset+7));
				bb_buffer->Indices.push_back((u32)(bb_offset+5));
				bb_buffer->Indices.push_back((u32)(bb_offset+4));

				bb_buffer->Indices.push_back((u32)(bb_offset+0));
				bb_buffer->Indices.push_back((u32)(bb_offset+1));
				bb_buffer->Indices.push_back((u32)(bb_offset+4));

				bb_buffer->Indices.push_back((u32)(bb_offset+1));
				bb_buffer->Indices.push_back((u32)(bb_offset+5));
				bb_buffer->Indices.push_back((u32)(bb_offset+4));

				bb_buffer->Indices.push_back((u32)(bb_offset+1));
				bb_buffer->Indices.push_back((u32)(bb_offset+2));
				bb_buffer->Indices.push_back((u32)(bb_offset+6));

				bb_buffer->Indices.push_back((u32)(bb_offset+1));
				bb_buffer->Indices.push_back((u32)(bb_offset+6));
				bb_buffer->Indices.push_back((u32)(bb_offset+5));

				bb_buffer->Indices.push_back((u32)(bb_offset+0));
				bb_buffer->Indices.push_back((u32)(bb_offset+3));
				bb_buffer->Indices.push_back((u32)(bb_offset+7));

				bb_buffer->Indices.push_back((u32)(bb_offset+0));
				bb_buffer->Indices.push_back((u32)(bb_offset+7));
				bb_buffer->Indices.push_back((u32)(bb_offset+4));

				bb_offset += 8;

				inbb.read(reinterpret_cast < char * > (&x1), sizeof(double));
				inbb.read(reinterpret_cast < char * > (&y1), sizeof(double));
				inbb.read(reinterpret_cast < char * > (&z1), sizeof(double));
				inbb.read(reinterpret_cast < char * > (&x2), sizeof(double));
				inbb.read(reinterpret_cast < char * > (&y2), sizeof(double));
				inbb.read(reinterpret_cast < char * > (&z2), sizeof(double));
			}

			inbb.close();
			inbb.clear();

			SMesh* bb_mesh = new SMesh;
			bb_mesh->addMeshBuffer(bb_buffer);
			IMeshSceneNode* bb_node = smgr->addMeshSceneNode( bb_mesh );
			bb_mesh->drop();
			if (bb_node)
			{
				bb_node->setVisible(false);
				bb_node->setMaterialFlag(EMF_LIGHTING, false);
				bb_node->setMaterialFlag(EMF_WIREFRAME, true);
				this->boundingBoxes.push_back( bb_node );
			}
		}
		// -----------------------------------------------------------------------


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
	PlayerFrame::last_was_volumic = load_volumic;


	// that's it !
	if (PLAYER_DEBUG)
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

double PlayerFrame::getTimestamp()
{
	return timestamp;
}