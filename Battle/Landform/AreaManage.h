/************************************************************* 
 *
 *
 *		Data : 2016.8.9
 *	
 *		Name : AreaManage
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
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
	class AreaManage : public CCObject
	{
	public:
		AreaManage();
		virtual bool init();
		virtual ~AreaManage();
		CREATE_FUNC(AreaManage);
		//管理一个障碍列表，当有武将的操作时，更新障碍列表

	private:
		AStar *mAStar;
		vector<int> mVecObstacle;

	};
};
#endif