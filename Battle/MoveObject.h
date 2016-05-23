
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

using namespace cocos2d;
using namespace std;
class WarAlive;
class ActObject;

class MoveObject : public CCNode
{
public:
	MoveObject();
	virtual bool init();
	void initMoveSprite();
	CREATE_FUNC(MoveObject);
	CC_SYNTHESIZE(ActObject*,m_Actobject,ActObject);
	CC_PROPERTY(WarAlive*,m_Alive,Alive);
	CC_PROPERTY(int,m_grid,grid);
	vector<int> grids;
private:
};

#endif