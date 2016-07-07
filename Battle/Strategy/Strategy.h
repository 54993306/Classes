/************************************************************* 
 *
 *
 *		Data : 2016.6.22
 *	
 *		Name : Strategy
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __Strategy_H_
#define __Strategy_H_
#include "cocos2d.h"
#include "Battle/BattleMacro.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class Strategy : public CCObject
	{
	public:
		Strategy(){};
		virtual ~Strategy(){};
		UNINITCREATE(Strategy);
		
	};
};
#endif