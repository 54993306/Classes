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
	int goalRow = 0;										//�õ�Ŀվλrow
	if (RowUp_InBoundary(originRow,aimRow))					//������Χ,���������ж�����
	{
		goalRow = originRow+aimRow;
	}else if (RowDown_InBoundary(originRow,aimRow))
	{
		goalRow = originRow-aimRow;
	}else{
		goalRow = 0;										//������õĹ���row̫���ˣ����row=0�����á�
	}
	int newRow = -1;										//�õ�λ�ú��ж��Ƿ�վ����ȥ
	if (RowUp_InBoundary(goalRow,aim_row))
	{
		newRow = goalRow;									//Ĭ�Ϸ������
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
		CCLOG("[ *ERROR ] BattleTools::JudgeGridExceedBoundary");			//���ݴ�����	
#endif
		return newRow;										//�Ѿ���row=0����,aim��size���ǹ���,������y>4�Ĺ���
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
	if (ColDown_InBoundary(originCol,aimCol))				//�ҷ���x���С�ķ������ȴ���
	{
		goalCol = originCol - aimCol;
	}else if (ColUp_InBoundary(originCol,aimCol))
	{
		goalCol = originCol + aimCol;
	}
	int newCol = C_GRID_COL-1-aimCol;
	if (ColDown_InBoundary(goalCol,aim_col))
	{
		newCol = goalCol;									//Ĭ�Ϸ���
	}else if (ColUp_InBoundary(goalCol,aim_col))
	{
		newCol = goalCol + aim_col;
	}else{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		CCAssert(false,"[ *ERROR ] BattleTools::getHeroNewCol");//���ݴ�����
#else
		CCLOG("[ *ERROR ] BattleTools::getHeroNewCol");			//���ݴ�����	
#endif
		return  newCol;
	}
	return	newCol;
}

int getMonsterNewCol( int originCol ,int aimCol ,int aim_col )
{
	int goalCol = 0;
	if (ColUp_InBoundary(originCol,aimCol))					//�з���x����ķ������ȴ���
	{
		goalCol = originCol + aimCol;
	}else if (ColDown_InBoundary(originCol,aimCol))
	{
		goalCol = originCol - aimCol;
	}
	int newCol = 0;
	if (ColUp_InBoundary(goalCol,aim_col))					//Ĭ�Ϸ���
	{
		newCol = goalCol;
	}else if (ColDown_InBoundary(goalCol,aim_col))
	{
		newCol = goalCol - aim_col;
	}else{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		CCAssert(false,"[ *ERROR ] BattleTools::getMonsterNewCol");	//���ݴ�����	
#else
		CCLOG("[ *ERROR ] BattleTools::getMonsterNewCol");			//���ݴ�����	
#endif
		return  newCol;
	}
	return newCol;
}
//�佫�̶�λ�ü���ٻ������佫
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
	if (row >0 && child->role->col<30)			//����� x ��Χ���Ǵ���30��Ϊ���ݴ���			
	{
		child->setGridIndex(row+col*C_GRID_ROW);
	}else{
		if (father->getEnemy())					//�ݴ��Դ���
		{
			child->setGridIndex(C_GRID_ROW-1);
		}else{
			child->setGridIndex(C_GRID_ROW-1+(C_GRID_COL-1-aimCol)*C_GRID_ROW);
		}
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		CCAssert(false,"[ *ERROR ] BattleTools::CallAliveByFixRange");	//���ݴ�����	
#else
		CCLOG("[ *ERROR ] BattleTools::CallAliveByFixRange");			//���ݴ�����	
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
	PlayEffectSound(str);				//����Ч����Ч
}

