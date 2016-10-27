#ifndef _HITCOUNT_H_
#define _HITCOUNT_H_
/************************************************************* 
 *
 *
 *		Data : 2016.6.14
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "cocos2d.h"
#include "Battle/BattleResult.h"
namespace BattleSpace
{
	class BaseRole;
	class WarManager;
	class BattleResult;
	class skEffectData;
	enum struct PlayHpType;
	class HurtCount : public cocos2d::CCObject
	{
	public:
		HurtCount();
		virtual bool init(){return true;};
		CREATE_FUNC(HurtCount);
		BattleResult* AttackExcute(BaseRole* target);
		BattleResult* SpineEffectAttack(BaseRole*pAtkRole,BaseRole*pHitRole,skEffectData* pSkillEffect);
	protected:
		void HurtExcute(BattleResult*Result,BaseRole*AtkAlive,BaseRole*HitAlive);
		void EffectTypeExcute(BattleResult*Result);								//
		STR_LostHp hitNum(BaseRole* AtcTarget,BaseRole* HitTarget);			//返回引用效率更高
		STR_LostHp lostCount(BaseRole* AtcTarget , BaseRole* HitTarget);		//攻击计算
		STR_LostHp gainCount(BaseRole* AtcTarget , BaseRole* HitTarget);		//加血计算
		bool hitJudge(BaseRole* AtcTarget , BaseRole* HitTarget);				//命中判断
		int  critJudge(BaseRole* AtcTarget , BaseRole* HitTarget);				//暴击判断
		float attributeHurt(BaseRole* AtcTarget);								//属性对技能伤害影响
		float raceDispose(BaseRole* AtcTarget , BaseRole* HitTarget);			//属性伤害计算
		PlayHpType lostType(float race,int crit);
		int ChangeLocation(BaseRole* AtcTarget,BaseRole* HitTarget);			//改变位置效果处理
		void addHittingAlive(BaseRole* AtcTarget , BaseRole* HitTarget);
		void woldBossHurt(BaseRole* pAlive,float pHurt);
		float getAllTargetLostHp(BattleResult* pResult);
	private:
		skEffectData* mSkillEffect;
	};

	enum TatgetType
	{
		usTargetType = 1,	//自方目标或自己
		hitTargetType = 2,
	};
};
#endif