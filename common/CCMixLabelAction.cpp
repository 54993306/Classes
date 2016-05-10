#include "CCMixLabelAction.h"  
#include "cocos2d.h"

USING_NS_CC;
  
// not really useful, but I like clean default constructors  
MixLabelAction::MixLabelAction()
{ 

} 
  
MixLabelAction* MixLabelAction::create( float duration, CCLabelProtocol* pLabel)  
{  
    // call other construction method with twice the same strength  
    return createWithStrength( duration, pLabel);  
}
  
MixLabelAction* MixLabelAction::createWithStrength( float duration, CCLabelProtocol* pLabel)  
{  
    MixLabelAction *pRet = new MixLabelAction();  
      
    if (pRet && pRet->initWithDuration(duration, pLabel))  
    {  
        pRet->autorelease();  
    }  
    else  
    {  
        CC_SAFE_DELETE(pRet);  
    }  
  
         
    return pRet;  
}  
  
bool MixLabelAction::initWithDuration( float duration, CCLabelProtocol* pLabel)  
{  
    if (CCActionInterval::initWithDuration(duration))  
    {  
        _strNumber = pLabel->getString();
		_pLabel = pLabel;
        return true;  
    }  
      
    return false;  
}  
  
// Helper function. I included it here so that you can compile the whole file  
// it returns a random value between min and max included  
static float fgRangeRand( float min, float max )  
{  
    float rnd = ((float)rand()/(float)RAND_MAX);  
    return rnd*(max-min)+min;  
}  
  
void MixLabelAction::update(float dt)  
{  
	//..//
	std::string str = _strNumber;
	for(int i=0; i<str.size(); i++)
	{
		char c = str.at(i);
		if(c>='0' && c<='9')
		{
			str.replace(i, 1, 1, '1'+(int)(CCRANDOM_0_1()*9));
		}
	}
	_pLabel->setString(str.c_str());
}  
  
void MixLabelAction::startWithTarget(CCNode *pTarget)  
{  
    CCActionInterval::startWithTarget( pTarget );
}  
  
void MixLabelAction::stop(void)  
{   
    CCActionInterval::stop();  

	// Action is done, reset clip position 
	//..//
	_pLabel->setString(_strNumber.c_str());
}  

CCActionInterval* MixLabelAction::reverse() 
{
	return NULL;
}

CCActionInterval * MixLabelAction::clone() 
{
	return NULL;
}