#include "BattleTools.h"
#include "ConstNum.h"
#include "model/DataDefine.h"
#include "common/CGameSound.h"
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
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
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
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
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
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		CCAssert(false,"[ *ERROR ] BattleTools::getMonsterNewCol");	//数据错误了	
#else
		CCLOG("[ *ERROR ] BattleTools::getMonsterNewCol");			//数据错误了	
#endif
		return  newCol;
	}
	return newCol;
}
//武将固定位置间隔召唤其他武将
void CallAliveByFixRange( WarAlive*father , WarAlive*child )
{
	int originCol = father->getGridIndex() / C_GRID_ROW;
	int originRow = father->getGridIndex() % C_GRID_ROW;
	int aimCol = child->role->grid / C_GRID_ROW;
	int aimRow = child->role->grid % C_GRID_ROW; 

	int row = getNewRow(originRow,aimRow,child->role->row);
	int col = 0;
	if (father->getEnemy())
	{
		col = getMonsterNewCol(originCol,aimCol,child->role->col);
	}else{
		col = getHeroNewCol(originCol,aimCol,child->role->col);
	}
	if (row >0 && child->role->col<30)			//怪物的 x 范围若是大于30则为数据错误			
	{
		child->setGridIndex(row+col*C_GRID_ROW);
	}else{
		if (father->getEnemy())					//容错性处理
		{
			child->setGridIndex(C_GRID_ROW-1);
		}else{
			child->setGridIndex(C_GRID_ROW-1+(C_GRID_COL-1-aimCol)*C_GRID_ROW);
		}
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		CCAssert(false,"[ *ERROR ] BattleTools::CallAliveByFixRange");	//数据错误了	
#else
		CCLOG("[ *ERROR ] BattleTools::CallAliveByFixRange");			//数据错误了	
#endif
	}
}

CEffect* getSummonEffect( CSkill* skill )
{
	if (skill->skillType == CallAtk)
		if (skill->EffectList.size())
			if (skill->EffectList.at(0).size())
				return &skill->EffectList.at(0).at(0);
	return nullptr;
}

void RemoveVectorRepeat( vector<int>& Vec )
{
	sort(Vec.begin(),Vec.end());
	Vec.erase(unique(Vec.begin(),Vec.end()),Vec.end());
}

void PlaySound_Event( int id )
{
	char str[60] = {0};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	sprintf(str,"SFX/Test/%d.mp3",id);											
#else
	sprintf(str,"SFX/%d.ogg",id);												
#endif
	PlayEffectSound(str);				//播放效果音效
}

