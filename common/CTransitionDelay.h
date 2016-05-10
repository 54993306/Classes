#pragma once

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
