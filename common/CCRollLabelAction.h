#pragma once

#include "cocos2d.h"  

USING_NS_CC;

class CCRollLabelAction : public CCActionInterval  
{  
public:  
	CCRollLabelAction();

	static CCRollLabelAction *create(float duration, int iFrom, int iEnd, CCLabelProtocol* pLabel);  

	bool initWithDuration(float duration, int iFrom, int iEnd, CCLabelProtocol* pLabel);
protected:
	void startWithTarget(cocos2d::CCNode *pTarget);  
	void update(float time);  
	void stop(void);
	virtual CCActionInterval* reverse();
	virtual CCActionInterval * clone();
private:
	CCLabelProtocol* _pLabel;
	int _iFrom;
	int _iEnd;
	float _fGap;
	char _cBuff[50];
};