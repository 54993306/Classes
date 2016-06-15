#include "battle/AreaCount.h"
#include "Battle/BaseRole.h"
#include "warscene/ConstNum.h"
#include "Battle/skEffectData.h"
#include "warscene/BattleTools.h"

using namespace cocos2d;
namespace BattleSpace{
	AreaCountInfo::AreaCountInfo(vector<int>& pVector,BaseRole* pAlive)
		:mGrid(0),mVector(pVector),mAlive(pAlive),mEnemy(false),mBackAttack(false)
		,mAreaType(0),mAreaRange(0),mDistance(0),mTargetType(0)
	{
		setAlive(mAlive);
		setEnemy(mAlive->getEnemy());
		setBackAttack(mAlive->getOpposite());
		setTargetType(mAlive->getCurrEffect()->getTargetType());
		//setAreaType(mAlive->getCurrEffect()->getAreaType());
		setAreaRange(mAlive->getCurrEffect()->getAreaSize());
		setDistance(mAlive->getCurrEffect()->getSpaceRange());
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		//setAreaRange(1);
		//setAreaType(eVertical);
#endif
	}

	void AreaCountInfo::excludeStandGrid()
	{
		for (auto tGrid : getAlive()->mStandGrids)
		{
			vector<int>::iterator iter = mVector.begin();
			for (;iter != mVector.end();iter++)
			{
				if (tGrid == *iter)
				{
					mVector.erase(iter);
					break;
				}
			}
		}
	}

	void AreaCountInfo::excludeCaptain( )
	{
		vector<int>::iterator iter = mVector.begin();
		for (;iter != mVector.end();)
		{
			if (*iter >= C_CAPTAINGRID)
				iter = mVector.erase(iter);
			else
				iter++;
		}
	}

	void AreaCountInfo::excludeInvalid()
	{
		vector<int>::iterator iter = mVector.begin();
		for (;iter != mVector.end();)
		{
			if (*iter < C_BEGINGRID || *iter > C_ENDGRID)
				iter = mVector.erase(iter);
			else
				iter++;
		}
		VectorRemoveRepeat(mVector);											//包括了正向遍历和除去重复
		if (!mAlive->getEnemy() && !mAlive->getOpposite())
			sort(mVector.begin(),mVector.end(),greater<int>());					//我方武将正向攻击的情况(对格子进行反向排序)
	}
	int AreaCountInfo::getRowByGrid( int pGrid )
	{
		return pGrid % C_GRID_ROW;
	}

	int AreaCountInfo::getColByInfo( int pGrod )
	{
		if (getEnemy() && !getBackAttack() || 
			!getEnemy() && getBackAttack())			//让技能区域整体移动的情况就类似于武将位置变动了的情况
		{
			return pGrod / C_GRID_ROW + getDistance();
		}else{
			return pGrod / C_GRID_ROW - getDistance();
		}
	}

	void AreaCountInfo::addGrid( int pGrid )
	{
		mVector.push_back(pGrid);
	}

	void AreaCountInfo::removeByIndex( int pIndex )
	{
		mVector.erase(mVector.begin()+pIndex,mVector.end());
	}

	bool AreaCountInfo::hasGrid( int pGrid )
	{
		if (find(mVector.begin(),mVector.end(),pGrid) == mVector.end())
			return true;
		return false;
	}

	void AreaCountInfo::inserVector( vector<int>& pVector )
	{
		mVector.insert(mVector.end(),pVector.begin(),pVector.end());
	}

	void AreaCountInfo::DisperseDispose()
	{
		vector<int> tVector;													//不能在遍历中添加元素
		for (auto tGrid : getVector())										
		{
			if (getAlive()->standInGrid(tGrid))									//武将站立区域不做分散处理
				continue;														
			int tRow = tGrid % C_GRID_ROW;
			int tCol = tGrid / C_GRID_ROW;
			for (int i = C_BEGINGRID; i<C_GRID_ROW*C_GRID_COL ; i++ )
			{
				int tTargetRow = i % C_GRID_ROW;
				int tTargetCol = i / C_GRID_ROW;
				if (getAreaRange() == 1)
				{
					if ((tRow == tTargetRow && tCol+1 >= tTargetCol && tTargetCol >= tCol-1)
						|| (tCol == tTargetCol && tRow+1 >= tTargetRow && tTargetRow >= tRow-1))
					{
						tVector.push_back(i);
					}
				}else{	//		pDisperseValue == 2
					if ((tRow == tTargetRow+1 || tRow == tTargetRow-1 || tRow==tTargetRow)
						&&	(tCol == tTargetCol+1 || tCol == tTargetCol-1 || tCol==tTargetCol)
						||	(tRow == tTargetRow && tCol+2 >= tTargetCol && tTargetCol >= tCol-2)
						||	(tCol == tTargetCol && tRow+2 >= tTargetRow && tTargetRow >= tRow-2))
					{
						tVector.push_back(i);
					}
				}
			}
		}
		inserVector(tVector);
	}

	void AreaCountInfo::RowType( int Type )
	{
		vector<int> tVector;
		for (auto tGrid : getVector())
		{
			int tCol = tGrid / C_GRID_ROW;
			for (int i = C_BEGINGRID;i<C_GRID_COL*C_GRID_ROW;i++)
			{
				int tarCol = i/C_GRID_ROW;
				switch (Type)
				{
				case eFrontDirection:
				case eCentenDirection:
					{
						if ((getTargetType()==eUsType		&& getEnemy())	||
							(getTargetType()==eEnemyType	&& !getEnemy())	||
							(getTargetType()==eAllType		&& !getEnemy()) )
						{
							if (tCol >= tarCol && tarCol > tCol - getAreaRange())		//偏向敌方
								tVector.push_back(i);
						}else if (	(getTargetType()==eEnemyType&& getEnemy())	||
							(getTargetType()==eUsType			&& !getEnemy())	|| 
							(getTargetType()==eAllType			&& getEnemy()))
						{
							if (tCol <= tarCol && tarCol < tCol + getAreaRange())		//偏向我方
								tVector.push_back(i);
						}else{
							CCLOG("[ERROR] AreaCountInfo::RowType");
						}
					}break;
				case eBackDirection:
					{
						if ((getTargetType()==eUsType		&& getEnemy())		||
							(getTargetType()==eEnemyType	&& !getEnemy())		||
							(getTargetType()==eAllType		&& !getEnemy()))
						{
							if (tCol <= tarCol && tarCol < tCol + getAreaRange())		//偏向我方
								tVector.push_back(i);
						}else if (	(getTargetType()==eEnemyType	&& getEnemy())	||
									(getTargetType()==eUsType		&& !getEnemy())	|| 
									(getTargetType()==eAllType		&& getEnemy()))
						{
							if (tCol >= tarCol && tarCol > tCol - getAreaRange())		//偏向敌方
								tVector.push_back(i);
						}else{
							CCLOG("[ERROR] AreaCountInfo::RowType");
						}
					}break;
				default:
					break;
				}
			}
		}
		inserVector(tVector);
	}
}
