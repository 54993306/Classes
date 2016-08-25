/************************************************************* 
 *
 *
 *		Data : 2016.8.9
 *	
 *		Name : AreaManage
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 回答格子是否能移动的问题给出具体的移动路径
 *
 *
*************************************************************/

#ifndef __AreaManage_H_
#define __AreaManage_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class AStar;
	class BaseRole;
	class AreaManage : public CCObject
	{
	public:
		AreaManage();
		virtual ~AreaManage();
		virtual bool init();
		CREATE_FUNC(AreaManage);
	public:
		bool roleThrough(BaseRole* pRole,int pGrid);
		void initRoleObstacle(BaseRole* pRole);
		void initRoleMovePath(BaseRole* pRole,int pGrid);
	private:
		AStar *mAStar;
	};
};
#endif