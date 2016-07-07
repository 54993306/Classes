/************************************************************* 
*
*
*		Data : 2016.6.22
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : 可分为AutoData初始化部分，触发防御条件的情况是一种，不触发防御条件的情况是另一种
*
*
*************************************************************/
#include "Battle/Strategy/TotalStrategy.h"
#include "Battle/BattleMessage.h"
#include "Battle/BaseRole.h"
#include "Battle/ConstNum.h"
#include "Battle/BaseRoleData.h"
#include "Battle/BattleTools.h"
#include "Battle/CaptainSkill.h"

#include "model/DataCenter.h"
#include "model/WarManager.h"
namespace BattleSpace
{
#define eGuardGrid 63					//用于判断开始策略的格子(可能需要根据不同的关卡计算出来，或者由关卡中配置传送过来的值)

	AutoData::AutoData():mCallHero(nullptr),mInterceptRole(nullptr),mInterceptGrid(INVALID_GRID)
	{}

	TotalStrategy::TotalStrategy():mAutoState(false),mButtonState(false),mSkillRole(nullptr)
		,mSkillState(true)
	{}

	TotalStrategy::~TotalStrategy()
	{
		removeEvent();
		CC_SAFE_RELEASE(mAutoData);
		mAutoData = nullptr;
	}

	void TotalStrategy::addEvent()
	{
		bNotification->addObserver(this,callfuncO_selector(TotalStrategy::changeAutoState),MsgAutoBattle,nullptr);
		bNotification->addObserver(this,callfuncO_selector(TotalStrategy::roleStateChange),MsgBattleStateChange,nullptr);
		bNotification->addObserver(this,callfuncO_selector(TotalStrategy::acceptButtonState),MsgSendButtonState,nullptr);
		bNotification->addObserver(this,callfuncO_selector(TotalStrategy::buttonUpdate),MsgButtonStateUpdate,nullptr);
		bNotification->addObserver(this,callfuncO_selector(TotalStrategy::roleGridChange),MsgRoleGridChange,nullptr);
		bNotification->addObserver(this,callfuncO_selector(TotalStrategy::upSkillState),MsgUpAutoSkillState,nullptr);
	}

	void TotalStrategy::removeEvent()
	{
		bNotification->removeAllObservers(this);
	}

	bool TotalStrategy::init()
	{
		addEvent();
		mAutoData = AutoData::create();
		mAutoData->retain();
		mManage = DataCenter::sharedData()->getWar();
		initCrossArea();
		return true;
	}

	void TotalStrategy::initCrossArea()
	{
		vector<int>* tCostArea = mManage->getAddcostVec();
		vector<int>* tMoveArea = mManage->getMoveVec();
		for (auto tMoveGrid : *tMoveArea)
		{
			for (auto tCostGrid : *tCostArea)
			{
				if (tMoveGrid == tCostGrid)
					mCrossArea.push_back(tMoveGrid);
			}
		}
	}

	void TotalStrategy::changeAutoState( CCObject* ob )
	{
		mAutoState = !mAutoState;
		for (auto tPair : *mManage->getRolesMap())
		{
			if (!tPair.second->getEnemy() || tPair.second->getFatherID())
				tPair.second->setAutoState(mAutoState);
		}
		updateStrategy();
	}
	//怪物上阵，武将死亡
	void TotalStrategy::roleStateChange( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		updateStrategy();
	}
	//按钮刷新
	void TotalStrategy::buttonUpdate( CCObject* ob )
	{
		updateStrategy();
	}
	//怪物格子刷新
	void TotalStrategy::roleGridChange( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (tRole->getEnemy() && tRole->getGridIndex() > eGuardGrid)
			updateStrategy();
	}
	//刷新判断当前策略
	void TotalStrategy::updateStrategy()
	{
		if (!mAutoState)return;
		interceptInfo();
		initCallHero();
	}

