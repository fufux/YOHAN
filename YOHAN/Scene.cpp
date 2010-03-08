// for using the xml parser
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>	
#include "Scene.h"
#include "BoundingBox.h"

using namespace xercesc;



Scene::Scene(void)
{
	plan = new Tetrahedron();
}

Scene::~Scene(void)
{
}

double* Scene::getForceField()
{
	return forceField;
}

double Scene::getDeltaT()
{
	return deltaT;
}



bool Scene::load(std::string tetrahedralizedSceneFile)
{
	/* parse */

	// initialization
	try 
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) 
	{
		char* message = XMLString::transcode(toCatch.getMessage());
            cout << "Error during xml initialization! :\n"
                 << message << "\n";
			XMLString::release(&message);
		return false;
	}

	XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true);    // optional

    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);

    const char* xmlFile = tetrahedralizedSceneFile.c_str();

	// begin
    try 
	{
        parser->parse(xmlFile);

		DOMNodeList* nodelist = parser->getDocument()->getChildNodes();

		// get scene node (root)
		if (nodelist->getLength() != 1) {
			cout << "nodelist->getLength() != 1" << endl;
			return false;
		} else
			nodelist = nodelist->item(0)->getChildNodes();
		
		// parse 
		for (int i = 0; i < (int)nodelist->getLength(); i++)
		{
			DOMNode* node = nodelist->item(i);
			
			// ignore the text node
			if (node->getNodeType() != DOMNode::ELEMENT_NODE && 
				node->getNodeType() != DOMNode::ENTITY_NODE)
				continue;

			const XMLCh* nodeName = node->getNodeName();

			if (XMLString::equals(nodeName, L"volumicmesh"))
			{
				std::wstring nodeFile;
				std::wstring eleFile;
				std::wstring faceFile;

				double pos[3];
				double rot[3];
				double speed[3];
				Material material;

				int id = XMLString::parseInt(node->getAttributes()->getNamedItem(L"id")->getNodeValue());

				DOMNodeList* vmlist = node->getChildNodes();
				for (int j = 0; j < (int)vmlist->getLength(); j++)
				{
					DOMNode* vmnode = vmlist->item(j);

					// ignore the text node
					if (vmnode->getNodeType() != DOMNode::ELEMENT_NODE && 
						vmnode->getNodeType() != DOMNode::ENTITY_NODE)
						continue;

					const XMLCh* vmname = vmnode->getNodeName();

					if (XMLString::equals(vmname, L"nodefile"))
					{
						nodeFile = (wchar_t*)vmnode->getAttributes()->getNamedItem(L"file")->getNodeValue();
					}
					else if (XMLString::equals(vmname, L"elefile"))
					{
						eleFile = (wchar_t*)vmnode->getAttributes()->getNamedItem(L"file")->getNodeValue();
					}
					else if (XMLString::equals(vmname, L"facefile"))
					{
						faceFile = (wchar_t*)vmnode->getAttributes()->getNamedItem(L"file")->getNodeValue();
					}
					else if (XMLString::equals(vmname, L"initialposition"))
					{
						char* tmp = new char[128];

						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"x")->getNodeValue(), 128);
						pos[0] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"y")->getNodeValue(), 128);
						pos[1] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"z")->getNodeValue(), 128);
						pos[2] = atof(tmp);
					}
					else if (XMLString::equals(vmname, L"initialrotation"))
					{
						char* tmp = new char[128];

						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"x")->getNodeValue(), 128);
						rot[0] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"y")->getNodeValue(), 128);
						rot[1] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"z")->getNodeValue(), 128);
						rot[2] = atof(tmp);
					}
					else if (XMLString::equals(vmname, L"initialspeed"))
					{
						char* tmp = new char[128];

						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"x")->getNodeValue(), 128);
						speed[0] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"y")->getNodeValue(), 128);
						speed[1] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"z")->getNodeValue(), 128);
						speed[2] = atof(tmp);
					}
					else if (XMLString::equals(vmname, L"materialproperties"))
					{
						char* tmp = new char[128];

						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Lambda")->getNodeValue(), 128);
						material.lambda = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Mu")->getNodeValue(), 128);
						material.mu = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Alpha")->getNodeValue(), 128);
						material.phi = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Beta")->getNodeValue(), 128);
						material.psy = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Density")->getNodeValue(), 128);
						material.rho = atof(tmp);

						/* Added by Ning, for fracture */
						material.toughness = 1400;
						/* END -- Added by Ning, for fracture */
					}
					else					
					{
						cout << "Error while parsing xml file" << endl;
						return false;
					}
				}

				// add into the list
				Volume* v = new Volume(id, this);
				v->load(util::ws2s(nodeFile), util::ws2s(eleFile), util::ws2s(faceFile), material, (double*)pos, (double*)rot, (double*)speed);
				volumes.push_back( v );
			}
			else if (XMLString::equals(nodeName, L"force"))
			{
				/*struct SceneForce sf;

				DOMNodeList* flist = node->getChildNodes();
				for (int j = 0; j < flist->getLength(); j++)
				{
					DOMNode* fnode = flist->item(j);

					// ignore the text node
					if (fnode->getNodeType() != DOMNode::NodeType::ELEMENT_NODE && 
						fnode->getNodeType() != DOMNode::NodeType::ENTITY_NODE)
						continue;

					const XMLCh* fname = fnode->getNodeName();

					if (XMLString::equals(fname, L"volumicmeshid"))
					{
						sf.vmeshID = XMLString::parseInt(fnode->getAttributes()->getNamedItem(L"value")->getNodeValue());
					}
					else if (XMLString::equals(fname, L"vertex"))
					{
						sf.vertexID = XMLString::parseInt(fnode->getAttributes()->getNamedItem(L"index")->getNodeValue());
					}
					else if (XMLString::equals(fname, L"intensity"))
					{
						char tmp[256];

						wcstombs(tmp, fnode->getAttributes()->getNamedItem(L"x")->getNodeValue(), 256);
						sf.intensity[0] = atof(tmp);
						wcstombs(tmp, fnode->getAttributes()->getNamedItem(L"y")->getNodeValue(), 256);
						sf.intensity[1] = atof(tmp);
						wcstombs(tmp, fnode->getAttributes()->getNamedItem(L"z")->getNodeValue(), 256);
						sf.intensity[2] = atof(tmp);
					}
					else
						fetalError();
				}

				// add into the list
				this->forceList.push_back(sf);*/
			}
			else if (XMLString::equals(nodeName, L"forcefield"))
			{
				DOMNodeList* fclist = node->getChildNodes();
				for (int j = 0; j < (int)fclist->getLength(); j++)
				{
					DOMNode* fcnode = fclist->item(j);

					// ignore the text node
					if (fcnode->getNodeType() != DOMNode::ELEMENT_NODE && 
						fcnode->getNodeType() != DOMNode::ENTITY_NODE)
						continue;

					const XMLCh* fcname = fcnode->getNodeName();

					if (XMLString::equals(fcname, L"intensity"))
					{
						char* tmp = new char[128];

						wcstombs(tmp, fcnode->getAttributes()->getNamedItem(L"x")->getNodeValue(), 128);
						this->forceField[0] = atof(tmp);
						wcstombs(tmp, fcnode->getAttributes()->getNamedItem(L"y")->getNodeValue(), 128);
						this->forceField[1] = atof(tmp);
						wcstombs(tmp, fcnode->getAttributes()->getNamedItem(L"z")->getNodeValue(), 128);
						this->forceField[2] = atof(tmp);
					}
					else
					{
						cout << "Error while parsing xml file" << endl;
						return false;
					}
				}
			}
		}
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        
		cout << "Error while parsing xml file" << endl;
		return false;
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        
		cout << "Error while parsing xml file" << endl;
		return false;
    }
    catch (...) {
        cout << "Unexpected Exception \n" ;
        
		cout << "Error while parsing xml file" << endl;
		return false;
    }

    delete parser;
    delete errHandler;


	// Term
	XMLPlatformUtils::Terminate();

	// update force fields and mass
	for (int i=0; i < (int)volumes.size(); i++)
		volumes[i]->generateMAndForceField();

	return true;
}



