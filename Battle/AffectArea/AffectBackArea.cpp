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
	//�󷽵���
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
			if ( (*iter)->getCaptain() )		//����˧�ĺ�λ��
				continue;
			pInfo.addGrid((*iter)->getGridIndex());
			return;
		}
	}

	/***************************************************************************/

	//�󷽷�ɢ
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

	//��n��
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