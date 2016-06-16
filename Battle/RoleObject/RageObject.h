#ifndef _RAGE_H_
#define _RAGE_H_
#include "cocos2d.h"
#include "AppUI.h"
/************************************************************* 
 *
 *
 *		Data : 2016.5.23
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 怒气值显示对象(已废弃)
 *
 *
 *************************************************************/
namespace BattleSpace{
	enum Direction_Rage
	{
		R_up,
		R_Down,
		R_left,
		R_Right,
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
};
#endif