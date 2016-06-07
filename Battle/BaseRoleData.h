


/************************************************************* 
*
*
*		Data : 2016.5.27
*	
*		Name : TempData
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __TempData__
#define __TempData__

#include "cocos2d.h"

namespace protos{
	namespace common{
		class Hero;
		class Monster;
	}
}
namespace BattleSpace{
	class RoleSkill;
	class BaseRoleData : public cocos2d::CCObject
	{
	public:
		BaseRoleData();
		~BaseRoleData();		
		CC_SYNTHESIZE(int,mServerID,ServerID);							//��������ɫID
		CC_SYNTHESIZE(int,mRoleModel,RoleModel);						//ģ��id
		CC_SYNTHESIZE(int,mRoleType,RoleType);							//�佫����(��1,ˮ2,ľ3)
		CC_SYNTHESIZE(int,mRoleRow,RoleRow);							//�佫��ռ����
		CC_SYNTHESIZE(int,mRoleCol,RoleCol);							//�佫��ռ����
		CC_SYNTHESIZE(int,mRoleHp,RoleHp);								//Ѫ��
		CC_SYNTHESIZE(int,mRoleAttack,RoleAttack);						//����
		CC_SYNTHESIZE(int,mRoleDefense,RoleDefense);					//����
		CC_SYNTHESIZE(int,mRoleCrit,RoleCrit);							//����
		CC_SYNTHESIZE(int,mRoleHit,RoleHit);							//����
		CC_SYNTHESIZE(int,mRoleDodge,RoleDodge);						//����
		CC_SYNTHESIZE(int,mRoleAgility,RoleAgility);					//����
		CC_SYNTHESIZE(int,mRoleRegain,RoleRegain);						//�ָ�
		CC_SYNTHESIZE(int,mRoleZoom,RoleZoom);							//����
		CC_SYNTHESIZE(float,mMoveSpeed,MoveSpeed);						//�ƶ��ٶ�(��/S)
		CC_SYNTHESIZE(float,mAttackSpeed,AttackSpeed);					//�����ٶ�(��/��)
		CC_SYNTHESIZE(int,mCallType,CallType);							//�ٻ�����(Control)
		CC_SYNTHESIZE(int,mCallID,CallID);								//�ٻ����佫ID
		CC_SYNTHESIZE(float,mDelayTime,DelayTime);						//�����ӳ�ʱ��

		CC_SYNTHESIZE(int,mAlertType,AlertType);						//��������
		CC_SYNTHESIZE(int,mAlertRange,AlertRange);						//���䷶Χ
		CC_SYNTHESIZE(int,mColdDown,ColdDown);							//����CD
		CC_SYNTHESIZE(int,mInitCost,InitCost);							//����Я��cost			����ֱ�Ӽ������������Ҫ�ٵ���Я����
		CC_SYNTHESIZE(int,mExpendCost,ExpendCost);						//��������cost
		CC_SYNTHESIZE(int,mCostSpeed,CostSpeed);						//cost�ָ��ٶ�(1/s)
		CC_SYNTHESIZE(int,mMaxCost,MaxCost);							//cost����
		CC_SYNTHESIZE(bool,mCaptain,Captain);							//�佫�Ƿ�Ϊ����״̬(ս���б�Ƕӳ�)

		CC_SYNTHESIZE(int,mMonsterType,MonsterType);					//��������
		CC_SYNTHESIZE(int,mMaxHp,MaxHp);								//���Ѫ��ֵ(����bossר��
		CC_SYNTHESIZE(bool,mCallRole,CallRole);							//�Ƿ�Ϊ�ٻ����佫(�з����ﶼ����,�ڹ����б��ж�ȡ)
		CC_SYNTHESIZE(int,mInitGrid,InitGrid);							//�佫��ʼλ��
		CC_SYNTHESIZE(int,mRoleDrop,RoleDrop);							//������Ʒ����1����2����3����4������5�����
		CC_SYNTHESIZE(float,mCritTime,CritTime);						//�����ͷű�ɱ��ʱ��
	public:
		const RoleSkill* getNormalSkill()const;
		const RoleSkill* getSpecialSkill()const;
		const RoleSkill* getActiveSkill()const;
		const RoleSkill* getCaptainSkill()const;
		bool hasSpecialSkill()const;
		bool hasActiveSkill()const;
		bool hasCaptainSkill()const;
		int getActiveSkillType()const;
	protected:
		RoleSkill* mNormalSkill;										//��ͨ����
		RoleSkill* mSpecialSkill;										//���⹥��
		RoleSkill* mActiveSkill;										//��ɱ����(��ɱ�����ٻ��ļ�������)
		RoleSkill* mCaptainSkill;										//��˧���� 	
	};
};


#endif