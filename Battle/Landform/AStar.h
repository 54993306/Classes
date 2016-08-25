/************************************************************* 
 *
 *
 *		Data : 2016.8.9
 *	
 *		Name : AStar
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AStar_H_
#define __AStar_H_

#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

namespace BattleSpace
{
	class MapNode;
	class AStar : public CCObject
	{
	public:
		AStar();
		virtual bool init(){return true;}
		CREATE_FUNC(AStar);
		CC_SYNTHESIZE(int,mRowMax,RowMax);
		CC_SYNTHESIZE(int,mColMax,ColMan);
	public:
		void initMovePath(const vector<int>&pObstacle,vector<int>& pPath,int pBegin,int pEnd);
	protected:
		void SearchPath(vector<int>& pPath,MapNode* pEnd);
		void checkRound(MapNode* pCurrNode,MapNode* pEndNode);
		bool checkPath(int pGrid,MapNode* pParent,MapNode* pEndNode,int pExpend);
		int isVectorContains(vector<MapNode*> pVector,int pGrid);
		void getMovePath(vector<int>& pVector,MapNode* pEndNode);
		void CountGValue(MapNode* pCurrNode,MapNode* pEndNode,int pExpend);
		void CountHValue(MapNode* pCurrNode,MapNode* pEndNode);
		void CountFValue(MapNode* pCurrNode);
		void CountNodeValue(MapNode* pCurrNode,MapNode* pEndNode,int pExpend);
	private:
		int mStraightExpend;
		int mObliqueExpend;
		vector<int> mObstacle;
		vector<MapNode*> mCloseVec;
		vector<MapNode*> mOpenVec;
	};
};
#endif