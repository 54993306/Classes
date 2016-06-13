/************************************************************* 
*
*
*		Data : 2016.6.12
*	
*		Name : AffectFrontType
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __AffectFrontType_H_
#define __AffectFrontType_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	class AffectFront : public AffectArea
	{
	public:
		AffectFront(){};
		virtual ~AffectFront(){};
		CREATE_UNINIT(AffectFront);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo& pInfo);
	};

	/***************************************************************************/

	//前方范围不贯穿类型
	class AffectPuncture : public AffectFront
	{
	public:
		AffectPuncture(){};
		virtual ~AffectPuncture(){};
		CREATE_UNINIT(AffectPuncture);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo& pInfo);
	};
};
#endif