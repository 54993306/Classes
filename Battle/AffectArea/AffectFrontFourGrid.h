/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectFrontFourGrid
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectFrontFourGrid_H_
#define __AffectFrontFourGrid_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	class AffectFrontFourGrid : public AffectArea
	{
	public:
		AffectFrontFourGrid(){};
		virtual ~AffectFrontFourGrid(){};
		UNINITCREATE(AffectFrontFourGrid);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};
};
#endif