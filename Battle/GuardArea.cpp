#include "Battle/GuardArea.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
#include "Battle/ConstNum.h"
#include "Battle/BattleTools.h"
#include "Battle/WarManager.h"
namespace BattleSpace{
	GuardArea::GuardArea(WarManager* pManage)
		:mManage(pManage)
	{}

	GuardArea* GuardArea::create( WarManager* pManage )
	{
		GuardArea* tGuardArea = new GuardArea(pManage);
		if (tGuardArea)
		{
			tGuardArea->autorelease();
			return tGuardArea;
		}else{
			delete tGuardArea; 
			tGuardArea = NULL; 
			return NULL; 
		}
	}
	//警戒区域的变化情况值跟武将的位置变化有关系,而且是武将的移动对象格子变化。不用每次都去刷新处理
	int GuardArea::getAliveGuard( BaseRole* pRole )
	{
		vector<int> Vecguard;
		initAliveGuard(pRole,Vecguard);
		if (pRole->getOtherCamp())
		{
			vector<int>::reverse_iterator iter= Vecguard.rbegin();
			for (;iter != Vecguard.rend();iter++)
			{
				if (hasTarget(pRole,*iter))
					return *iter;
			}
		}else{
			for (auto tGrid : Vecguard)
			{
				if (hasTarget(pRole,tGrid))
					return tGrid;
			}
		}
		return INVALID_GRID;												//没有警戒区域自动不处理
	}

	bool GuardArea::hasTarget( BaseRole* pRole,int pGrid )
	{
		for (auto tRole:pRole->getCurrSkillTargets())
		{
			if (tRole->inStandGrid(pGrid))
			{
				int tMoveGrid = pGrid;
				int tRow = pGrid%C_GRID_ROW;
				if ( tRow+pRole->getBaseData()->getRoleRow() > C_GRID_ROW )				//我方武将占3行,地方怪物在row=3的位置，超出地图范围了
					tMoveGrid = pGrid-((pRole->getBaseData()->getRoleRow()+tRow)-C_GRID_ROW);//把超出的部分减掉
				if ( tMoveGrid < C_BEGINGRID || tMoveGrid > C_ENDGRID )
					return false;
				return true;
			}
		}
		return false;
	}

	void GuardArea::initAliveGuard( BaseRole* pAlive,vector<int>& pGuards )
	{
		switch (pAlive->getBaseData()->getAlertType())
		{
		case eFrontGuard:
			{
				if (pAlive->getOtherCamp())
					guradBack(pAlive,pGuards);
				else
					guardFront(pAlive,pGuards);
			}break;
		case eBackGuard:
			{
				if (pAlive->getOtherCamp())
					guardFront(pAlive,pGuards);
				else
					guradBack(pAlive,pGuards);
			}break;
		case eFrontAndBackGuard:
			{
				guradFrontAndBack(pAlive,pGuards);
			}break;
		case eUpDownGuard:
			{
				guradUpAndDown(pAlive,pGuards);							//没有长度的限制
			}break;
		default:
			{
				if (pAlive->getBaseData()->getAlertType())
					CCLOG("[ *ERROR ] SkillRange::initAliveGuard Type = %d",pAlive->getBaseData()->getAlertType());
				return;
			}
		}
		VectorUnique(pGuards);
		sort(pGuards.begin(),pGuards.end(),greater<int>());				//警戒区域优先判断后方与上方区域是否有人
	}

	void GuardArea::initAliveCurrGrids( BaseRole* pAlive,vector<int>& pVector )
	{
		if (pAlive->getTouchState())
		{
			if (pAlive->mTouchGrids.empty())
				return;
			pVector.assign(pAlive->mTouchGrids.begin(),pAlive->mTouchGrids.end());
		}else{
			if (pAlive->mStandGrids.empty())
				return;
			pVector.assign(pAlive->mStandGrids.begin(),pAlive->mStandGrids.end());
		}
	}

	void GuardArea::guradUpAndDown( BaseRole* pAlive,vector<int>& pGuards )
	{
		vector<int>tGrids;
		initAliveCurrGrids(pAlive,tGrids);
		for (auto tGrid:tGrids)
		{
			int aliveCol = tGrid / C_GRID_ROW;
			int aliveRow = tGrid % C_GRID_ROW;
			for (int i=aliveRow-1;i<=aliveRow+1;i++)
			{
				if (i>=C_GRID_ROW||i<0)continue;
				pGuards.push_back(aliveCol*C_GRID_ROW+i);						
			}
		}
	}

	void GuardArea::guardFront( BaseRole* pAlive,vector<int>& pGuards )
	{
		vector<int>tGrids;
		initAliveCurrGrids(pAlive,tGrids);
		for (auto tGrid:tGrids)
		{
			for (int i=1;i<=pAlive->getBaseData()->getAlertRange();i++)		//从2开始的原因是因为1是外层的圈(现在外层的圈去掉了从1开始)
				pGuards.push_back(tGrid - i*C_GRID_ROW);
		}
	}

	void GuardArea::guradBack( BaseRole* pAlive,vector<int>& pGuards )
	{
		vector<int>tGrids;
		initAliveCurrGrids(pAlive,tGrids);
		for (auto tGrid:tGrids)
		{
			for (int i=1;i<=pAlive->getBaseData()->getAlertRange();i++)		//从2开始的原因是因为1是外层的圈(现在外层的圈去掉了从1开始)
				pGuards.push_back(tGrid + i*C_GRID_ROW);
		}
	}

	void GuardArea::guradFrontAndBack( BaseRole* pAlive,vector<int>& pGuards )
	{
		vector<int>tGrids;
		initAliveCurrGrids(pAlive,tGrids);
		for (auto tGrid:tGrids)
		{
			for (int i=1;i<=pAlive->getBaseData()->getAlertRange();i++)		//从2开始的原因是因为1是外层的圈(现在外层的圈去掉了从1开始)
			{
				pGuards.push_back(tGrid - i*C_GRID_ROW);
				pGuards.push_back(tGrid + i*C_GRID_ROW);
			}
		}
	}

}