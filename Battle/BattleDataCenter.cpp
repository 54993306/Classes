#include "Battle/BattleDataCenter.h"
#include "Battle/RoleBaseData.h"
#include <protos/stage_protocol.pb.h>
#include <protos/boss_protocol.pb.h>




/******************************************************/
#include "model/DataCenter.h"
#include "model/MapManager.h"
#include "model/WarManager.h"
#include "common/CommonFunction.h"


BattleDataCenter* BattleDataCenter::mDataControl = nullptr;

BattleDataCenter::SingletonDestroy BattleDataCenter::mDestroy;

BattleDataCenter::BattleDataCenter()
{

}

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
}

const  vector<HeroRoleData*>& BattleDataCenter::getHeroVector() const
{
	return mHeroVec;
}

const vector<MonsterRoleData*>& BattleDataCenter::getMonsterVector() const
{
	return mMonsterVec;
}

void BattleDataCenter::initHeroData( const protos::common::Hero* pData )
{
	HeroRoleData* tHeroData = HeroRoleData::create();
	tHeroData->readData(pData);
	tHeroData->retain();
	mHeroVec.push_back(tHeroData);
}

void BattleDataCenter::initMonsterData( const protos::common::Monster* pData )
{
	MonsterRoleData* tMonsterData = MonsterRoleData::create();
	tMonsterData->readData(pData);
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
}

void BattleDataCenter::initNormalStage( const google::protobuf::Message *pResponse )
{
	protos::BattleResponse*tData = (protos::BattleResponse*)pResponse;
	for (int i=0; i< tData->herolist_size(); i++)				//英雄		  
		initHeroData(&tData->herolist(i));
	for (int j=0; j< tData->monsterlist_size(); j++)			//怪物
		initMonsterData(&tData->monsterlist(j));
}