void Scene::saveStep(std::string filename)
{
	// we want to create sub directories each 256 steps
	std::stringstream dir;
	dir << simulatedSceneOutDir;
	dir << "/sub" << (stepNumber / 256);
	createDir(dir.str().c_str());
	dir << "/frame-" << stepNumber;
	createDir(dir.str().c_str());

	ofstream fp (filename.c_str(), ios::app);
	if (!fp || !fp.good())
	{
		cout << "Can not create simulated scene file : " << filename.c_str() << endl;
		return;
	}

	fp << "<frame id=\"" << stepNumber << "\" timestamp=\"" << currentTime << "\">" << endl;

	for (int i=0; i < (int)volumes.size(); i++)
	{
		vector<std::string> files;
		files = volumes[i]->save(dir.str());
		fp << "<object nodefile=\"" << files[0] << "\" facefile=\"" << files[2] << "\" elefile=\"" << files[1] << "\" bbfile=\"" << files[3];
		fp << "\" />" << endl;
	}

	fp << "</frame>" << endl;

	fp.flush();
	fp.close();
	fp.clear();
}



bool Scene::simulate(std::string simulatedSceneOutDir, double deltaT, int nbSteps)
{
	this->currentTime = 0;
	this->deltaT = deltaT;
	this->simulatedSceneOutDir = simulatedSceneOutDir;


	// Initialize collision response constants
	kerr = 1000;
	kdmp = 7500;
	kfrc = 0.5;


	std::string filename = simulatedSceneOutDir;
	filename += "/scene.xml";

	// prepare XML scene file
	ofstream fp (filename.c_str(), ios::out);
	if (!fp || !fp.good())
	{
		cout << "Can not create simulated scene file : " << filename.c_str() << endl;
		return false;
	}

	fp << "<video frames=\"" << nbSteps << "\">" << endl;

	fp.flush();
	fp.close();
	fp.clear();


	// SIMULATE
	long tstart, tend, tdif, tdif2;
			
	for (stepNumber=0; stepNumber < nbSteps; stepNumber++)
	{
		currentTime += deltaT;
		tstart = GetTickCount();
		cout << "step n" << stepNumber<<endl;

		// reset
		for (int i=0; i < (int)volumes.size(); i++) {
			//volumes[i]->collisionBidon();
			volumes[i]->resetAll();
		}


		// compute
		handleCollisions();
		for (int i=0; i < (int)volumes.size(); i++) 
		{
			volumes[i]->evolve(deltaT);
			volumes[i]->calculFracture();
		}
		tend = GetTickCount();
		tdif = tend - tstart;

		// save step
		saveStep(filename);
		tdif2 = GetTickCount() - tend;

		// log
		if (stepNumber % 10 == 0) {
			std::stringstream s;
			s << "Step n" << stepNumber << " computed.";
			util::log( s.str() );
		}
	}


	// end XML scene file
	fp.open(filename.c_str(), ios::app);
	if (!fp || !fp.good())
	{
		cout << "Can not create simulated scene file : " << filename.c_str() << endl;
		return false;
	}

	fp << "</video>" << endl;

	fp.flush();
	fp.close();
	fp.clear();

	util::log( "Finished well !" );

	return true;
}

