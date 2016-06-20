


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
	enum struct sDataType					//数据类型
	{
		eBase						= 0,	//基础数据
		eHero						= 1,	//英雄类角色
		eMonster					= 2,	//怪物类角色
	};
	class RoleSkill;
	class BaseRoleData : public cocos2d::CCObject
	{
	public:
		BaseRoleData();
		virtual ~BaseRoleData();		
		static BaseRoleData* create(sDataType pType,const google::protobuf::Message* pMessage);
		CC_SYNTHESIZE(int,mServerID,ServerID);											//服务器角色ID
		CC_SYNTHESIZE(int,mRoleModel,RoleModel);										//模型id
		CC_SYNTHESIZE(sAttribute,mAttribute,Attribute);									//武将属性(火1,水2,木3)
		CC_SYNTHESIZE(int,mRoleRow,RoleRow);											//武将所占行数
		CC_SYNTHESIZE(int,mRoleCol,RoleCol);											//武将所占列数
		CC_SYNTHESIZE(int,mRoleHp,RoleHp);												//血量
		CC_SYNTHESIZE(int,mRoleAttack,RoleAttack);										//攻击
		CC_SYNTHESIZE(int,mRoleDefense,RoleDefense);									//防御
		CC_SYNTHESIZE(int,mRoleCrit,RoleCrit);											//暴击
		CC_SYNTHESIZE(int,mRoleHit,RoleHit);											//命中
		CC_SYNTHESIZE(int,mRoleDodge,RoleDodge);										//闪避
		CC_SYNTHESIZE(int,mRoleAgility,RoleAgility);									//敏捷
		CC_SYNTHESIZE(int,mRoleRegain,RoleRegain);										//恢复
		CC_SYNTHESIZE(int,mRoleZoom,RoleZoom);											//缩放
		CC_SYNTHESIZE(float,mMoveSpeed,MoveSpeed);										//移动速度(格/S)
		CC_SYNTHESIZE(float,mAttackSpeed,AttackSpeed);									//攻击速度(秒/次)
		CC_SYNTHESIZE(sCallType,mCallRoleType,CallRoleType);							//召唤的角色类型(双方都可以使用目前仅我方武将使用)
		CC_SYNTHESIZE(int,mCallID,CallID);												//召唤的武将ID
		CC_SYNTHESIZE(float,mDelayTime,DelayTime);										//出现延迟时间
		CC_SYNTHESIZE(int,mInitGrid,InitGrid);											//武将初始位置
		CC_SYNTHESIZE(sBehavior,mBehavior,Behavior);									//行为类型
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
		RoleSkill* mNormalSkill;														//普通攻击
		RoleSkill* mSpecialSkill;														//特殊攻击
		RoleSkill* mActiveSkill;														//必杀技能(必杀技有召唤的技能类型)
		RoleSkill* mCaptainSkill;														//主帅技能 	
	};
};
#endif