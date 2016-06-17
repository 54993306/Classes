#include "BattleTools.h"
#include "Battle/ConstNum.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
#include "common/CGameSound.h"
namespace BattleSpace{
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
		if (row >0 && child->getBaseData()->getRoleCol()<30)			//����� x ��Χ���Ǵ���30��Ϊ���ݴ���			
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
	//���佫Ѫ���ٷֱȽ�������
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
	//SortAliveHp(VecAlive,0,VecAlive->size()-1);			//��������ó�Ѫ���ٷֱ���С���������

	void VectorRemoveRepeat( vector<int>& pVector )
	{
		sort(pVector.begin(),pVector.end());
		pVector.erase(unique(pVector.begin(),pVector.end()),pVector.end());
	}
	//����
	bool sortAliveGrid(const BaseRole*apAlive,const BaseRole*bpAlive)	
	{ 
		return  apAlive->getGridIndex() < bpAlive->getGridIndex(); 
	}

	void VectorRemoveRepeat( vector<BaseRole*> &pVector )
	{
		vector<BaseRole*>::iterator pos;
		pos = unique(pVector.begin(),pVector.end());		//�õ��ظ�Ԫ�ؿ�ʼ��λ��
		pVector.erase(pos,pVector.end());
		sort(pVector.begin(),pVector.end(),sortAliveGrid);
	}

	bool sortAliveHp(const BaseRole* apAlive,const BaseRole* bpAlive)
	{
		float atHpPerent = (float)apAlive->getHp()/apAlive->getMaxHp();
		float btHpPerent = (float)bpAlive->getHp()/bpAlive->getMaxHp();
		return atHpPerent < btHpPerent;
	}

	void VectorSortAliveHp( vector<BaseRole*> &pVector )
	{
		vector<BaseRole*>::iterator pos;
		pos = unique(pVector.begin(),pVector.end());		//�õ��ظ�Ԫ�ؿ�ʼ��λ��
		pVector.erase(pos,pVector.end());
		sort(pVector.begin(),pVector.end(),sortAliveHp);
	}
}