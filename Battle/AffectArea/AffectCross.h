/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectCrossType
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectCrossType_H_
#define __AffectCrossType_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	class AffectCross : public AffectArea
	{
	public:
		AffectCross(){};
		virtual ~AffectCross(){};
		UNINITCREATE(AffectCross);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
	};
};
#endif