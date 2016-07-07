/************************************************************* 
*
*
*		Data : 2016.6.12
*	
*		Name : AffectRange
*
*		Author : Lin_Xiancheng
*
*		Description : ����Ч����Χ
*
*
*************************************************************/

#ifndef __AffectRange_H_
#define __AffectRange_H_
#include "cocos2d.h"
#include "Battle/BattleMacro.h"
#include "Battle/AreaCount.h"
#include "Battle/BaseRole.h"
#include "Battle/ConstNum.h"
#include "Battle/skEffectData.h"
namespace BattleSpace{
	enum struct sAffectType 
	{
		eNullArea						= 0,
		eFront							= 1,//ǰ����������				
		eNearby							= 2,//�ܱ߷�Χ					
		eVertical						= 3,//�����������				
		eCross							= 4,//ʮ��						
		eFrontTow						= 5,//ǰ������					
		eNearbyUnself					= 6,//�ܱ߷�Χ����������			
		eEnemy							= 7,//�з�ȫ��					
		eOur							= 8,//�ҷ�ȫ��					
		eAllArea						= 9,//ȫͼ					
		ePuncture						= 10,//ǰ���������Ͳ��ᴩ			
		eTArea							= 11,//T������				
		//���佫վλ�����޹��๥����Χ		(��100��ʼ�������⻯����)		
		RestArea						= 100,
		eFrontSingle					= 101,//ǰ������					
		eCenterSingle					= 102,//�о�����					
		eBackSingle						= 103,//�������					
		eFrontDisperse					= 104,//ǰ����ɢ��Χ				
		eCenterDisperse					= 105,//�о���ɢ��Χ				
		eBackDisperse					= 106,//�����ɢ��Χ				
		eFrontRow						= 107,//ǰ��n��					
		eCenterRow						= 108,//�о�n��					
		eBackRow						= 109,//���n��					
		eAnyFixGrid						= 110,//����̶�����				
		eAnyFixGridDisperse				= 111,//����̶����ӷ�ɢ����		
		eAnyFixAlive					= 112,//����̶��佫				
		eAnyFixAliveDisperse			= 113,//����̶��佫����			
		eLowestHp						= 114,//Ѫ����Ͷ���				
		eMapCenterTwoLine				= 115,//��ͼ�м���������			
		eFrontFourGrid					= 116,//��ͼ��������				
		eLowestHpUnCaptain				= 117,//����˧��Ѫ����Ͷ���		
		eMapAnyDoubleLine				= 118,//��ͼ����������			
	};
	//��һЩ�����ķ�����Ҫ���е�������ʵ�ֵ�
	class AffectArea : public CCObject
	{
	public:
		AffectArea():mAffectType(sAffectType::eNullArea){};
		virtual ~AffectArea(){};
		static AffectArea* create(int pAffectType);
		void setEffectData(skEffectData* pData);
		virtual sAffectType getAreaType(){return sAffectType::eNullArea;}
		virtual void commonHandle(AreaCountInfo& pInfo);
		virtual void initArea(AreaCountInfo& pInfo){};
	protected:
		void initValidGrids(BaseRole* pRole,vector<int>& pValids);				//��ʼ����Ч�жϸ���
		virtual void moriGridRole(BaseRole* pRole,vector<int>& pValids);
	protected:
		skEffectData* mEffectData;
		sAffectType mAffectType;
	};
};


#endif