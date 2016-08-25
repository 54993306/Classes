#include "BattleTools.h"
#include "Battle/ConstNum.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
#include "common/CGameSound.h"
namespace BattleSpace
{
	bool RowUp_InBoundary( int originRow,int aimRow )
	{
		if (originRow-(aimRow-1)>= 0)
			return true;
		return false;
	}

	bool RowDown_InBoundary( int originRow,int aimRow )
	{
		if (originRow+(aimRow-1) <=  C_GRID_ROW-1)
			return true;
		return false;
	}

	int getNewRow( int originRow ,int aimRow ,int aim_row )
	{
		int goalRow = 0;										//得到目站位row
		if (RowUp_InBoundary(originRow,aimRow))					//超出范围,则做反向判定处理
		{
			goalRow = originRow+aimRow;
		}else if (RowDown_InBoundary(originRow,aimRow))
		{
			goalRow = originRow-aimRow;
		}else{
			goalRow = 0;										//如果放置的怪物row太大了，则从row=0开放置。
		}
		int newRow = -1;										//得到位置后判定是否站的下去
		if (RowUp_InBoundary(goalRow,aim_row))
		{
			newRow = goalRow;									//默认放置情况
		}else if (RowDown_InBoundary(goalRow,aim_row))
		{
			newRow = goalRow - aim_row;
		}else if (goalRow)
		{
			newRow = C_GRID_ROW-1;
		}else{
#if BATTLE_TEST
			CCAssert(false,"[ *ERROR ] BattleTools::JudgeGridExceedBoundary ");
#else
			CCLOG("[ *ERROR ] BattleTools::JudgeGridExceedBoundary");			//数据错误了	
#endif
			return newRow;										//已经在row=0处了,aim的size还是过大,出现了y>4的怪物
		}
		return newRow;
	}

	bool ColUp_InBoundary( int originCol,int aimCol )
	{
		if (originCol + aimCol <= C_GRID_COL-1)
			return true;
		return false;
	}

	bool ColDown_InBoundary( int originCol,int aimCol )
	{
		if (originCol - aimCol >= 0)
			return true;
		return false;
	}

	int getHeroNewCol( int originCol ,int aimCol ,int aim_col )
	{
		int goalCol = C_GRID_COL-1-aimCol;
		if (ColDown_InBoundary(originCol,aimCol))				//我方往x轴变小的方向优先处理
		{
			goalCol = originCol - aimCol;
		}else if (ColUp_InBoundary(originCol,aimCol))
		{
			goalCol = originCol + aimCol;
		}
		int newCol = C_GRID_COL-1-aimCol;
		if (ColDown_InBoundary(goalCol,aim_col))
		{
			newCol = goalCol;									//默认方向
		}else if (ColUp_InBoundary(goalCol,aim_col))
		{
			newCol = goalCol + aim_col;
		}else{
#if BATTLE_TEST
			CCAssert(false,"[ *ERROR ] BattleTools::getHeroNewCol");//数据错误了
#else
			CCLOG("[ *ERROR ] BattleTools::getHeroNewCol");			//数据错误了	
#endif
			return  newCol;
		}
		return	newCol;
	}

	int getMonsterNewCol( int originCol ,int aimCol ,int aim_col )
	{
		int goalCol = 0;
		if (ColUp_InBoundary(originCol,aimCol))					//敌方往x轴变大的方向优先处理
		{
			goalCol = originCol + aimCol;
		}else if (ColDown_InBoundary(originCol,aimCol))
		{
			goalCol = originCol - aimCol;
		}
		int newCol = 0;
		if (ColUp_InBoundary(goalCol,aim_col))					//默认方向
		{
			newCol = goalCol;
		}else if (ColDown_InBoundary(goalCol,aim_col))
		{
			newCol = goalCol - aim_col;
		}else{
#if BATTLE_TEST
			CCAssert(false,"[ *ERROR ] BattleTools::getMonsterNewCol");	//数据错误了	
#else
			CCLOG("[ *ERROR ] BattleTools::getMonsterNewCol");			//数据错误了	
#endif
			return  newCol;
		}
		return newCol;
	}
	//武将固定位置间隔召唤其他武将
	void CallAliveByFixRange( BaseRole*father , BaseRole*child )
	{
		int originCol = father->getGridIndex() / C_GRID_ROW;
		int originRow = father->getGridIndex() % C_GRID_ROW;
		int aimCol = child->getBaseData()->getInitGrid() / C_GRID_ROW;
		int aimRow = child->getBaseData()->getInitGrid() % C_GRID_ROW; 

		int row = getNewRow(originRow,aimRow,child->getBaseData()->getRoleRow());
		int col = 0;
		if (father->getEnemy())
		{
			col = getMonsterNewCol(originCol,aimCol,child->getBaseData()->getRoleCol());
		}else{
			col = getHeroNewCol(originCol,aimCol,child->getBaseData()->getRoleCol());
		}
		if (row >0 && child->getBaseData()->getRoleCol()<30)			//怪物的 x 范围若是大于30则为数据错误			
		{
			child->setGridIndex(row+col*C_GRID_ROW);
		}else{
			if (father->getEnemy())					//容错性处理
			{
				child->setGridIndex(C_GRID_ROW-1);
			}else{
				child->setGridIndex(C_GRID_ROW-1+(C_GRID_COL-1-aimCol)*C_GRID_ROW);
			}
#if BATTLE_TEST
			CCAssert(false,"[ *ERROR ] BattleTools::CallAliveByFixRange");	//数据错误了	
#else
			CCLOG("[ *ERROR ] BattleTools::CallAliveByFixRange");			//数据错误了	
#endif
		}
	}

