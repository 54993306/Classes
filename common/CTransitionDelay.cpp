#include "CTransitionDelay.h"


CTransitionDelay::CTransitionDelay()
{
}
CTransitionDelay::~CTransitionDelay()
{
}

CTransitionDelay * CTransitionDelay::create(float duration, CCScene *scene)
{
    CTransitionDelay * pTransition = new CTransitionDelay();
    pTransition->initWithDuration(duration, scene);
    pTransition->autorelease();
    return pTransition;
}

bool CTransitionDelay::initWithDuration(float t, CCScene *scene)
{
    return CCTransitionScene::initWithDuration(t, scene);
}

void CTransitionDelay :: onEnter()
{
    CCTransitionScene::onEnter();

	m_pInScene->setVisible(false);

    CCActionInterval* a = (CCActionInterval *)CCSequence::create
        (
            CCDelayTime::create(m_fDuration),
            CCCallFunc::create(this, callfunc_selector(CCTransitionScene::finish)),
            NULL
        );
    this->runAction(a);
}

void CTransitionDelay::onExit()
{
    CCTransitionScene::onExit();
}