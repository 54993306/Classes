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
#include "Battle/BaseRoleData.h"
namespace BattleSpace
{
	class MonsterData :public BaseRoleData
	{ 
	public:
		virtual ~MonsterData(){};
		static MonsterData* create(const google::protobuf::Message* pMessage = nullptr);
		virtual sDataType getDataType();
		CC_SYNTHESIZE(int,mMonsterID,MonsterID);						//怪物ID
		CC_SYNTHESIZE(int,mBatchNumber,BatchNumber);					//批次
		CC_SYNTHESIZE(bool,mBossMonster,BossMonster);					//是否为boss
		CC_SYNTHESIZE(bool,mMoveState,MoveState);						//是否可以移动
		CC_SYNTHESIZE(int,mMaxHp,MaxHp);								//最大血量值(世界boss专用
		CC_SYNTHESIZE(bool,mCallRole,HeroCall);							//是否为召唤类武将(敌方怪物都可能,在怪物列表中读取)
		CC_SYNTHESIZE(int,mRoleDrop,RoleDrop);							//掉落物品类型1、蓝2、金3、彩4、道具5、金币
		CC_SYNTHESIZE(float,mCritTime,CritTime);						//怪物释放必杀技时间
		bool currBatchRole(int pBatch);
	protected:
		void parseData(const google::protobuf::Message* pMessage);
		MonsterData(const google::protobuf::Message* pMessage = nullptr);
	};
};
#endif