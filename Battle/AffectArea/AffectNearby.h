/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectNearbyType
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectNearbyType_H_
#define __AffectNearbyType_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	class AffectNearby  : public AffectArea
	{
	public:
		AffectNearby(){};
		virtual ~AffectNearby(){};
		CREATE_UNINIT(AffectNearby);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo& pInfo);
		virtual void moriGridRole(BaseRole* pRole,std::vector<int>& pValids);
	};

	class AffectNearbyUnself : public AffectNearby				//子类继承了子类
	{
	public:
		AffectNearbyUnself(){};
		virtual ~AffectNearbyUnself(){};
		CREATE_UNINIT(AffectNearbyUnself);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};
};
#endif