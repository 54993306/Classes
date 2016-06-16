/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectTArea
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectTArea_H_
#define __AffectTArea_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	class AffectTArea : public AffectArea
	{
	public:
		AffectTArea(){};
		virtual ~AffectTArea(){};
		UNINITCREATE(AffectTArea);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
	};
};
#endif