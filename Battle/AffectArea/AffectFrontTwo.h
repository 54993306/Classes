/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectFrontTwoType
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectFrontTwoType_H_
#define __AffectFrontTwoType_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	class AffectFrontTwo : public AffectArea
	{
	public:
		AffectFrontTwo(){};
		virtual ~AffectFrontTwo(){};
		UNINITCREATE(AffectFrontTwo);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};
};


#endif