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
	//����
	class MonsterData :public RoleBaseData
	{ 
	public:
		MonsterData();
		static MonsterData* create();
		CC_SYNTHESIZE(int,mMonsterID,MonsterID);						//����ID
		CC_SYNTHESIZE(int,mBatchNumber,BatchNumber);					//����
		CC_SYNTHESIZE(bool,mBossMonster,BossMonster);					//�Ƿ�Ϊboss
		CC_SYNTHESIZE(bool,mMoveState,MoveState);						//�Ƿ�����ƶ�
		void readData(const protos::common::Monster* monster);
	};
};
#endif