	void TotalStrategy::acceptButtonState( CCObject* ob )
	{
		mButtonState = ((CCBool*)ob)->getValue();
	}
	//未上阵的可召唤的武将
	void TotalStrategy::initCallHero()
	{
		for (auto tPair : *mManage->getRolesMap())
		{
			BaseRole* tRole = tPair.second;
			if (!tRole->getEnemy() && 
				!tRole->getBattle()&&
				!tRole->getCaptain()&&
				!tRole->getFatherID()&&
				(!tRole->getRoleObject() || ( !tRole->getBattle() && tRole->getRoleObject())))	//武将未创建成功或创建在初始位置
			{
				bNotification->postNotification(MsgGetButtonState,tRole);
				if (mButtonState)
				{
					mButtonState = false;
					mAutoData->setCallHero(tRole);//武将未上阵且可以召唤才有用
					break;
				}
			}
		}
	}
	//初始化拦截信息
	void TotalStrategy::interceptInfo()
	{
		mManage->updateAlive();
		vector<BaseRole*>* tMonsterVec = mManage->inBattleMonsters(true);
		vector<BaseRole*>::reverse_iterator iter = tMonsterVec->rbegin();
		for (;iter != tMonsterVec->rend();iter++)
		{
			if ((*iter)->getGridIndex() < eGuardGrid || frontHasHero(*iter) )
				continue;
			mAutoData->setInterceptRole(*iter);
			mAutoData->setInterceptGrid((*iter)->getGridIndex() + C_GRID_ROW);
			return;
		}
		mAutoData->setInterceptRole(nullptr);
		mAutoData->setInterceptGrid(INVALID_GRID);
	}
	//持续判断条件达成的情况执行AI
	void TotalStrategy::excuteStrategy( float dt )
	{
		if ( !mAutoState )//将该武将召唤到可挡住怪物的位置，优先召唤再我方cost区域内，等怪物过来
			return;
		if ( mAutoData->getInterceptRole())					//需要马上被拦截的武将,围堵策略有两种，召唤和移位
		{
			if (mAutoData->getCallHero())
			{
				initCallGrid();
				excuteCallRole();
			}else{
				moveRoleObject();							//找到距离这个怪物最近的，且当前没有在阻挡其他怪物的武将过来支援
			}
		}else{
			playSkillOrCallRole();
			moveUnTargetRole();								//主动寻敌策略(不能干等)武将不会追出警戒列
		}
	}

	void TotalStrategy::initCallGrid()
	{
		int tCallGrid = getGridByCrossArea(mAutoData->getCallHero(),mAutoData->getInterceptGrid());
		mAutoData->setInterceptGrid(tCallGrid);
	}

	void TotalStrategy::excuteCallRole()					//在可最快拦截且增加cost的可移动区域召唤武将
	{
		mAutoData->setInterceptRole(nullptr);
		summonHero(mAutoData->getCallHero(),mAutoData->getInterceptGrid());
		mAutoData->setCallHero(nullptr);
		mAutoData->setInterceptGrid(INVALID_GRID);
	}

	void TotalStrategy::summonHero( BaseRole* pRole,int pGrid )
	{
		if (pRole->getHp()<=0)
		{
			pRole->initAliveData();
			CaptainSkill::create()->ExecuteSkill();
		}
		bNotification->postNotification(MsgCreateRoleObject,pRole);		//创建武将显示对象
		pRole->setCommandGrid( pGrid );
	}

