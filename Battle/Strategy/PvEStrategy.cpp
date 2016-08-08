/************************************************************* 
*
*
*		Data : 2016.6.22
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : 
*	//�ƶ����Ի����PVE��ѡ��ģʽ��ͬ����ͬ�Ĵ���
*	//�佫�ͷż��ܵĲ���ѡ����һ���ġ� �������佫�ٻ������������佫�ٻ���ɺ����ǳ����ܷ�Χ�ڴ���Ŀ�����ͷż��ܡ�
*	1. ���ز����Ƕ£����ȫ�ķ���ȥ�¡�
*	2. ���������ǲ��£�ֱ�ӳ��
*	3. ƽ����ԣ�ĳһ���ǰ��û����ʱ����Ǹ�λ�ù���ȥ
*
*************************************************************/
#include "Battle/Strategy/PvEStrategy.h"
#include "Battle/Strategy/TotalStrategy.h"
#include "Battle/BaseRole.h"
#include "Battle/BattleDataCenter.h"
#include "Battle/BattleModel.h"
#include "Battle/BaseRoleData.h"
#include "Battle/BattleMessage.h"
#include "Battle/BattleTools.h"
#include "Battle/RoleSkill.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/MoveObject.h"

#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "Battle/ConstNum.h"

namespace BattleSpace
{
	PvEStrategy::PvEStrategy():mPvEBattle(false),mBattleModel(nullptr),mExcuteStrategy(false),mAutoData(nullptr)
		,mTotal(nullptr),mCurrCost(0),mMaxCost(0),mCostSpeed(0),mManage(nullptr),mSkillRole(nullptr)
	{}

	PvEStrategy::~PvEStrategy()
	{
		CC_SAFE_RELEASE(mAutoData);
		mAutoData;
	}

	bool PvEStrategy::init()
	{
		mManage = DataCenter::sharedData()->getWar();
		mBattleModel = BattleData->getBattleModel();
		mPvEBattle = mBattleModel->isPvEBattle();
		mManage->initOtherCamp(mOtherRoles);
		mAutoData = AutoData::create();
		mAutoData->retain();
		initCostInfo();
		return true;
	}
	//�ܵĺ�������������֮�������
	void PvEStrategy::setTotalStrategy( TotalStrategy* pStrategy )
	{
		mTotal = pStrategy;
	}

	void PvEStrategy::initCostInfo()
	{
		for (auto tRole : mOtherRoles)
		{
			mMaxCost += tRole->getCostMax();
			mCostSpeed += tRole->getAddCost();
			if (tRole->getCaptain())
				mCurrCost += tRole->getInitCost();
		}

	}
	//��һ��������ս���ľ����жϣ���ȡ�佫��Ҫ��ע�ľ�����Ϣ
	void PvEStrategy::updatePvEStrategy()
	{
		if ( !mPvEBattle )
			return;
		//CCLOG("PvEStrategy CurrCost = %f,CostSpeed = %f",mCurrCost,mCostSpeed);
		mExcuteStrategy = true;
		mManage->updateAlive();
		mHeros = *mManage->inBattleHeros(true);
		mAliveOthers = *mManage->inBattleMonsters(true);
		interceptInfo();
		initCallHero();
#if BATTLE_TEST
		//mBattleModel->setStrategyType(sPvEStrategy::eDefense);
#endif
	}

	void PvEStrategy::loopPvEBattle(float dt)
	{
		if (!mPvEBattle)
			return;
		if (mCurrCost < mMaxCost)
		{
			mCurrCost += (mCostSpeed * dt + dt);
#if BATTLE_TEST
			//mCurrCost += 100;
#endif
			if (mCurrCost >= mMaxCost)
				mCurrCost = mMaxCost;
		}
		if (mExcuteStrategy)
		{
			mExcuteStrategy = false;
			excuteStrategy();
		}
	}

	void PvEStrategy::excuteStrategy()
	{
		switch (mBattleModel->getStrategyType())
		{
		case BattleSpace::sPvEStrategy::eBalance:
		case BattleSpace::sPvEStrategy::eDefense:
			{
				//��ʵ���ٻ���ʱ��Ҫ��һ����cost�Ĵ���Ӣ�۷�����UI��������
				if ( mAutoData->getInterceptRole())					//��Ҫ���ϱ����ص��佫,Χ�²��������֣��ٻ�����λ
				{
					if (mAutoData->getCallHero())
					{
						excuteCallRole();							
					}else{
						moveRoleObject();							//�ҵ����������������ģ��ҵ�ǰû�����赲����������佫����֧Ԯ
					}
				}else{
					if (mAutoData->getCallHero())					//�ޱ�������佫�����
					{
						callOtherRole();
					}else{
						playSkill();
					}
#if BATTLE_TEST
					//return ;
#endif
					if (mBattleModel->getStrategyType() == sPvEStrategy::eBalance)
						freeOtherMove();							//����Ѱ�в���(���ܸɵ�)�佫����׷��������
				}
			}break;
		case BattleSpace::sPvEStrategy::eAttack:
			{
				if (mAutoData->getCallHero())					//�ޱ�������佫�����
				{
					otherEnterMaxGrid(mAutoData->getCallHero());
				}else{
					playSkill();
				}
				freeOtherMove();									//�ٻ�����һ�н��г���ǹؼ�
			}break;
		default:CCLOG("[ *ERROR ]PvEStrategy::excutePvEStrategy");
		}
	}

