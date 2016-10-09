


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
#include "Battle/BattleMacro.h"
#include "Battle/RoleMacro.h"
namespace protos{
	namespace common{
		class Hero;
		class Monster;
	}
}
namespace BattleSpace
{
	class RoleSkill;
	class BaseRoleData : public cocos2d::CCObject
	{
	public:
		BaseRoleData();
		~BaseRoleData();		
		CC_SYNTHESIZE(int,mServerID,ServerID);							//��������ɫID
		CC_SYNTHESIZE(int,mRoleModel,RoleModel);						//ģ��id
		CC_SYNTHESIZE(sRoleNature,mProperty,Property);					//�佫����(��1,ˮ2,ľ3)
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
		CC_SYNTHESIZE(sCallType,mCallType,CallType);					//�ٻ�����(Control)
		CC_SYNTHESIZE(float,mDelayTime,DelayTime);						//�����ӳ�ʱ��
		CC_SYNTHESIZE(sRoleCamp,mRoleNature,RoleNature);				//��ɫ��Ӫ
		CC_SYNTHESIZE(sMoveType,mMoveType,MoveType);					//�ƶ�����
		CC_SYNTHESIZE(bool,mOtherCamp,OtherCamp);						//������Ӫ�佫(�������Ӫ)
		bool isProperty(sRoleNature pType);
		float judgeAttribute( sRoleNature pType );
	public:
		//hero
		CC_SYNTHESIZE(int,mAlertType,AlertType);						//��������
		CC_SYNTHESIZE(int,mAlertRange,AlertRange);						//���䷶Χ
		CC_SYNTHESIZE(int,mColdDown,ColdDown);							//����CD
		CC_SYNTHESIZE(int,mInitCost,InitCost);							//����Я��cost			����ֱ�Ӽ������������Ҫ�ٵ���Я����
		CC_SYNTHESIZE(int,mExpendCost,ExpendCost);						//��������cost
		CC_SYNTHESIZE(float,mCostSpeed,CostSpeed);						//cost�ָ��ٶ�(1/s)
		CC_SYNTHESIZE(int,mMaxCost,MaxCost);							//cost����
		CC_SYNTHESIZE(bool,mCaptain,Captain);							//�佫�Ƿ�Ϊ����״̬(ս���б�Ƕӳ�)
	public:
		//monster
		CC_SYNTHESIZE(sBehavior,mBehavior,Behavior);						//��Ϊ����
		CC_SYNTHESIZE(int,mMaxHp,MaxHp);								//���Ѫ��ֵ(����bossר��
		CC_SYNTHESIZE(int,mInitGrid,InitGrid);							//�佫��ʼλ��
		CC_SYNTHESIZE(int,mRoleDrop,RoleDrop);							//������Ʒ����1����2����3����4������5�����
		CC_SYNTHESIZE(float,mCritTime,CritTime);						//�����ͷű�ɱ��ʱ��
	public:
		RoleSkill* getNormalSkill()const;
		RoleSkill* getSpecialSkill()const;
		RoleSkill* getActiveSkill()const;
		RoleSkill* getCaptainSkill()const;
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