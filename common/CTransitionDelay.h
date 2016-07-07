#pragma once

/******************************************************
*文件名称:	CTransitionDelay.h
*编写日期:	2016-6-14-11:40
*编写作者:	YPF
*功能描述:	场景过渡效果-延时保证下一个场景加载不卡顿
*******************************************************/

#include "cocos2d.h"

USING_NS_CC;

class CTransitionDelay : public CCTransitionScene
{
public:
    CTransitionDelay();

    virtual ~CTransitionDelay();
    
    static CTransitionDelay* create(float duration,CCScene* scene);

    virtual bool initWithDuration(float t,CCScene* scene);

    virtual void onEnter();

    virtual void onExit();
};
