#include "StageData.h"
#include "support/tinyxml2/tinyxml2.h"

StageWidget::StageWidget()
	:type(""),widgetId("")
	,tag(0),normalImage("")
	,selectImage("")
	,scaleX(0),scaleY(0)
	,stageId(0)
{
}


CStageData::CStageData()
{
	parseXml("csv/stage.xml");
}

void CStageData::parseXml(const char* xml)
{
	tinyxml2::XMLDocument doc;
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
	std::string filePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(xml);
	CCString* xmlContent = CCString::createWithContentsOfFile(filePath.c_str());
	tinyxml2::XMLError error = doc.Parse(xmlContent->getCString());
	if( error != tinyxml2::XML_SUCCESS ) return;
#else
	tinyxml2::XMLError error = doc.LoadFile( CCFileUtils::sharedFileUtils()->fullPathForFilename(xml).c_str() );
	if( error != tinyxml2::XML_SUCCESS ) return;
#endif
	tinyxml2::XMLNode* node = doc.FirstChildElement();
	tinyxml2::XMLElement* stages = node->FirstChildElement("chapter");
	while (stages)
	{
		vector<StageWidget> taskVec;
		tinyxml2::XMLElement* xmlChild = stages->FirstChildElement();
		while( xmlChild != NULL )
		{
			StageWidget widget;
			widget.type = xmlChild->Attribute("type");
			if (xmlChild->Attribute("id"))
			{
				widget.widgetId = xmlChild->Attribute("id");
			}
			widget.tag = xmlChild->IntAttribute("tag");
			widget.scaleX = xmlChild->FloatAttribute("scaleX");
			widget.scaleY= xmlChild->FloatAttribute("scaleY");
			widget.normalImage = xmlChild->Attribute("normalImage");
			widget.selectImage = xmlChild->Attribute("selectImage");
			widget.position.x = xmlChild->FloatAttribute("posX");
			widget.position.y = xmlChild->FloatAttribute("posY");
			widget.stageId = xmlChild->IntAttribute("stageId");
			widget.stageType = xmlChild->IntAttribute("stageType");
			taskVec.push_back(widget);
			xmlChild = xmlChild->NextSiblingElement();
		}
		m_stageMap[stages->IntAttribute("lv")] = taskVec;
		stages = stages->NextSiblingElement();
	}
}

const vector<StageWidget>* CStageData::getStageWidgets(int chapter)
{
	StageMap::iterator iter = m_stageMap.find(chapter);
	if (iter!= m_stageMap.end())
	{
		return &iter->second;
	}
    return nullptr;
}

const StageWidget* CStageData::getWidgetByStage(int chapter, int index)
{
	StageMap::iterator iter = m_stageMap.find(chapter);
	if (iter!= m_stageMap.end())
	{
		vector<StageWidget> &vec = iter->second;
		if (index<vec.size()&&index>=0)
		{
			return &iter->second.at(index);
		}
	}
	return nullptr;
}

void CStageData::reload()
{
	m_stageMap.clear();
	parseXml("csv/stage.xml");
}

