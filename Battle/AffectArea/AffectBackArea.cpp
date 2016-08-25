/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "Battle/AffectArea/AffectBackArea.h"
namespace BattleSpace
{
	//后方单体
	sAffectType AffectBackSingle::getAreaType()
	{
		return sAffectType::eBackSingle;
	}

	void AffectBackSingle::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*> tRoles = pInfo.getAlive()->getCurrSkillTargets();
		vector<BaseRole*>::reverse_iterator iter = tRoles.rbegin();
		for (;iter != tRoles.rend();iter++)
		{
			if ( (*iter)->getCaptain() )		//非主帅的后方位置
				continue;
			pInfo.addGrid((*iter)->getGridIndex());
			return;
		}
	}

	/***************************************************************************/

	//后方分散
	sAffectType AffectBackDisperse::getAreaType()
	{
		return sAffectType::eBackDisperse;
	}

	void AffectBackDisperse::initArea(AreaCountInfo& pInfo)
	{
		AffectBackSingle::initArea(pInfo);
		pInfo.DisperseDispose();
	}

	/***************************************************************************/

	//后方n排
	sAffectType AffectBackRow::getAreaType()
	{
		return sAffectType::eBackRow;
	}

	void AffectBackRow::initArea(AreaCountInfo& pInfo)
	{
		AffectBackSingle::initArea(pInfo);
		pInfo.RowType(eBackDirection);
	}

}