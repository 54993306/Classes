#include "Battle/HeroData.h"
#include "Battle/BattleDataCenter.h"
#include "Battle/BaseRoleData.h"
#include "Battle/MonsterData.h"
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
		for (auto tData : mCallRoleVec)
		{
			CC_SAFE_RELEASE(tData);
		}
		mCallRoleVec.clear();
		mBaseRoleData.clear();
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
		initBaseRoleDataVector();
		DataCenter::sharedData()->getWar()->initCommonData();
	}

	void BattleDataCenter::initBaseRoleDataVector()
	{
		mBaseRoleData.insert(mBaseRoleData.end(),mHeroVec.begin(),mHeroVec.end());
		mBaseRoleData.insert(mBaseRoleData.end(),mMonsterVec.begin(),mMonsterVec.end());
		mBaseRoleData.insert(mBaseRoleData.end(),mCallRoleVec.begin(),mCallRoleVec.end());
	}

	const vector<BaseRoleData*>& BattleDataCenter::getRoleDatas() const
	{
		return mBaseRoleData;
	}

	const vector<HeroData*>& BattleDataCenter::getHeroVector() const
	{
		return mHeroVec;
	}

	const vector<MonsterData*>& BattleDataCenter::getMonsterVector() const
	{
		return mMonsterVec;
	}

	const vector<MonsterData*>& BattleDataCenter::getCallRoleDatas() const
	{
		return mCallRoleVec;
	}

	void BattleDataCenter::initHeroData( const protos::common::Hero* pData )
	{
		HeroData* tHeroData = HeroData::create();
		tHeroData->readData(pData);
		tHeroData->retain();
		mHeroVec.push_back(tHeroData);
	}

	void BattleDataCenter::initMonsterData( const protos::common::Monster* pData )
	{
		MonsterData* tMonsterData = MonsterData::create();
		tMonsterData->readData(pData);
		tMonsterData->retain();
		if (tMonsterData->getCallRole())
		{
			mCallRoleVec.push_back(tMonsterData);			//召唤类武将可能是英雄也可能是怪物
		}else{
			mMonsterVec.push_back(tMonsterData);
		}
	}

	BaseRoleData* BattleDataCenter::getCallRoleData( int pRoleID ) const 
	{
		for (auto tRole : mCallRoleVec)
		{
			if (tRole->getMonsterID() == pRoleID)
				return tRole;
		}
		return nullptr;
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