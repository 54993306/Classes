/************************************************************* 
*
*
*		Data : 2016.7.11
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : 多种战斗模式
*
*
*************************************************************/
#include "Battle/BattleModel.h"
#include "Battle/BaseRole.h"
#include "Battle/ConstNum.h"
#include "Battle/MoveObject.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "Battle/Config/ConfigManage.h"
namespace BattleSpace
{
	BattleModel::BattleModel()
	:mBattleType(sBattleType::eNormalBattle),mStrategyType(sPvEStrategy::eNull)
	{}

	BattleModel::~BattleModel()
	{}

	bool BattleModel::init()
	{
		return true;
	}

	void BattleModel::clearModelData()
	{
		mBattleType = sBattleType::eNormalBattle;
		mStrategyType = sPvEStrategy::eNull;
	}

	bool BattleModel::isBattleType( sBattleType pType )
	{
		if ( mBattleType == pType )
			return true;
		return false;
	}

	bool BattleModel::isPvEBattle()
	{
		if (mBattleType == sBattleType::ePvEBattle)
			return true;
		return false;
	}

	bool BattleModel::moveJudge( BaseRole* pRole,int pGrid )
	{
#if BATTLE_TEST
		return true;
#endif
		if ( isPvEBattle() )
		{
			if (pRole->getCallType() != sCallType::eCommon)
				return true;
			if (pGrid < C_PVEStopGrid)		//不能往回移动
				return false;
			return true;
		}
		return true;
	}
	//PVE中我方武将的自动移动策略
	bool BattleModel::heroMove( BaseRole* pRole )
	{
		if ( !isPvEBattle() || pRole->getCriAtk() || pRole->getCaptain())
			return false;
		if (pRole->getRoleObject()->getMoveState()  != sStateCode::eNullState || !pRole->getMove())
			return true;
		if (pRole->getGridIndex() <= C_BEGINGRID-C_GRID_ROW*3)			
		{
			pRole->roleDie();							//超出预定范围执行死亡操作
			return true;
		}			
		int tNextGrid = pRole->getGridIndex()-C_GRID_ROW;
		vector<int> tDestinations;										//判断移动区域内是否存在其他的武将（自动移动AI无法实现切换武将效果）
		pRole->initContainGrid(tNextGrid,tDestinations);
		vector<BaseRole*> tAreaAlives = pRole->getAliveInArea(tDestinations);
		if (tAreaAlives.size())
			return false;
		if ( BattleConfig->inUnDefineArea( tNextGrid ) || tNextGrid < C_PVEStopGrid )
		{
			return false;
		}else{
			pRole->setCommandGrid( tNextGrid );					//这里的移动范围是写死了的
			pRole->getRoleObject()->setMoveState(sStateCode::eWalkState);
		}
		return true;
	}

	bool BattleModel::layerMove()
	{
		return true;
	}

	bool BattleModel::battlePause()
	{
		if ( isPvEBattle() )
			return false;
		return true;
	}
}