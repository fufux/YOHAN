// for using the xml parser
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>	
#include "Scene.h"

using namespace xercesc;



Scene::Scene(void)
{
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
					}
					else					
					{
						cout << "Error while parsing xml file" << endl;
						return false;
					}
				}

				// add into the list
				Volume* v = new Volume(id, this);
				v->load(util::ws2s(nodeFile), util::ws2s(eleFile), util::ws2s(faceFile), material, (double*)speed);
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
		fp << "<object nodefile=\"" << files[0] << "\" facefile=\"" << files[2] << "\" elefile=\"" << files[1] << "\" />" << endl;
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

	std::string filename = simulatedSceneOutDir;
	filename += "/scene.xml";

	// prepare XML scene file
	ofstream fp (filename.c_str(), ios::out);
	if (!fp || !fp.good())
	{
		cout << "Can not create simulated scene file : " << filename.c_str() << endl;
		return false;
	}

	fp << "<video name=\"test\">" << endl;

	fp.flush();
	fp.close();
	fp.clear();


	// SIMULATE
	for (stepNumber=0; stepNumber < nbSteps; stepNumber++)
	{
		currentTime += deltaT;

		// compute
		for(int i=0; i<(int)volumes.size(); i++){
			volumes[i]->collisionBidon();
			volumes[i]->evolve(deltaT);
		}

		// save step
		saveStep(filename);
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

	return true;
}