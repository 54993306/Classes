// ΥπΖΑ
// 


#include "CCShake.h"

USING_NS_CC;

CCShake::CCShake() : m_strength_x(0), m_strength_y(0), m_initial_x(0), m_initial_y(0)
{
}

CCShake* CCShake::create( float d, float strength )
{
    return createWithStrength( d, strength, strength );
}

CCShake* CCShake::createWithStrength(float duration, float strength_x, float strength_y)
{
    CCShake *pRet = new CCShake();
    
    if (pRet && pRet->initWithDuration(duration, strength_x, strength_y))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

bool CCShake::initWithDuration(float duration, float strength_x, float strength_y)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_strength_x = strength_x;
        m_strength_y = strength_y;
        return true;
    }
   
    return false;
}

static float fgRangeRand( float min, float max )
{
    float rnd = ((float)rand()/(float)RAND_MAX);
    return rnd*(max-min)+min;
}

void CCShake::update(float dt)
{
    float randx = fgRangeRand( -m_strength_x, m_strength_x )*dt;
    float randy = fgRangeRand( -m_strength_y, m_strength_y )*dt;
    
    m_pTarget->setPosition( ccpAdd(ccp(m_initial_x, m_initial_y),ccp( randx, randy)));
}

void CCShake::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget( pTarget );
    m_initial_x = pTarget->getPosition().x;
    m_initial_y = pTarget->getPosition().y;
}

void CCShake::stop(void)
{
	 this->getTarget()->setPosition( ccp( m_initial_x, m_initial_y ) );
	CCActionInterval::stop();
}