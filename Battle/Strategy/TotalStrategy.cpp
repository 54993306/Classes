/************************************************************* 
*
*
*		Data : 2016.6.22
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : �ɷ�ΪAutoData��ʼ�����֣��������������������һ�֣������������������������һ��
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
#define eGuardGrid 63					//�����жϿ�ʼ���Եĸ���(������Ҫ���ݲ�ͬ�Ĺؿ���������������ɹؿ������ô��͹�����ֵ)

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
	//���������佫����
	void TotalStrategy::roleStateChange( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		updateStrategy();
	}
	//��ťˢ��
	void TotalStrategy::buttonUpdate( CCObject* ob )
	{
		updateStrategy();
	}
	//�������ˢ��
	void TotalStrategy::roleGridChange( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (tRole->getEnemy() && tRole->getGridIndex() > eGuardGrid)
			updateStrategy();
	}
	//ˢ���жϵ�ǰ����
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
	//δ����Ŀ��ٻ����佫
	void TotalStrategy::initCallHero()
	{
		for (auto tPair : *mManage->getRolesMap())
		{
			BaseRole* tRole = tPair.second;
			if (!tRole->getEnemy() && 
				!tRole->getBattle()&&
				!tRole->getCaptain()&&
				!tRole->getFatherID()&&
				(!tRole->getRoleObject() || ( !tRole->getBattle() && tRole->getRoleObject())))	//�佫δ�����ɹ��򴴽��ڳ�ʼλ��
			{
				bNotification->postNotification(MsgGetButtonState,tRole);
				if (mButtonState)
				{
					mButtonState = false;
					mAutoData->setCallHero(tRole);//�佫δ�����ҿ����ٻ�������
					break;
				}
			}
		}
	}
	//��ʼ��������Ϣ
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
	//�����ж�������ɵ����ִ��AI
	void TotalStrategy::excuteStrategy( float dt )
	{
		if ( !mAutoState )//�����佫�ٻ����ɵ�ס�����λ�ã������ٻ����ҷ�cost�����ڣ��ȹ������
			return;
		if ( mAutoData->getInterceptRole())					//��Ҫ���ϱ����ص��佫,Χ�²��������֣��ٻ�����λ
		{
			if (mAutoData->getCallHero())
			{
				initCallGrid();
				excuteCallRole();
			}else{
				moveRoleObject();							//�ҵ����������������ģ��ҵ�ǰû�����赲����������佫����֧Ԯ
			}
		}else{
			playSkillOrCallRole();
			moveUnTargetRole();								//����Ѱ�в���(���ܸɵ�)�佫����׷��������
		}
	}

	void TotalStrategy::initCallGrid()
	{
		int tCallGrid = getGridByCrossArea(mAutoData->getCallHero(),mAutoData->getInterceptGrid());
		mAutoData->setInterceptGrid(tCallGrid);
	}

	void TotalStrategy::excuteCallRole()					//�ڿ��������������cost�Ŀ��ƶ������ٻ��佫
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
		bNotification->postNotification(MsgCreateRoleObject,pRole);		//�����佫��ʾ����
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
	//��ɫ����Ҫ�����ø��ӵ�վλ,Ŀ��㲻�����ҷ��佫,����Σ�չ���ĵ�
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
	//((����� / �ٶȲ�   =   ʱ��)   *  ׷�ϵĽ�ɫ�ٶ� + ��ǰλ��)   =  Ŀ��λ��
	int TotalStrategy::meetGrid( BaseRole* pMonster,BaseRole* pHero )
	{
		if (pHero->getMoveSpeed() >= pMonster->getMoveSpeed())										//׷���ϣ�����˧ǰһ��,��һ����ʹ��ʱ�䣬Խ��ͱ�ʾ�ٶ�ԽС			
			return pMonster->getGridIndex() % C_GRID_ROW + (C_GRID_COL-2) * C_GRID_ROW;	
		int tGridInterval = max(
			abs((pMonster->getGridIndex() % C_GRID_ROW) - (pHero->getGridIndex() % C_GRID_ROW)) ,	//������м��������
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
	//�ҵ������佫������������������Ҳ���
	BaseRole* TotalStrategy::findFreeHero()
	{
		vector<BaseRole*>* tHeroVec = mManage->inBattleHeros(true);
		vector<int> tLines;
		for (auto tHero : * tHeroVec)
		{
			if (tHero->getCaptain()		|| 
				tHero->getCommandGrid()	||
				tHero->getFatherID())			//���ƶ�ָ������ִ������AI
				continue;
			if ( !frontHasMonster(tHero) )
				return tHero;
			if (hasHeroLine(tLines,tHero))			//�ظ��е�������ƶ��Ǹ�λ�ýϴ���λ�ý�С��Ӣ��Ҳ���ڵ�ǰ�и�������������ƶ���Сλ��Ӣ��
				return tHero;
		}
		return nullptr;
	}

	bool TotalStrategy::hasHeroLine( vector<int>& pLines,BaseRole* pRole )
	{
		if ( !VectorHasInt(pLines,pRole->getGridIndex()%C_GRID_ROW))	//���ظ��е��佫
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
	//�ƶ�����
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
			if (isMoveIntercept(tHero,pMonster))		//�ж��Ƿ��Ѿ����������佫��ȥ�½�
				return true;
		}
		return false;
	}

	bool TotalStrategy::frontHasMonster( BaseRole* pHero )
	{
		vector<BaseRole*>* tMonsterVec = mManage->inBattleMonsters(true);
		vector<BaseRole*>::reverse_iterator iter = tMonsterVec->rbegin();			//�������,��λ�����Ŀ�ʼ
		for(;iter != tMonsterVec->rend();iter++)
		{
			if ((*iter)->getGridIndex() < eGuardGrid)
				continue;
			if (sameRow(pHero->mStandGrids,(*iter)->mStandGrids))
			{
				if (pHero->getMaxGrid() < (*iter)->getGridIndex())					//��ͬ�еĹ��ﳬԽ
					return false;
				return true;
			}
		}
		return false;
	}
	//����δ�����佫
	BaseRole* TotalStrategy::hasUnBattleHero()
	{
		for (auto tPair : *mManage->getRolesMap())
		{
			BaseRole* tRole = tPair.second;
			if (tRole->getEnemy() || tRole->getFatherID())
				continue;
			if (!tRole->getBattle())
				return tRole;				//����δ�����佫���ж��Ƿ�����ٻ��������ٻ����ٻ��������ٻ�λ�ã���
		}
		return nullptr;						//���ͷż����ж�
	}
	//�����佫
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
	//�ǿ����佫
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
	//ȫ����ס�;�������(�佫����AI״̬)
	void TotalStrategy::moveUnTargetRole()
	{
		BaseRole* tHero = unTargetHero();				//������ǰ��û�й�����佫
		if ( !tHero )return;
		//�ҵ�û��Ŀ��Ŀ��ƶ�����,���ڼ�cost�������ƶ��طǼ������ҵ�ǰ���й������С���Ӹ��ӿ�վ��
		//�佫��ǰ�д��ڹ���ж��佫�Ƿ����ھ����������ķǼ�cost���������ǣ�������ܹ������ķǼ�cost����ĵ�(�佫����ΪAI״̬)�������ƶ���ȥ
	}
	//�ж��佫�Ƿ�����ƶ���ĳ�����ӵķ���
	//�Ƿ��ٻ����佫���Ƿ�����ٻ��佫�������ٻ���������ٻ��佫
	void TotalStrategy::playSkillOrCallRole()			
	{
		if (mAutoData->getCallHero())										//δ�����ҿ��ٻ����佫
		{
			mAutoData->setInterceptGrid(getGridByCrossArea(mAutoData->getCallHero()));
			excuteCallRole();
		}else{
			if (!mSkillState || hasUnBattleHero())			//����δ������佫
				return;
			BaseRole* tRole = hasTargetHero();
			if (tRole)
			{
				mSkillState = false;
				bNotification->postNotification(MsgGetButtonState,tRole);
			}
		}
	}
	//�ٻ���Ŀ�����(�ٻ�����ǰ��ĸ���)
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
	//�����Ƿ񳬹������е��佫��Ҫ��������
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