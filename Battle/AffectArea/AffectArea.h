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
#include "warscene/ConstNum.h"
#include "Battle/SkillMacro.h"
namespace BattleSpace{

	//��һЩ�����ķ�����Ҫ���е�������ʵ�ֵ�
	class AffectArea : public CCObject
	{
	public:
		AffectArea(){};
		virtual ~AffectArea(){};
		static AffectArea* create(int pAffectType);
		virtual AffectType getAreaType(){return AffectType::eNullArea;}
		virtual void initArea(AreaCountInfo& pInfo){};
		void initValidGrids(BaseRole* pRole,vector<int>& pValids);				//��ʼ����Ч�жϸ���
		virtual void moriGridRole(BaseRole* pRole,vector<int>& pValids);
	};
};


#endif