	void PlaySound( int id )
	{
		char str[60] = {0};
#if BATTLE_TEST
		sprintf(str,"SFX/Test/%d.mp3",id);											
#else
		sprintf(str,"SFX/%d.ogg",id);												
#endif
		PlayEffectSound(str);				//播放效果音效
	}
	//对武将血量百分比进行排序
	void SortAliveHp(vector<BaseRole*>* VecAlive,int left,int right)
	{
		if ( left < right )
		{
			int i = left;
			int j = right;
			BaseRole* alive = VecAlive->at(i);
			float pe = (float)alive->getHp()/alive->getMaxHp();
			while ( i < j ) 
			{
				while (	i < j  && 
					(float)VecAlive->at(j)->getHp()/VecAlive->at(j)->getMaxHp() > pe )
					j--;
				if ( i < j )
				{
					VecAlive->at(i) = VecAlive->at(j);
					i++;
				}
				while ( i < j  &&
					(float)VecAlive->at(j)->getHp()/VecAlive->at(j)->getMaxHp() <= pe )
					i++;
				if( i < j )
				{
					VecAlive->at(j) = VecAlive->at(i);
					j--;
				}
			}
			VecAlive->at(i) = alive;
			SortAliveHp(VecAlive,left,i-1);
			SortAliveHp(VecAlive,i+1,right);
		}
	}
	//SortAliveHp(VecAlive,0,VecAlive->size()-1);			//快速排序得出血量百分比由小到大的数组

	void VectorUnique( vector<int>& pVector , bool pReverse /*= false*/)
	{
		sort(pVector.begin(),pVector.end());
		pVector.erase(unique(pVector.begin(),pVector.end()),pVector.end());
		if (pReverse)
			sort(pVector.begin(),pVector.end(),greater<int>());
	}
	//升序
	bool sortGrid(const BaseRole*apAlive,const BaseRole*bpAlive)	
	{ 
		return  apAlive->getGridIndex() < bpAlive->getGridIndex(); 
	}

	void VectorUnique( vector<BaseRole*> &pVector )
	{
		vector<BaseRole*>::iterator pos;
		pos = unique(pVector.begin(),pVector.end());		//得到重复元素开始的位置
		pVector.erase(pos,pVector.end());
		sort(pVector.begin(),pVector.end(),sortGrid);
	}

	bool sortHp(const BaseRole* apAlive,const BaseRole* bpAlive)
	{
		float atHpPerent = (float)apAlive->getHp()/apAlive->getMaxHp();
		float btHpPerent = (float)bpAlive->getHp()/bpAlive->getMaxHp();
		return atHpPerent < btHpPerent;
	}

	void SortRoleHp( vector<BaseRole*> &pVector )
	{
		vector<BaseRole*>::iterator pos;
		pos = unique(pVector.begin(),pVector.end());		//得到重复元素开始的位置
		pVector.erase(pos,pVector.end());
		sort(pVector.begin(),pVector.end(),sortHp);
	}

	bool VectorHasInt( vector<int> &pVec , int pGrid )
	{
		for (auto tGrid : pVec)
		{
			if (tGrid == pGrid)
				return true;
		}
		return false;
	}

	bool VectorHasObject( vector<CCObject*> &pVec, CCObject* ob )
	{
		for (auto tObject : pVec)
		{
			if (tObject == ob)
				return true;
		}
		return false;
	}

	int converRow( int pGrid )
	{
		return pGrid%C_GRID_ROW;
	}

	int converCol( int pGrid )
	{
		return pGrid/C_GRID_ROW;
	}

	bool sameRow( int pGrid1 , int pGrid2 )
	{
		if (converRow(pGrid1) == converRow(pGrid2))
			return true;
		return false;
	}

	bool sameCol( int pGrid1 , int pGrid2 )
	{
		if (converCol(pGrid1) == converCol(pGrid2))
			return true;
		return false;
	}
	//得到格子周围区域
	void initGridRound( vector<int>& pVec,int pGrid )
	{
		int tRow = pGrid % C_GRID_ROW;
		int tCol = pGrid / C_GRID_ROW;
		if ((tRow+1)<C_GRID_ROW)
			pVec.push_back(tCol * C_GRID_ROW + tRow+1);
		if ((tRow-1)>=0)
			pVec.push_back(tCol * C_GRID_ROW + tRow-1);
		if ((tCol+1)<C_GRID_COL)
		{
			pVec.push_back((tCol+1) * C_GRID_ROW + tRow);
			if ((tRow+1)<C_GRID_ROW)
				pVec.push_back((tCol+1) * C_GRID_ROW + tRow+1);
			if ((tRow-1)>=0)
				pVec.push_back((tCol+1) * C_GRID_ROW + tRow-1);
		}	
		if ((tCol-1)>=0)
		{
			pVec.push_back((tCol-1) * C_GRID_ROW + tRow);
			if ((tRow+1)<C_GRID_ROW)
				pVec.push_back((tCol-1) * C_GRID_ROW + tRow+1);
			if ((tRow-1)>=0)
				pVec.push_back((tCol-1) * C_GRID_ROW + tRow-1);
		}
	}

	int getRandomByGrid( int pGrid )
	{
		vector<int> tVecGrid;
		initGridRound(tVecGrid,pGrid);
		if (tVecGrid.empty()) return pGrid;
		float r = CCRANDOM_0_1();
		if (r == 1) r = 0;			// index <= size
		return tVecGrid.at(tVecGrid.size()*r);
	}

}