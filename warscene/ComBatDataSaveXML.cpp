#include "ComBatDataSaveXML.h"

bool CombatDataSaveXML::init()
{
	WriteXML();
	ReadXML("map/text.xml");
	return true;
}

void CombatDataSaveXML::ReadXML(const char* path)
{
	tinyxml2::XMLDocument doc;
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
	std::string filePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(path);
	CCString* xmlContent = CCString::createWithContentsOfFile(filePath.c_str());
	tinyxml2::XMLError error = doc.Parse(xmlContent->getCString());
	if( error != tinyxml2::XML_SUCCESS ) return;
#else
	tinyxml2::XMLError error = doc.LoadFile( CCFileUtils::sharedFileUtils()->fullPathForFilename(path).c_str() );
	if( error != tinyxml2::XML_SUCCESS ) return;
#endif
	tinyxml2::XMLNode* rootChild = doc.FirstChildElement();										//FDTinyXML2_ROOT_NAME
	tinyxml2::XMLElement* groundGrassChild = rootChild->FirstChildElement("groundGrass");		//groundGrass
	if ( groundGrassChild != NULL )
	{
		tinyxml2::XMLElement* xmlChild = groundGrassChild->FirstChildElement();
		if ( xmlChild != NULL )
		{

		}
		//task.className = xmlChild->Attribute("className");
		//task.tag = xmlChild->IntAttribute("tag");											//数字类的需要加类型说明符  IntAttribute(Int)
		//xmlChild = xmlChild->NextSiblingElement();
	}
}

bool CombatDataSaveXML::WriteXML()
{
	bool bRet = false;  
	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();  
	if (NULL == pDoc) {  
		return false;  
	}  
	tinyxml2::XMLDeclaration *pDeclaration = pDoc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");  
	if (NULL == pDeclaration) {  
		return false;  
	}  
	pDoc->LinkEndChild(pDeclaration);  

	tinyxml2::XMLElement *pRootEle = pDoc->NewElement("FDTinyXML2_ROOT_NAME");  
	if (NULL == pRootEle) {  
		return false;  
	}  
	pDoc->LinkEndChild(pRootEle);			//节点添加时需要做容错性判断	

	tinyxml2::XMLElement *ElementExplain = pDoc->NewElement("ElementExplain"); 
	pRootEle->LinkEndChild(ElementExplain);	
	tinyxml2::XMLText* XMLText = pDoc->NewText("This Is FirstElement");					//添加XML文本类内容(第一个节点添加才能输出内容)
	ElementExplain->LinkEndChild(XMLText);  
	CCLOG("\n------------------------------%s",ElementExplain->GetText());

	 
	tinyxml2::XMLElement* FirstElement = pDoc->NewElement("FirstElement");				
	pRootEle->LinkEndChild(FirstElement);	  

	tinyxml2::XMLElement *FirstElementChild1 = pDoc->NewElement("FirstElementChild1");  
	FirstElementChild1->SetAttribute("Attribute1", "11");  
	FirstElementChild1->SetAttribute("Attribute2", "2.2");  
	FirstElementChild1->SetAttribute("Attribute3", "name"); 
	FirstElement->LinkEndChild(FirstElementChild1);

	tinyxml2::XMLElement *FirstElementChild2 = pDoc->NewElement("FirstElementChild2");  
	FirstElementChild2->SetAttribute("Attribute1", "11");  
	FirstElementChild2->SetAttribute("Attribute2", "2.2");  
	FirstElementChild2->SetAttribute("Attribute3", "name"); 
	FirstElement->LinkEndChild(FirstElementChild2);


	tinyxml2::XMLElement* SecondElement = pDoc->NewElement("SecondElement");				
	pRootEle->LinkEndChild(SecondElement);	 

	tinyxml2::XMLElement *SecondElementChild1 = pDoc->NewElement("SecondElementChild1");  
	SecondElementChild1->SetAttribute("Attribute1", "11");  
	SecondElementChild1->SetAttribute("Attribute2", "2.2");  
	SecondElementChild1->SetAttribute("Attribute3", "name"); 
	SecondElement->LinkEndChild(SecondElementChild1);

	tinyxml2::XMLElement *SecondElementChild2 = pDoc->NewElement("SecondElementChild2");  
	SecondElementChild2->SetAttribute("Attribute1", "11");  
	SecondElementChild2->SetAttribute("Attribute2", "2.2");  
	SecondElementChild2->SetAttribute("Attribute3", "name"); 
	SecondElement->LinkEndChild(SecondElementChild2);

	bRet = tinyxml2::XML_SUCCESS == pDoc->SaveFile("map/text.xml");						//输出新的文件会覆盖原来的文件 ( 先判断右边是否为true然后再赋值 )
	if (pDoc) {  
		delete pDoc;  
	}  
	return bRet;
}
