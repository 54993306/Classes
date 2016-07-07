/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectMapCenterTwoLine
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectMapCenterTwoLine_H_
#define __AffectMapCenterTwoLine_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	//��ͼ�������й�����Χ
	class AffectMapCenterTwoLine : public AffectArea
	{
	public:
		AffectMapCenterTwoLine(){};
		virtual ~AffectMapCenterTwoLine(){};
		UNINITCREATE(AffectMapCenterTwoLine);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};

	/***************************************************************************/

	//��ͼ���������й�����Χ
	class AffectMapAnyDoubleLine : public AffectArea
	{
	public:
		AffectMapAnyDoubleLine(){};
		virtual ~AffectMapAnyDoubleLine(){};
		UNINITCREATE(AffectMapAnyDoubleLine);
		virtual sAffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};
};
#endif