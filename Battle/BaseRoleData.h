


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
#include "Battle/RoleMacro.h"
using namespace cocos2d;
using namespace std;
namespace google{
	namespace protobuf{
		class Message;
	}
}

namespace BattleSpace
{
	enum struct sDataType					//��������
	{
		eBase						= 0,	//��������
		eHero						= 1,	//Ӣ�����ɫ
		eMonster					= 2,	//�������ɫ
	};
	class RoleSkill;
	class BaseRoleData : public cocos2d::CCObject
	{
	public:
		BaseRoleData();
		virtual ~BaseRoleData();		
		static BaseRoleData* create(sDataType pType,const google::protobuf::Message* pMessage);
		CC_SYNTHESIZE(int,mServerID,ServerID);											//��������ɫID
		CC_SYNTHESIZE(int,mRoleModel,RoleModel);										//ģ��id
		CC_SYNTHESIZE(sAttribute,mAttribute,Attribute);									//�佫����(��1,ˮ2,ľ3)
		CC_SYNTHESIZE(int,mRoleRow,RoleRow);											//�佫��ռ����
		CC_SYNTHESIZE(int,mRoleCol,RoleCol);											//�佫��ռ����
		CC_SYNTHESIZE(int,mRoleHp,RoleHp);												//Ѫ��
		CC_SYNTHESIZE(int,mRoleAttack,RoleAttack);										//����
		CC_SYNTHESIZE(int,mRoleDefense,RoleDefense);									//����
		CC_SYNTHESIZE(int,mRoleCrit,RoleCrit);											//����
		CC_SYNTHESIZE(int,mRoleHit,RoleHit);											//����
		CC_SYNTHESIZE(int,mRoleDodge,RoleDodge);										//����
		CC_SYNTHESIZE(int,mRoleAgility,RoleAgility);									//����
		CC_SYNTHESIZE(int,mRoleRegain,RoleRegain);										//�ָ�
		CC_SYNTHESIZE(int,mRoleZoom,RoleZoom);											//����
		CC_SYNTHESIZE(float,mMoveSpeed,MoveSpeed);										//�ƶ��ٶ�(��/S)
		CC_SYNTHESIZE(float,mAttackSpeed,AttackSpeed);									//�����ٶ�(��/��)
		CC_SYNTHESIZE(sCallType,mCallRoleType,CallRoleType);							//�ٻ��Ľ�ɫ����(˫��������ʹ��Ŀǰ���ҷ��佫ʹ��)
		CC_SYNTHESIZE(int,mCallID,CallID);												//�ٻ����佫ID
		CC_SYNTHESIZE(float,mDelayTime,DelayTime);										//�����ӳ�ʱ��
		CC_SYNTHESIZE(int,mInitGrid,InitGrid);											//�佫��ʼλ��
		CC_SYNTHESIZE(sBehavior,mBehavior,Behavior);									//��Ϊ����
		virtual sDataType getDataType();
		virtual void copySelf(BaseRoleData* pData);
	public:
		bool isAttribute(sAttribute pType);
		float judgeAttribute(sAttribute pType);
		RoleSkill* getNormalSkill()const;
		RoleSkill* getSpecialSkill()const;
		RoleSkill* getActiveSkill()const;
		RoleSkill* getCaptainSkill()const;
		bool hasSpecialSkill()const;
		bool hasActiveSkill()const; 
		bool hasCaptainSkill()const;
		int getActiveSkillType()const;
	protected:
		virtual void parseData(const google::protobuf::Message* pMessage){};
		RoleSkill* mNormalSkill;														//��ͨ����
		RoleSkill* mSpecialSkill;														//���⹥��
		RoleSkill* mActiveSkill;														//��ɱ����(��ɱ�����ٻ��ļ�������)
		RoleSkill* mCaptainSkill;														//��˧���� 	
	};
};
#endif