	void PvEStrategy::interceptInfo()
	{
		for (auto tHero : mHeros)
		{
			if (tHero->getGridIndex() > C_CAPTAINGRID || 
				tHero->getCaptain() ||
				frontHasOther(tHero) ) /*C_PVEOtherArea*/
				continue;
			mAutoData->setInterceptRole(tHero);
			if (tHero->getGridIndex() - C_GRID_ROW >= C_PVEStopGrid )
			{
				mAutoData->setInterceptGrid(tHero->getGridIndex() - C_GRID_ROW);		
			}else{
				mAutoData->setInterceptGrid(tHero->getGridIndex());		
			}
			return;
		}
		mAutoData->setInterceptRole(nullptr);
		mAutoData->setInterceptGrid(INVALID_GRID);
	}
	//�ع��ķ����ǰ���ͬ�ĺͲ�ͬ�����ֳ������ѿɱ�ĺͲ��ɱ�����ֳ���
	void PvEStrategy::initCallHero()
	{
		for (auto tRole : mOtherRoles)
		{
			if (!tRole->getBattle()&&
				!tRole->getCaptain()&&
				!tRole->getFatherID()&&
				(!tRole->getRoleObject() || ( !tRole->getBattle() && tRole->getRoleObject())))	//�佫δ�����ɹ��򴴽��ڳ�ʼλ��
			{
				if (!tRole->getEnterTime() && mCurrCost > tRole->getBaseData()->getExpendCost())
				{
					mAutoData->setCallHero(tRole);
					break;
				}
			}
		}
	}
	//frontDifferentCamp   ��������ģʽ�������Ʒ��������ع�
	bool PvEStrategy::frontHasOther( BaseRole* pHero )
	{
		if (pHero->mStandGrids.empty())
			return true;
		for (auto tOther : mAliveOthers)
		{
			if (tOther->getCaptain() || tOther->getFatherID())
				continue;
			if (mTotal->sameRow(pHero->mStandGrids,tOther->mStandGrids))
			{
				if (tOther->getMaxGrid() <= pHero->getGridIndex())		//С�ں�
					return true;
			}
			if (mTotal->isMoveIntercept(tOther,pHero))			//�ж��Ƿ��Ѿ����������佫��ȥ�½�
				return true;
		}
		return false;
	}

	void PvEStrategy::excuteCallRole()
	{
		mAutoData->setInterceptRole(nullptr);
		BaseRole* tRole = mAutoData->getCallHero();
		mAutoData->setCallHero(nullptr);
		summonHero(tRole,mAutoData->getInterceptGrid());
		mAutoData->setInterceptGrid(INVALID_GRID);
	}

	void PvEStrategy::summonHero( BaseRole* pRole,int pGrid )
	{
		if (pRole && mCurrCost >= pRole->getBaseData()->getExpendCost())				//����cost�ж�
		{
			if (!pRole->getAliveState())
			{
				pRole->initAliveData();
				mManage->executeCaptainSkill(true);
			}
			bNotification->postNotification(MsgCreateRoleObject,pRole);		//�����佫��ʾ����
			setSummonGrid(pRole,pGrid);
			mCurrCost -=  pRole->getBaseData()->getExpendCost();				//�ۼ�cost  
		}
	}

	void PvEStrategy::setSummonGrid( BaseRole* pRole,int pGrid )
	{
		if (!mManage->inOtherEnter(pGrid) || pRole->hasOtherRole(pGrid))
		{
			for (auto tGrid : *mManage->getOtherEnter())
			{
				if (sameRow(pGrid,tGrid) && !pRole->hasOtherRole(tGrid))
				{
					pRole->setCommandGrid( tGrid );
					return;
				}
			}
			otherEnterMaxGrid(pRole);
		}else{
			pRole->setCommandGrid( pGrid );
		}
	}
	//�佫�ط�����
	void PvEStrategy::moveRoleObject()
	{
		BaseRole* tRole = findFreeHero();
		if ( !tRole )
		{
			if ( urgencyJudge() )
			{
				tRole = urgencyMove();
				if ( !tRole) return;
			}else{
				return ;
			}
		}
		int tGrid = converRow( mAutoData->getInterceptGrid() ) + C_PVPCol* C_GRID_ROW;		//ֱ�ӻط���˧ǰվλ������ס
		//vector<int> tStands;
		//tRole->initContainGrid(tGrid,tStands);												//Ӧ����һ���ж��Ƿ����ƶ���ȥ�Ĺ��̣��ҵ��Ǹ��佫���ƶ�������ס�ĸ���
		mAutoData->setInterceptRole(nullptr);
		mAutoData->setInterceptGrid(INVALID_GRID);
		if (hasOtherMove(tGrid))
			return;
		tRole->setCommandGrid( tGrid );
	}

