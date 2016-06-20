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
		CC_SYNTHESIZE(int,mMonsterID,MonsterID);						//����ID
		CC_SYNTHESIZE(int,mBatchNumber,BatchNumber);					//����
		CC_SYNTHESIZE(bool,mBossMonster,BossMonster);					//�Ƿ�Ϊboss
		CC_SYNTHESIZE(bool,mMoveState,MoveState);						//�Ƿ�����ƶ�
		CC_SYNTHESIZE(int,mMaxHp,MaxHp);								//���Ѫ��ֵ(����bossר��
		CC_SYNTHESIZE(bool,mCallRole,HeroCall);							//�Ƿ�Ϊ�ٻ����佫(�з����ﶼ����,�ڹ����б��ж�ȡ)
		CC_SYNTHESIZE(int,mRoleDrop,RoleDrop);							//������Ʒ����1����2����3����4������5�����
		CC_SYNTHESIZE(float,mCritTime,CritTime);						//�����ͷű�ɱ��ʱ��
		bool currBatchRole(int pBatch);
	protected:
		void parseData(const google::protobuf::Message* pMessage);
		MonsterData(const google::protobuf::Message* pMessage = nullptr);
	};
};
#endif