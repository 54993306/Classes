#include "ArmatureEventDataMgr.h"
#include "tools/ToolDefine.h"
#include "Global.h"

//=======================================================================//

ArmatureEventData::ArmatureEventData()
{
	m_armatureId = 0;
}

vector<ArmatureEvent*>& ArmatureEventData::getEventVector( const char* sAction, int iIndex )
{
	_vectorTemp.clear();
	ArmatureActionMap::iterator iter = m_armatureActionMap.find(sAction);
	if (iter != m_armatureActionMap.end())
	{
		vector<ArmatureEvent>& eventData = iter->second;
		for(auto& data: eventData)
		{
			if(data.index==iIndex)
			{
				_vectorTemp.push_back(&data);
			}
		}
	}
	return _vectorTemp;
}

//=======================================================================//


//=======================================================================//
ArmatureEventDataMgr::ArmatureEventDataMgr()
{
	
}

ArmatureEventDataMgr::~ArmatureEventDataMgr()
{
	//需要释放数据对象
	for (ArmatureEventDataMap::iterator iter = armatureEventDataMap.begin();iter != armatureEventDataMap.end();++iter)
	{
		CC_SAFE_DELETE(iter->second);
	}
}

bool ArmatureEventDataMgr::init()
{
	string fileName = CCFileUtils::sharedFileUtils()->fullPathForFilename("csv/RoleAction.json");
	if (!CCFileUtils::sharedFileUtils()->isFileExist(fileName))
	{
		CCLOG("[ ERROR ] ArmatureEventDataMgr::init");
		return false;
	}
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(fileName.c_str(),"r",&size);
	string contentStr ((const char*)buff, size);
	rapidjson::Document doc;
	doc.Parse<0>(contentStr.c_str());

	if (doc.HasParseError())
	{    
		//解析错误  
		//CC_ASSERT(false, doc.GetParseError());
	}   
	else if (doc.IsObject() && doc.HasMember("RoleAction"))
	{    
		const rapidjson::Value &armatureArray =doc["RoleAction"];

		if (armatureArray.IsArray())
		{    
			for(unsigned int i=0; i<armatureArray.Size(); ++i)
			{  
				const rapidjson::Value &armatureSingle = armatureArray[i];

				//初始化当个骨骼帧数据
				ArmatureEventData* pArmatureEventData = new ArmatureEventData;
				pArmatureEventData->setArmatureId(armatureSingle["RoleID"].GetInt());
				
				const rapidjson::Value &actionArray = armatureSingle["ActionArray"];
				for(unsigned int j=0; j<actionArray.Size(); j++)
				{
					const rapidjson::Value &actionSingle = actionArray[j];
					
					//动作名
					string actionName = actionSingle["ActionName"].GetString();

					//帧事件列表
					vector<ArmatureEvent> armtureEventVector;
					const rapidjson::Value &eventArray = actionSingle["Event"];
					for(unsigned int k=0; k<eventArray.Size(); k++)
					{
						const rapidjson::Value &eventSingle = eventArray[k];

						//帧事件
						ArmatureEvent event;
						event.name = eventSingle["EventName"].GetString();
						event.index = eventSingle["Index"].GetInt();

						//额外信息
						if(eventSingle.HasMember("extra_info"))
						{
							event.extraInfo = eventSingle["extra_info"].GetString();
						}

						if(eventSingle.HasMember("soundNum"))
						{
							event.sound = eventSingle["soundNum"].GetInt();
						}

						armtureEventVector.push_back(event);
					}

					//单个骨骼的帧事件数据存储
					pArmatureEventData->m_armatureActionMap.insert(pair<string, vector<ArmatureEvent>>(actionName, armtureEventVector));
				}

				//帧事件管理器
				armatureEventDataMap.insert(pair<int, ArmatureEventData*>(pArmatureEventData->getArmatureId(), pArmatureEventData));
			}
		}
	}

	return true;
}

ArmatureEventData* ArmatureEventDataMgr::getArmatureEventData( int iArmatureId )
{
	ArmatureEventDataMap::iterator iter = armatureEventDataMap.find(iArmatureId);
	if(iter != armatureEventDataMap.end()) return iter->second;
	CCLOG("[ ERROR ]ArmatureEventDataMgr::getArmatureEventData iArmatureId=%d",iArmatureId); 
	return nullptr;
}

void ArmatureEventDataMgr::reloadData()
{
	//需要释放数据对象
	for (auto i:armatureEventDataMap)
	{
		CC_SAFE_DELETE(i.second);
	}
	//清空map
	armatureEventDataMap.clear();
	//加载数据
	init();
}
