#include "Battle/BattleDataCenter.h"
#include "Battle/BaseRoleData.h"
#include <protos/stage_protocol.pb.h>
#include <protos/boss_protocol.pb.h>

/******************************************************/
#include "model/DataCenter.h"
#include "model/MapManager.h"
#include "model/WarManager.h"
#include "common/CommonFunction.h"							//随机函数
namespace BattleSpace{
	BattleDataCenter* BattleDataCenter::mDataControl = nullptr;

	BattleDataCenter::SingletonDestroy BattleDataCenter::mDestroy;

	BattleDataCenter::BattleDataCenter(){}

	BattleDataCenter* BattleDataCenter::ShareBattleDataCenter()
	{
		if (mDataControl == nullptr)
			mDataControl = new BattleDataCenter;
		return mDataControl;
	}

	void BattleDataCenter::releaseRoleData()
	{
		for (auto tData : mHeroVec)
		{
			CC_SAFE_RELEASE(tData);
		}
		mHeroVec.clear();
		for (auto tData : mMonsterVec)
		{
			CC_SAFE_RELEASE(tData);
		}
		mMonsterVec.clear();
		mCallRole.clear();
	}

	void BattleDataCenter::initBattleData( const google::protobuf::Message *pResponse,bool pWorldBoss /*=false*/ )
	{
		srandNum();
		DataCenter::sharedData()->getWar()->BattleDataClear();
		DataCenter::sharedData()->getMap()->clearMap();
		if (pWorldBoss)
		{
			initWordBossStage(pResponse);
		}else{
			initNormalStage(pResponse);
		}
		DataCenter::sharedData()->getWar()->initCommonData();
	}

	const vector<BaseRoleData*>& BattleDataCenter::getHeroVector() const
	{
		return mHeroVec;
	}

	const vector<BaseRoleData*>& BattleDataCenter::getMonsterVector() const
	{
		return mMonsterVec;
	}

	const vector<BaseRoleData*>& BattleDataCenter::getCallRoleVector()const
	{		
		return mCallRole;
	}

	void BattleDataCenter::addCallRole( BaseRoleData* pBaseData )
	{
		for (auto tData : mCallRole)
		{
			if ( tData == pBaseData )
			{
#if CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM
				CCAssert(false,"BattleDataCenter::addCallRole( BaseRoleData* pBaseData )");
#else
				CCLOG("[ *ERROR ]BattleDataCenter::addCallRole");
				return;
#endif
			}
		}
		mCallRole.push_back(pBaseData);
	}

	void BattleDataCenter::initHeroData( const google::protobuf::Message *pResponse )
	{
		BaseRoleData* tHeroData = BaseRoleData::create(sDataType::eHero,pResponse);
		tHeroData->retain();
		mHeroVec.push_back(tHeroData);
	}

	void BattleDataCenter::initMonsterData( const google::protobuf::Message *pResponse )
	{
		BaseRoleData* tMonsterData = BaseRoleData::create(sDataType::eMonster,pResponse);
		tMonsterData->retain();
		mMonsterVec.push_back(tMonsterData);
	}

	void BattleDataCenter::initWordBossStage( const google::protobuf::Message *pResponse )
	{
		protos::WarResponse* tData = (protos::WarResponse*)pResponse;
		for (int i=0; i<tData->herolist_size(); i++)
			initHeroData(&tData->herolist(i));
		int tBossID = 0;											//用于做关卡ID
		for (int j=0; j<tData->monsters_size(); j++)
		{
			initMonsterData(&tData->monsters(j));
			if (tData->monsters(j).isboss())
				tBossID = tData->monsters(j).id();
		}
		DataCenter::sharedData()->getWar()->setBossHurtPe(tData->addhurt());
		DataCenter::sharedData()->getWar()->setWorldBoss(true);
		DataCenter::sharedData()->getWar()->setBatch(0);
		DataCenter::sharedData()->getWar()->setStageID(tBossID);
		DataCenter::sharedData()->getMap()->initMap(tBossID);
	}

	void BattleDataCenter::initNormalStage( const google::protobuf::Message *pResponse )
	{
		protos::BattleResponse*tData = (protos::BattleResponse*)pResponse;
		for (int i=0; i< tData->herolist_size(); i++)				//英雄		  
			initHeroData(&tData->herolist(i));
		for (int j=0; j< tData->monsterlist_size(); j++)			//怪物
			initMonsterData(&tData->monsterlist(j));
		DataCenter::sharedData()->getWar()->setBatch(tData->batch());
		DataCenter::sharedData()->getWar()->setStageID(tData->stageid());
		DataCenter::sharedData()->getMap()->initMap(tData->stageid()); 
	}

}