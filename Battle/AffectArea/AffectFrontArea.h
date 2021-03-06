/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectFrontRow
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectFrontRow_H_
#define __AffectFrontRow_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	//前方单体
	class AffectFrontSingle : public AffectArea
	{
	public:
		AffectFrontSingle(){};
		virtual ~AffectFrontSingle(){};
		UNINITCREATE(AffectFrontSingle);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
	};

	/***************************************************************************/

	//前方分散
	class AffectFrontDisperse : public AffectFrontSingle
	{
	public:
		AffectFrontDisperse(){};
		virtual ~AffectFrontDisperse(){};
		UNINITCREATE(AffectFrontDisperse);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};

	/***************************************************************************/

	//前军n排
	class AffectFrontRow : public AffectFrontSingle
	{
	public:
		AffectFrontRow(){};
		virtual ~AffectFrontRow(){};
		UNINITCREATE(AffectFrontRow);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};
};
#endif