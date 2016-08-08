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

#include "model/DataCenter.h"
#include "model/WarManager.h"
namespace BattleSpace
{
	BattleModel::BattleModel()
	:mBattleType(sBattleType::eNormalBattle),mStrategyType(sPvEStrategy::eNull),mManage(nullptr)
	{}

	BattleModel::~BattleModel()
	{}
	//��ս���������Խ��ж෽λ����չ����
	BattleModel* BattleModel::CreateBattleModel(sBattleType pBattleType)
	{
		BattleModel* pModel = new BattleModel();
		if (pModel && pModel->init())
		{
			pModel->setBattleType(pBattleType);
			pModel->autorelease();
			return pModel;
		}else{
			delete pModel;
			pModel = nullptr;
			return nullptr;
		}
	}

	bool BattleModel::init()
	{
		mManage = DataCenter::sharedData()->getWar();
		return true;
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
			if (pGrid / C_GRID_ROW > pRole->getMoveObject()->getgrid() / C_GRID_ROW ||
				pGrid < C_PVEStopGrid)		//���������ƶ�
				return false;
			return true;
		}
		return true;
	}
	//PVE���ҷ��佫���Զ��ƶ�����
	bool BattleModel::heroMove( BaseRole* pRole )
	{
		if ( !isPvEBattle() || pRole->getCriAtk() )
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
		if ( mManage->inUnDefineArea( tNextGrid ) || tNextGrid < C_PVEStopGrid )
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
		if ( isPvEBattle() )
			return false;
		return true;
	}

	bool BattleModel::battlePause()
	{
		if ( isPvEBattle() )
			return false;
		return true;
	}

}