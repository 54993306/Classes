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
		//����һ���ϰ��б������佫�Ĳ���ʱ�������ϰ��б�

	private:
		AStar *mAStar;
		vector<int> mVecObstacle;

	};
};
#endif