/************************************************************* 
 *
 *
 *		Data : 2016.8.29
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/RoleVariant.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
#include "Battle/RoleConfigData.h"
#include "Battle/ConstNum.h"
#include "Battle/BaseRoleData.h"
#include "Battle/BattleMessage.h"

namespace BattleSpace
{
	RoleVariant::RoleVariant(BaseRole* pRole):mRole(pRole),mTime(0),mVariantState(eVariantState::eNull)
		,mConfigData(nullptr),mUserTime(0),mVariantProcess(false),mBlinkState(true),mTouchTime(0)
	{}

	RoleVariant* RoleVariant::CreateVariant( BaseRole* pRole )
	{
		RoleVariant* tVariant = new RoleVariant(pRole);
		if (tVariant && tVariant->init())
		{
			tVariant->autorelease();
			return tVariant;
		}else{
			delete tVariant;
			tVariant = nullptr;
			return nullptr;
		}
	}

	bool RoleVariant::init()
	{
		return true;
	}

	void RoleVariant::updateVariant( float dt )
	{
		if ( !variantJudge() )
			return ;
		mTime += dt	;
		updateRage((mTime / mUserTime)*100);			//变身怒气值变化情况
		if (mTime < mUserTime) return;
		if (mVariantState != eVariantState::eCooldown || mRole->getOtherCamp())
		{
			mTime = 0;
			variantExcute();						//达到条件后触摸2s才变身
		}else{
			if (mRole->getInTouchState())
			{
				mTouchTime += dt;
				if (mTouchTime >= 2)
				{
					mTime = 0;
					variantExcute();						//达到条件后触摸2s才变身
				}
			}else{
				mTouchTime = 0;
			}
		}
	}

	bool RoleVariant::variantJudge()
	{
		if (!mConfigData)
		{
			mConfigData = mRole->getConfigData();		//没有配置变身的武将不会执行变身操作
			if (mConfigData->getVariant())
			{
				mVariantState = eVariantState::eCooldown;
				mUserTime = mConfigData->getVariantCooldown();
			}
		}
		if (mVariantState == eVariantState::eNull || !mRole->getBattle() ||
			mVariantProcess)
			return false;
		return true;
	}
	//这是一个循环的状态机,切换的方法是固定不变的。
	void RoleVariant::variantExcute()
	{
		switch (mVariantState)
		{
		case BattleSpace::eVariantState::eInVariant:			//变回原来的模型,播放特效
			{
				mVariantProcess = true;
				mRole->VariantBegin(false);
			}break;
		case BattleSpace::eVariantState::eCooldown:				//周围武将造成100点真是伤害,固定值，写死。
			{
				mVariantProcess = true;
				mRole->VariantBegin(true);
			}break;
		case BattleSpace::eVariantState::eCooling:
			{
				mVariantState = eVariantState::eCooldown;
				mUserTime = mConfigData->getVariantCooldown();
			}break;
		default:
			{
				CCLOG("[ *ERROR ] RoleVariant::updateVariant");
				mVariantState = eVariantState::eNull;
				mRole->updateRage(0);
			}break;
		}
	}
	//刷新进度条值
	void RoleVariant::updateRage(float pRate)
	{
		if (mVariantState == eVariantState::eCooling)
			return;
		if ((int)pRate == 90 && mBlinkState)	//武将将要变身，和变身将要结束时有一个闪动的效果
		{
			if (mVariantState == eVariantState::eCooldown)
			{
				mRole->colorBlink(C_ForEver,ccc3(255,0,255));
			}else{
				mRole->colorBlink(2,ccc3(255,0,255));
			}
			mBlinkState = false;
		}
			
		if (isVariant())
		{
			mRole->updateRage(100 - pRate);
			return;
		}
		mRole->updateRage(pRate);
	}
	//武将是否应该由一个常用属性变化的方法,应该有一个基础属性变化的方法,增加基础属性,和减少基础属性,一般都是以百分比来计算的
	void RoleVariant::PropertyChange( bool pAddProperty /*= false*/ )
	{
		mBlinkState = true;
		mVariantProcess = false;
		if (pAddProperty)
		{
			mVariantState = eVariantState::eInVariant;
			mUserTime = mConfigData->getVariantDuration();
			mRole->colorBlink(C_StopForEver,ccc3(255,0,255));		//停掉闪烁状态
			mRole->changeModel(mConfigData->getVariantModel());
			mRole->setMaxHp(mRole->getMaxHp() + mRole->getBaseData()->getRoleHp()*mConfigData->getVariantRate());
			mRole->setHp(mRole->getHp() + mRole->getBaseData()->getRoleHp()*mConfigData->getVariantRate());
			mRole->setAtk(mRole->getAtk() + mRole->getBaseData()->getRoleAttack()*mConfigData->getVariantRate());
			mRole->setDef(mRole->getDef() + mRole->getBaseData()->getRoleDefense()*mConfigData->getVariantRate());
			mRole->setHit(mRole->getHit() + mRole->getBaseData()->getRoleHit()*mConfigData->getVariantRate());
			mRole->setDoge(mRole->getDoge() + mRole->getBaseData()->getRoleDodge()*mConfigData->getVariantRate());
			mRole->setCrit(mRole->getCrit() + mRole->getBaseData()->getRoleCrit()*mConfigData->getVariantRate());
			mRole->setRegain(mRole->getRegain() + mRole->getBaseData()->getRoleRegain()*mConfigData->getVariantRate());
		}else{
			mVariantState = eVariantState::eCooling;
			mUserTime = mConfigData->getVariantCooling();	//冷却时间
			mRole->changeModel(C_DefaultModel);
			mRole->setMaxHp(mRole->getMaxHp() - mRole->getBaseData()->getRoleHp()*mConfigData->getVariantRate());
			//mRole->setHp(mRole->getHp() - mRole->getBaseData()->getRoleHp()*mConfigData->getVariantRate());			//血量只增不减
			mRole->setAtk(mRole->getAtk() - mRole->getBaseData()->getRoleAttack()*mConfigData->getVariantRate());
			mRole->setDef(mRole->getDef() - mRole->getBaseData()->getRoleDefense()*mConfigData->getVariantRate());
			mRole->setHit(mRole->getHit() - mRole->getBaseData()->getRoleHit()*mConfigData->getVariantRate());
			mRole->setDoge(mRole->getDoge() - mRole->getBaseData()->getRoleDodge()*mConfigData->getVariantRate());
			mRole->setCrit(mRole->getCrit() - mRole->getBaseData()->getRoleCrit()*mConfigData->getVariantRate());
			mRole->setRegain(mRole->getRegain() - mRole->getBaseData()->getRoleRegain()*mConfigData->getVariantRate());
		}
		bNotification->postNotification(MsgRoleVariant,mRole);
	}

	bool RoleVariant::isVariant()
	{
		if (mVariantState == eVariantState::eInVariant)
			return true;
		return false;
	}
}