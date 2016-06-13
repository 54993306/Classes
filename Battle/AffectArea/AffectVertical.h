/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectVerticalType
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectVerticalType_H_
#define __AffectVerticalType_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	class AffectVertical : public AffectArea
	{
	public:
		AffectVertical(){};
		virtual ~AffectVertical(){};
		CREATE_UNINIT(AffectVertical);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo& pInfo);
		virtual void moriGridRole(BaseRole* pRole,std::vector<int>& pValids);
	};
};


#endif