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
		virtual ~AStar();
		virtual bool init();
		CREATE_FUNC(AStar);
	private:
		vector<MapNode*> mVecNodes;
		
	};
};
#endif