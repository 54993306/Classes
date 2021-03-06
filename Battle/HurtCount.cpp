﻿/************************************************************* 
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
#include "HurtCount.h"
#include "Battle/ConstNum.h"
#include "MoveRule.h"
#include "Battle/BaseRoleData.h"
#include "Battle/BuffManage.h"
#include "Battle/BaseRole.h"
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "Battle/BattleResult.h"
#include "common/CommonFunction.h"
#include "Battle/RoleObject/HPObject.h"
#include "Battle/BuffData.h"
#include "Battle/skEffectData.h"
#include "Battle/BattleTools.h"
#include "Battle/RoleObjectMacro.h"
namespace BattleSpace
{
	HurtCount::HurtCount():mSkillEffect(nullptr)
	{}

	BattleResult* HurtCount::SpineEffectAttack( BaseRole*pAtkRole,BaseRole*pHitRole,skEffectData* pSkillEffect )
	{
		BattleResult* Result = BattleResult::create();
		Result->setAlive(pAtkRole);
		mSkillEffect = pSkillEffect;
		HurtExcute(Result,pAtkRole,pHitRole);
		EffectTypeExcute(Result);						//用于计算我方受伤信息
		mSkillEffect = nullptr;
		return Result;
	}

	// << 血量变化 >> 、 << 怒气值变化 >> 、<< 打击位移效果 >> 处理,目标为受击数组
	BattleResult* HurtCount::AttackExcute(BaseRole* pRole)
	{
		BattleResult* Result = BattleResult::create();
		Result->setAlive(pRole);
		vector<BaseRole*>::iterator iter = pRole->mAreaTargets.begin();
		mSkillEffect = pRole->getCurrEffect();
		for (;iter != pRole->mAreaTargets.end();iter ++)
		{
			HurtExcute(Result,pRole,*iter);
		}
		EffectTypeExcute(Result);						//用于计算我方受伤信息
		mSkillEffect = nullptr;
		return Result;
	}
	//多人打击的情况有返回值才能确定具体移动
	int HurtCount::ChangeLocation(BaseRole* AtcAlive , BaseRole* HitAlive)
	{
		if (HitAlive->getCallType() == sCallType::eFixedlyCantFly 
			|| HitAlive->getCallType() == sCallType::eNotAttack||HitAlive->getCaptain()
			|| HitAlive->getMonsterSpecies() == sMonsterSpecies::eWorldBoss)//不可被击飞类型武将
			return HitAlive->getGridIndex();
		if (mSkillEffect->getBatter() != AtcAlive->getSortieNum() || !mSkillEffect->getRepel())
			return HitAlive->getGridIndex();									//连击的最后一次才做击退
		bool tEnemy = AtcAlive->getOtherCamp();
		if (AtcAlive->getOpposite())
			tEnemy = !tEnemy;
		int grid = MoveRule::create()->FrontBack(HitAlive,mSkillEffect->getRepel(),tEnemy);
		if (grid != INVALID_GRID)
		{
			if (!BattleManage->getNormal())
			{
				if (grid >= C_PVEStopGrid)
					return grid;
			}else{
				if ( grid >= C_BEGINGRID )						//击退范围做一个边界处理
					return grid;								//精英关卡的击退距离要做一个处理，因为精英关卡是不能够移动。
			}
		}		
		return HitAlive->getGridIndex();
	}
	//连击的时候武将的攻击掉血帧数如果不够则无法计算完全部伤害,无法处理击退效果
	void HurtCount::HurtExcute(BattleResult*Result,BaseRole*AtkAlive,BaseRole*HitAlive)
	{
		unsigned int hitID = HitAlive->getAliveID();
		Result->m_HitTargets.push_back(hitID);
		Result->m_Alive_s[hitID] = HitAlive;
		Result->m_LostHp[hitID] = hitNum(AtkAlive,HitAlive);	//伤害计算(血量、怒气值)
		Result->m_CurrHp[hitID] = HitAlive->getHp();					//相对于传入伤害的血量
		if (Result->m_LostHp[hitID].hitType != PlayHpType::typeface)	//不是显示字体类型就可以判断击退
			Result->m_Repel[hitID] = ChangeLocation(AtkAlive,HitAlive);	//击退效果(直接改变武将逻辑值)
		else
			Result->m_Repel[hitID] = HitAlive->getGridIndex();
	}
	//负值为扣血，正值为加血，伤害跟效果挂钩
	STR_LostHp HurtCount::hitNum(BaseRole* AtcTarget , BaseRole* HitTarget)
	{
		STR_LostHp vec;
		if (mSkillEffect->getTargetType() == usTargetType)						//判断是攻击还是加血
		{
			vec = gainCount(AtcTarget,HitTarget);
			addHittingAlive(AtcTarget,HitTarget);
		}else{
			if(hitJudge(AtcTarget,HitTarget))
			{
				vec.hitType = PlayHpType::typeface;
			}else{
				vec = lostCount(AtcTarget,HitTarget);	
				addHittingAlive(AtcTarget,HitTarget);
			}
		}
		return vec; 
	}

	void HurtCount::addHittingAlive( BaseRole* AtcTarget , BaseRole* HitTarget )
	{
		if (AtcTarget->mHittingAlive.size())
		{
			for (auto i : AtcTarget->mHittingAlive)
			{
				if (i->getAliveID() == HitTarget->getAliveID())
					return;	
			}
			AtcTarget->mHittingAlive.push_back(HitTarget);
		}else{
			AtcTarget->mHittingAlive.push_back(HitTarget);
		}
		VectorUnique(AtcTarget->mHittingAlive);
	}

	void HurtCount::woldBossHurt( BaseRole* pAlive,float pHurt )
	{
		if (pAlive->getMonsterSpecies() == sMonsterSpecies::eWorldBoss)							//世界boss受击
		{
			pHurt *= (1+BattleManage->getBossHurtPe()*0.01f);								//鼓舞效果
			BattleManage->setBossHurtCount(pHurt);
			BattleManage->setVerifyNum(pHurt);
		}
	}
	//对一个武将造成伤害计算
	STR_LostHp HurtCount::lostCount(BaseRole* AtcTarget , BaseRole* HitTarget)
	{
		HitTarget->setCloaking(false);														//伤害击中取消隐身状态		
		STR_LostHp hp;
		float race_hurt = raceDispose(AtcTarget,HitTarget);									//属性伤害
		int crit_pe = critJudge(AtcTarget,HitTarget);										//暴伤修正百分比
		hp.hitType = lostType(race_hurt,crit_pe);											//得到掉血类型
		//普通伤害 =(攻击力*(1+百分比))^2/(攻击力*(1+百分比)+目标防御))*暴击伤害*属性伤害
		float tAttackNum = pow(AtcTarget->getAtk()*(1+mSkillEffect->getDamageRate()),2);
		float tDefenseNum = AtcTarget->getAtk()*(1+mSkillEffect->getDamageRate())+HitTarget->getDef();
		float BaseHurt = tAttackNum / tDefenseNum;
		float base_hurt_max = BaseHurt * (1 + mSkillEffect->getHurtRatio());						//基础伤害max
		float base_hurt_min = BaseHurt * (1 - mSkillEffect->getHurtRatio());						//基础伤害min
		BaseHurt = CCRANDOM_0_1()*(base_hurt_max-base_hurt_min) + base_hurt_min;			//浮动后的基础伤害
		float TotlaHurt = BaseHurt * crit_pe * race_hurt + mSkillEffect->getRealHurt();
		woldBossHurt(HitTarget,TotlaHurt);
		if (TotlaHurt <= 1)
			TotlaHurt = 1;
		hp.hitNum = -TotlaHurt;
		HitTarget->setHp(HitTarget->getHp() - TotlaHurt);									//实际扣血
		return hp;
	}
	//加血的公式是不存在的。
	STR_LostHp HurtCount::gainCount(BaseRole* AtcTarget, BaseRole* HitTarget)
	{
		STR_LostHp str;
		float addNum = 0;	//加血只与加血方有关
		float erange = mSkillEffect->getHurtRatio() * 0.01f;							//伤害浮动值
		float hurt   = mSkillEffect->getRealHurt();									//真实伤害
		//注意百分比和正负值运算问题
		float addhp = attributeHurt(AtcTarget);							//属性伤害的值
		addhp += HitTarget->getRegain();
		if (!erange)erange = 0.05f;
		float hp_max = addhp*(1 + erange);			  
		float hp_min = addhp*(1 - erange);			  
		float base_hp = CCRANDOM_0_1()*(hp_max-hp_min) + hp_min;		//浮动后的血量
		//属性影响类型*属性影响频率*浮动值+真实伤害
		addNum = base_hp + hurt;
		str.hitType = PlayHpType::gainType;											//加血只显示一种字体
		str.hitNum = HitTarget->getRegain()*mSkillEffect->getDamageRate();		
		if (HitTarget->getCallType() != sCallType::eUNAddHP)				//不可被加血类型武将
			HitTarget->setHp(HitTarget->getHp() + str.hitNum);			//血量实际变化的位置
		return str;
	}
	//命中=命中/(命中+目标闪避）* 100
	bool HurtCount::hitJudge(BaseRole* AtcTarget , BaseRole* HitTarget)
	{
		int ranNum = CCRANDOM_0_1()*100;									//0到100的数
		float atc_hit = AtcTarget->getHit();
		float hit_dodge = HitTarget->getDoge();
		int hitNum = atc_hit/(atc_hit+hit_dodge)*100;
		if (ranNum > hitNum)
			return true;
		return false;
	}

	int  HurtCount::critJudge(BaseRole* AtcTarget , BaseRole* HitTarget)
	{
		//暴击百分比 = 暴击*0.25/300
		int ranNum = CCRANDOM_0_1()*100;		//0到100的数
		if ((AtcTarget->getCrit() * 0.25f/300 * 100) > ranNum)
			return 2;							//暴击造成2倍伤害
		return 1;
	}

	PlayHpType HurtCount::lostType(float race,int crit)
	{
		if (crit == 2)
		{
			if (race == 1)
			{
				return PlayHpType::genralCritType;
			}else{
				if (race > 1)
					return PlayHpType::addCritType;
				return PlayHpType::cutCritType;
			}
		}else{
			if (race == 1)
			{
				return PlayHpType::generalType;
			}else{
				if (race > 1)
					return PlayHpType::addType;
				return PlayHpType::cutType;
			}
		}
	}
	//根据不同的效果类型对攻击武将进行不同处理,扣自己血量给其他目标加血
	void HurtCount::EffectTypeExcute( BattleResult*Result )
	{
		BaseRole *tRole = Result->getAlive();
		int tLostHp = abs(getAllTargetLostHp(Result));				//掉血的具体数字
		switch (mSkillEffect->getCountType())
		{
		case eEffectCountType::eSuckblood:	//吸血型效果
			{		
				tRole->setHp(tRole->getHp() + mSkillEffect->getImpactRate()*tLostHp*0.01f);
				Result->setusNum(mSkillEffect->getImpactRate()*tLostHp*0.01f);																
				Result->setusType(PlayHpType::gainType);
			}break;
		case eEffectCountType::eBoom:			//自爆型效果
			{
				Result->setusNum(- mSkillEffect->getImpactRate());											//爆炸中用于计算自身伤害的血量
				Result->setusType(PlayHpType::generalType);
				tRole->setHp(tRole->getHp() - mSkillEffect->getImpactRate());
			}break;
		case eEffectCountType::eRateBlood:	//扣自己最大血量百分比型吸血技能,可加减血互换
			{
				tRole->setHp(tRole->getHp() - mSkillEffect->getImpactNumber()*0.01f*tRole->getMaxHp()+mSkillEffect->getImpactRate()*tLostHp*0.01f);		
				Result->setusNum(mSkillEffect->getImpactRate()*tLostHp*0.01f- mSkillEffect->getImpactNumber()*0.01f*tRole->getMaxHp());																
				Result->setusType(PlayHpType::generalType);
			}break;
		case eEffectCountType::eNumberBoold:
			{
				tRole->setHp(tRole->getHp() - mSkillEffect->getImpactNumber()+mSkillEffect->getImpactRate()*tLostHp*0.01f);	//扣自己数值血量型吸血技能,可加减血互换
				Result->setusNum(mSkillEffect->getImpactRate()*tLostHp*0.01f-mSkillEffect->getImpactNumber());																
				Result->setusType(PlayHpType::generalType);
			}break;
		case eEffectCountType::eCurrBooldRate:
			{
				tRole->setHp(tRole->getHp() - mSkillEffect->getImpactNumber()*0.01f*tRole->getHp());		
				Result->setusNum(mSkillEffect->getImpactRate()*tLostHp*0.01f-mSkillEffect->getImpactNumber()*0.01f*tRole->getMaxHp());																
				Result->setusType(PlayHpType::generalType);
			}break;
		}
	}

	float HurtCount::getAllTargetLostHp( BattleResult* pResult )
	{
		int lostHp = 0;
		vector<unsigned int>::iterator iter = pResult->m_HitTargets.begin();
		for(;iter!=pResult->m_HitTargets.end();++iter)
			lostHp += pResult->m_LostHp[*iter].hitNum;
		return lostHp;
	}
	//属性影响类型*属性影响频率
	float HurtCount::attributeHurt(BaseRole* AtcTarget)
	{
		switch (mSkillEffect->getImpactType())
		{
		case sAttribute::eAttack:
			{
				return AtcTarget->getAtk() * mSkillEffect->getImpactRate() *0.01f;
			}break;
		case sAttribute::eDefense:
			{
				return AtcTarget->getDef() * mSkillEffect->getImpactRate()*0.01f;
			}break;
		case sAttribute::eBlood:
			{
				return AtcTarget->getHp() * mSkillEffect->getImpactRate()*0.01f;
			}break;
		case sAttribute::eHit:
			{
				return AtcTarget->getHit() * mSkillEffect->getImpactRate()*0.01f;
			}break;
		case sAttribute::eCrit:
			{
				return AtcTarget->getCrit() * mSkillEffect->getImpactRate()*0.01f;
			}break;
		default:
			{
				if (mSkillEffect->getImpactType() != sAttribute::eNull)
					CCLOG("ERROR: in [ HurtCount::attribute_hurt ]");
				return 0;
			}break;
		}
	}
	//属性克制
	float HurtCount::raceDispose(BaseRole* AtcTarget , BaseRole* HitTarget)
	{
		return 1;
		sRoleNature hitType = (sRoleNature)HitTarget->getBaseData()->getProperty();
		return AtcTarget->getBaseData()->judgeAttribute(hitType);
	}

};