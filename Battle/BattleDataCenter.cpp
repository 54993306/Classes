
#include "Battle/HeroData.h"
#include "Battle/BattleDataCenter.h"
#include "Battle/BaseRoleData.h"
#include "Battle/MonsterData.h"
#include "Battle/BattleModel.h"
#include "Battle/ConstNum.h"
#include <protos/stage_protocol.pb.h>
#include <protos/boss_protocol.pb.h>
#include <protos/pvp_protocol.pb.h>

/******************************************************/
#include "Battle/BattleCenter.h"
#include "Battle/MapManager.h"
#include "Battle/WarManager.h"
#include "common/CommonFunction.h"							//随机函数
namespace BattleSpace
{
	BattleDataCenter* BattleDataCenter::mDataControl = nullptr;

	BattleDataCenter::SingletonDestroy BattleDataCenter::mDestroy;

	BattleDataCenter::BattleDataCenter()
	:mBattleModel(nullptr)
	{}

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
		for (auto tData : mChildRoleVec)
		{
			CC_SAFE_RELEASE(tData);
		}
		mChildRoleVec.clear();
		for (auto tData : mPvPHeros)
		{
			CC_SAFE_RELEASE(tData);
		}
		mPvPHeros.clear();
		mBaseRoleData.clear();
		CC_SAFE_RELEASE(mBattleModel);
		mBattleModel = nullptr;
	}

	void BattleDataCenter::initBaseRoleDataVector()
	{
		mBaseRoleData.insert(mBaseRoleData.end(),mHeroVec.begin(),mHeroVec.end());
		mBaseRoleData.insert(mBaseRoleData.end(),mPvPHeros.begin(),mPvPHeros.end());
		mBaseRoleData.insert(mBaseRoleData.end(),mMonsterVec.begin(),mMonsterVec.end());
		mBaseRoleData.insert(mBaseRoleData.end(),mChildRoleVec.begin(),mChildRoleVec.end());
		BattleManage->initCommonData();
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

	const vector<MonsterData*>& BattleDataCenter::getChildRoleDatas() const
	{
		return mChildRoleVec;
	}

	const vector<HeroData*>& BattleDataCenter::getPvPHeros() const
	{
		return mPvPHeros;
	}

	BattleModel* BattleDataCenter::getBattleModel()
	{
		return mBattleModel;
	}

	void BattleDataCenter::initHeroData( const protos::common::Hero* pData,bool pPvPHero/*= false */)
	{
		HeroData* tHeroData = HeroData::create();
		tHeroData->readData(pData);
		tHeroData->retain();
		if (pPvPHero)
		{
			tHeroData->setOtherCamp(true);
			tHeroData->setRoleNature(sRoleType::eRivalRole);
			mPvPHeros.push_back(tHeroData);
		}else{
			mHeroVec.push_back(tHeroData);
			tHeroData->setRoleNature(sRoleType::eHeroRole);
		}
	}

	void BattleDataCenter::initMonsterData( const protos::common::Monster* pData )
	{
		MonsterData* tMonsterData = MonsterData::create();
		tMonsterData->readData(pData);
		tMonsterData->retain();
		tMonsterData->setOtherCamp(true);
		if (tMonsterData->getRoleNature() == sRoleType::eChildRole)
		{
			mChildRoleVec.push_back(tMonsterData);			//召唤类武将可能是英雄也可能是怪物
		}else{
			mMonsterVec.push_back(tMonsterData);
		}
	}

	BaseRoleData* BattleDataCenter::getChildRoleData( int pRoleID ) const 
	{
		for (auto tRole : mChildRoleVec)
		{
			if (tRole->getMonsterID() == pRoleID)
				return tRole;
		}
		return nullptr;
	}


	void BattleDataCenter::initWordBossStage( const google::protobuf::Message *pResponse )
	{
		BattleManage->clearOldData();
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
		BattleManage->setBossHurtPe(tData->addhurt());
		BattleManage->setWorldBoss(true);
		BattleManage->setBatch(0);
		BattleManage->setStageID(tBossID);
		ManageCenter->getMap()->initMap(tBossID);
		mBattleModel = BattleModel::CreateBattleModel(sBattleType::eWordBossBattle);
		mBattleModel->retain();
		initBaseRoleDataVector();
	}

	void BattleDataCenter::initNormalStage( const google::protobuf::Message *pResponse )
	{
		BattleManage->clearOldData();
		protos::BattleResponse*tData = (protos::BattleResponse*)pResponse;
		for (int i=0; i< tData->herolist_size(); i++)				//英雄		  
			initHeroData(&tData->herolist(i));
		for (int j=0; j< tData->monsterlist_size(); j++)			//怪物
			initMonsterData(&tData->monsterlist(j));
		BattleManage->setBatch(tData->batch());
		BattleManage->setStageID(tData->stageid());
		ManageCenter->getMap()->initMap(tData->stageid()); 
		mBattleModel = BattleModel::CreateBattleModel(sBattleType::eNormalBattle);
		mBattleModel->retain();
		initBaseRoleDataVector();
	}

	void BattleDataCenter::initPvEData( const google::protobuf::Message *pResponse)
	{
		protos::StartPvpRes*tData = (protos::StartPvpRes*)pResponse;
		if (!tData->result())		//挑战次数不足挑战失败
			return ;
		BattleManage->clearOldData();
		mBattleModel = BattleModel::CreateBattleModel(sBattleType::ePvEBattle);
		mBattleModel->retain();
		mBattleModel->setStrategyType((sPvEStrategy)tData->status());
		for (int i=0;i<tData->opponents_size	();i++)
			initHeroData(&tData->opponents(i),true);
		for (int i=0;i<tData->hero_list_size();i++)
			initHeroData(&tData->hero_list(i));
		for (int i=0;i<tData->call_monster_size();i++)
			initMonsterData(&tData->call_monster(i));
		BattleManage->setBatch(0);	
		BattleManage->setStageID(C_PVEStage);		//关卡ID类的需要做特殊的处理
		ManageCenter->getMap()->initMap(C_PVEStage); 
		initBaseRoleDataVector();
	}
}