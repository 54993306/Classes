
#ifndef __MoveObject__
#define __MoveObject__
/************************************************************* 
 *
 *
 *		Data : 2016.5.23
 *	
 *		Name : MoveObject
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/
#include "cocos2d.h"
namespace BattleSpace
{
	class BaseRole;
	class MoveObject : public cocos2d::CCNode
	{
	public:
		MoveObject();
		void initMoveSprite();
		CREATE_FUNC(MoveObject);
		CC_SYNTHESIZE(BaseRole*,m_Alive,MoveAlive);
		CC_PROPERTY(int,m_grid,grid);
		void setRowCol(int pRow,int pCol);
		void setOffs(float pOffsX,float pOffsY);
		std::vector<int> grids;
	private:
		int m_Row;
		int m_Col;
		float m_OffsX;
		float m_OffsY;
	};
};
#endif