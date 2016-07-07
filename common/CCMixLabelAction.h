#pragma once

/******************************************************
*文件名称:	CCMixLabelAction.h
*编写日期:	2016-6-14-11:36
*编写作者:	YPF
*功能描述:	数字随机滚动动作
*******************************************************/

#include "cocos2d.h"  

USING_NS_CC;

class MixLabelAction : public CCActionInterval  
{  
public:  
	MixLabelAction();

	static MixLabelAction *create(float duration, CCLabelProtocol* pLabel);  

	static MixLabelAction *createWithStrength(float duration, CCLabelProtocol* pLabel);  
	bool initWithDuration(float duration, CCLabelProtocol* pLabel);
protected:
	void startWithTarget(cocos2d::CCNode *pTarget);  
	void update(float time);  
	void stop(void);
	virtual CCActionInterval* reverse();
	virtual CCActionInterval * clone();
private:
	std::string _strNumber;
	CCLabelProtocol* _pLabel;
};