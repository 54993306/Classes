
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
namespace BattleSpace
{
	//������������
	enum GuardType
	{
		eFrontGuard =1,					//ǰ����Χ
		eBackGuard,						//�󷽷�Χ
		eFrontAndBackGuard,				//ǰ��Χ
		eUpDownGuard,					//��������Ϊ���·�Χ
	};
	class BaseRole;
	class WarManager;
	class GuardArea : public cocos2d::CCObject
	{
	public:
		GuardArea();
		static GuardArea* create();
		int getAliveGuard(BaseRole* pAlive);	
		void initAliveGuard(BaseRole* pAlive,std::vector<int>& pGuards);									//
	protected:
		bool hasTarget(BaseRole* pRole,int pGrid);
		void initAliveCurrGrids(BaseRole* pAlive,std::vector<int>& pVector);
		void guradUpAndDown(BaseRole* pAlive,std::vector<int>& pGuards);
		void guardFront(BaseRole* pAlive,std::vector<int>& pGuards);
		void guradBack(BaseRole* pAlive,std::vector<int>& pGuards);
		void guradFrontAndBack(BaseRole* pAlive,std::vector<int>& pGuards);
	};
};
#endif