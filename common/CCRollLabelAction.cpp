#include "CCRollLabelAction.h"  
#include "cocos2d.h"

USING_NS_CC;
  
// not really useful, but I like clean default constructors  
CCRollLabelAction::CCRollLabelAction()
{ 
	_pLabel = nullptr;
	_iFrom = 0;
	_iEnd = 0;
	_fGap = 0.0f;
} 
  
CCRollLabelAction* CCRollLabelAction::create( float duration, int iFrom, int iEnd, CCLabelProtocol* pLabel)  
{  
	CCRollLabelAction *pRet = new CCRollLabelAction();  

	if (pRet && pRet->initWithDuration(duration, iFrom, iEnd, pLabel))  
	{  
		pRet->autorelease();  
	}  
	else  
	{  
		CC_SAFE_DELETE(pRet);  
	}  

	return pRet;  
}

bool CCRollLabelAction::initWithDuration( float duration, int iFrom, int iEnd, CCLabelProtocol* pLabel)  
{  
    if (CCActionInterval::initWithDuration(duration))  
    {
		_pLabel = pLabel;
		_iFrom = iFrom;
		_iEnd = iEnd;
		_fGap = iEnd-iFrom;
        return true;  
    }  
      
    return false;  
}
  
void CCRollLabelAction::update(float dt)  
{  
	//..//
	int iCurrent = _iFrom + _fGap*dt;
	//CCLOG("%d_______%f_________%d_______%f", _iFrom, _fGap*dt, iCurrent, dt);
	sprintf( _cBuff, "%d", iCurrent);
	_pLabel->setString(_cBuff);
}  
  
void CCRollLabelAction::startWithTarget(CCNode *pTarget)  
{  
    CCActionInterval::startWithTarget( pTarget );
}  
  
void CCRollLabelAction::stop(void)  
{   
    CCActionInterval::stop();  

	// Action is done, reset clip position 
	//..//
	sprintf( _cBuff, "%d", _iEnd);
	_pLabel->setString(_cBuff);
}  

CCActionInterval* CCRollLabelAction::reverse() 
{
	return NULL;
}

CCActionInterval * CCRollLabelAction::clone() 
{
	return NULL;
}