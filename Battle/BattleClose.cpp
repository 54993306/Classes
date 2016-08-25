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
#include "Battle/BattleConfigMacro.h"
#include "Battle/CombatGuideManage.h"
#include "Battle/BattleModel.h"

#include "tools/ShowTexttip.h"
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "common/CGameSound.h"
#include "netcontrol/CPlayerControl.h"
namespace BattleSpace
{
	BattleClose::BattleClose():mManage(nullptr),mRecvFinish(false)
	{}

	BattleClose::~BattleClose()
	{
		eventRemove();
	}

	bool BattleClose::init()
	{
		eventMonitor();
		mManage = BattleManage;
		return true;
	}

	void BattleClose::eventMonitor()
	{
		bNotification->addObserver(this,callfuncO_selector(BattleClose::roleDieMsg),MsgRoleDie,nullptr);
		bNotification->addObserver(this,callfuncO_selector(BattleClose::displayRoundTips),MsgMonsterTips,nullptr	);
		bNotification->addObserver(this,callfuncO_selector(BattleClose::battleResult),MsgBattleOver,nullptr);
		bNotification->addObserver(this,callfuncO_selector(BattleClose::roleObjectRemove),B_ActObjectRemove,nullptr);
	}

	void BattleClose::eventRemove()
	{
		bNotification->removeAllObservers(this);
	}

	void BattleClose::NextBatch( float dt )
	{
		srandNum();																	//设置随机种子
		mManage->setCurrBatch(mManage->getCurrBatch() + 1);
		mManage->initMonsterByBatch(mManage->getCurrBatch());								//初始化批次武将数据
		bNotification->postNotification(MsgUpBatchNumber);
		bNotification->postNotification(MsgNextBatchEnemy);
		if ( !mManage->lastBatch())
			displayRoundTips(nullptr);
		ManageCenter->getCombatGuideMg()->setCurrBatchGuide(nullptr);
		if(mManage->lastBatch() && !mManage->getAliveByType(sMonsterSpecies::eBoss))
			bNotification->postNotification(MsgShowStageWarning);
	}

	void BattleClose::displayRoundTips( CCObject* ob )
	{
		CCLabelAtlas* labAt = CCLabelAtlas::create(ToString(mManage->getCurrBatch()+1),"label/wave_number.png", 81, 84, '0');
		ShowTexttip(labAt, ccc3(255, 255, 255), ROUNDNUM, CCPointZero, 0, 0, 0, 200);
	}

	void BattleClose::roleObjectRemove( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (tRole->getOtherCamp() && tRole->getLastAlive())
		{
			if (!mManage->lastBatch())
			{
				this->scheduleOnce(schedule_selector(BattleClose::NextBatch),1);				//打下一批次延时时间
			}else{
				mManage->setLogicState(false);
				bNotification->postNotification(MsgHideControlUp);
				bNotification->postNotification(MsgCreateStory,CCInteger::create((int)StoryType::eOverStory));
				CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
			}
		}
	}

	void BattleClose::roleDieMsg( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (tRole->getOtherCamp())
		{
			otherCampDie(tRole);
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

	void BattleClose::otherCampDie( BaseRole* pRole )
	{
		if (mManage->checkBatchOver() || pRole->getCaptain())
		{
			pRole->setLastAlive(true);				//逻辑判定为最后一个死亡怪物
			if (!mManage->lastBatch())				//判断最后一回合
				return;
			mManage->setLogicState(false);
			bNotification->postNotification(MsgControlRemove);
			if (mManage->getStageIndex())
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
		if(mRecvFinish)
			return;
		if ((mManage->getVerifyNum() + mManage->getBossHurtCount()) != 97231000)
		{
			CCLOG("[ *ERROR ] CombatLoginc::scheduleForRequesBossFinish");				//验证造成的伤害是否被修改内存
			return;
		}
		CPlayerControl::getInstance().sendWorldBossFinish(
			mManage->getBossHurtCount(), mManage->getVerifyNum(), *mManage->getBossHurtVec());
	}

	void BattleClose::normalStage( CCObject* ob )
	{
		if ( mManage->getStageIndex() )
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
		if(mRecvFinish)
			return;
		if (BattleModelManage->isPvEBattle())
		{
			if (m_finishData.res)
			{
				CPlayerControl::getInstance().sendPveBattleFinish(m_finishData.res, 60, 0, 100);
			}else{
				CPlayerControl::getInstance().sendPveBattleFinish(m_finishData.res, 60, 100, 0);
			}
		}else{
			CPlayerControl::getInstance().sendBattleFinish(1, m_finishData.res, m_finishData.roundNum);
		}
	}

}