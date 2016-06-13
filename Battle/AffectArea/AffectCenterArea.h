/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectCenterRow
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectCenterRow_H_
#define __AffectCenterRow_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	//���ĵ���
	class AffectCenterSingle : public AffectArea
	{
	public:
		AffectCenterSingle(){};
		virtual ~AffectCenterSingle(){};
		CREATE_UNINIT(AffectCenterSingle);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};

	/***************************************************************************/

	//���ķ�ɢ
	class AffectCenterDisperse : public AffectCenterSingle
	{
	public:
		AffectCenterDisperse(){};
		virtual ~AffectCenterDisperse(){};
		CREATE_UNINIT(AffectCenterDisperse);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);

	};

	/***************************************************************************/

	//����n��
	class AffectCenterRow : public AffectCenterSingle
	{
	public:
		AffectCenterRow(){};
		virtual ~AffectCenterRow(){};
		CREATE_UNINIT(AffectCenterRow);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
		
	};
};
#endif