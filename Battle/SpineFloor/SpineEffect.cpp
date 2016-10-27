/************************************************************* 
*
*	
*		Name : SpineEffect
*
*		Author : Lin_Xiancheng
*
*		Time : 	10/22/2016 
*
*		Description : ��ǰʵ�ֵ�ģʽ���ӳ��˺�ģʽ(�˺������Ǵ��佫��������)
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
		tAnimation->completeListener = [tAnimation](int trackIndex, int loopCount)	//lande���ʽ�ǲ����հ��ĸ���
		{
			tAnimation->removeFromParentAndCleanup(true);
		};

		this->addChild(mAnimation);
	}
	//��ʼ���ܻ�Ŀ��
	void SpineEffect::initHitRole()
	{	
		vector<BaseRole*> VecAlive = mRole->getCurrSkillTargets(mSkillEffect->getTargetType());				//��ֱ��ʹ�ø���ȥѰ����Ϊ�˼��ٱ����Ĵ���																
		for (BaseRole* tRole: VecAlive)
		{
			if ( tRole->inStandGrid(mGrid) )
			{
				if (tRole->getLogicState() == sLogicState::eInvincible ||
					tRole->getLogicState() == sLogicState::eFree		||
					tRole->getLogicState() == sLogicState::eUnHit	)
					break;
				mHitRole = tRole;
				return;		//ֻ��һ���ܻ�Ŀ������
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
				AttackExcute();					//��ǰֻ����������֡�¼�(����������ж��֡�¼��������ô������ι�����)
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
		if (Event->intValue >= 200)			//��Ч��200���ļ���ʼ����
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
			BattleResult* tResult = BattleHurtCount->SpineEffectAttack(mRole,mHitRole,mSkillEffect);		//����˺�
			mRole->mHittingAlive.clear();
			bNotification->postNotification(MsgDelayAttack,tResult);
		}
		//��ɱ�������û�ý���ʵʱ�Ĵ���
		//spine������֡�¼������佫�����Ĵ���
	}
	//spineЧ���Լ������Լ���״̬��ֻ����һ����Χ�ģ������Χ������ʹ������spineЧ��
	//����Ҫ���spineЧ��һ�����������ٶԶ��spineЧ�����д���
	//����һ��spineЧ���Զ����Χ����˺�����
	//
};