/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectAnyAlive
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectAnyAlive_H_
#define __AffectAnyAlive_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	//����̶��佫
	class AffectAnyAlive : public AffectArea
	{
	public:
		AffectAnyAlive(){};
		virtual ~AffectAnyAlive(){};
		UNINITCREATE(AffectAnyAlive);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};

	/***************************************************************************/

	//����̶��佫����
	class AffectAnyAliveDisperse : public AffectAnyAlive
	{
	public:
		AffectAnyAliveDisperse(){};
		virtual ~AffectAnyAliveDisperse(){};
		UNINITCREATE(AffectAnyAliveDisperse);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};
};
#endif