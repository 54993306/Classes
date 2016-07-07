#pragma once

/******************************************************
*文件名称:	CCRollLabelAction.h
*编写日期:	2016-6-14-11:37
*编写作者:	YPF
*功能描述:	数字线性滚动动作
*******************************************************/

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