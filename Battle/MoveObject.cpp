
#include "Battle/MoveObject.h"
#include "Battle/BattleRole.h"
#include "model/DataCenter.h"
#include "model/MapManager.h"
#include "warscene/ConstNum.h"
#include "scene/alive/ActObject.h"
#include "Battle/RoleBaseData.h"
namespace BattleSpace{
	MoveObject::MoveObject()
		:m_grid(0),m_Alive(nullptr),m_Row(1),m_Col(1),m_OffsX(0),m_OffsY(0)
	{}
	//这个方法应该由持有移动对象的显示对象来调用
	void MoveObject::setgrid(int var)
	{
		if (var == m_grid)
			return;
		m_grid = var;
		grids.clear();
		for (int j =0;j<m_Col;j++)								//以这样的顺序得出来的数序列的排序好的
			for (int i=0;i<m_Row;i++)
				grids.push_back(m_grid+j*C_GRID_ROW+i);
		sort(grids.begin(),grids.end());
		WarMapData* mapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
		setPosition(ccpAdd(mapData->getPoint(m_grid),ccp(m_OffsX,m_OffsY)));
	}
	int MoveObject::getgrid(){return m_grid;}

	void MoveObject::initMoveSprite()
	{
		CCSprite* ef = nullptr;
		for (int i=0;i<m_Row;i++)
			for (int j =0;j<m_Col;j++)
			{
				ef = CCSprite::create("warScene/fanglandingwei.png");
				ef->setPosition(ccp(-m_OffsX+(j*(GRID_WIDTH+C_GRIDOFFSET_X)),-m_OffsY-(i*(GRID_HEIGHT+C_GRIDOFFSET_Y))));
				addChild(ef);
			}
	}

	void MoveObject::setRowCol( int pRow,int pCol )
	{
		m_Row = pRow;
		m_Col = pCol;
	}

	void MoveObject::setOffs( float pOffsX,float pOffsY )
	{
		m_OffsX = pOffsX;
		m_OffsY = pOffsY;
	}
}