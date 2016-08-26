
/************************************************************* 
 *
 *
 *		Data : 2016.8.26
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "RoleConfig.h"
#include "Battle/EffectObject.h"

namespace BattleSpace
{
	RoleConfigData::RoleConfigData():m_ModeID(0),m_MoveSound(542),m_Turn(false),m_HitEffect(0)
	,m_HitEffectX(0),m_HitEffectY(0),m_HitEffectScale(1)
	{}
	RoleConfig::RoleConfig(){}
	RoleConfig::~RoleConfig()
	{
		ClearData();
	}
	void RoleConfig::ClearData()
	{
		for (auto i : mMapConfigData)
		{
			CC_SAFE_RELEASE(i.second);
		}
		mMapConfigData.clear();
	}

	RoleConfigData* RoleConfig::getConfigData( int ModeId )
	{
		map<int,RoleConfigData*>::iterator iter = mMapConfigData.find(ModeId);
		if (iter != mMapConfigData.end())
		{
			return iter->second;
		}else{
			return nullptr;
		}
	}

	void RoleConfig::AddConfigData( RoleConfigData* Data)
	{
		map<int,RoleConfigData*>::iterator iter = mMapConfigData.find(Data->getModeID());
		if (iter != mMapConfigData.end())
		{
			CC_SAFE_RELEASE(iter->second);
			mMapConfigData.erase(iter);
			CCLOG("[ *ERROR ] CRroleData::AddConfigData Mode Repeat = %d",Data->getModeID());
		}
		Data->retain();
		mMapConfigData[Data->getModeID()] = Data;
	}

	bool RoleConfig::init()
	{
		ClearData();
		string strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("csv/RoleData.json");
		if (!CCFileUtils::sharedFileUtils()->isFileExist(strPath))
		{
			CCLOG("[ ERROR ] CHeroSoundData::reloadFile_isFileExist");
			return false;
		}
		unsigned long size = 0;
		unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData("csv/RoleData.json","r",&size);
		string contentStr ((const char*)buff, size);
		rapidjson::Document doc;
		doc.Parse<0>(contentStr.c_str());

		if (doc.HasParseError())
		{    
			CCLOG("[ ERROR ] CHeroSoundData::reloadFile_doc.HasParseError() %s",doc.GetParseError());
			return false;
		}   
		if (!doc.IsObject() || !doc.HasMember("heroSoundData"))
		{
			return false;
		}
		const rapidjson::Value &array =doc["heroSoundData"];

		if (!array.IsArray())
		{
			CCLOG("[ *ERROR ] CRroleData::reloadFile");
			return false;
		}
		for(unsigned int i=0; i<array.Size(); ++i)
		{  
			const rapidjson::Value &single = array[i];
			RoleConfigData* data = RoleConfigData::create();
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
		return true;
	}

	bool RoleConfig::isTurn( int id )
	{
		RoleConfigData* data = getConfigData(id);
		if (data)
		{
			return data->getTurn();
		}else{
			return false;
		}
	}

	int RoleConfig::getMoveSound( int ModeID )
	{
		RoleConfigData* data = getConfigData(ModeID);
		if (data)
		{
			return data->getMoveSound();
		}else{
			return 542;									//默认移动音效
		}
	}

	CCNode* RoleConfig::getActionEffect( int ModeID )
	{
		RoleConfigData* data = getConfigData(ModeID);
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
}