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
		MapNode(int pGrid,MapNode* pFather);
		static MapNode* createMapNode(int pGrid,MapNode* pParent);
		CC_SYNTHESIZE(int,mXRow,XRow);
		CC_SYNTHESIZE(int,mYCol,YCol);
		CC_SYNTHESIZE(int,mGValue,GValue);
		CC_SYNTHESIZE(int,mHValue,HValue);
		CC_SYNTHESIZE(int,mFValue,FValue);
		CC_PROPERTY(int,mGridIndex,GridIndex);
		CC_SYNTHESIZE(MapNode*,mParentNode,ParentNode);
	};
};
#endif