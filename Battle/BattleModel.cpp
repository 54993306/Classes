/************************************************************* 
*
*
*		Data : 2016.7.11
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : ����ս��ģʽ
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
			if (pGrid < C_PVEStopGrid)		//���������ƶ�
				return false;
			return true;
		}
		return true;
	}
	//PVE���ҷ��佫���Զ��ƶ�����
	bool BattleModel::heroMove( BaseRole* pRole )
	{
		if ( !isPvEBattle() || pRole->getCriAtk() || pRole->getCaptain())
			return false;
		if (pRole->getRoleObject()->getMoveState()  != sStateCode::eNullState || !pRole->getMove())
			return true;
		if (pRole->getGridIndex() <= C_BEGINGRID-C_GRID_ROW*3)			
		{
			pRole->roleDie();							//����Ԥ����Χִ����������
			return true;
		}			
		int tNextGrid = pRole->getGridIndex()-C_GRID_ROW;
		vector<int> tDestinations;										//�ж��ƶ��������Ƿ�����������佫���Զ��ƶ�AI�޷�ʵ���л��佫Ч����
		pRole->initContainGrid(tNextGrid,tDestinations);
		vector<BaseRole*> tAreaAlives = pRole->getAliveInArea(tDestinations);
		if (tAreaAlives.size())
			return false;
		if ( BattleConfig->inUnDefineArea( tNextGrid ) || tNextGrid < C_PVEStopGrid )
		{
			return false;
		}else{
			pRole->setCommandGrid( tNextGrid );					//������ƶ���Χ��д���˵�
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