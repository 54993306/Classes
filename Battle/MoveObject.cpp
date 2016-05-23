
#include "Battle/MoveObject.h"
#include "model/DataDefine.h"
#include "model/DataCenter.h"
#include "model/MapManager.h"
#include "warscene/ConstNum.h"
#include "scene/alive/ActObject.h"
MoveObject::MoveObject():m_grid(0),m_Actobject(nullptr),m_Alive(nullptr){}

void MoveObject::setAlive(WarAlive* var)
{
	if (!m_Alive)
	{
		m_Alive = var;
	}else{CCLOG(" [ TIPS :MoveObj::setAlive SetAlive Repeat]");}
	if (m_Alive->getCallType() != CommonType)
		this->setVisible(false);
}

WarAlive* MoveObject::getAlive(){return m_Alive;}

void MoveObject::setgrid(int var)
{
	if (var == m_grid)
		return;
	grids.clear();
	for (int i=0;i<m_Alive->role->row;i++)
		for (int j =0;j<m_Alive->role->col;j++)
			grids.push_back(var+j*C_GRID_ROW+i);
	sort(grids.begin(),grids.end());
	m_grid = grids.at(0);
	WarMapData* Warmap = DataCenter::sharedData()->getMap()->getCurrWarMap();
	CCPoint p = Warmap->getPoint(m_grid);
	m_Actobject->getAlive()->setMoveGrid(m_grid);
	setPosition(p+m_Actobject->getoffs());	
}
int MoveObject::getgrid(){return m_grid;}

void MoveObject::initMoveSprite()
{
	CCSprite* ef = nullptr;
	for (int i=0;i<m_Alive->role->row;i++)
		for (int j =0;j<m_Alive->role->col;j++)
		{
			ef = CCSprite::create("warScene/fanglandingwei.png");
			ef->setPosition(ccp(-m_Actobject->getoffs().x+(j*(GRID_WIDTH+C_GRIDOFFSET_X)),-m_Actobject->getoffs().y-(i*(GRID_HEIGHT+C_GRIDOFFSET_Y))));
			addChild(ef);
		}
}