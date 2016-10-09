/************************************************************* 
 *
 *
 *		Data : 2016.9.6
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "Battle/Buff/BattleBuff.h"
#include "Battle/BuffData.h"
#include "Battle/RoleMacro.h"
#include "Battle/Buff/Buff_Property.h"
#include "Battle/Buff/Buff_Simpleness.h"

namespace BattleSpace
{
	BattleBuff::BattleBuff( const BuffData* pData,BaseRole* pRole ):mData(pData),mExcuteTime(0)
		,mDuration(0),mBuffState(sBuffState::eUnExcute),mRole(pRole)
	{}

	BattleBuff* BattleBuff::CreateDefault( const BuffData* pData,BaseRole* pRole )
	{
		BattleBuff* tBuff = new BattleBuff(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}

	BattleBuff* BattleBuff::CreateBuff( const BuffData* pData,BaseRole* pRole )
	{
		switch ((sBuffType)pData->getBuffType())
		{
		case BattleSpace::sBuffType::eClearDebuff:
			{
				return Buff_ClearDbuff::CreateBuff(pData,pRole);
			}break;
		case BattleSpace::sBuffType::eCurrBlood:
			{
				return Buff_Blood::CreateBuff(pData,pRole);
			}break;
		case BattleSpace::sBuffType::eAttack:
			{
				return Buff_Attack::CreateBuff(pData,pRole);
			}break;;
		case BattleSpace::sBuffType::eDefense:
			{
				return Buff_Defense::CreateBuff(pData,pRole);
			}break;;
		case BattleSpace::sBuffType::eCrit:
			{
				return Buff_Crit::CreateBuff(pData,pRole);
			}break;;
		case BattleSpace::sBuffType::eDodge:
			{
				return Buff_Dodge::CreateBuff(pData,pRole);
			}break;;
		case BattleSpace::sBuffType::eHit:
			{
				return Buff_HitRate::CreateBuff(pData,pRole);
			}break;;
		case BattleSpace::sBuffType::eMoveSpeed:
			{
				return Buff_MoveSpeed::CreateBuff(pData,pRole);
			}break;;
		case BattleSpace::sBuffType::eAttackSpeed:
			{
				return Buff_AttackSpeed::CreateBuff(pData,pRole);
			}break;;
		case BattleSpace::sBuffType::eFiring:
			{
				return Buff_Blood::CreateBuff(pData,pRole);
			}break;;
		case BattleSpace::sBuffType::eBleed:
			{
				return Buff_Blood::CreateBuff(pData,pRole); 
			}break;;
		case BattleSpace::sBuffType::eUnMove:
			{
				return Buff_UnMove::CreateBuff(pData,pRole);
			}break;;
		default:
			{
				return BattleBuff::CreateDefault(pData,pRole);
			}break;
		}
	}

	bool BattleBuff::init()
	{
		mDuration = mData->getBuffDuration();
		return true;
	}

	BaseRole* BattleBuff::getRole()
	{
		return mRole;
	}

	//可以根据客户端的使用情况,生成自己的ID。
	int BattleBuff::getBuffID() const
	{
		return mData->getBuffID();
	}

	sBuffType BattleBuff::getBuffType() const
	{
		return (sBuffType)mData->getBuffType();
	}

	int BattleBuff::getBF_Number() const
	{
		return mData->getChangeNum();
	}

	float BattleBuff::getBF_Rate() const
	{
		return mData->getPrecent()* 0.01f;
	}

	int BattleBuff::getTriggerRate() const
	{
		return mData->getTriggerRate();
	}

	bool BattleBuff::getDBuff() const
	{
		return mData->getIsDBuff();
	}

	int BattleBuff::getBF_Target() const
	{
		return mData->getBuffTarget();
	}

	int BattleBuff::getBF_Level() const
	{
		return mData->getBuffLevel();
	}

	sRoleNature BattleBuff::getBF_Property() const
	{
		return mData->getProperty();
	}

	sBuffState BattleBuff::getBF_State() const
	{
		return mBuffState;
	}

	bool BattleBuff::isRemove() const
	{
		if ( mBuffState == sBuffState::eRemove || 
			 mBuffState == sBuffState::eNull  )
			return true;
		return false;
	}

	void BattleBuff::updateBuff( float dt )
	{
		if ( isRemove() )
			return;
		if (mDuration < dt)				//持续时间已经达到,标记移除buff
		{
			mDuration = 0;
			mBuffState = sBuffState::eRemove;
			ExcuteLogic(false);
		}else{
			mDuration -= dt;
			mExcuteTime += dt;
			if (mExcuteTime >= 1)		//默认1s执行一次buff,掉血类的，其他类型只执行一起
			{
				mExcuteTime = 0;
				if (mBuffState == sBuffState::eUnExcute)
				{
					mBuffState = sBuffState::eExcuted;
					ExcuteLogic(true);
				}
			}
		}
	}
}