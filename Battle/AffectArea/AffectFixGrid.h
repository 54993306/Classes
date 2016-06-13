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
		CREATE_UNINIT(AffectAnyFixGrid);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};

	/***************************************************************************/

	//随机固定格子区域
	class AffectAnyFixGridDisperse : public AffectAnyFixGrid
	{
	public:
		AffectAnyFixGridDisperse(){};
		virtual ~AffectAnyFixGridDisperse(){};
		CREATE_UNINIT(AffectAnyFixGridDisperse);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};
};
#endif