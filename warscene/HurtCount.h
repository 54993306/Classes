#ifndef _HITCOUNT_H_
#define _HITCOUNT_H_
#include "AppUI.h"
/************************************************************************/
/* 
	用于做伤害计算、攻击闪避、暴击等相关战斗结果的处理数值运算等   //  [9/9/2014 xc_lin]                                                                 
*/
/************************************************************************/
#include "warscene/BattleResult.h"

namespace BattleSpace{
	class BaseRole;
	class WarManager;
	class BattleResult;
	class HurtCount : public cocos2d::CCObject
	{
	public:
		HurtCount();
		virtual ~HurtCount();
		virtual bool init();
		CREATE_FUNC(HurtCount);
		BattleResult* AttackExcute(BaseRole* target);
		void HurtExcute(BattleResult*Result,BaseRole*AtkAlive,BaseRole*HitAlive);
		void EffectTypeExcute(BattleResult*Result);								//
		STR_LostHp hitNum(BaseRole* AtcTarget , BaseRole* HitTarget);			//返回引用效率更高
		STR_LostHp lostCount(BaseRole* AtcTarget , BaseRole* HitTarget);		//攻击计算
		STR_LostHp gainCount(BaseRole* AtcTarget , BaseRole* HitTarget);		//加血计算
		bool hitJudge(BaseRole* AtcTarget , BaseRole* HitTarget);				//命中判断
		int  critJudge(BaseRole* AtcTarget , BaseRole* HitTarget);				//暴击判断
		float attributeHurt(BaseRole* AtcTarget);								//属性对技能伤害影响
		float raceDispose(BaseRole* AtcTarget , BaseRole* HitTarget);			//属性伤害计算
		int lostType(float race,int crit);
		void BuffHandleLogic(BaseRole* pAlive);								//对每一个效果进行BUF逻辑处理
		int ChangeLocation(BaseRole* AtcTarget , BaseRole* HitTarget);			//改变位置效果处理
		void addHittingAlive(BaseRole* AtcTarget , BaseRole* HitTarget);
		void woldBossHurt(BaseRole* pAlive,float pHurt);
		float getAllTargetLostHp(BattleResult* pResult);
	private:
		WarManager* m_Manage;
	};
	enum TatgetType
	{
		usTargetType = 1,	//自方目标或自己
		hitTargetType = 2,
	};
	enum HURTCOUNT_EffectType
	{
		Suckblood_Type=1,			//吸血类效果
		Boom_Type,					//自爆类效果(配pro_Rate)
		RateBlood_Type,				//扣血百分比型吸血效果
		NumBoold_Type,				//扣数值型吸血效果
		RateNowBoold_Type,			//扣当前血量百分比
	};
};
#endif