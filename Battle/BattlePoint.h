/************************************************************* 
 *
 *
 *		Data : 2016.7.6
 *	
 *		Name : BattlePoint
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __BattlePoint_H_
#define __BattlePoint_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class BattlePoint : public CCObject
	{
	public:
		BattlePoint(float pX,float pY)
			:m_X(pX),m_Y(pY){};
		virtual ~BattlePoint(){};
		CC_SYNTHESIZE(float,m_X,X);
		CC_SYNTHESIZE(float,m_Y,Y);
		static BattlePoint* create(float  pX,float pY)
		{
			BattlePoint* tPoint = new BattlePoint(pX,pY);
			if (tPoint)
			{
				tPoint->autorelease();
			}
			return tPoint;
		}
		
	};
};
#endif