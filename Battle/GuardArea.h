
/************************************************************* 
 *
 *
 *		Data : 2016.6.1
 *	
 *		Name : GuardArea
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __GuardArea_H_
#define __GuardArea_H_
#include "cocos2d.h"
#include <vector>
namespace BattleSpace{
	//警戒区域类型
	enum GuardType
	{
		eFrontGuard =1,					//前方范围
		eBackGuard,						//后方范围
		eFrontAndBackGuard,				//前后范围
		eUpDownGuard,					//警戒区域为上下范围
	};
	class WarAlive;
	class WarManager;
	class GuardArea : public cocos2d::CCObject
	{
	public:
		GuardArea(WarManager* pManage);
		static GuardArea* create(WarManager* pManage);
		int getAliveGuard(WarAlive* pAlive);	
		void initAliveGuard(WarAlive* pAlive,std::vector<int>& pGuards);									//
	protected:
		void initAliveCurrGrids(WarAlive* pAlive,std::vector<int>& pVector);
		void guradUpAndDown(WarAlive* pAlive,std::vector<int>& pGuards);
		void guardFront(WarAlive* pAlive,std::vector<int>& pGuards);
		void guradBack(WarAlive* pAlive,std::vector<int>& pGuards);
		void guradFrontAndBack(WarAlive* pAlive,std::vector<int>& pGuards);
	private:
		WarManager* mManage;
	};
};
#endif