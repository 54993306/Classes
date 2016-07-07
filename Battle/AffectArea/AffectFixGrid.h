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
	//随机固定格子
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

	//随机固定格子区域
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