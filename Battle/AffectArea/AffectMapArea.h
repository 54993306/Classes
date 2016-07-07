/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectMapCenterTwoLine
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectMapCenterTwoLine_H_
#define __AffectMapCenterTwoLine_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	//地图中心两行攻击范围
	class AffectMapCenterTwoLine : public AffectArea
	{
	public:
		AffectMapCenterTwoLine(){};
		virtual ~AffectMapCenterTwoLine(){};
		UNINITCREATE(AffectMapCenterTwoLine);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};

	/***************************************************************************/

	//地图中任意两行攻击范围
	class AffectMapAnyDoubleLine : public AffectArea
	{
	public:
		AffectMapAnyDoubleLine(){};
		virtual ~AffectMapAnyDoubleLine(){};
		UNINITCREATE(AffectMapAnyDoubleLine);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};
};
#endif