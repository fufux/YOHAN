#include "stdafx.h"

#include "Base.h"

#include <iostream>

// for using the xml parser
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>	

using namespace std;
using namespace xercesc;

using namespace yohan;
using namespace base;

SceneController::SceneController(char* filename)
{
	// allocation
	this->vmcList = std::vector<VolumeModelController>();
	this->forceList = std::list<struct SceneForce>();

	/* parse */

	// initialization
	try 
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) 
	{
		char* message = XMLString::transcode(toCatch.getMessage());
            cout << "Error during initialization! :\n"
                 << message << "\n";
			XMLString::release(&message);

		fetalError();
	}

	XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true);    // optional

    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);

    char* xmlFile = filename;

	// begin
    try 
	{
        parser->parse(xmlFile);

		DOMNodeList* nodelist = parser->getDocument()->getChildNodes();

		// get scene node (root)
		if (nodelist->getLength() != 1)
			fetalError();
		else
			nodelist = nodelist->item(0)->getChildNodes();
		
		// parse 
		for (int i = 0; i < nodelist->getLength(); i++)
		{
			DOMNode* node = nodelist->item(i);
			
			// ignore the text node
			if (node->getNodeType() != DOMNode::NodeType::ELEMENT_NODE && 
				node->getNodeType() != DOMNode::NodeType::ENTITY_NODE)
				continue;

			const XMLCh* nodeName = node->getNodeName();

			if (XMLString::equals(nodeName, L"volumicmesh"))
			{
				char *nodeFile = new char[256];
				char *eleFile = new char[256];
				char *faceFile = new char[256];

				DATA speed[3];
				DATA materialPropriety[32];

				int id = XMLString::parseInt(node->getAttributes()->getNamedItem(L"id")->getNodeValue());

				DOMNodeList* vmlist = node->getChildNodes();
				for (int j = 0; j < vmlist->getLength(); j++)
				{
					DOMNode* vmnode = vmlist->item(j);

					// ignore the text node
					if (vmnode->getNodeType() != DOMNode::NodeType::ELEMENT_NODE && 
						vmnode->getNodeType() != DOMNode::NodeType::ENTITY_NODE)
						continue;

					const XMLCh* vmname = vmnode->getNodeName();

					if (XMLString::equals(vmname, L"nodefile"))
					{
						wcstombs(nodeFile, vmnode->getAttributes()->getNamedItem(L"file")->getNodeValue(), 256);
					}
					else if (XMLString::equals(vmname, L"elefile"))
					{
						wcstombs(eleFile, vmnode->getAttributes()->getNamedItem(L"file")->getNodeValue(), 256);
					}
					else if (XMLString::equals(vmname, L"facefile"))
					{
						wcstombs(faceFile, vmnode->getAttributes()->getNamedItem(L"file")->getNodeValue(), 256);
					}
					else if (XMLString::equals(vmname, L"initialspeed"))
					{
						char tmp[256];

						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"x")->getNodeValue(), 256);
						speed[0] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"y")->getNodeValue(), 256);
						speed[1] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"z")->getNodeValue(), 256);
						speed[2] = atof(tmp);
					}
					else if (XMLString::equals(vmname, L"materialproperties"))
					{
						char tmp[256];

						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Lambda")->getNodeValue(), 256);
						materialPropriety[0] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Mu")->getNodeValue(), 256);
						materialPropriety[1] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Alpha")->getNodeValue(), 256);
						materialPropriety[2] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Beta")->getNodeValue(), 256);
						materialPropriety[3] = atof(tmp);
						wcstombs(tmp, vmnode->getAttributes()->getNamedItem(L"Density")->getNodeValue(), 256);
						materialPropriety[4] = atof(tmp);
					}
					else					
						fetalError();
				}

				// add into the list
				VolumeModelController vmc = VolumeModelController(nodeFile, faceFile, eleFile, speed, materialPropriety);
				this->vmcList.push_back(vmc);

				// deallocate the tmp resource
				delete[] nodeFile;
				delete[] eleFile;
				delete[] faceFile;

			}
			else if (XMLString::equals(nodeName, L"force"))
			{
				struct SceneForce sf;

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
				this->forceList.push_back(sf);
			}
			else if (XMLString::equals(nodeName, L"forcefield"))
			{
				DOMNodeList* fclist = node->getChildNodes();
				for (int j = 0; j < fclist->getLength(); j++)
				{
					DOMNode* fcnode = fclist->item(j);

					// ignore the text node
					if (fcnode->getNodeType() != DOMNode::NodeType::ELEMENT_NODE && 
						fcnode->getNodeType() != DOMNode::NodeType::ENTITY_NODE)
						continue;

					const XMLCh* fcname = fcnode->getNodeName();

					if (XMLString::equals(fcname, L"intensity"))
					{
						char tmp[256];

						wcstombs(tmp, fcnode->getAttributes()->getNamedItem(L"x")->getNodeValue(), 256);
						this->gravity[0] = atof(tmp);
						wcstombs(tmp, fcnode->getAttributes()->getNamedItem(L"y")->getNodeValue(), 256);
						this->gravity[1] = atof(tmp);
						wcstombs(tmp, fcnode->getAttributes()->getNamedItem(L"z")->getNodeValue(), 256);
						this->gravity[2] = atof(tmp);
					}
					else
						fetalError();
				}
			}
		}
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        
		fetalError();
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        
		fetalError();
    }
    catch (...) {
        cout << "Unexpected Exception \n" ;
        
		fetalError();
    }

    delete parser;
    delete errHandler;


	// Term
	XMLPlatformUtils::Terminate();

}

void SceneController::simulate(char* filename)
{
	DATA delta_t = 0.0001;

	int step = 0;

	SceneRecorder sr = SceneRecorder(filename);

	DATA totalTime = 0;

	while (step < 10)
	{
		//detect the collision 

		/* response the collision by generate events for each VolumeModelController in this round */

		// send force
		for (std::list<SceneForce>::iterator iter = forceList.begin(); iter != forceList.end(); ++iter)
		{
			SceneForce sf = *iter;
			MotivationEvent* e = new ForceEvent(sf);			
			this->vmcList[sf.vmeshID].sendEvent(e);
		}
		forceList.clear();

		//decide delta-t
		delta_t = 0.01;

		// send force field and start
		for (std::vector<VolumeModelController>::iterator iter = vmcList.begin(); iter != vmcList.end(); ++iter)
		{
			MotivationEvent* e = new ForceFieldEvent(true, this->gravity, delta_t);
			iter->sendEvent(e);

			iter->evoluteModel();
		}

		totalTime += delta_t;
		sr.newFrame(totalTime);

		//for each VolumeModelController vmc
			//vmc.retrieveReport();
		for (std::vector<VolumeModelController>::iterator iter = vmcList.begin(); iter != vmcList.end(); ++iter)
		{
			VolumeModel* vm = iter->retrieveReport().getModel();

			//record
			sr.record(vm);

			// simple detection
			//vm->fillForceList(&forceList);

			// resolve conflict
			//vm->resolveConflit();
		}

		sr.endFrame();

		step++;
		
	}

	sr.endScene();
}