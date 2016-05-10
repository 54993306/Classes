

#include "CHeroSoundData.h"
#include "scene/effect/EffectObject.h"

RoleConfig::RoleConfig()
	:m_ModeID(0),m_MoveSound(542),m_ActionEffectType(0),m_ActionEffectID(0)
	,m_ActionEffectX(0),m_ActionEffectY(0),m_Turn(false),m_HitEffect(0)
	,m_HitEffectX(0),m_HitEffectY(0),m_HitEffectScale(1)
{}
CRroleData::CRroleData(){}
CRroleData::~CRroleData(){
	ClearData();
}
void CRroleData::ClearData(){
	for (auto i : m_MapConfigData)
	{
		CC_SAFE_RELEASE(i.second);
	}
	m_MapConfigData.clear();
}

RoleConfig* CRroleData::getConfigData( int ModeId )
{
	map<int,RoleConfig*>::iterator iter = m_MapConfigData.find(ModeId);
	if (iter != m_MapConfigData.end())
	{
		return iter->second;
	}else{
		return nullptr;
	}
}

void CRroleData::AddConfigData( RoleConfig* Data)
{
	map<int,RoleConfig*>::iterator iter = m_MapConfigData.find(Data->getModeID());
	if (iter != m_MapConfigData.end())
	{
		CC_SAFE_RELEASE(iter->second);
		m_MapConfigData.erase(iter);
		CCLOG("[ *ERROR ] CRroleData::AddConfigData Mode Repeat = %d",Data->getModeID());
	}
	Data->retain();
	m_MapConfigData[Data->getModeID()] = Data;
}

void CRroleData::initData()
{
	ClearData();
	string strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("csv/RoleData.json");
	if (!CCFileUtils::sharedFileUtils()->isFileExist(strPath))
	{
		CCLOG("[ ERROR ] CHeroSoundData::reloadFile_isFileExist");
		return;
	}
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData("csv/RoleData.json","r",&size);
	string contentStr ((const char*)buff, size);
	rapidjson::Document doc;
	doc.Parse<0>(contentStr.c_str());

	if (doc.HasParseError())
	{    
		CCLOG("[ ERROR ] CHeroSoundData::reloadFile_doc.HasParseError() %s",doc.GetParseError());
		return;
	}   
	if (!doc.IsObject() || !doc.HasMember("heroSoundData"))
	{
		return;
	}
	const rapidjson::Value &array =doc["heroSoundData"];

	if (!array.IsArray())
	{
		CCLOG("[ *ERROR ] CRroleData::reloadFile");
		return;
	}
	for(unsigned int i=0; i<array.Size(); ++i)
	{  
		const rapidjson::Value &single = array[i];
		RoleConfig* data = RoleConfig::create();
		if(single.HasMember("model_id"))
		{
			data->setModeID(single["model_id"].GetInt());
		}else{
			CCLOG("[ *ERROR ] CRroleData::initData Can't Find Mode ID");
			continue;
		}
		if(single.HasMember("sound_move"))
			data->setMoveSound(single["sound_move"].GetInt());
		if(single.HasMember("Turn"))
			data->setTurn(single["Turn"].GetBool());
		if(single.HasMember("HitEffect"))
			data->setHitEffect(single["HitEffect"].GetInt());
		if(single.HasMember("HitEffectX"))
			data->setHitEffectX(single["HitEffectX"].GetInt());
		if(single.HasMember("HitEffectY"))
			data->setHitEffectY(single["HitEffectY"].GetInt());
		if(single.HasMember("HitEffectScale"))
			data->setHitEffectScale(single["HitEffectScale"].GetDouble());

		AddConfigData(data);
	}
}

bool CRroleData::isTurn( int id )
{
	RoleConfig* data = getConfigData(id);
	if (data)
	{
		return data->getTurn();
	}else{
		return false;
	}
}

int CRroleData::getMoveSound( int ModeID )
{
	RoleConfig* data = getConfigData(ModeID);
	if (data)
	{
		return data->getMoveSound();
	}else{
		return 542;									//默认移动音效
	}
}

CCNode* CRroleData::getActionEffect( int ModeID )
{
	RoleConfig* data = getConfigData(ModeID);
	if (data && data->getHitEffect())				//当前的需求只有这个,先把这个写出来后面再拓展
	{
		//EffectObject* eff = EffectObject::create()
		char str[60] = {0};
		sprintf(str,"ActionEffect/%d.png",data->getHitEffect());
		CCSprite* sp = CCSprite::create(str);
		if (sp)
		{
			sp->setPosition(ccp(data->getHitEffectX(),data->getHitEffectY()));
			sp->setScale(data->getHitEffectScale());
#if CC_PLATFORM_WIN32  == CC_TARGET_PLATFORM
			sp->runAction(CCSequence::create(CCFadeOut::create(0.3f),CCRemoveSelf::create(),nullptr));
#else
			sp->runAction(CCSequence::create(CCFadeOut::create(0.3f),CCRemoveSelf::create(),nullptr));
#endif
			return sp;
		}else{
			CCLOG("[ *ERROR ] CRroleData::getActionEffect %s",str);
		}
	}
	return nullptr;
}



