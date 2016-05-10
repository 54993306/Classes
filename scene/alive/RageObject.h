#ifndef _RAGE_H_
#define _RAGE_H_
#include "cocos2d.h"
#include "AppUI.h"
/************************************************************************/
/*      
							怒气值对象管理	//  [11/5/2014 xc_lin]
*/
/************************************************************************/
enum rageTag
{
	agility_tag = 1,
};
class RageObject : public CCNode
{
public:
	RageObject();
	virtual ~RageObject();
	CREATE_FUNC(RageObject);
	void setSkin(const char* bg,const char* fore);
	void setDirection(int Direction);
	void setVal(float rage);
	void setRageMax(float rageMax);
	inline int getVal(){return m_rageval;}
	inline int getRageMax(){return m_rageMax;}
	inline float getPercent(){return m_percent;}
	void setPercent(float pe,float p);
	void junpNum(CCObject* ob);
	void setDisplay(bool display);
	CCProgressTimer* getPro();
protected:
	void updateRage();
private:
	CCProgressTimer* m_rage;
	CCSprite* m_bg;
	float m_rageval;
	float m_rageMax;
	float m_percent;
};

enum Direction_Rage
{
	R_up,
	R_Down,
	R_left,
	R_Right,
};
#endif