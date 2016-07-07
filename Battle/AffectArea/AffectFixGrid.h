/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectAnyFix
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectAnyFix_H_
#define __AffectAnyFix_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	//����̶�����
	class AffectAnyFixGrid : public AffectArea
	{
	public:
		AffectAnyFixGrid(){};
		virtual ~AffectAnyFixGrid(){};
		UNINITCREATE(AffectAnyFixGrid);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};

	/***************************************************************************/

	//����̶���������
	class AffectAnyFixGridDisperse : public AffectAnyFixGrid
	{
	public:
		AffectAnyFixGridDisperse(){};
		virtual ~AffectAnyFixGridDisperse(){};
		UNINITCREATE(AffectAnyFixGridDisperse);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};
};
#endif