#include "Battle/GuardArea.h"
#include "Battle/BattleRole.h"
#include "Battle/RoleBaseData.h"
#include "warscene/ConstNum.h"
#include "warscene/BattleTools.h"
#include "model/WarManager.h"
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
int GuardArea::getAliveGuard( WarAlive* pAlive )
{
	vector<int> Vecguard;
	initAliveGuard(pAlive,Vecguard);
	for (auto i:Vecguard)
	{
		for (auto tAlive:*mManage->getVecMonsters())
		{
			vector<int>::reverse_iterator iter= tAlive->mStandGrids.rbegin();
			for (;iter != tAlive->mStandGrids.rend();iter++)			//我方对怪物应该从最大的格子开始进行判断
			{
				if ( i != *iter )										//当前警戒区域范围内存在敌方目标
					continue;
				int tMoveGrid = i;
				int tRow = i%C_GRID_ROW;
				if ( tRow+pAlive->getBaseData()->getRoleRow() > C_GRID_ROW )				//我方武将占3行,地方怪物在row=3的位置，超出地图范围了
					tMoveGrid = i-((pAlive->getBaseData()->getRoleRow()+tRow)-C_GRID_ROW);//把超出的部分减掉
				if ( tMoveGrid < C_BEGINGRID || tMoveGrid > C_ENDGRID )
					break;
				return tMoveGrid;
			}
		}
	}
	return INVALID_GRID;												//没有警戒区域自动不处理
}

void GuardArea::initAliveGuard( WarAlive* pAlive,vector<int>& pGuards )
{
	switch (pAlive->getBaseData()->getAlertType())
	{
	case eFrontGuard:
		{
			guardFront(pAlive,pGuards);
		}break;
	case eBackGuard:
		{
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
	VectorRemoveRepeat(pGuards);
	sort(pGuards.begin(),pGuards.end(),greater<int>());				//警戒区域优先判断后方与上方区域是否有人
}

void GuardArea::initAliveCurrGrids( WarAlive* pAlive,vector<int>& pVector )
{
	if (pAlive->getTouchState())
	{
		pVector.assign(pAlive->mTouchGrids.begin(),pAlive->mTouchGrids.end());
	}else{
		pVector.assign(pAlive->mStandGrids.begin(),pAlive->mStandGrids.end());
	}
}

void GuardArea::guradUpAndDown( WarAlive* pAlive,vector<int>& pGuards )
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

void GuardArea::guardFront( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		for (int i=1;i<=pAlive->getBaseData()->getAlertRange();i++)		//从2开始的原因是因为1是外层的圈(现在外层的圈去掉了从1开始)
			pGuards.push_back(tGrid - i*C_GRID_ROW);
	}
}

void GuardArea::guradBack( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		for (int i=1;i<=pAlive->getBaseData()->getAlertRange();i++)		//从2开始的原因是因为1是外层的圈(现在外层的圈去掉了从1开始)
			pGuards.push_back(tGrid + i*C_GRID_ROW);
	}
}

void GuardArea::guradFrontAndBack( WarAlive* pAlive,vector<int>& pGuards )
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
