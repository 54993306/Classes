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
#include "Battle/ConstNum.h"
namespace BattleSpace
{
	MapNode::MapNode(int pGrid,MapNode* pParent)
		:mXRow(0),mYCol(0),mGValue(0),mHValue(0),mFValue(0),mParentNode(nullptr),mGridIndex(0)
	{
		mXRow = (pGrid%C_GRID_ROW);
		mYCol = (pGrid/C_GRID_ROW);
		mParentNode = pParent;
		mGridIndex = pGrid;
	}

	MapNode* MapNode::createMapNode( int pGrid,MapNode* pFather )
	{
		MapNode* tNode = new MapNode(pGrid,pFather);
		if (tNode)
		{
			tNode->autorelease();
			return tNode;
		}else{
			delete tNode;
			tNode = nullptr;
			return nullptr;
		}
	}

	int MapNode::getGridIndex()
	{
		return mGridIndex = mYCol*C_GRID_ROW + mXRow;
	}

	void MapNode::setGridIndex(int pGrid)
	{
		mGridIndex = pGrid;
	}
}