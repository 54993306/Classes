#ifndef _TASKOBJECT_H_
#define _TASKOBJECT_H_
#include "AppUI.h"
/************************************************************* 
 *
 *
 *		Data : 2016.9.22
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 这个类主要是做一个延时的效果，当运行时间大于fInterval则
 *					  把arr传入到回调函数m_pfnSelector里面去执行				//  [9/9/2014 xc_lin]
 *
 *
 *************************************************************/
const float NAME_PER_TIME = 0.2f;

class CombatTask : public CCObject
{
public:
	CombatTask();
	virtual ~CombatTask();
	static CombatTask* create(CCObject* pTraget,SEL_CallFuncO m_pfnSelector,float fInterval,CCObject*arr=nullptr);
	inline int getTaskID(){return m_id;}
	inline float getInterval(){return m_fInterval;}
	inline SEL_CallFuncO getFun(){return m_pfnSelector;}
	inline CCObject* getParam(){return m_param;}
	inline CCObject* getPtarget(){return m_target;}
	void shake(CCObject* obj);
protected:
	int m_id;
	float m_fInterval;
	SEL_CallFuncO m_pfnSelector;
	CCObject* m_param;
	CCObject* m_target;
};

#endif