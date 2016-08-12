/************************************************************* 
 *
 *
 *		Data : 2016.8.9
 *	
 *		Name : MapNode
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __MapNode_H_
#define __MapNode_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class MapNode : public CCObject
	{
	public:
		MapNode();
		virtual ~MapNode();
		virtual bool init();
		CREATE_FUNC(MapNode);
		CC_SYNTHESIZE(int,mRow,Row);
		CC_SYNTHESIZE(int,mCol,Col);
		CC_SYNTHESIZE(int,mGValue,GValue);
		CC_SYNTHESIZE(int,mHValue,HValue);
		CC_SYNTHESIZE(int,mFValue,FValue);
		CC_SYNTHESIZE(MapNode*,mParentNode,ParentNode);


	};
};
#endif