#include "PlayerFrame.h"

extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* env;
extern scene::ICameraSceneNode* camera[CAMERA_COUNT];


s32 PlayerFrame::totalLoadedFrames = 0;
stringc PlayerFrame::lastEleFileName = "";
scene::SMeshBuffer* PlayerFrame::lastBuf = new SMeshBuffer();


PlayerFrame::PlayerFrame(s32 id, stringc nodeFile, stringc eleFile)
{
	// increase total number of loaded frames
	PlayerFrame::totalLoadedFrames++;
	device->getLogger()->log((stringw("Loading frame num ")+stringw(PlayerFrame::totalLoadedFrames)).c_str());

	// initialize some variables
	this->id = id;
	this->nodeFile = nodeFile;
	this->eleFile = eleFile;
	this->buffer = new SMeshBuffer();
	this->node = NULL;

	// LOADING
	ifstream indata; // indata is like cin
	ifstream inele; // inele is like cin

	u32 nb_of_points, nb_of_tetrahedra, dim, nb_of_attr, boundary_marker;
	u32 index, p1, p2, p3, p4;
	f32 x, y, z;

	// -----------------------------------------------------------------------
	indata.open(nodeFile.c_str()); // opens the nodes file
	if (!indata || !indata.good())
	{
		device->getLogger()->log(( stringc("ERROR: This node file could not be opened : ") + nodeFile ).c_str());
		buffer->drop();
		return;
	}

	// first line data
	indata >> nb_of_points >> dim >> nb_of_attr >> boundary_marker;
	if (nb_of_points > 65535)
	{
		device->getLogger()->log("ERROR: a node file should not contain more than 65535 points.");
		buffer->drop();
		return;
	}
	device->getLogger()->log((stringw("Loading ")+stringw(nb_of_points)+L" points from "+stringw(nodeFile.c_str())+L"...").c_str());

	// we should have at least one tetrahedra (4 points) and each point should have 3 coordinates
	if (nb_of_points < 4 || dim != 3)
	{
		buffer->drop();
		return;
	}

	// default color
	video::SColor clr(255,100,100,200);

	// lets add the vertices to the buffer
	buffer->Vertices.reallocate( nb_of_points );

	indata >> index >> x >> y >> z; // this is one line
	while (!indata.eof() && indata.good())
	{
		buffer->Vertices.push_back(video::S3DVertex(10*x+myRandom(-0.1f,0.1f), 10*y+myRandom(-0.1f,0.1f), 10*z+myRandom(-0.1f,0.1f), 1,1,1, clr, 0,0));
		indata >> index >> x >> y >> z; // this is one line
	}

	indata.close();
	// -----------------------------------------------------------------------

	// lets check if verticies have been added well
	if (buffer->Vertices.size() != nb_of_points)
	{
		device->getLogger()->log("ERROR: the node file does not seem to be valid.");
		buffer->drop();
		return;
	}


	// -----------------------------------------------------------------------
	// at first we check if the ele file has not been already opened
	if (eleFile != PlayerFrame::lastEleFileName || PlayerFrame::lastBuf->Indices.size() == 0)
	{
		PlayerFrame::lastEleFileName = eleFile; // store the name of this ele file as the last opened ele file
		inele.open(eleFile.c_str()); // opens the ele file
		if (!inele || !inele.good())
		{
			device->getLogger()->log(( stringc("ERROR: This ele file could not be opened : ") + eleFile ).c_str());
			buffer->drop();
			return;
		}

		// first line data
		inele >> nb_of_tetrahedra >> dim >> nb_of_attr;
		device->getLogger()->log((stringw("Loading ")+stringw(nb_of_tetrahedra)+L" tetrahedras from "+stringw(eleFile.c_str())+L"...").c_str());

		// we should have at least one tetrahedra and each tetrahedra should have 4 points
		if (nb_of_tetrahedra < 1 || dim != 4)
		{
			buffer->drop();
			return;
		}

		// lets add the indices to the buffer
		buffer->Indices.set_used( (u32)(3 * 4 * nb_of_tetrahedra) );
		u32 i = 0;

		inele >> index >> p1 >> p2 >> p3 >> p4; // this is one line
		while (!inele.eof() && inele.good())
		{
			// check if we are not out of bounds
			if (i > 3 * 4 * nb_of_tetrahedra - 12)
			{
				device->getLogger()->log("ERROR: the ele file does not seem to be valid. ");
				buffer->drop();
				return;
			}

			// add 4 polygons per tetrahedra. Not optimized !
			s32 ajust_index = -1;
			buffer->Indices[(u32)(i+0)] = (u32)(p1 + ajust_index);
			buffer->Indices[(u32)(i+1)] = (u32)(p2 + ajust_index);
			buffer->Indices[(u32)(i+2)] = (u32)(p3 + ajust_index);
			i += 3;

			buffer->Indices[(u32)(i+0)] = (u32)(p1 + ajust_index);
			buffer->Indices[(u32)(i+1)] = (u32)(p2 + ajust_index);
			buffer->Indices[(u32)(i+2)] = (u32)(p4 + ajust_index);
			i += 3;

			buffer->Indices[(u32)(i+0)] = (u32)(p3 + ajust_index);
			buffer->Indices[(u32)(i+1)] = (u32)(p2 + ajust_index);
			buffer->Indices[(u32)(i+2)] = (u32)(p4 + ajust_index);
			i += 3;

			buffer->Indices[(u32)(i+0)] = (u32)(p3 + ajust_index);
			buffer->Indices[(u32)(i+1)] = (u32)(p1 + ajust_index);
			buffer->Indices[(u32)(i+2)] = (u32)(p4 + ajust_index);
			i += 3;

			inele >> index >> p1 >> p2 >> p3 >> p4; // this is one line
		}

		inele.close();

		// copy the indices of the buffer in PlayerFrame::lastBuf in order to re-use it next time if possible
		PlayerFrame::lastBuf->Indices.set_used( buffer->Indices.size() );
		for (u32 j=0; j < buffer->Indices.size(); j++)
		{
			PlayerFrame::lastBuf->Indices[j] = buffer->Indices[j];
		}
	}
	else // we do not need to reload the file !
	{
		// copy the indices of the PlayerFrame::lastBuf in the buffer
		buffer->Indices.set_used( PlayerFrame::lastBuf->Indices.size() );
		for (u32 j=0; j < PlayerFrame::lastBuf->Indices.size(); j++)
		{
			buffer->Indices[j] = PlayerFrame::lastBuf->Indices[j];
		}
	}
	// -----------------------------------------------------------------------


	// lets recalculate the bounding box and create the mesh
	for (u32 j=0; j < buffer->Vertices.size(); ++j)
		buffer->BoundingBox.addInternalPoint(buffer->Vertices[j].Pos);

	SMesh* mesh = new SMesh;
	mesh->addMeshBuffer(buffer);
	buffer->drop();

	mesh->recalculateBoundingBox();

	// here we go, lets create the node that will owns the mesh data
	node = smgr->addMeshSceneNode( mesh );
	if (!node)
	{
		node = NULL;
		return;
	}
	node->setVisible(false);
	//node->setMaterialFlag(video::EMF_WIREFRAME, true);

	// that's it !
	device->getLogger()->log("Loaded.");
}



PlayerFrame::~PlayerFrame(void)
{
	if (node) node->remove();
}

void PlayerFrame::display()
{
	node->setVisible(true);
}

void PlayerFrame::hide()
{
	node->setVisible(false);
}

scene::IMeshSceneNode* PlayerFrame::getNode()
{
	return node;
}

s32 PlayerFrame::getId()
{
	return id;
}