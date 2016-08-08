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
*	//移动策略会根据PVE的选择模式不同做不同的处理。
*	//武将释放技能的策略选择都是一样的。 优先做武将召唤处理，当所有武将召唤完成后，若是出技能范围内存在目标则释放技能。
*	1. 稳守策略是堵，以最安全的方法去堵。
*	2. 进攻策略是不堵，直接冲锋
*	3. 平衡策略：某一侧的前方没有人时则从那个位置攻过去
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
	//总的函数跟下属函数之间的区别
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
	//第一步首先做战场的局势判断，提取武将需要关注的局势信息
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
				//在实际召唤的时候要做一个减cost的处理，英雄方面在UI处处理了
				if ( mAutoData->getInterceptRole())					//需要马上被拦截的武将,围堵策略有两种，召唤和移位
				{
					if (mAutoData->getCallHero())
					{
						excuteCallRole();							
					}else{
						moveRoleObject();							//找到距离这个怪物最近的，且当前没有在阻挡其他怪物的武将过来支援
					}
				}else{
					if (mAutoData->getCallHero())					//无必须防守武将的情况
					{
						callOtherRole();
					}else{
						playSkill();
					}
#if BATTLE_TEST
					//return ;
#endif
					if (mBattleModel->getStrategyType() == sPvEStrategy::eBalance)
						freeOtherMove();							//主动寻敌策略(不能干等)武将不会追出警戒列
				}
			}break;
		case BattleSpace::sPvEStrategy::eAttack:
			{
				if (mAutoData->getCallHero())					//无必须防守武将的情况
				{
					otherEnterMaxGrid(mAutoData->getCallHero());
				}else{
					playSkill();
				}
				freeOtherMove();									//召唤在哪一行进行冲锋是关键
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
	//重构的方向是把相同的和不同的区分出来，把可变的和不可变的区分出来
	void PvEStrategy::initCallHero()
	{
		for (auto tRole : mOtherRoles)
		{
			if (!tRole->getBattle()&&
				!tRole->getCaptain()&&
				!tRole->getFatherID()&&
				(!tRole->getRoleObject() || ( !tRole->getBattle() && tRole->getRoleObject())))	//武将未创建成功或创建在初始位置
			{
				if (!tRole->getEnterTime() && mCurrCost > tRole->getBaseData()->getExpendCost())
				{
					mAutoData->setCallHero(tRole);
					break;
				}
			}
		}
	}
	//frontDifferentCamp   两个策略模式都有类似方法可以重构
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
				if (tOther->getMaxGrid() <= pHero->getGridIndex())		//小于号
					return true;
			}
			if (mTotal->isMoveIntercept(tOther,pHero))			//判断是否已经调派其他武将过去堵截
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
		if (pRole && mCurrCost >= pRole->getBaseData()->getExpendCost())				//二次cost判断
		{
			if (!pRole->getAliveState())
			{
				pRole->initAliveData();
				mManage->executeCaptainSkill(true);
			}
			bNotification->postNotification(MsgCreateRoleObject,pRole);		//创建武将显示对象
			setSummonGrid(pRole,pGrid);
			mCurrCost -=  pRole->getBaseData()->getExpendCost();				//扣减cost  
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
	//武将回防策略
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
		int tGrid = converRow( mAutoData->getInterceptGrid() ) + C_PVPCol* C_GRID_ROW;		//直接回防主帅前站位，拦不住
		//vector<int> tStands;
		//tRole->initContainGrid(tGrid,tStands);												//应该是一个判断是否能移动过去的过程，找到那个武将能移动且能拦住的格子
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
				tOther->getCriAtk())				//必杀技状态不移动
				continue;
			if ( !frontHasHero(tOther) )
				return tOther;
			if (mTotal->hasHeroLine(tLines,tOther))			//重复行的情况，移动那个位置较大，且位置较小的英雄也大于当前行格子最大怪物，否则移动较小位置英雄
				return tOther;
		}
		return nullptr;
	}

	bool PvEStrategy::frontHasHero( BaseRole* pOther )
	{
		vector<BaseRole*>::reverse_iterator iter = mHeros.rbegin();			//反向遍历,从位置最大的开始
		for(;iter != mHeros.rend();iter++)
		{
			if ((*iter)->getCaptain())
				continue;
			if (mTotal->sameRow(pOther->mStandGrids,(*iter)->mStandGrids))
			{
				if (pOther->getMaxGrid() > (*iter)->getGridIndex())			//被同行的英雄超越
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
		vector<BaseRole*>::reverse_iterator iter = mHeros.rbegin();			//反向遍历,从位置最大的开始
		for (auto tEnterGrid : *mManage->getOtherEnter())
		{
			if (mBattleModel->getStrategyType() == sPvEStrategy::eBalance && !hasSameRowByOther(tEnterGrid))			//类似的这种判断可以放到类本身中去进行，跟判断是否为PVE的战斗是一样的方式
			{
				mAutoData->setInterceptGrid(tEnterGrid);					//找出能让多格子武将包含一个点的所有的格子（先逐个找，然后再找规律）
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
					mAutoData->setInterceptGrid(tEnterGrid);					//找出能让多格子武将包含一个点的所有的格子（先逐个找，然后再找规律）
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
				return tOther;				//存在未上阵武将，判断是否可以召唤，可以召唤则召唤出来（召唤位置？）
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
		if ( hasUnBattleOther() )										//存在未上阵的武将
			return;
		BaseRole* tRole = hasTargetOther();								//技能时间，扣减cost值
		if (tRole && !tRole->getCriAtk() && !tRole->getSkillTime())		//武将可以释放技能时是非技能状态
		{
			const RoleSkill* skill = tRole->getBaseData()->getActiveSkill();
			if (mCurrCost < skill->getExpendCost()					||
				tRole->getLogicState() == sLogicState::eFree			|| 
				tRole->getLogicState() == sLogicState::eInvincible	)
				return;
			tRole->getRoleObject()->setMoveState(sStateCode::eNullState);
			tRole->getRoleObject()->TurnStateTo(sStateCode::eStandState);
			tRole->ResetAttackState();																	//点击了必杀技按钮，但是武将并没有进入必杀技状态的情况,强制切换至必杀技状态
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
				tOther->setCommandGrid( tOther->getGridIndex() + C_GRID_ROW );			//往前走策略
			}break;
		case BattleSpace::sPvEStrategy::eDefense:
			{
				return ;
			}break;
		case BattleSpace::sPvEStrategy::eAttack:
			{
				if ( tOther->hasOtherRole(tOther->getGridIndex() + C_GRID_ROW) )
					return;
				tOther->setCommandGrid( tOther->getGridIndex() + C_GRID_ROW );					//一直往前走策略
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
		vector<BaseRole*>::reverse_iterator iter = mHeros.rbegin();			//反向遍历,从位置最大的开始
		for(;iter != mHeros.rend();iter++)							//迎上怪物的策略
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
			mAutoData->setInterceptGrid(tGrid);					//找出能让多格子武将包含一个点的所有的格子（先逐个找，然后再找规律）
			excuteCallRole();
			return;
		}
		for (auto tGrid : *mManage->getOtherEnter())				//优先找不站人的，都站人了找能站得下去的
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