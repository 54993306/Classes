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
		CREATE_UNINIT(AffectFrontSingle);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
	};

	/***************************************************************************/

	//前方分散
	class AffectFrontDisperse : public AffectFrontSingle
	{
	public:
		AffectFrontDisperse(){};
		virtual ~AffectFrontDisperse(){};
		CREATE_UNINIT(AffectFrontDisperse);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};

	/***************************************************************************/

	//前军n排
	class AffectFrontRow : public AffectFrontSingle
	{
	public:
		AffectFrontRow(){};
		virtual ~AffectFrontRow(){};
		CREATE_UNINIT(AffectFrontRow);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};
};
#endif