
/************************************************************* 
*
*
*		Data : 2016.8.16
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#include "Battle/Landform/BattleTrap.h"
#include "Battle/BaseRole.h"
#include "Battle/TrapData.h"
#include "Battle/ConstNum.h"
#include "Battle/RoleObjectMacro.h"
#include "Battle/Landform/ObstacleTrap.h"
#include "Battle/Landform/NatureTrap.h"
namespace BattleSpace
{
	BattleTrap::BattleTrap(const TrapData* pData):mActivate(false),mTriggerSpace(C_TrapSpace)
	,mConstData(pData),mExcute(true),mTriggerRole(nullptr)
	{}

	BattleTrap* BattleTrap::createDefult(const TrapData* pData)
	{
		BattleTrap* tTrap = new BattleTrap(pData);
		if (tTrap)
		{
			tTrap->autorelease();
			return tTrap;
		}else{
			delete tTrap;
			tTrap = nullptr;
			return nullptr;
		}
	}
	//��̬�����в���ʹ����ĳ�Ա������Ҫ�ж������ʹ����ĳ�Ա
	BattleTrap* BattleTrap::createTrap( const TrapData* pData )
	{
		switch (pData->getTrapType())
		{
		case BattleSpace::sTrapType::eFireObstacle:
			{
				return FireObstacle::createTrap(pData);
			}break;
		case BattleSpace::sTrapType::eNullType:
			{
				return BattleTrap::createDefult(pData);		
			}break;
		case BattleSpace::sTrapType::eObstacle:
			{
				return ObstacleTrap::createTrap(pData);
			}break;
		case BattleSpace::sTrapType::eFireTrap:
			{
				return FireTrap::createTrap(pData);	//��ͨ�������û����Ҫ���⴦��ĵط�
			}break;
		case BattleSpace::sTrapType::eWaterTrap:
			{
				return WaterTrap::createTrap(pData);	//��ͨˮ�����û����Ҫ���⴦��ĵط�
			}break;
		case BattleSpace::sTrapType::eWoodTrap:
			{
				return WoodTrap::createTrap(pData);	//��ͨľ�����û����Ҫ���⴦��ĵط�
			}break;
		case BattleSpace::sTrapType::ePlayerObstacle:
			{
				return PlayerObstacle::createTrap(pData);
			}break;
		case BattleSpace::sTrapType::eMonsterObstacle:
			{
				return MonsterObstacle::createTrap(pData);
			}break;
		case BattleSpace::sTrapType::eBlood:
			{
				return BloodTrap::createTrap(pData);
			}break;
		default:
			{
				CCLOG("[ *ERROR ] BattleTrap::CreateTrap Lost Type %d",(int)pData->getTrapType());
				return BattleTrap::createDefult(pData);
			}break;
		}
	}

	void BattleTrap::updateState( float dt )
	{
		if (mExcute)
		{
			if ( !mActivate ) return;
			mExcute = false;
			ExcuteAI();
		}else{
			mTriggerSpace -= dt	;
			if ( mTriggerSpace <= 0 )
			{
				mTriggerSpace = C_TrapSpace;
#if BATTLE_TEST
				//mTriggerSpace = 0.2f;
#endif
				mExcute = true;
			}
		}
	}

	int BattleTrap::getTrapID() const
	{
		return mConstData->getTrapID();
	}

	int BattleTrap::getTrapModel() const
	{
		return mConstData->getTrapModel();
	}

	BattleSpace::sTrapType BattleTrap::getTrapType() const
	{
		return mConstData->getTrapType();
	}

	int BattleTrap::getGridIndex() const
	{
		return mConstData->getPosition();
	}

	int BattleTrap::getAtbType() const
	{
		return mConstData->getAtbType();
	}

	float BattleTrap::getRate() const
	{
		return mConstData->getRate();
	}

	int BattleTrap::getDamage() const
	{
		return mConstData->getDmage();
	}

	int BattleTrap::getRotation() const
	{
		return mConstData->getRotatione();
	}

	bool BattleTrap::throughJudge(BaseRole* pRole /*=nullptr*/)
	{
		return true;
	}

	bool BattleTrap::RoleJudge()
	{
		return true;
	}

	void BattleTrap::ExcuteAI()
	{
		if ( !RoleJudge() )
			return ;
		TrapImpact();
	}
	//���Ե�Ӱ��������򣬻���һ������ֻ��buffʵ�������������Ч������
	//�������԰�ͬʱ���������仯�Ķ����Ե�Ӱ���ɢ�����������е�buff�н��д���
	void BattleTrap::TrapImpact()
	{
		booldImpact();
		buffImpact();
	}
	//�м�Ѫ��Ѫ��Ч������Ŀǰ����ֻ�м�Ѫ
	void BattleTrap::booldImpact()
	{
		//ֻ��ʵ�ּӼ�Ѫ�����ǿ��Ը�������ֵ�����㣬����Ѫ�����ٷֱȣ����ݷ���ֵ�ȣ�ֵԽ��Խ���������
		mTriggerRole->changeBoold(- getDamage());
	}
	//����buff����϶��ŵ��佫����ȥ����
	void BattleTrap::buffImpact()
	{
		mTriggerRole->AddBuff(mConstData->getTrapBuff());
	}
}