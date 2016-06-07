#ifndef _HITCOUNT_H_
#define _HITCOUNT_H_
#include "AppUI.h"
#include "warscene/BattleResult.h"
/************************************************************************/
/* 
	用于做伤害计算、攻击闪避、暴击等相关战斗结果的处理数值运算等   //  [9/9/2014 xc_lin]                                                                 
*/
/************************************************************************/
namespace BattleSpace{
	class WarAlive;
	class WarManager;
	class HurtCount : public cocos2d::CCObject
	{
	public:
		HurtCount();
		virtual ~HurtCount();
		virtual bool init();
		CREATE_FUNC(HurtCount);
		BattleResult* AttackExcute(WarAlive* target);
		void HurtExcute(BattleResult*Result,WarAlive*AtkAlive,WarAlive*HitAlive);
		void EffectTypeExcute(BattleResult*Result);								//
		STR_LostHp hitNum(WarAlive* AtcTarget , WarAlive* HitTarget);			//返回引用效率更高
		STR_LostHp lostCount(WarAlive* AtcTarget , WarAlive* HitTarget);		//攻击计算
		STR_LostHp gainCount(WarAlive* AtcTarget , WarAlive* HitTarget);		//加血计算
		bool hitJudge(WarAlive* AtcTarget , WarAlive* HitTarget);				//命中判断
		int  critJudge(WarAlive* AtcTarget , WarAlive* HitTarget);				//暴击判断
		float attributeHurt(WarAlive* AtcTarget);								//属性对技能伤害影响
		float raceDispose(WarAlive* AtcTarget , WarAlive* HitTarget);			//属性伤害计算
		int lostType(float race,int crit);
		void BuffHandleLogic(WarAlive* pAlive);								//对每一个效果进行BUF逻辑处理
		int ChangeLocation(WarAlive* AtcTarget , WarAlive* HitTarget);			//改变位置效果处理
		void addHittingAlive(WarAlive* AtcTarget , WarAlive* HitTarget);
		void woldBossHurt(WarAlive* pAlive,float pHurt);
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