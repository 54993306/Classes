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
		updateRage((mTime / mUserTime)*100);			//����ŭ��ֵ�仯���
		if (mTime < mUserTime) return;
		if (mVariantState != eVariantState::eCooldown || mRole->getOtherCamp())
		{
			mTime = 0;
			variantExcute();						//�ﵽ��������2s�ű���
		}else{
			if (mRole->getInTouchState())
			{
				mTouchTime += dt;
				if (mTouchTime >= 2)
				{
					mTime = 0;
					variantExcute();						//�ﵽ��������2s�ű���
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
			mConfigData = mRole->getConfigData();		//û�����ñ�����佫����ִ�б������
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
	//����һ��ѭ����״̬��,�л��ķ����ǹ̶�����ġ�
	void RoleVariant::variantExcute()
	{
		switch (mVariantState)
		{
		case BattleSpace::eVariantState::eInVariant:			//���ԭ����ģ��,������Ч
			{
				mVariantProcess = true;
				mRole->VariantBegin(false);
			}break;
		case BattleSpace::eVariantState::eCooldown:				//��Χ�佫���100�������˺�,�̶�ֵ��д����
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
	//ˢ�½�����ֵ
	void RoleVariant::updateRage(float pRate)
	{
		if (mVariantState == eVariantState::eCooling)
			return;
		if ((int)pRate == 90 && mBlinkState)	//�佫��Ҫ�����ͱ���Ҫ����ʱ��һ��������Ч��
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
	//�佫�Ƿ�Ӧ����һ���������Ա仯�ķ���,Ӧ����һ���������Ա仯�ķ���,���ӻ�������,�ͼ��ٻ�������,һ�㶼���԰ٷֱ��������
	void RoleVariant::PropertyChange( bool pAddProperty /*= false*/ )
	{
		mBlinkState = true;
		mVariantProcess = false;
		if (pAddProperty)
		{
			mVariantState = eVariantState::eInVariant;
			mUserTime = mConfigData->getVariantDuration();
			mRole->colorBlink(C_StopForEver,ccc3(255,0,255));		//ͣ����˸״̬
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
			mUserTime = mConfigData->getVariantCooling();	//��ȴʱ��
			mRole->changeModel(C_DefaultModel);
			mRole->setMaxHp(mRole->getMaxHp() - mRole->getBaseData()->getRoleHp()*mConfigData->getVariantRate());
			//mRole->setHp(mRole->getHp() - mRole->getBaseData()->getRoleHp()*mConfigData->getVariantRate());			//Ѫ��ֻ������
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