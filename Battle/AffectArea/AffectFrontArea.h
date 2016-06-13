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
	//ǰ������
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

	//ǰ����ɢ
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

	//ǰ��n��
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