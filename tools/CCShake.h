#ifndef __SHAKE_H__
#define __SHAKE_H__

#include "cocos2d.h"
/************************************************************************
	ΥπΖΑ     CCNode->runaction(CCShake::Create(dt,range));
	//  [9/9/2014 xc_lin]
************************************************************************/

class CCShake : public cocos2d::CCActionInterval
{
   
public:

    CCShake(); 
    static CCShake* create(float d, float strength );
    static CCShake* createWithStrength(float d, float strength_x, float strength_y );
    bool initWithDuration(float d, float strength_x, float strength_y );
protected:
  
    void startWithTarget(cocos2d::CCNode *pTarget);
    void update(float time);
    void stop(void);
    float m_initial_x, m_initial_y;
    float m_strength_x, m_strength_y;
};

#endif //__SHAKE_H__