/************************************************************* 
*
*
*		Data : 2016.6.6
*	
*		Name : MonsterData
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __MonsterData_H_
#define __MonsterData_H_
#include "Battle/RoleBaseData.h"
namespace BattleSpace{
	//怪物
	class MonsterData :public RoleBaseData
	{ 
	public:
		MonsterData();
		static MonsterData* create();
		CC_SYNTHESIZE(int,mMonsterID,MonsterID);						//怪物ID
		CC_SYNTHESIZE(int,mBatchNumber,BatchNumber);					//批次
		CC_SYNTHESIZE(bool,mBossMonster,BossMonster);					//是否为boss
		CC_SYNTHESIZE(bool,mMoveState,MoveState);						//是否可以移动
		void readData(const protos::common::Monster* monster);
	};
};
#endif