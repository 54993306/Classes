

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
	class BuffData;
	class skEffectData : public cocos2d::CCObject
	{
	public:
		skEffectData();									//详细参见datadefine Bufinfo
		~skEffectData();
		virtual bool init(){return true;}
		CREATE_FUNC(skEffectData);
		CC_SYNTHESIZE(int,mEffectID,EffectID);
		CC_SYNTHESIZE(int,mGroup,Group);				//效果所属效果组
		CC_SYNTHESIZE(int,mGroupPos,GroupPos);			//效果组内位置
		CC_SYNTHESIZE(int,mEffectType,EffectType);		//效果类型
		CC_SYNTHESIZE(int,mUserRate,userRate);			//触发概率
		CC_SYNTHESIZE(int,mChangeCost,ChangeCost);		//怒气值影响(可增减)
		CC_SYNTHESIZE(float,mDamageRate,DamageRate);	//伤害百分比
		CC_SYNTHESIZE(int,mRealHurt,RealHurt);			//真实伤害
		CC_SYNTHESIZE(int,mTargetType,TargetType);		//效果目标类型(1:友方  2:敌方  3:敌我双方)
		CC_SYNTHESIZE(int,mBatter,Batter);				//连击数(怪物配置一次召唤多少怪物时使用)
		CC_SYNTHESIZE(int,mRepel,Repel);				//受击移动距离(受击对象移动距离，分敌我双方)
		CC_SYNTHESIZE(float,mHurtRatio,HurtRatio);		//伤害浮动值(百分比)
		CC_SYNTHESIZE(int,mAreaType,AreaType);			//范围类型(1 直线群体...27 随机固定武将区域)
		CC_SYNTHESIZE(int,mSpaceRange,SpaceRange);		//范围距离行数
		CC_SYNTHESIZE(int,mAreaSize,AreaSize);			//范围大小
		CC_SYNTHESIZE(int,mImpactType,ImpactType);		//属性影响类型(召唤数量限制)
		CC_SYNTHESIZE(int,mImpactRate,ImpactRate);		//属性影响比率(加血吸血使用)
	public:
		void readData(const protos::common::Effect* pEffect);
		void clearBuffData();
		const std::vector<BuffData*>& getBuffVector()const;
	private:
		void addBuffData(const protos::common::Buff* pBuffData);
		std::vector<BuffData*> mBuffVector;
	};

	bool EffectSort(const skEffectData* Effect1,const skEffectData* Effect2);//{return Effect1.pos>Effect2.pos;}
};
#endif