	BaseRole* PvEStrategy::findFreeHero()
	{
		vector<int> tLines;
		for (auto tOther : mAliveOthers)
		{
			if (tOther->getCaptain()		|| 
				tOther->getCommandGrid()	||
				tOther->getFatherID()	||
				tOther->getCriAtk())				//��ɱ��״̬���ƶ�
				continue;
			if ( !frontHasHero(tOther) )
				return tOther;
			if (mTotal->hasHeroLine(tLines,tOther))			//�ظ��е�������ƶ��Ǹ�λ�ýϴ���λ�ý�С��Ӣ��Ҳ���ڵ�ǰ�и�������������ƶ���Сλ��Ӣ��
				return tOther;
		}
		return nullptr;
	}

	bool PvEStrategy::frontHasHero( BaseRole* pOther )
	{
		vector<BaseRole*>::reverse_iterator iter = mHeros.rbegin();			//�������,��λ�����Ŀ�ʼ
		for(;iter != mHeros.rend();iter++)
		{
			if ((*iter)->getCaptain())
				continue;
			if (mTotal->sameRow(pOther->mStandGrids,(*iter)->mStandGrids))
			{
				if (pOther->getMaxGrid() > (*iter)->getGridIndex())			//��ͬ�е�Ӣ�۳�Խ
					return false;
				return true;
			}
		}
		return false;
	}

	bool PvEStrategy::urgencyJudge()
	{
		for (auto tOther : mAliveOthers)
		{
			if ( tOther->getCaptain()	|| tOther->getFatherID())
				continue;
			if (converCol(tOther->getGridIndex()) < converCol(mAutoData->getInterceptRole()->getGridIndex()))
				return false;
		}
		return true;
	}

	BaseRole* PvEStrategy::urgencyMove()
	{
		for (auto tOther : mAliveOthers)
		{
			if (tOther->getCaptain()			||
				tOther->getFatherID()		||
				tOther->getCommandGrid()	)
				continue;
			return tOther;
		}
		return nullptr;
	}

	bool PvEStrategy::hasOtherMove( int pGrid )
	{
		if (pGrid == INVALID_GRID)return true;
		for (auto tOther : mAliveOthers)
		{
			if (tOther->getCaptain())
				continue;
			if (tOther->getMoveGrid() == pGrid)
				return true;
		}
		return false;
	}

	void PvEStrategy::callOtherRole()
	{
		vector<BaseRole*>::reverse_iterator iter = mHeros.rbegin();			//�������,��λ�����Ŀ�ʼ
		for (auto tEnterGrid : *mManage->getOtherEnter())
		{
			if (mBattleModel->getStrategyType() == sPvEStrategy::eBalance && !hasSameRowByOther(tEnterGrid))			//���Ƶ������жϿ��Էŵ��౾����ȥ���У����ж��Ƿ�ΪPVE��ս����һ���ķ�ʽ
			{
				mAutoData->setInterceptGrid(tEnterGrid);					//�ҳ����ö�����佫����һ��������еĸ��ӣ�������ң�Ȼ�����ҹ��ɣ�
				excuteCallRole();
				return ;
			}
			for(;iter != mHeros.rend();iter++)
			{
				if ( (*iter)->getCaptain() )
					continue;
				if (mTotal->vectorHasRow((*iter)->mStandGrids,converRow(tEnterGrid)))
				{
					if ((*iter)->hasOtherRole(tEnterGrid))
						continue;
					mAutoData->setInterceptGrid(tEnterGrid);					//�ҳ����ö�����佫����һ��������еĸ��ӣ�������ң�Ȼ�����ҹ��ɣ�
					excuteCallRole();
					return ;
				}
			}
		}
		otherEnterMaxGrid(mAutoData->getCallHero());
	}

	BaseRole* PvEStrategy::hasUnBattleOther()
	{
		for (auto tOther : mOtherRoles)
		{
			if (!tOther->getBattle() && !tOther->getCaptain())
				return tOther;				//����δ�����佫���ж��Ƿ�����ٻ��������ٻ����ٻ��������ٻ�λ�ã���
		}
		return nullptr;
	}

