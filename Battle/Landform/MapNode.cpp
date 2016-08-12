/************************************************************* 
 *
 *
 *		Data : 2016.8.9
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/Landform/MapNode.h"

namespace BattleSpace
{
	MapNode::MapNode():mRow(0),mCol(0),mGValue(0),mHValue(0),mFValue(0),mParentNode(nullptr)
	{}

	MapNode::~MapNode()
	{

	}

	bool MapNode::init()
	{
		return true;
	}

}