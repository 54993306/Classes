
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
	//静态函数中不能使用类的成员，必须要有对象才能使用类的成员
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
				return FireTrap::createTrap(pData);	//普通火类地形没有需要特殊处理的地方
			}break;
		case BattleSpace::sTrapType::eWaterTrap:
			{
				return WaterTrap::createTrap(pData);	//普通水类地形没有需要特殊处理的地方
			}break;
		case BattleSpace::sTrapType::eWoodTrap:
			{
				return WoodTrap::createTrap(pData);	//普通木类地形没有需要特殊处理的地方
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
	//属性的影响除了正向，还有一个反向，只有buff实现了这个正反的效果处理
	//（不可以把同时存在增减变化的对属性的影响分散到各处，集中到buff中进行处理）
	void BattleTrap::TrapImpact()
	{
		booldImpact();
		buffImpact();
	}
	//有加血减血的效果处理，目前来看只有减血
	void BattleTrap::booldImpact()
	{
		//只能实现加减血，但是可以根据属性值来计算，根据血量最大百分比，根据防御值等，值越大越大或其他。
		mTriggerRole->changeBoold(- getDamage());
	}
	//将对buff的耦合都放到武将身上去处理。
	void BattleTrap::buffImpact()
	{
		mTriggerRole->AddBuff(mConstData->getTrapBuff());
	}
}