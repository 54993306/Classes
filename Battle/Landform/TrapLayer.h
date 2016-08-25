
#ifndef __TRAPLAYER_H__
#define __TRAPLAYER_H__

/************************************************************* 
 *
 *
 *		Data : 2016.8.16
 *	
 *		Name : µÿ–Œœ‘ æ≤„
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : //  [9/9/2014 xc_lin]
 *
 *
 *************************************************************/
#include "cocos2d.h"
using namespace cocos2d;
namespace BattleSpace
{
	class TrapLayer : public CCNode
	{
	public:
		TrapLayer();
		~TrapLayer();
		CREATE_FUNC(TrapLayer);
		void initBattleTrap();
	protected:
		virtual bool init();
	protected:
		CCNode* mTrapObjects;
	};
};

#endif