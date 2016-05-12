#ifndef _MOVEDISPOSE_
#define _MOVEDISPOSE_

#include "AppUI.h"
class WarAlive;
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
class MoveRule : public CCObject
{
public:
	MoveRule();
	~MoveRule();
	virtual bool init();
	CREATE_FUNC(MoveRule);
	void changeTestState(CCObject* ob);
	bool HeroMoveDispose(WarAlive* alive,int grid );
	int One_FourTypeDispose(WarAlive* alive,int grid,bool typeOne = true);//移动类型1And4处理
	int Two_FiveTypeDispose(WarAlive* alive,int grid,bool typeTwo = true);//移动类型2And5处理
	int ThreeTypeDispose(WarAlive* alive,int grid);
	int getMoveGrid(WarAlive* alive,int grid);
	int MoveJudge(WarAlive* alive,int grid);				//用于判断是否可以移动，多格站位处理
	bool TouchJudge(WarAlive* alive,int grid);
	int FrontBack(WarAlive* alive,int range,bool enemy);
	int MonstMoveAreaDispose(WarAlive* target);
	bool MonstMoveExcute(WarAlive* monster);
	int getCurrRandomGrid(int grid,bool hasAlive = false);
private:
	bool m_testState;
};
#endif