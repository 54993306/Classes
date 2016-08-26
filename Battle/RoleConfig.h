/** 
 *
 *		Data : 2016.4.20
 *	
 *		Name : RoleConfig
 *
 *		Author : Lin_Xiancheng
 *
 *		Description :  解析处理武将配置数据
 *
 */
#ifndef __ROLECONFIG__
#define __ROLECONFIG__
#include "AppUI.h"
namespace BattleSpace
{
	//定义的数据结构不用对外展示,将可能的用法在CRoleData中实现
	class RoleConfigData : public cocos2d::CCObject
	{
	public:
		RoleConfigData();
		virtual bool init(){return true;}
		CREATE_FUNC(RoleConfigData);
	public:
		CC_SYNTHESIZE(int,		m_ModeID,					ModeID);				//根据模型来索引不同武将的配置数据
		CC_SYNTHESIZE(bool,		m_Turn,						Turn);
		CC_SYNTHESIZE(int,		m_MoveSound,				MoveSound);				//移动音效
		CC_SYNTHESIZE(int,		m_HitEffect,				HitEffect);				//
		CC_SYNTHESIZE(int,		m_HitEffectX,				HitEffectX);
		CC_SYNTHESIZE(int,		m_HitEffectY,				HitEffectY);
		CC_SYNTHESIZE(float,	m_HitEffectScale,			HitEffectScale);		//受击效果缩放
		CC_SYNTHESIZE(float,	mVariantSpace,				VariantSpace);			//变身CD时间
		CC_SYNTHESIZE(float,	mVariantDuration,			VariantDuration);		//变身持续时间
		CC_SYNTHESIZE(float,	mVariantCooling,			VariantCooling);		//变身冷却时间
		CC_SYNTHESIZE(float,	mVariantRate,				VariantRate);			//变身属性变化比率
		CC_SYNTHESIZE(int,		mVariantEffect,				VariantEffect);			//变身特效(虚影跟模型都是绑定在一起的。还有变身模型,命名好可以不需要配置)
		CC_SYNTHESIZE(int,		mVariantModel,				VariantModel);			//变身模型(变身特效和返回特效都是固定的)
	};	

	class RoleConfig : public CCObject
	{
	public:
		RoleConfig();
		~RoleConfig();
		virtual bool init();
		CREATE_FUNC(RoleConfig);
		void AddConfigData(RoleConfigData* Data);
		void ClearData();
		CCNode* getActionEffect(int ModeID);
		RoleConfigData* getConfigData(int ModeId);
		int getMoveSound(int ModeID);
		bool isTurn(int id);
	private:
		std::map<int,RoleConfigData*> mMapConfigData;
	};
}
#endif