	void TotalStrategy::moveRoleObject()
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
		int tTargetGrid = roleContainGrid(tRole,meetGrid(mAutoData->getInterceptRole(),tRole));
		mAutoData->setInterceptRole(nullptr);
		mAutoData->setInterceptGrid(INVALID_GRID);
		if (hasHeroMove(tTargetGrid))
			return;
		tRole->setCommandGrid( tTargetGrid );
	}
	//角色若需要包含该格子的站位,目标点不会有我方武将,拦截危险怪物的点
	int TotalStrategy::roleContainGrid( BaseRole* pRole,int pGrid )
	{
		int tRoleRow = pRole->getBaseData()->getRoleRow();
		int tRoleCol = pRole->getBaseData()->getRoleCol();
		if (standGrid(pRole,pGrid))
			return pGrid;
		int tGrid = INVALID_GRID;
		int tRow = converRow(pGrid);
		int tCol = converCol(pGrid);
		if (tRoleRow + tRow > C_GRID_ROW)
			tGrid = pGrid - (tRoleRow + tRow - C_GRID_ROW);
		if (tRoleCol + tCol > C_GRID_COL-2)
			tGrid = pGrid - (tRoleCol + tCol - (C_GRID_COL-1))*C_GRID_ROW;
		return tGrid;
	}

	bool TotalStrategy::standGrid( BaseRole* pRole,int pGrid )
	{
		vector<int> tStandGrids;
		pRole->initContainGrid(pGrid,tStandGrids);
		if (unCrossHeros(tStandGrids,pRole))
			return true;
		return false;
	}
	//((距离差 / 速度差   =   时间)   *  追赶的角色速度 + 当前位置)   =  目标位置
	int TotalStrategy::meetGrid( BaseRole* pMonster,BaseRole* pHero )
	{
		if (pHero->getMoveSpeed() >= pMonster->getMoveSpeed())										//追不上，则主帅前一格,跑一格所使用时间，越大就表示速度越小			
			return pMonster->getGridIndex() % C_GRID_ROW + (C_GRID_COL-2) * C_GRID_ROW;	
		int tGridInterval = max(
			abs((pMonster->getGridIndex() % C_GRID_ROW) - (pHero->getGridIndex() % C_GRID_ROW)) ,	//求出行列间的最大距离
			abs((pMonster->getGridIndex() / C_GRID_ROW) - (pHero->getGridIndex() / C_GRID_ROW)) );
		float tSpeedInterval = 1/pHero->getMoveSpeed() - 1/pMonster->getMoveSpeed();
		float tTimeGap = tGridInterval / tSpeedInterval;
		int tMeetCol = tTimeGap * (1/pMonster->getMoveSpeed()) + pMonster->getGridIndex()/C_GRID_ROW;
		//CCLOG("target=%d,follow=%d,MeetCol=%d",pTRole->getGridIndex(),pFRole->getGridIndex(),tMeetCol);
		if (tMeetCol > C_GRID_COL-2)
		{
			return pMonster->getGridIndex() % C_GRID_ROW + (C_GRID_COL-2) * C_GRID_ROW;
		}else{
			return (tMeetCol + 1)*C_GRID_ROW + pMonster->getGridIndex()%C_GRID_ROW;
		}
	}

	bool TotalStrategy::hasHeroMove( int pGrid )
	{
		if (pGrid == INVALID_GRID)return true;
		vector<BaseRole*>* tHeroVec = mManage->inBattleHeros();
		for (auto tHero : *tHeroVec)
		{
			if (tHero->getCaptain())
				continue;
			if (tHero->getMoveGrid() == pGrid)
				return true;
		}
		return false;
	}
	//找到空闲武将，视情况而定，可能找不到
	BaseRole* TotalStrategy::findFreeHero()
	{
		vector<BaseRole*>* tHeroVec = mManage->inBattleHeros(true);
		vector<int> tLines;
		for (auto tHero : * tHeroVec)
		{
			if (tHero->getCaptain()		|| 
				tHero->getCommandGrid()	||
				tHero->getFatherID())			//有移动指令则不再执行其他AI
				continue;
			if ( !frontHasMonster(tHero) )
				return tHero;
			if (hasHeroLine(tLines,tHero))			//重复行的情况，移动那个位置较大，且位置较小的英雄也大于当前行格子最大怪物，否则移动较小位置英雄
				return tHero;
		}
		return nullptr;
	}

	bool TotalStrategy::hasHeroLine( vector<int>& pLines,BaseRole* pRole )
	{
		if ( !VectorHasInt(pLines,pRole->getGridIndex()%C_GRID_ROW))	//找重复行的武将
		{
			for (int tRow = 1; tRow < pRole->getBaseData()->getRoleRow();tRow++)
			{
				if ( !VectorHasInt(pLines,converRow(pRole->getGridIndex()) - tRow))
				{
					pLines.push_back(converRow(pRole->getGridIndex()) - tRow);
				}else{
					return true;
				}
			}
			pLines.push_back(pRole->getGridIndex() % C_GRID_ROW);
			return false;
		}else{
			return true;
		}
	}

	bool TotalStrategy::sameRow( vector<int>& pGridsA,vector<int>& pGridsB )
	{
		if (pGridsA.empty() || pGridsB.empty())return false;
		for (auto tGridA : pGridsA)
		{
			if (vectorHasRow(pGridsB,converRow(tGridA)))
				return true;
		}
		return false;
	}

	bool TotalStrategy::vectorHasRow( vector<int>& pGrids ,int pRow )
	{
		if (pGrids.empty())return false;
		for (auto tGrid : pGrids)
		{
			if (converRow(tGrid) == pRow)
				return true;
		}
		return false;
	}
	//移动拦截
	bool TotalStrategy::isMoveIntercept( BaseRole* pHero,BaseRole* pMonster )
	{
		if (pHero->getMoveGrid() == INVALID_GRID)
			return false;
		vector<int> tMoveEnds;
		pHero->initContainGrid(pHero->getMoveGrid(),tMoveEnds);
		if (tMoveEnds.at(tMoveEnds.size()-1) < pMonster->getGridIndex())
			return false;
		for (auto tGrid : tMoveEnds)
		{
			if (vectorHasRow(pMonster->mStandGrids,converRow(tGrid)))
				return true;
		}
		return false;
	}

	bool TotalStrategy::frontHasHero( BaseRole* pMonster )
	{
		vector<BaseRole*>*tVecHero = mManage->inBattleHeros();
		for (auto tHero : *tVecHero)
		{
			if (tHero->getCaptain() || tHero->getFatherID())
				continue;
			if (sameRow(pMonster->mStandGrids,tHero->mStandGrids))
			{
				if (tHero->getMaxGrid() >= pMonster->getGridIndex())
					return true;
			}
			if (isMoveIntercept(tHero,pMonster))		//判断是否已经调派其他武将过去堵截
				return true;
		}
		return false;
	}

	bool TotalStrategy::frontHasMonster( BaseRole* pHero )
	{
		vector<BaseRole*>* tMonsterVec = mManage->inBattleMonsters(true);
		vector<BaseRole*>::reverse_iterator iter = tMonsterVec->rbegin();			//反向遍历,从位置最大的开始
		for(;iter != tMonsterVec->rend();iter++)
		{
			if ((*iter)->getGridIndex() < eGuardGrid)
				continue;
			if (sameRow(pHero->mStandGrids,(*iter)->mStandGrids))
			{
				if (pHero->getMaxGrid() < (*iter)->getGridIndex())					//被同行的怪物超越
					return false;
				return true;
			}
		}
		return false;
	}
	//存在未上阵武将
	BaseRole* TotalStrategy::hasUnBattleHero()
	{
		for (auto tPair : *mManage->getRolesMap())
		{
			BaseRole* tRole = tPair.second;
			if (tRole->getEnemy() || tRole->getFatherID())
				continue;
			if (!tRole->getBattle())
				return tRole;				//存在未上阵武将，判断是否可以召唤，可以召唤则召唤出来（召唤位置？）
		}
		return nullptr;						//做释放技能判断
	}
	//空闲武将
	BaseRole* TotalStrategy::unTargetHero()
	{
		vector<BaseRole*>* tHeros = mManage->inBattleHeros(true);
		for (auto tHero : *tHeros)
		{
			if (tHero->getHasTarget()	|| 
				tHero->getCommandGrid()	|| 
				tHero->getCaptain()		||
				tHero->getFatherID()		)
				continue;
			return tHero;
		}
		return nullptr;
	}	
	//非空闲武将
	BaseRole* TotalStrategy::hasTargetHero()
	{
		vector<BaseRole*>* tHeros = mManage->inBattleHeros(true);
		for (auto tHero : *tHeros)
		{
			if (tHero->getCaptain() || 
				tHero == mSkillRole || 
				tHero->getFatherID())
				continue;
			if (tHero->getHasTarget())
			{
				mSkillRole = tHero;
				return tHero;
			}
		}
		return nullptr;
	}
	//全部堵住和尽量靠近(武将进入AI状态)
	void TotalStrategy::moveUnTargetRole()
	{
		BaseRole* tHero = unTargetHero();				//包括了前方没有怪物的武将
		if ( !tHero )return;
		//找到没有目标的可移动对象,若在减cost区域则移动回非减区域，且当前行有怪物的最小格子格子可站立
		//武将当前行存在怪物，判断武将是否是在距离怪物最近的非减cost区域，若不是，则求出能够相遇的非减cost区域的点(武将设置为AI状态)，而后移动过去
	}
	//判断武将是否可以移动到某个格子的方法
	//是否召唤完武将，是否可以召唤武将，可以召唤的情况下召唤武将
	void TotalStrategy::playSkillOrCallRole()			
	{
		if (mAutoData->getCallHero())										//未上阵且可召唤的武将
		{
			mAutoData->setInterceptGrid(getGridByCrossArea(mAutoData->getCallHero()));
			excuteCallRole();
		}else{
			if (!mSkillState || hasUnBattleHero())			//存在未上阵的武将
				return;
			BaseRole* tRole = hasTargetHero();
			if (tRole)
			{
				mSkillState = false;
				bNotification->postNotification(MsgGetButtonState,tRole);
			}
		}
	}
	//召唤的目标格子(召唤到最前面的格子)
	int TotalStrategy::getGridByCrossArea(BaseRole* pRole,int pGrid/* =0*/)
	{
		vector<int> tStandGrids;
		for (auto tGrid : mCrossArea)
		{
			pRole->initContainGrid(tGrid,tStandGrids);
			if (pGrid)
			{
				if (pGrid > tGrid || !vectorHasRow(tStandGrids,converRow(pGrid)))
					continue;
			}
			if (unCrossHeros(tStandGrids,pRole))
				return tGrid;
		}
		if (pGrid)return pGrid;
		return INVALID_GRID;
	}

	bool TotalStrategy::unCrossHeros( vector<int>& pGrids,BaseRole* pSelf /*=nullptr*/ )
	{
		if (pGrids.empty())return false;
		vector<BaseRole*>* tHeros = mManage->inBattleHeros(true);
		for (auto tHero : *tHeros)
		{
			if (tHero->getCaptain()		|| 
				tHero == pSelf			|| 
				tHero->getFatherID()		)
				continue;
			if (tHero->inStandVector(pGrids))
				return false;
		}
		return true;
	}

	void TotalStrategy::upSkillState( CCObject* ob )
	{
		mSkillState = true;
	}
	//怪物是否超过了所有的武将需要马上拦截
	BaseRole* TotalStrategy::urgencyMove()
	{
		vector<BaseRole*>* tHeros = mManage->inBattleHeros(true);
		for (auto tHero : *tHeros)
		{
			if ( tHero->getCaptain()			||
				 tHero->getFatherID()		||
				 tHero->getCommandGrid()		||
				roleInLastLine(tHero)		)
				continue;
			return tHero;
		}
		return nullptr;
	}

	bool TotalStrategy::roleInLastLine( BaseRole* pRole )
	{
		for (auto tGrid : pRole->mStandGrids)
		{
			if (converCol(tGrid) >= C_GRID_COL-2)
				return true;
		}
		return false;
	}

	bool TotalStrategy::urgencyJudge()
	{
		vector<BaseRole*>* tHeros = mManage->inBattleHeros(true);
		for (auto tHero : *tHeros)
		{
			if ( tHero->getCaptain()	|| tHero->getFatherID())
				continue;
			if (converCol(tHero->getGridIndex()) >= converCol(mAutoData->getInterceptRole()->getGridIndex()))
				return false;
		}
		return true;
	}

}