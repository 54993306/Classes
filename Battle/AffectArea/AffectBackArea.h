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
	//�󷽵���
	class AffectBackSingle : public AffectArea
	{
	public:
		AffectBackSingle(){};
		virtual ~AffectBackSingle(){};
		UNINITCREATE(AffectBackSingle);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};

	/***************************************************************************/

	//�󷽷�ɢ
	class AffectBackDisperse : public AffectBackSingle
	{
	public:
		AffectBackDisperse(){};
		virtual ~AffectBackDisperse(){};
		UNINITCREATE(AffectBackDisperse);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};

	/***************************************************************************/

	//��n��
	class AffectBackRow : public AffectBackSingle
	{
	public:
		AffectBackRow(){};
		virtual ~AffectBackRow(){};
		UNINITCREATE(AffectBackRow);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};
};
#endif