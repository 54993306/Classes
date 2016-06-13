/************************************************************* 
*
*
*		Data : 2016.6.12
*	
*		Name : AffectRange
*
*		Author : Lin_Xiancheng
*
*		Description : 技能效果范围
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

	//有一些公共的方法是要集中到父类中实现的
	class AffectArea : public CCObject
	{
	public:
		AffectArea(){};
		virtual ~AffectArea(){};
		static AffectArea* create(int pAffectType);
		virtual AffectType getAreaType(){return AffectType::eNullArea;}
		virtual void initArea(AreaCountInfo& pInfo){};
		void initValidGrids(BaseRole* pRole,vector<int>& pValids);				//初始化有效判断格子
		virtual void moriGridRole(BaseRole* pRole,vector<int>& pValids);
	};
};


#endif