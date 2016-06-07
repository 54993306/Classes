


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
		CC_SYNTHESIZE(int,mServerID,ServerID);							//服务器角色ID
		CC_SYNTHESIZE(int,mRoleModel,RoleModel);						//模型id
		CC_SYNTHESIZE(int,mRoleType,RoleType);							//武将属性(火1,水2,木3)
		CC_SYNTHESIZE(int,mRoleRow,RoleRow);							//武将所占行数
		CC_SYNTHESIZE(int,mRoleCol,RoleCol);							//武将所占列数
		CC_SYNTHESIZE(int,mRoleHp,RoleHp);								//血量
		CC_SYNTHESIZE(int,mRoleAttack,RoleAttack);						//攻击
		CC_SYNTHESIZE(int,mRoleDefense,RoleDefense);					//防御
		CC_SYNTHESIZE(int,mRoleCrit,RoleCrit);							//暴击
		CC_SYNTHESIZE(int,mRoleHit,RoleHit);							//命中
		CC_SYNTHESIZE(int,mRoleDodge,RoleDodge);						//闪避
		CC_SYNTHESIZE(int,mRoleAgility,RoleAgility);					//敏捷
		CC_SYNTHESIZE(int,mRoleRegain,RoleRegain);						//恢复
		CC_SYNTHESIZE(int,mRoleZoom,RoleZoom);							//缩放
		CC_SYNTHESIZE(float,mMoveSpeed,MoveSpeed);						//移动速度(格/S)
		CC_SYNTHESIZE(float,mAttackSpeed,AttackSpeed);					//攻击速度(秒/次)
		CC_SYNTHESIZE(int,mCallType,CallType);							//召唤类型(Control)
		CC_SYNTHESIZE(int,mCallID,CallID);								//召唤的武将ID
		CC_SYNTHESIZE(float,mDelayTime,DelayTime);						//出现延迟时间

		CC_SYNTHESIZE(int,mAlertType,AlertType);						//警戒类型
		CC_SYNTHESIZE(int,mAlertRange,AlertRange);						//警戒范围
		CC_SYNTHESIZE(int,mColdDown,ColdDown);							//上阵CD
		CC_SYNTHESIZE(int,mInitCost,InitCost);							//上阵携带cost			（可直接计算出来，不需要再单独携带）
		CC_SYNTHESIZE(int,mExpendCost,ExpendCost);						//上阵消耗cost
		CC_SYNTHESIZE(int,mCostSpeed,CostSpeed);						//cost恢复速度(1/s)
		CC_SYNTHESIZE(int,mMaxCost,MaxCost);							//cost上限
		CC_SYNTHESIZE(bool,mCaptain,Captain);							//武将是否为上阵状态(战斗中标记队长)

		CC_SYNTHESIZE(int,mMonsterType,MonsterType);					//怪物类型
		CC_SYNTHESIZE(int,mMaxHp,MaxHp);								//最大血量值(世界boss专用
		CC_SYNTHESIZE(bool,mCallRole,CallRole);							//是否为召唤类武将(敌方怪物都可能,在怪物列表中读取)
		CC_SYNTHESIZE(int,mInitGrid,InitGrid);							//武将初始位置
		CC_SYNTHESIZE(int,mRoleDrop,RoleDrop);							//掉落物品类型1、蓝2、金3、彩4、道具5、金币
		CC_SYNTHESIZE(float,mCritTime,CritTime);						//怪物释放必杀技时间
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
		RoleSkill* mNormalSkill;										//普通攻击
		RoleSkill* mSpecialSkill;										//特殊攻击
		RoleSkill* mActiveSkill;										//必杀技能(必杀技有召唤的技能类型)
		RoleSkill* mCaptainSkill;										//主帅技能 	
	};
};


#endif