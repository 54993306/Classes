/************************************************************* 
*
*	
*		Name : SpineEffect
*
*		Author : Lin_Xiancheng
*
*		Time : 	10/22/2016 
*
*		Description : 当前实现的模式是延迟伤害模式(伤害数据是从武将身上来的)
*
*
*************************************************************/
#include "Battle/SpineFloor/SpineEffect.h"
#include "Battle/BaseRole.h"
#include "Battle/BattleCenter.h"
#include "Battle/skEffectData.h"
#include "Battle/EffectData.h"
#include "Battle/BattleTools.h"
#include "Battle/HurtCount.h"
#include "Battle/BattleMessage.h"
#include "Battle/Config/ConfigManage.h"
#include "Battle/ActionNameDefine.h"
namespace BattleSpace
{
	SpineEffect::SpineEffect( BaseRole* pRole,int pGrid )
	:mRole(pRole),mGrid(pGrid),mAnimation(nullptr),mHitRole(nullptr),mSortieNumber(0)
	{
		mSkillEffect = pRole->getCurrEffect();
		const EffectInfo* effectinfo = BattleConfig->getEffectInfo(mSkillEffect->getEffectID());
		SpineData* tData = SpineManage->getSpineData(ToString(effectinfo->getfoeEft()));
		if (!tData)
		{
			tData = SpineManage->getSpineData("20002");
			CCLOG("[ *ERROR ]  SpineEffect::SpineEffect Spine Model=%d IS NULL",effectinfo->getfoeEft()); 
		}
		SkeletonAnimation*  tAnimation = SkeletonAnimation::createWithData(tData->first);
		CCAssert(tAnimation,"SpineEffect::SpineEffect Spine NULL");
		initHitRole();
		mAnimation = tAnimation;
		mAnimation->setScale(0.5f);
		tAnimation->setAnimation(0,Attack_Action,true);
		tAnimation->eventListener = std::bind(&SpineEffect::SpineActionEvent,this,std::placeholders::_1,std::placeholders::_2);
		tAnimation->completeListener = [tAnimation](int trackIndex, int loopCount)	//lande表达式是参数闭包的概念
		{
			tAnimation->removeFromParentAndCleanup(true);
		};

		this->addChild(mAnimation);
	}
	//初始化受击目标
	void SpineEffect::initHitRole()
	{	
		vector<BaseRole*> VecAlive = mRole->getCurrSkillTargets(mSkillEffect->getTargetType());				//不直接使用格子去寻找是为了减少遍历的次数																
		for (BaseRole* tRole: VecAlive)
		{
			if ( tRole->inStandGrid(mGrid) )
			{
				if (tRole->getLogicState() == sLogicState::eInvincible ||
					tRole->getLogicState() == sLogicState::eFree		||
					tRole->getLogicState() == sLogicState::eUnHit	)
					break;
				mHitRole = tRole;
				return;		//只有一个受击目标的情况
			}
		}
	}

	void SpineEffect::SpineActionEvent( int trackIndex,spEvent* Event )
	{
		switch ((eActionEvent)Event->intValue)
		{
		case eActionEvent::eHitEvent:
			{
#ifndef BATTLE_TEST
				mSortieNumber += 1;
				if (mSortieNumber > mSkillEffect->getBatter() )
					break;
#endif
				AttackExcute();					//当前只处理了两种帧事件(如果动画上有多个帧事件的情况怎么处理？多次攻击吗)
			}break;
		case eActionEvent::eShark:
			{
				bNotification->postNotification(B_Shark,nullptr);
			}break;
		case eActionEvent::eAttackEventEnd:
			{
				if (mHitRole && mHitRole->getHp() <= 0)
					mHitRole->roleDie();
			}break;
		default:
			break;
		}
		if (Event->intValue >= 200)			//音效从200号文件开始播放
			PlaySound(Event->intValue);
	}


	SpineEffect* SpineEffect::createSpineEffect( BaseRole*pRole ,int pGrid)
	{
		SpineEffect* tEffect = new SpineEffect(pRole,pGrid);
		if (tEffect)
		{
			return tEffect;
		}else{
			delete tEffect;
			tEffect = nullptr;
			return tEffect;
		}
	}

	void SpineEffect::AttackExcute()
	{
		if (mHitRole)
		{
			BattleResult* tResult = BattleHurtCount->SpineEffectAttack(mRole,mHitRole,mSkillEffect);		//造成伤害
			mRole->mHittingAlive.clear();
			bNotification->postNotification(MsgDelayAttack,tResult);
		}
		//击杀的情况还没用进行实时的处理
		//spine结束的帧事件进行武将死亡的处理
	}
	//spine效果自己处理自己的状态，只处理一个范围的，多个范围的情况就创建多个spine效果
	//等需要多个spine效果一起出来的情况再对多个spine效果进行处理。
	//或是一个spine效果对多个范围造成伤害处理。
	//
};