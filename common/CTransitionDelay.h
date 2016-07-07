#pragma once

/******************************************************
*�ļ�����:	CTransitionDelay.h
*��д����:	2016-6-14-11:40
*��д����:	YPF
*��������:	��������Ч��-��ʱ��֤��һ���������ز�����
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
