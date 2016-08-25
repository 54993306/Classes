/************************************************************* 
 *
 *
 *		Data : 2016.8.21
 *	
 *		Name : TrapObject
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 地形显示对象
 *
 *
*************************************************************/

#ifndef __TrapObject_H_
#define __TrapObject_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class BattleTrap;
	class TrapObject : public CCNode
	{
	protected:
		TrapObject(BattleTrap* pData);
		virtual bool init();
		virtual void initSpineTrap();
		virtual void initImageTrap();
	public:
		virtual ~TrapObject();
		static TrapObject* createTrapObject(BattleTrap* pData);
	protected:
		const BattleTrap* mData;
		CCNode* mTrapNode;
	};
};
#endif