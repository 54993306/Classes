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
#include "Battle/Landform//AreaManage.h"
#include "Battle/Landform/AStar.h"

namespace BattleSpace
{
	AreaManage::AreaManage():mAStar(nullptr)
	{

	}

	AreaManage::~AreaManage()
	{

	}

	bool AreaManage::init()
	{
		mAStar = AStar::create();
		return true;
	}

}