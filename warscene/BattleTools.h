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
 *		Description : 战斗中用得到的工具类，或是一些还没想好放到哪些类的公共方法
 *
 *
 *************************************************************/
namespace BattleSpace{
	class BaseRole;
	class RoleSkill;
	class skEffectData;


	bool RowUp_InBoundary(int originRow,int aimRow);
	bool RowDown_InBoundary(int originRow,int aimRow);
	bool ColUp_InBoundary(int originCol,int aimCol);
	bool ColDown_InBoundary(int originCol,int aimCol);
	int getNewRow(int originRow ,int aimRow ,int aim_row);
	int getHeroNewCol(int originCol ,int aimCol ,int aim_col);
	int getMonsterNewCol(int originCol ,int aimCol ,int aim_col);
	void CallAliveByFixRange( BaseRole*father , BaseRole*child );

	void VectorRemoveRepeat(vector<int>& Vec);

	void VectorRemoveRepeat(vector<BaseRole*> &pVector);

	void VectorSortAliveHp(vector<BaseRole*> &pVector);

	void PlaySound_Event(int id);

};
#endif