
/************************************************************* 
 *
 *
 *		Data : 2016.8.26
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 解析管理武将配置数据
 *
 *
 *************************************************************/

#include "RoleConfig.h"
#include "Battle/EffectObject.h"
#include "Battle/RoleConfigData.h"
namespace BattleSpace
{
	RoleConfig::RoleConfig():mDefultData(nullptr)
	{}

	RoleConfig::~RoleConfig()
	{
		ClearData();
	}

	void RoleConfig::ClearData()
	{
		for (auto i : mMapData)
		{
			CC_SAFE_RELEASE(i.second);
		}
		mMapData.clear();
		CC_SAFE_RELEASE(mDefultData);
		mDefultData = nullptr;
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

		if (doc.HasParseError()||!doc.IsObject() || !doc.HasMember("heroSoundData"))
		{    
			CCLOG("[ ERROR ] CHeroSoundData::reloadFile_doc.HasParseError() %s",doc.GetParseError());
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
			const rapidjson::Value &tData = array[i];
			RoleConfigData* tConfigData = RoleConfigData::create();
			if(tData.HasMember("model_id"))
			{
				tConfigData->setModeID(tData["model_id"].GetInt());
			}else{
				CCLOG("[ *ERROR ] CRroleData::initData Can't Find Mode ID");
				continue;
			}
			if(tData.HasMember("sound_move"))
				tConfigData->setMoveSound(tData["sound_move"].GetInt());
			if(tData.HasMember("Turn"))
				tConfigData->setTurn(tData["Turn"].GetBool());
			if(tData.HasMember("HitEffect"))
				tConfigData->setHitEffect(tData["HitEffect"].GetInt());
			if(tData.HasMember("HitEffectX"))
				tConfigData->setHitEffectX(tData["HitEffectX"].GetInt());
			if(tData.HasMember("HitEffectY"))
				tConfigData->setHitEffectY(tData["HitEffectY"].GetInt());
			if(tData.HasMember("HitEffectScale"))
				tConfigData->setHitEffectScale(tData["HitEffectScale"].GetDouble());
			if (tData.HasMember("Variant"))
				tConfigData->setVariant(tData["Variant"].GetBool());
			if(tData.HasMember("VariantScale"))
				tConfigData->setVariantScale(tData["VariantScale"].GetDouble());
			if (tData.HasMember("VariantCooldown"))
				tConfigData->setVariantCooldown(tData["VariantCooldown"].GetDouble());
			if (tData.HasMember("VariantDuration"))
				tConfigData->setVariantDuration(tData["VariantDuration"].GetDouble());
			if (tData.HasMember("VariantCooling"))
				tConfigData->setVariantCooling(tData["VariantCooling"].GetDouble());
			if (tData.HasMember("VariantRate"))
				tConfigData->setVariantRate(tData["VariantRate"].GetDouble()*0.01f);
			if (tData.HasMember("VariantEffect"))
				tConfigData->setVariantEffect(tData["VariantEffect"].GetInt());
			if (tData.HasMember("VariantModel"))
				tConfigData->setVariantModel(tData["VariantModel"].GetInt());
			AddConfigData(tConfigData);
		}
		mDefultData = RoleConfigData::create();
		mDefultData->retain();
		return true;
	}

	void RoleConfig::AddConfigData( RoleConfigData* pData)
	{
		map<int,RoleConfigData*>::iterator iter = mMapData.find(pData->getModeID());
		if (iter != mMapData.end())
		{
			CC_SAFE_RELEASE(iter->second);
			mMapData.erase(iter);
			CCLOG("[ *ERROR ] CRroleData::AddConfigData Mode Repeat = %d",pData->getModeID());
		}
		pData->retain();
		mMapData[pData->getModeID()] = pData;
	}

	RoleConfigData* RoleConfig::getConfigData( int ModeId )
	{
		map<int,RoleConfigData*>::iterator iter = mMapData.find(ModeId);
		if (iter != mMapData.end())
			return iter->second;
		return mDefultData;
	}
}