#include "GuideData.h"
#include "support/tinyxml2/tinyxml2.h"

GuideTask::GuideTask()
	:step(0),className("")
	,type(""),widgetId("")
	,tag(0),tag1(0)
	,desc(""),name("")
	,autoJump(0)
	,isFinish(false)
	,text1(""),text2(""),nodeClickType(GuideNodeClipWithStencil)
{
}


CGuideData::CGuideData()
{
	parseXml("csv/guide.xml");
}

void CGuideData::parseXml(const char* xml)
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
	tinyxml2::XMLElement* tasks = node->FirstChildElement("tasks");
	while (tasks)
	{
		vector<GuideTask> taskVec;
		tinyxml2::XMLElement* xmlChild = tasks->FirstChildElement();
		while( xmlChild != NULL )
		{
			GuideTask task;
			task.step = xmlChild->IntAttribute("step");
			task.className = xmlChild->Attribute("className");
			task.type = xmlChild->Attribute("type");
			task.widgetId = xmlChild->Attribute("widgetId");
			task.tag = xmlChild->IntAttribute("tag");
			task.autoJump = xmlChild->IntAttribute("isAtuoJump");
			task.nodeClickType = (GuideNodeClipType)xmlChild->IntAttribute("NodeClipType");

			string tagStr = xmlChild->Attribute("tag");

			int pos = task.className.find('/',0);
			if (pos!=string::npos)
			{
				string cname =  task.className;
				task.className = cname.substr(0,pos);
				task.className1 = cname.substr(pos+1,cname.length()-1);
			}
			else
			{
				task.className1 ="";
			}

		    pos = tagStr.find('/',0);
			if (tagStr!=""&&pos!=string::npos)
			{
				task.tag = atoi(tagStr.substr(0,pos).c_str());
				task.tag1 = atoi(tagStr.substr(pos+1,tagStr.length()-1).c_str());
			}
			else
			{
				task.tag = atoi(tagStr.c_str());
				task.tag1 = 0;
			}
			task.desc = xmlChild->Attribute("desc");
			task.name = xmlChild->Attribute("name");
			const char* offset = xmlChild->Attribute("offset");
			if (offset)
			{
				string offStr = offset;
				int pos = offStr.find(',',0);
				task.offset.x = atof(offStr.substr(0,pos).c_str());
				task.offset.y = atof(offStr.substr(pos+1,offStr.length()-1).c_str());
			}

			const char* text1= xmlChild->Attribute("text1");
			const char* text2= xmlChild->Attribute("text2");
			if (text1)
			{
				task.text1 = text1;
			}
			if (text2)
			{		
				task.text2 = text2;
			}
			taskVec.push_back(task);
			xmlChild = xmlChild->NextSiblingElement();
		}
		m_guideMap[tasks->IntAttribute("lv")] = taskVec;
		tasks = tasks->NextSiblingElement();
	}
}

const vector<GuideTask>& CGuideData::getGuideTasks(int task)
{
	GuideMap::iterator iter = m_guideMap.find(task);
	if (iter!= m_guideMap.end())
	{
		return iter->second;
	}
    return m_guideMap.begin()->second;
}

GuideTask* CGuideData::getTaskByStep(int task, int step)
{
	GuideMap::iterator iter = m_guideMap.find(task);
	if (iter!= m_guideMap.end())
	{
		vector<GuideTask> &vec = iter->second;
		for (int i = 0; i < vec.size(); i++)
		{
			if (vec.at(i).step == step)
			{
				return &iter->second.at(i);
			}
		}
	}
	return nullptr;
}


