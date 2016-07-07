

#ifndef __SkillEffect__
#define __SkillEffect__
/************************************************************* 
*
*
*		Data : 2016.5.26
*	
*		Name : SkillEffect
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/
namespace protos{
	namespace common{
		class Buff;
		class Effect;
	}
}
#include "cocos2d.h"
namespace BattleSpace{
	enum struct sAttribute
	{
		eNull						= 0,
		eAttack						= 1,//攻击
		eDefense					= 2,//防御
		eBlood						= 3,//血量上限 
		eHit						= 4,//命中
		eCrit						= 5,//暴击
		DefRate						= 6,//防禦=6*
		AtkRate						= 7,//攻擊=7*
		NedHpRate					= 8,//血量=8*
		AgilityRate					= 9,//攻速=9*
		RenewRate					= 10,//回復=10*
		DogeRate					= 11,//闪避=11
		HitRate						= 12,//命中=12
		CritRate					= 13,//暴擊=13
		HpMaxRate					= 14,//血量上限=14*
	};
	enum eEffectCountType
	{
		eNull						= 0,
		eSuckblood					= 1,//吸血类效果
		eBoom						= 2,//自爆类效果(配pro_Rate)
		eRateBlood					= 3,//扣血百分比型吸血效果
		eNumberBoold				= 4,//扣数值型吸血效果
		eCurrBooldRate				= 5,//扣当前血量百分比
	};
	class BaseRole;
	class BuffData;
	class AffectArea;
	class AreaCountInfo;
	class skEffectData : public cocos2d::CCObject
	{
	public:
		virtual ~skEffectData();
		static skEffectData* create();
		CC_SYNTHESIZE(int,mEffectID,EffectID);
		CC_SYNTHESIZE(int,mGroup,Group);						//效果所属效果组
		CC_SYNTHESIZE(int,mGroupPos,GroupPos);					//效果组内位置
		CC_SYNTHESIZE(eEffectCountType,mCountType,CountType);	//效果类型
		CC_SYNTHESIZE(int,mUserRate,userRate);					//触发概率
		CC_SYNTHESIZE(int,mChangeCost,ChangeCost);				//怒气值影响(可增减)
		CC_SYNTHESIZE(float,mDamageRate,DamageRate);			//伤害百分比
		CC_SYNTHESIZE(int,mRealHurt,RealHurt);					//真实伤害
		CC_SYNTHESIZE(int,mTargetType,TargetType);				//效果目标类型(1:友方  2:敌方  3:敌我双方)
		CC_SYNTHESIZE(int,mCallRoleID,CallRoleID);				//被召唤武将ID
		CC_SYNTHESIZE(int,mBatter,Batter);						//连击数(怪物配置一次召唤多少怪物时使用)
		CC_SYNTHESIZE(int,mRepel,Repel);						//受击移动距离(受击对象移动距离，分敌我双方)
		CC_SYNTHESIZE(float,mHurtRatio,HurtRatio);				//伤害浮动值(百分比)
		CC_SYNTHESIZE(int,mAreaType,AreaType);					//范围类型(1 直线群体...27 随机固定武将区域)
		CC_SYNTHESIZE(int,mSpaceRange,SpaceRange);				//范围距离行数
		CC_SYNTHESIZE(int,mAreaSize,AreaSize);					//范围大小
		CC_SYNTHESIZE(sAttribute,mImpactType,ImpactType);		//属性影响类型(召唤数量限制)
		CC_SYNTHESIZE(int,mCallNumber,CallNumber);
		CC_SYNTHESIZE(int,mImpactNumber,ImpactNumber);
		CC_SYNTHESIZE(int,mImpactRate,ImpactRate);				//属性影响比率
	public:
		void clearBuffData();
		void initAffect(int pType);
		void readData(const protos::common::Effect* pEffect);
		void initArea(AreaCountInfo& pInfo) ;
		const std::vector<BuffData*>& getBuffVector()const;
	private:
		skEffectData();											//详细参见datadefine Bufinfo
		void addBuffData(const protos::common::Buff* pBuffData);
		std::vector<BuffData*> mBuffVector;
		AffectArea* mAffectArea;
	};

	enum E_TargetType
	{
		eUsType	= 1,			//我方目标
		eEnemyType,				//敌方目标
		eAllType,				//双方目标
	};
	bool EffectSort(const skEffectData* Effect1,const skEffectData* Effect2);//{return Effect1.pos>Effect2.pos;}
};
#endif