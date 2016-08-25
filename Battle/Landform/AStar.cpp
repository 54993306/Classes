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
#include "Battle/Landform/AStar.h"
#include "Battle/Landform/MapNode.h"
#include "Battle/ConstNum.h"
#include "AppUI.h"
namespace BattleSpace
{
	AStar::AStar():mRowMax(C_GRID_ROW),mColMax(C_GRID_COL),mStraightExpend(10),mObliqueExpend(14)
	{}

	void AStar::initMovePath( const vector<int>&pObstacle,vector<int>& pPath,int pBegin,int pEnd )
	{
		pPath.clear();
		mOpenVec.clear();
		mCloseVec.clear();
		mObstacle.clear();
		mObstacle.assign(pObstacle.begin(),pObstacle.end());
		if (pEnd >= C_CAPTAINGRID || pEnd < C_BEGINGRID)
			return;
		for (auto tGrid : pObstacle)
		{
			if (tGrid == pBegin || tGrid == pEnd)
				return;
		}
		MapNode* tBeginNode = MapNode::createMapNode(pBegin,nullptr);
		MapNode* tEndNode	= MapNode::createMapNode(pEnd,nullptr);
		mOpenVec.push_back(tBeginNode);
		vector<int> tPath;
		SearchPath(tPath,tEndNode);
		if (!tPath.empty())
			pPath.assign(tPath.begin()+1,tPath.end());		//��һ��λ��Ϊ�佫��վ��λ��
	}
	bool sortNode(const MapNode*pNodeA,const MapNode*pNodeB)	
	{ 
		return  pNodeA->getFValue() < pNodeB->getFValue(); 
	}
	void AStar::SearchPath( vector<int>& pPath,MapNode* pEndNode )
	{
		vector<MapNode*> tPathNode;
		bool tFindPath = false;
		MapNode* tCurrNode = nullptr;
		while (!mOpenVec.empty())
		{
			tCurrNode = mOpenVec.at(0);
			if (tCurrNode->getGridIndex() == pEndNode->getGridIndex())
			{
				tFindPath = true;
				break;					//�ҵ�·��
			}
			checkRound(tCurrNode,pEndNode);
			mCloseVec.push_back(tCurrNode);
			mOpenVec.erase(mOpenVec.begin());
			std::sort(mOpenVec.begin(),mOpenVec.end(),sortNode);
		}
		if (!tFindPath)
			return;
		getMovePath(pPath,tCurrNode);
		//tFindPath������path��ֵ
	}

	void AStar::getMovePath( vector<int>& pVector,MapNode* pEndNode )
	{
		if (pEndNode->getParentNode())
		{
			getMovePath(pVector,pEndNode->getParentNode());
		}
		pVector.push_back(pEndNode->getGridIndex());
	}

	void AStar::checkRound(MapNode* pCurrNode,MapNode* pEndNode)
	{
		if ((pCurrNode->getYCol()-1) >= 0)
		{
			checkPath(pCurrNode->getGridIndex() - C_GRID_ROW,pCurrNode,pEndNode,mStraightExpend);
		}
		if ((pCurrNode->getYCol()+1)<C_GRID_COL)
		{
			checkPath(pCurrNode->getGridIndex() + C_GRID_ROW,pCurrNode,pEndNode,mStraightExpend);
		}
		if ((pCurrNode->getXRow()-1) >= 0)
		{
			checkPath(pCurrNode->getGridIndex() - 1,pCurrNode,pEndNode,mStraightExpend);
		}
		if ((pCurrNode->getXRow()+1) < C_GRID_ROW)
		{
			checkPath(pCurrNode->getGridIndex() + 1,pCurrNode,pEndNode,mStraightExpend);
		}
		if ((pCurrNode->getYCol()-1) >= 0 && (pCurrNode->getXRow()-1) >= 0)
		{
			checkPath(pCurrNode->getGridIndex() - C_GRID_ROW - 1,pCurrNode,pEndNode,mObliqueExpend);
		}
		if ((pCurrNode->getYCol()-1) >= 0 && (pCurrNode->getXRow()+1) < C_GRID_ROW)
		{
			checkPath(pCurrNode->getGridIndex() - C_GRID_ROW + 1 ,pCurrNode,pEndNode,mObliqueExpend);
		}
		if ((pCurrNode->getYCol()+1) >= 0 && (pCurrNode->getXRow()-1) >= 0)
		{
			checkPath(pCurrNode->getGridIndex() + C_GRID_ROW - 1 ,pCurrNode,pEndNode,mObliqueExpend);
		}
		if ((pCurrNode->getYCol()+1) >= 0 && (pCurrNode->getXRow()+1) < C_GRID_ROW)
		{
			checkPath(pCurrNode->getGridIndex() + C_GRID_ROW + 1 ,pCurrNode,pEndNode,mObliqueExpend);
		}
	}

	bool AStar::checkPath( int pGrid,MapNode* pParent,MapNode* pEndNode,int pExpend )
	{
		MapNode* tNextNode = MapNode::createMapNode(pGrid,pParent);
		for (auto tGrod : mObstacle)
		{
			if (pGrid == tGrod)								//�ϰ�����
			{
				mCloseVec.push_back(tNextNode);
				return false;
			}
		}

		if (isVectorContains(mCloseVec,pGrid) != -1)	//�ѽ��ڹر��б���
			return false;
		int tIndex = isVectorContains(mOpenVec,pGrid);	
		if ( tIndex != -1)								//�ѽ��ڿ����б���
		{
			//ͨ����ǰ�㵽�￪���б��еĽڵ������ǲ��Ǹ���
			if ((pParent->getGValue() + pExpend) < mOpenVec.at(tIndex)->getGValue())
			{
				tNextNode->setParentNode(pParent);		//�޸Ľڵ㸸�ڵ�
				CountGValue(tNextNode,pEndNode,pExpend);
				CountFValue(tNextNode);
				mOpenVec.insert(mOpenVec.begin()+tIndex,tNextNode);
			}
		}else{
			tNextNode->setParentNode(pParent);
			mOpenVec.push_back(tNextNode);
			CountNodeValue(tNextNode,pEndNode,pExpend);
		}
		return true;
	}

	int AStar::isVectorContains( vector<MapNode*> pVector,int pGrid )
	{
		for (int tIndex = 0;tIndex<pVector.size();tIndex++)
		{
			if (pVector.at(tIndex)->getGridIndex() == pGrid)
				return tIndex;
		}
		return -1;
	}
	//���㵱ǰ�ڵ㵽�յ��Gֵ
	void AStar::CountGValue( MapNode* pCurrNode,MapNode* pEndNode,int pExpend )
	{
		if (pCurrNode->getParentNode())
		{
			pCurrNode->setGValue(pCurrNode->getParentNode()->getGValue() + pExpend);
		}else{
			pCurrNode->setGValue(pExpend);
		}
	}

	void AStar::CountHValue( MapNode* pCurrNode,MapNode* pEndNode )
	{
		pCurrNode->setHValue((abs(pCurrNode->getXRow() - pEndNode->getXRow()) + abs(pCurrNode->getYCol() - pEndNode->getYCol())) * 10);
	}

	void AStar::CountFValue( MapNode* pCurrNode )
	{
		pCurrNode->setFValue(pCurrNode->getGValue() + pCurrNode->getHValue());
	}

	void AStar::CountNodeValue( MapNode* pCurrNode,MapNode* pEndNode,int pExpend )
	{
		CountGValue(pCurrNode,pEndNode,pExpend);
		CountHValue(pCurrNode,pEndNode);
		CountFValue(pCurrNode);
	}
}