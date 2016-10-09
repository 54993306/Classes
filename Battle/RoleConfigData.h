
/************************************************************* 
 *
 *
 *		Data : 2016.8.29
 *	
 *		Name : RoleConfigData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __RoleConfigData_H_
#define __RoleConfigData_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class RoleConfigData : public cocos2d::CCObject
	{
	protected:
		RoleConfigData();
		virtual bool init(){return true;}
	public:
		CREATE_FUNC(RoleConfigData);
	public:
		CC_SYNTHESIZE(int,		m_ModeID,					ModeID);				//根据模型来索引不同武将的配置数据
		CC_SYNTHESIZE(bool,		m_Turn,						Turn);
		CC_SYNTHESIZE(int,		m_MoveSound,				MoveSound);				//移动音效
		CC_SYNTHESIZE(int,		m_HitEffect,				HitEffect);				//
		CC_SYNTHESIZE(int,		m_HitEffectX,				HitEffectX);
		CC_SYNTHESIZE(int,		m_HitEffectY,				HitEffectY);
		CC_SYNTHESIZE(float,	m_HitEffectScale,			HitEffectScale);		//受击效果缩放
		CC_SYNTHESIZE(bool,		mVariant,					Variant);				//是否可变身
		CC_SYNTHESIZE(float,	mVariantScale,				VariantScale);			//变身缩放
		CC_SYNTHESIZE(float,	mVariantCooldown,			VariantCooldown);		//变身CD时间
		CC_SYNTHESIZE(float,	mVariantDuration,			VariantDuration);		//变身持续时间
		CC_SYNTHESIZE(float,	mVariantCooling,			VariantCooling);		//变身冷却时间
		CC_SYNTHESIZE(float,	mVariantRate,				VariantRate);			//变身属性变化比率
		CC_SYNTHESIZE(int,		mVariantEffect,				VariantEffect);			//变身特效(虚影跟模型都是绑定在一起的。还有变身模型,命名好可以不需要配置)
		CC_SYNTHESIZE(int,		mVariantModel,				VariantModel);			//变身模型(变身特效和返回特效都是固定的)
	public:
		CCNode* getActionEffect();
	};
};
#endif