/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectEnemy
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectEnemy_H_
#define __AffectEnemy_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	class AffectEnemy : public AffectArea
	{
	public:
		AffectEnemy(){};
		virtual ~AffectEnemy(){};
		UNINITCREATE(AffectEnemy);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
	};

	/***************************************************************************/

	class AffectOur : public AffectEnemy
	{
	public:
		AffectOur(){};
		virtual ~AffectOur(){};
		UNINITCREATE(AffectOur);
		virtual AffectType getAreaType();
	};

	/***************************************************************************/

	class AffectAllArea : public AffectEnemy
	{
	public:
		AffectAllArea(){};
		virtual ~AffectAllArea(){};
		UNINITCREATE(AffectAllArea);
		virtual AffectType getAreaType();
	};
};
#endif