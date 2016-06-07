#ifndef _MOVEDISPOSE_
#define _MOVEDISPOSE_

#include "AppUI.h"
/************************************************************* 
 *
 *
 *		Data : 2016.4.19
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 移动策略
 *
 *
 *************************************************************/
namespace BattleSpace{
	class WarAlive;
	class MoveRule : public CCObject
	{
	public:
		MoveRule();
		~MoveRule();
		virtual bool init();
		CREATE_FUNC(MoveRule);
		void changeTestState(CCObject* ob);
		int getMonsterMoveGrid(WarAlive* alive);

		int getMoveGrid(WarAlive* alive,int grid);
		int MoveJudge(WarAlive* alive,int grid);				//用于判断是否可以移动，多格站位处理
		int FrontBack(WarAlive* alive,int range,bool enemy);
		int monsterMove(WarAlive* target);
		bool MonstMoveExcute(WarAlive* monster);
		int getCurrRandomGrid(int grid,bool hasAlive = false);
	private:
		bool m_testState;
	};
};

#endif