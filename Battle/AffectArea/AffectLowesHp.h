/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : AffectLowesHp
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AffectLowesHp_H_
#define __AffectLowesHp_H_
#include "Battle/AffectArea/AffectArea.h"
namespace BattleSpace
{
	//Ѫ������Ŀ��
	class AffectLowesHp : public AffectArea
	{
	public:
		AffectLowesHp(){};
		virtual ~AffectLowesHp(){};
		CREATE_UNINIT(AffectLowesHp);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
	};

	/***************************************************************************/

	//Ѫ������Ŀ�����˧��
	class AffectLowesHpUnCaptain : public AffectLowesHp
	{
	public:
		AffectLowesHpUnCaptain(){};
		virtual ~AffectLowesHpUnCaptain(){};
		CREATE_UNINIT(AffectLowesHpUnCaptain);
		virtual AffectType getAreaType();
		virtual void initArea(AreaCountInfo &pInfo);
	};
};
#endif