#pragma once

/******************************************************
*文件名称:	HeroEvolveEffectLayer.h
*编写日期:	2016-6-22-11:15
*编写作者:	YPF
*功能描述:	进阶特效界面
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "model/CRoleData.h"

enum struct HeroEvolveEffectuUnlockType;

typedef void (CCObject::*EvolveEffectCallbackForChange)();
#define evolveEffectSelector(TBACK) (EvolveEffectCallbackForChange)(&TBACK)

class CHeroEvolveEffectLayer: public BaseLayer
{
public:
	CHeroEvolveEffectLayer();
	CREATE_LAYER(CHeroEvolveEffectLayer);
    virtual bool init();
    void onEnter();
    void onExit();

public:
	CC_SYNTHESIZE(CLayout *, m_pSkillPanel, SkillPanel);		//类似-技能框

	void showEffect(
		CHero *pBaseHero, CHero *pHero, 
		CCPoint posAim, CLayout* pSkillPanel, 
		HeroEvolveEffectuUnlockType iType);			//显示特效: baseHero 基础属性, hero 当前阶段属性, skillPanel技能面板
	
	void hideEffect();			//隐藏特效

	void bindBallChangeCallBack(CCObject* pTarget, EvolveEffectCallbackForChange callback);			//绑定球变化回调
	void bindAllEffectChangeCallBack(CCObject* pTarget, EvolveEffectCallbackForChange callback);			//绑定全部变化结束回调

private:
	void initEffect();		//初始化特效基础

	void frameEventCallBack(CCBone* pBone, const char* sEvent, int iTag1, int iTag2);	//骨骼帧事件回调
	void movementCallBack(CCArmature *pArmature, MovementEventType type, const char *str);			//骨骼动作事件回调

	void removeAllPlistFile();						//基础动画的额外plist图集
	void loadPlistFileWithRank(int iRank);		//基础动画的额外plist图集

	void updateData();		//更新数据

	void callbackChangeBall();					//球变化
	void callbackChangeAll();					//全部变化后

	void showDataFlyInByType(int iType);			//1基础属性 2特殊属性
	void showDataFlyOutByType(int iType);			//1基础属性 2特殊属性

private:
	CLayout											*m_pUI;
	CCArmature										*m_pEffectBase;							//基础特效，只有光效，球
	CCArmature										*m_pEffectForChange;					//属性变化的特效，数据，技能，宝箱，飞升
	CCPoint											m_posAim;									//飞入目标位置
	CHero												*m_pBaseHero;								//基础数据
	CHero												*m_pAimHero;								//目标数据	
	HeroEvolveEffectuUnlockType			m_eUnLockType;							//解锁类型
	CImageViewScale9							*m_pSkillBg;									//技能背景
	CCObject											*m_pTarget;									//执行回调的对象
	EvolveEffectCallbackForChange	m_effectCallbackForBallChange;	//球变化回调
	EvolveEffectCallbackForChange	m_effectCallbackForAllChange;		//全部特效执行完回调
};