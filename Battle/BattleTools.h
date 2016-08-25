#ifndef __BATTLETOOLS__
#define __BATTLETOOLS__
#include "AppUI.h"
/************************************************************* 
 *
 *
 *		Data : 2016.5.3
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : ս�����õõ��Ĺ����࣬����һЩ��û��÷ŵ���Щ��Ĺ�������
 *
 *
 *************************************************************/
namespace BattleSpace
{
	class BaseRole;

	bool RowUp_InBoundary(int originRow,int aimRow);
	bool RowDown_InBoundary(int originRow,int aimRow);
	bool ColUp_InBoundary(int originCol,int aimCol);
	bool ColDown_InBoundary(int originCol,int aimCol);
	int getNewRow(int originRow ,int aimRow ,int aim_row);
	int getHeroNewCol(int originCol ,int aimCol ,int aim_col);
	int getMonsterNewCol(int originCol ,int aimCol ,int aim_col);
	void CallAliveByFixRange( BaseRole*father , BaseRole*child );

	void VectorUnique(vector<int>& Vec,bool pReverse = false);

	void VectorUnique(vector<BaseRole*> &pVector);

	void SortRoleHp(vector<BaseRole*> &pVector);

	bool VectorHasInt(vector<int> &pVec , int pGrid);

	bool VectorHasObject(vector<CCObject*> &pVec, CCObject* ob);

	void PlaySound(int id);

	int converRow(int pGrid);

	int converCol(int pGrid);

	bool sameRow(int pGrid1 , int pGrid2);

	bool sameCol(int pGrid1 , int pGrid2);

	void initGridRound(vector<int>& pVec,int pGrid);

	int getRandomByGrid(int pGrid);
	//�����վλ����ĳ����������
};
#endif