void Scene::planCollisionResponse(vector<Tetrahedron*>* tets)
{
	Tetrahedron* tet;
	Polyhedron p;
	Point* pt;
	vector<Point*> overPts, underPts;
	vector<int> overPtsInds, underPtsInds;
	vector<Point*>* pts;
	vector<Vector3d*>* vertices;
	vector<Face*>* faces = new vector<Face*>();
	Vector3d* pI;
	Face* face;
	double xA,yA,zA,xB,yB,zB;
	Vector3d*** m;
	m = new Vector3d**[4];
	for (int i=0;i<4; i++)
		m[i] = new Vector3d*[4];

	// true for positive, false for negative
	int ind;
	for(int i=0; i<(int)tets->size(); i++){
		faces->clear();
		overPts = vector<Point*>();
		underPts = vector<Point*>();
		overPtsInds = vector<int>();
		underPtsInds = vector<int>();
		// Construction of points below and over y=O
		for(int j=0; j<4; j++){
			tet = (*tets)[i];
			pt = (*tets)[i]->getPoints()[j];
			if(pt->getX()[1]<0){
				underPts.push_back(pt);
				underPtsInds.push_back(j);	
			}else{
				overPts.push_back(pt);
				overPtsInds.push_back(j);
			}
		}
		// Construction of intersections points with y=o plan
		for(int j=0; j<(int)underPts.size(); j++){
			for(int k=0; k<(int)overPts.size(); k++){
				pt = underPts[j];
				xA = pt->getX()[0];
				yA = pt->getX()[1];
				zA = pt->getX()[2];
				pt = overPts[k];
				xB = pt->getX()[0];
				yB = pt->getX()[1];
				zB = pt->getX()[2];
				pI = new Vector3d(xA + yA*(xB-xA)/(yA-yB), 0, zA + yA*(zB-zA)/(yA-yB));
				m[underPtsInds[j]][overPtsInds[k]] = pI;
				m[overPtsInds[k]][underPtsInds[j]] = pI;
			}
		}
		// Fake collision
		if(underPts.size()==0)
			continue;

		// Constructions of overlaping polyhedron faces

		pts = &(*tets)[i]->getPoints();
		ind = (*tets)[i]->getID();
		// First face: p2->p3->p1	
		face = new Face(pts,1,2,0,(Vector3d***)m,ind);
		if(face->size()>=3)
			faces->push_back(face);
		// Second face: p4->p2->p1
		face = new Face(pts,3,1,0,(Vector3d***)m,ind);
		if(face->size()>=3)
			faces->push_back(face);
		// Third face: p2->p4->p3
		face = new Face(pts,1,3,2,(Vector3d***)m,ind);
		if(face->size()>=3)
			faces->push_back(face);
		// Fourth face: p3->p4->p1
		face = new Face(pts,2,3,0,(Vector3d***)m,ind);
		if(face->size()>=3)
			faces->push_back(face);
		// The hidden face
		if(underPts.size()==1){
			vertices = new vector<Vector3d*>();
			if((*pts)[0]->getX()[1]<0){		
				vertices->push_back(m[0][1]);
				vertices->push_back(m[0][3]);
				vertices->push_back(m[0][2]);
			}else if((*pts)[1]->getX()[1]<0){
				vertices->push_back(m[1][2]);
				vertices->push_back(m[1][3]);
				vertices->push_back(m[1][0]);
			}else if((*pts)[2]->getX()[1]<0){
				vertices->push_back(m[2][3]);
				vertices->push_back(m[2][1]);
				vertices->push_back(m[2][0]);
			}else if((*pts)[3]->getX()[1]<0){
				vertices->push_back(m[3][1]);
				vertices->push_back(m[3][2]);
				vertices->push_back(m[3][0]);
			}
			face = new Face(vertices, ind);
			faces->push_back(face);
		}else if(underPts.size()==2){
			vertices = new vector<Vector3d*>();
			if((*pts)[0]->getX()[1]<0 && (*pts)[1]->getX()[1]<0){			
				vertices->push_back(m[0][3]); 
				vertices->push_back(m[0][2]); 
				vertices->push_back(m[1][2]);
				vertices->push_back(m[1][3]);
			}else if((*pts)[0]->getX()[1]<0 && (*pts)[2]->getX()[1]<0){			
				vertices->push_back(m[0][1]); 
				vertices->push_back(m[0][3]); 
				vertices->push_back(m[2][3]);
				vertices->push_back(m[2][1]);
			}else if((*pts)[0]->getX()[1]<0 && (*pts)[3]->getX()[1]<0){			
				vertices->push_back(m[0][2]); 
				vertices->push_back(m[0][1]); 
				vertices->push_back(m[3][1]);
				vertices->push_back(m[3][2]);
			}else if((*pts)[1]->getX()[1]<0 && (*pts)[2]->getX()[1]<0){			
				vertices->push_back(m[1][3]); 
				vertices->push_back(m[1][0]); 
				vertices->push_back(m[2][0]);
				vertices->push_back(m[2][3]);
			}else if((*pts)[1]->getX()[1]<0 && (*pts)[3]->getX()[1]<0){			
				vertices->push_back(m[1][0]); 
				vertices->push_back(m[1][2]); 
				vertices->push_back(m[3][2]);
				vertices->push_back(m[3][0]);
			}else if((*pts)[2]->getX()[1]<0 && (*pts)[3]->getX()[1]<0){			
				vertices->push_back(m[2][1]); 
				vertices->push_back(m[2][0]); 
				vertices->push_back(m[3][0]);
				vertices->push_back(m[3][1]);
			}
			face = new Face(vertices, ind);
			faces->push_back(face);
		}else if(underPts.size()==3){
			vertices = new vector<Vector3d*>();
			if((*pts)[0]->getX()[1]>=0){
				vertices->push_back(m[0][2]);
				vertices->push_back(m[0][3]);
				vertices->push_back(m[0][1]);
			}else if((*pts)[1]->getX()[1]>=0){
				vertices->push_back(m[1][0]);
				vertices->push_back(m[1][3]);
				vertices->push_back(m[1][2]);
			}else if((*pts)[2]->getX()[1]>=0){
				vertices->push_back(m[2][0]);
				vertices->push_back(m[2][1]);
				vertices->push_back(m[2][3]);
			}else if((*pts)[3]->getX()[1]>=0){
				vertices->push_back(m[3][0]);
				vertices->push_back(m[3][2]);
				vertices->push_back(m[3][1]);
			}
			face = new Face(vertices, ind);
			faces->push_back(face);
		}
		p = Polyhedron(faces, (*tets)[i], plan);
		p.collisionForces(kerr,kdmp,kfrc);
	}
	delete[] m;
}


void Scene::handleCollisions()
{
	std::vector<Tetrahedron*> *found = new std::vector<Tetrahedron*>();
	for (int i=0; i < (int)volumes.size(); i++) {
		volumes[i]->getMasterBoundingBox()->recalculateBoundingBoxes();
	}
	for (int i=0; i < (int)volumes.size(); i++) {
		volumes[i]->getMasterBoundingBox()->getCollidingTetrahedra(0, found);
	}
	planCollisionResponse(found);
	delete found;
}