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
	AffectType AffectBackSingle::getAreaType()
	{
		return AffectType::eBackSingle;
	}

	void AffectBackSingle::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*>* tRoles = pInfo.getAlive()->getSkillTargets();
		vector<BaseRole*>::reverse_iterator iter = (*tRoles).rbegin();
		for (;iter != (*tRoles).rend();iter++)
		{
			if ( (*iter)->getCaptain() )		//����˧�ĺ�λ��
				continue;
			pInfo.addGrid((*iter)->getGridIndex());
			return;
		}
	}

	/***************************************************************************/

	//�󷽷�ɢ
	AffectType AffectBackDisperse::getAreaType()
	{
		return AffectType::eBackDisperse;
	}

	void AffectBackDisperse::initArea(AreaCountInfo& pInfo)
	{
		AffectBackSingle::initArea(pInfo);
		pInfo.DisperseDispose();
	}

	/***************************************************************************/

	//��n��
	AffectType AffectBackRow::getAreaType()
	{
		return AffectType::eBackRow;
	}

	void AffectBackRow::initArea(AreaCountInfo& pInfo)
	{
		AffectBackSingle::initArea(pInfo);
		pInfo.RowType(eBackDirection);
	}

}