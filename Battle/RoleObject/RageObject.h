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
 *		Description : 怒气值显示对象(武将变身使用)
 *
 *
 *************************************************************/
namespace BattleSpace
{
	enum Direction_Rage
	{
		R_up,
		R_Down,
		R_left,
		R_Right,
	};
	class RageObject : public CCNode
	{
	protected:
		RageObject();
		virtual bool init();
	public:
		virtual ~RageObject();
		CREATE_FUNC(RageObject);
		void setDirection(int Direction);
		void setRageValue(float pRageValue);
		inline int getVal(){return mRageValue;}
		void setRageMax(float pMaxValue);
		inline int getRageMax(){return mRageMax;}
		void setPercent(float pPercent);
		inline float getPercent(){return mPercent;}
		void setDisplay(bool pDisplay);
		CCProgressTimer* getPro();
	protected:
		void updateRage();
	private:
		CCProgressTimer* mProgress;
		CCSprite* mBackdrop;
		float mRageValue;
		float mRageMax;
		float mPercent;
	};
};
#endif