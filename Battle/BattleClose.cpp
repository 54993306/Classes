/************************************************************* 
 *
 *
 *		Data : 2016.7.6
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "Battle/BattleClose.h"
#include "Battle/BattleMessage.h"
#include "Battle/BaseRole.h"
#include "Battle/BattleMessage.h"
#include "Battle/BattleDataCenter.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/ConstNum.h"
#include "Battle/BaseRoleData.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "common/CommonFunction.h"

#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "common/CGameSound.h"
#include "netcontrol/CPlayerControl.h"
namespace BattleSpace
{
	BattleClose::BattleClose():mManage(nullptr)
	{}

	BattleClose::~BattleClose()
	{
		enentRemove();
	}

	bool BattleClose::init()
	{
		eventMonitor();
		mManage = DataCenter::sharedData()->getWar();
		return true;
	}

	void BattleClose::eventMonitor()
	{
		bNotification->addObserver(this,callfuncO_selector(BattleClose::roleDieMsg),MsgRoleDie,nullptr);
	}

	void BattleClose::enentRemove()
	{
		bNotification->removeAllObservers(this);
	}

	void BattleClose::roleDieMsg( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (tRole->getEnemy())
		{
			monsterDie(tRole);
		}else{
			if ( tRole->getFatherID() )								//喽啰死亡不进行初始化处理
				return;
			if (tRole->getCaptain())
			{
				NOTIFICATION->postNotification(MsgRoleStand);
				mManage->setLogicState(false);
				battleResult(nullptr);												//战斗失败
			}else{
				NOTIFICATION->postNotification(MsgRoleBattleState,tRole);
			}
		}
	}

	void BattleClose::monsterDie( BaseRole* pRole )
	{
		if (mManage->checkMonstOver())
		{
			pRole->setLastAlive(true);
			if (mManage->getBatch() > m_CurrBatchNum)				//判断最后一回合
				return;
			mManage->setLogicState(false);
			bNotification->postNotification(MsgControlRemove);
			if (mManage->getStageID())
				CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.3f);
			mManage->setbattleOver(true);
		}else{
			PlayEffectSound(SFX_423);
			mManage->changeCost(pRole->getInitCost());											//击杀敌方武将添加cost
			bNotification->postNotification(MsgMonsterDie,pRole->getRoleObject());
		}
	}

	void BattleClose::battleResult( CCObject* ob )
	{
		if (mManage->getWorldBoss())
		{
			wordBossFinish();						//世界boss结束情况
		}else{
			normalStage(ob);
		}
	}

	void BattleClose::wordBossFinish()
	{
		mManage->setbattleOver(true);
		scheduleForRequesBossFinish();
		this->runAction(
			CCRepeatForever::create(CCSequence::createWithTwoActions(
			CCDelayTime::create(5.0f), 
			CCCallFunc::create(this, callfunc_selector(BattleClose::scheduleForRequesBossFinish)))));
	}

	void BattleClose::scheduleForRequesBossFinish()
	{
		if(m_bRecvFinish)
			return;
		int hurt = mManage->getBossHurtCount();
		int checkNum = mManage->getVerifyNum();
		if ((checkNum + hurt) != 97231000)
		{
			CCLOG("[ *ERROR ] CombatLoginc::scheduleForRequesBossFinish");				//验证造成的伤害是否被修改内存
			return;
		}
		vector<int>* vec= mManage->getBossHurtVec();
		CPlayerControl::getInstance().sendWorldBossFinish(hurt, checkNum, *vec);
	}

	void BattleClose::normalStage( CCObject* ob )
	{
		if ( mManage->getStageID() )
		{
			if( ob )				
			{
				battleWin();
			}else{		
				battleFail();
			}
		}else{
			downloadPackageEnd(true);			//新手引导关卡
		}
	}

	void BattleClose::downloadPackageEnd( bool bAnswer )
	{
		if(!bAnswer)
			return;
		CScene* scene = GETSCENE(LoadBattleResource);
		((LoadBattleResource*)scene)->setRelease(true,skipSelectHero);
		CSceneManager::sharedSceneManager()->replaceScene(scene);
	}

	void BattleClose::battleWin()
	{
		PlayBackgroundMusic(SFX_Win,false);	
		BaseRole* tRole = mManage->getAliveByGrid(C_CAPTAINGRID);	
		int hp = tRole->getHp();
		if (hp>tRole->getBaseData()->getRoleHp())
			hp = tRole->getBaseData()->getRoleHp();
		m_finishData.res = true;
		m_finishData.roundNum = tRole->getBaseData()->getRoleHp();							//这个验证并没有什么意义
		scheduleForRequestFinish();
		this->runAction(
			CCRepeatForever::create(CCSequence::createWithTwoActions(
			CCDelayTime::create(5.0f), 
			CCCallFunc::create(this, callfunc_selector(BattleClose::scheduleForRequestFinish)))));
	}

	void BattleClose::battleFail()
	{
		m_finishData.res = false;
		m_finishData.roundNum = 0;
		scheduleForRequestFinish();
		this->runAction(
			CCRepeatForever::create(CCSequence::createWithTwoActions(
			CCDelayTime::create(5.0f), 
			CCCallFunc::create(this, callfunc_selector(BattleClose::scheduleForRequestFinish)))));
	}

	void BattleClose::scheduleForRequestFinish()
	{
		if(m_bRecvFinish)
			return;
		CPlayerControl::getInstance().sendBattleFinish(1, m_finishData.res, m_finishData.roundNum);
	}

	void BattleClose::roleObjectRemove( CCObject* ob )
	{
		BaseRole* alive = (BaseRole*)ob;
		if (alive->getEnemy() && alive->getLastAlive())
		{
			if (mManage->getBatch() > m_CurrBatchNum)										//可能会出现的一个bug，两个人同时死亡的间隔太近，会多次调用这个方法。应该从逻辑处进行最后一个死亡武将的判定而不应该从这里进行处理
			{
				srandNum();																	//设置随机种子
				mManage->initMonsterByBatch(m_CurrBatchNum+1);								//初始化批次武将数据(必须马上初始化数据,但是绘制可以延迟,否则多个武将连续死亡会直接战斗胜利出现)
				this->scheduleOnce(schedule_selector(BattleClose::NextBatch),1);				//打下一批次延时时间
			}else{
				mManage->setLogicState(false);
				bNotification->postNotification(MsgHideControlUp);
				CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
				bNotification->postNotification(MsgCreateStory,CCInteger::create((int)StoryType::eOverStory));
			}
		}
	}

	void BattleClose::NextBatch( float dt )
	{

	}

	
}