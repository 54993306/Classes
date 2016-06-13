/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectBackRow
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectBackRow_H_
#define __AffectBackRow_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	//后方单体
	class AffectBackSingle : public AffectArea
	{
	public:
		AffectBackSingle(){};
		virtual ~AffectBackSingle(){};
		CREATE_UNINIT(AffectBackSingle);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};

	/***************************************************************************/

	//后方分散
	class AffectBackDisperse : public AffectBackSingle
	{
	public:
		AffectBackDisperse(){};
		virtual ~AffectBackDisperse(){};
		CREATE_UNINIT(AffectBackDisperse);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};

	/***************************************************************************/

	//后方n排
	class AffectBackRow : public AffectBackSingle
	{
	public:
		AffectBackRow(){};
		virtual ~AffectBackRow(){};
		CREATE_UNINIT(AffectBackRow);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};
};
#endif