	BaseRole* PvEStrategy::hasTargetOther()
	{
		for (auto tOther : mAliveOthers)
		{
			if (tOther->getCaptain() || 
				tOther == mSkillRole || 
				tOther->getFatherID())
				continue;
			if (tOther->skillAreaHasTarget())
				return tOther;
		}
		return nullptr;
	}

	void PvEStrategy::playSkill()
	{
		if ( hasUnBattleOther() )										//����δ������佫
			return;
		BaseRole* tRole = hasTargetOther();								//����ʱ�䣬�ۼ�costֵ
		if (tRole && !tRole->getCriAtk() && !tRole->getSkillTime())		//�佫�����ͷż���ʱ�ǷǼ���״̬
		{
			const RoleSkill* skill = tRole->getBaseData()->getActiveSkill();
			if (mCurrCost < skill->getExpendCost()					||
				tRole->getLogicState() == sLogicState::eFree			|| 
				tRole->getLogicState() == sLogicState::eInvincible	)
				return;
			tRole->getRoleObject()->setMoveState(sStateCode::eNullState);
			tRole->getRoleObject()->TurnStateTo(sStateCode::eStandState);
			tRole->ResetAttackState();																	//����˱�ɱ����ť�������佫��û�н����ɱ��״̬�����,ǿ���л�����ɱ��״̬
			tRole->setCriAtk(true);
			mSkillRole = tRole;
			mCurrCost -= skill->getExpendCost();
		}
	}

	void PvEStrategy::moveUnTargetRole(BaseRole* tOther)
	{

		switch (mBattleModel->getStrategyType())
		{
		case BattleSpace::sPvEStrategy::eBalance:
			{
				if (hasSameRowByHero(tOther) || tOther->hasOtherRole(tOther->getGridIndex() + C_GRID_ROW))
					return;
				tOther->setCommandGrid( tOther->getGridIndex() + C_GRID_ROW );			//��ǰ�߲���
			}break;
		case BattleSpace::sPvEStrategy::eDefense:
			{
				return ;
			}break;
		case BattleSpace::sPvEStrategy::eAttack:
			{
				if ( tOther->hasOtherRole(tOther->getGridIndex() + C_GRID_ROW) )
					return;
				tOther->setCommandGrid( tOther->getGridIndex() + C_GRID_ROW );					//һֱ��ǰ�߲���
			}break;
		default:CCLOG("[ *ERROR ]PvEStrategy::excutePvEStrategy");
		}
	}

	void PvEStrategy::freeOtherMove()
	{
		for (auto tOther : mOtherRoles)
		{
			if (tOther->getHasTarget()		||		
				tOther->getAIState()			||
				tOther->getCommandGrid()		|| 
				tOther->getCaptain()			||
				tOther->getFatherID()		||
				!tOther->getBattle()			||
				tOther->currGridHasTarget()	)
				continue;
			moveUnTargetRole(tOther);
		}
	}

	bool PvEStrategy::hasSameRowByHero( BaseRole* tOther )
	{
		vector<BaseRole*>::reverse_iterator iter = mHeros.rbegin();			//�������,��λ�����Ŀ�ʼ
		for(;iter != mHeros.rend();iter++)							//ӭ�Ϲ���Ĳ���
		{
			if ((*iter)->getCaptain())
				continue;
			if (mTotal->sameRow(tOther->mStandGrids,(*iter)->mStandGrids))
				return true;
		}
		return false;
	}

	void PvEStrategy::otherEnterMaxGrid( BaseRole* pOther )
	{
		for (auto tGrid : *mManage->getOtherEnter())
		{
			if ( pOther->hasOtherRole(tGrid) || hasSameRowByOther(tGrid))
				continue;
			mAutoData->setInterceptGrid(tGrid);					//�ҳ����ö�����佫����һ��������еĸ��ӣ�������ң�Ȼ�����ҹ��ɣ�
			excuteCallRole();
			return;
		}
		for (auto tGrid : *mManage->getOtherEnter())				//�����Ҳ�վ�˵ģ���վ��������վ����ȥ��
		{
			if ( pOther->hasOtherRole(tGrid))
				continue;
			mAutoData->setInterceptGrid(tGrid);
			excuteCallRole();
			return;
		}
	}

	bool PvEStrategy::hasSameRowByOther( int pGrid )
	{
		for (auto tOther : mAliveOthers)
		{
			if (tOther->getCaptain())
				continue;
			for (int tGrid : tOther->mStandGrids)
			{
				if (sameRow(tGrid,pGrid))
					return true;
			}
		}
		return false;
	}
}