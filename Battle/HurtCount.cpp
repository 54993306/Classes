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
#include "HurtCount.h"
#include "Battle/ConstNum.h"
#include "MoveRule.h"
#include "Battle/BaseRoleData.h"
#include "model/BuffManage.h"
#include "Battle/BaseRole.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "Battle/BattleResult.h"
#include "common/CommonFunction.h"
#include "Battle/RoleObject/HPObject.h"
#include "Battle/BuffData.h"
#include "Battle/skEffectData.h"
namespace BattleSpace{

	HurtCount::HurtCount():m_Manage(nullptr){}
	HurtCount::~HurtCount(){}
	bool HurtCount::init()
	{
		m_Manage = DataCenter::sharedData()->getWar();
		return true;
	}

	// << 血量变化 >> 、 << 怒气值变化 >> 、<< 打击位移效果 >> 处理,目标为受击数组
	BattleResult* HurtCount::AttackExcute(BaseRole* alive)
	{
		BattleResult* Result = BattleResult::create();
		Result->setAlive(alive);
		vector<BaseRole*>::iterator iter = alive->mAreaTargets.begin();
		for (;iter != alive->mAreaTargets.end();iter ++)
		{
			BaseRole* HitAlive = *iter;
			HurtExcute(Result,alive,HitAlive);
		}
		EffectTypeExcute(Result);												//用于计算我方受伤信息
		return Result;
	}
	//多人打击的情况有返回值才能确定具体移动
	int HurtCount::ChangeLocation(BaseRole* AtcAlive , BaseRole* HitAlive)
	{
		if (HitAlive->getCallType() == sCallType::eFixedlyCantFly 
			|| HitAlive->getCallType() == sCallType::eNotAttack||HitAlive->getCaptain()
			|| HitAlive->getAliveType() == sMonsterSpecies::eWorldBoss)//不可被击飞类型武将
			return HitAlive->getGridIndex();
		const skEffectData* effect = AtcAlive->getCurrEffect();
		if (effect->getBatter() != AtcAlive->getSortieNum() || !effect->getRepel())
			return HitAlive->getGridIndex();									//连击的最后一次才做击退
		bool enemy = AtcAlive->getEnemy();
		if (AtcAlive->getOpposite())
			enemy = !enemy;
		int grid = MoveRule::create()->FrontBack(HitAlive,effect->getRepel(),enemy);
		if (grid != INVALID_GRID)
		{
			if ( grid>=C_BEGINGRID && grid<C_CAPTAINGRID )						//击退范围做一个边界处理
				return grid;
		}		
		return HitAlive->getGridIndex();
	}
	//连击的时候武将的攻击掉血帧数如果不够则无法计算完全部伤害,无法处理击退效果
	void HurtCount::HurtExcute(BattleResult*Result,BaseRole*AtkAlive,BaseRole*HitAlive)
	{
		unsigned int hitID = HitAlive->getAliveID();
		Result->m_HitTargets.push_back(hitID);
		Result->m_Alive_s[hitID] = HitAlive;
		Result->m_LostHp[hitID] = hitNum(AtkAlive , HitAlive);			//伤害计算(血量、怒气值)
		Result->m_CurrHp[hitID] = HitAlive->getHp();					//相对于传入伤害的血量
		if (Result->m_LostHp[hitID].hitType != typeface)				//不是显示字体类型就可以判断击退
			Result->m_Repel[hitID] = ChangeLocation(AtkAlive,HitAlive);	//击退效果(直接改变武将逻辑值)
		else
			Result->m_Repel[hitID] = HitAlive->getGridIndex();
	}
	//负值为扣血，正值为加血，伤害跟效果挂钩
	STR_LostHp HurtCount::hitNum(BaseRole* AtcTarget , BaseRole* HitTarget)
	{
		STR_LostHp vec;
		const skEffectData* effect = AtcTarget->getCurrEffect();
		if (effect->getTargetType() == usTargetType)						//判断是攻击还是加血
		{
			vec = gainCount(AtcTarget,HitTarget);
			addHittingAlive(AtcTarget,HitTarget);
		}else{
			if(hitJudge(AtcTarget,HitTarget))
			{
				vec.hitType = typeface;
			}else{
				vec = lostCount(AtcTarget,HitTarget);	
				addHittingAlive(AtcTarget,HitTarget);
			}
		}
		return vec; 
	}

	void HurtCount::addHittingAlive( BaseRole* AtcTarget , BaseRole* HitTarget )
	{
		if (AtcTarget->HittingAlive.size())
		{
			for (auto i : AtcTarget->HittingAlive)
			{
				if (i->getAliveID() == HitTarget->getAliveID())
					return;	
			}
			AtcTarget->HittingAlive.push_back(HitTarget);
		}else{
			AtcTarget->HittingAlive.push_back(HitTarget);
		}
	}

	void HurtCount::woldBossHurt( BaseRole* pAlive,float pHurt )
	{
		if (pAlive->getAliveType() == sMonsterSpecies::eWorldBoss)							//世界boss受击
		{
			pHurt *= (1+m_Manage->getBossHurtPe()*0.01f);								//鼓舞效果
			m_Manage->setBossHurtCount(pHurt);
			m_Manage->setVerifyNum(pHurt);
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
		const skEffectData* effect = AtcTarget->getCurrEffect();
		float tAttackNum = pow(AtcTarget->getAtk()*(1+effect->getDamageRate()),2);
		float tDefenseNum = AtcTarget->getAtk()*(1+effect->getDamageRate())+HitTarget->getDef();
		float BaseHurt = tAttackNum / tDefenseNum;
		float base_hurt_max = BaseHurt * (1 + effect->getHurtRatio());						//基础伤害max
		float base_hurt_min = BaseHurt * (1 - effect->getHurtRatio());						//基础伤害min
		BaseHurt = CCRANDOM_0_1()*(base_hurt_max-base_hurt_min) + base_hurt_min;			//浮动后的基础伤害
		float TotlaHurt = BaseHurt * crit_pe * race_hurt + effect->getRealHurt();
		woldBossHurt(HitTarget,TotlaHurt);
		if (TotlaHurt <= 0)
			TotlaHurt = 1;
		hp.hitNum = -TotlaHurt;
		HitTarget->setHp(HitTarget->getHp() - TotlaHurt);									//实际扣血
		return hp;
	}

	STR_LostHp HurtCount::gainCount(BaseRole* AtcTarget, BaseRole* HitTarget)
	{
		STR_LostHp str;
		float addNum = 0;	//加血只与加血方有关
		const skEffectData* effect = AtcTarget->getCurrEffect();	
		float erange = effect->getHurtRatio() * 0.01f;							//伤害浮动值
		float hurt   = effect->getRealHurt();									//真实伤害
		//注意百分比和正负值运算问题
		float addhp = attributeHurt(AtcTarget);							//属性伤害的值
		if (!erange)erange = 0.05f;
		float hp_max = addhp*(1 + erange);			  
		float hp_min = addhp*(1 - erange);			  
		float base_hp = CCRANDOM_0_1()*(hp_max-hp_min) + hp_min;		//浮动后的血量
		//属性影响类型*属性影响频率*浮动值+真实伤害
		addNum = base_hp + hurt;
		str.hitType = gainType;											//加血只显示一种字体
		str.hitNum = addNum;		
		if (HitTarget->getCallType() != sCallType::eUNAddHP)						//不可被加血类型武将
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

	int HurtCount::lostType(float race,int crit)
	{
		if (crit == 2)
		{
			if (race == 1)
			{
				return genralCritType;
			}else{
				if (race > 1)
					return addCritType;
				return cutCritType;
			}
		}else{
			if (race == 1)
			{
				return generalType;
			}else{
				if (race > 1)
					return addType;
				return cutType;
			}
		}
	}
	//根据不同的效果类型对攻击武将进行不同处理,扣自己血量给其他目标加血
	void HurtCount::EffectTypeExcute( BattleResult*Result )
	{
		BaseRole *alive = Result->getAlive();
		const skEffectData* effect = alive->getCurrEffect();
		int tLostHp = getAllTargetLostHp(Result);
		switch (effect->getEffectType())
		{
		case Suckblood_Type:	//吸血型效果
			{		
				alive->setHp(alive->getHp() + effect->getImpactRate()*tLostHp*0.01f);
				Result->setusNum(effect->getImpactRate()*tLostHp*0.01f);																
				Result->setusType(gainType);
			}break;
		case Boom_Type:			//自爆型效果
			{
				Result->setusNum(- effect->getImpactRate());											//爆炸中用于计算自身伤害的血量
				Result->setusType(generalType);
				alive->setHp(alive->getHp() - effect->getImpactRate());
			}break;
		case RateBlood_Type:	//扣自己最大血量百分比型吸血技能,可加减血互换
			{
				alive->setHp(alive->getHp() - effect->getImpactType()*0.01f*alive->getMaxHp()+effect->getImpactRate()*tLostHp*0.01f);		
				Result->setusNum(effect->getImpactRate()*tLostHp*0.01f-effect->getImpactType()*0.01f*alive->getMaxHp());																
				Result->setusType(generalType);
			}break;
		case NumBoold_Type:
			{
				alive->setHp(alive->getHp() - effect->getImpactType()+effect->getImpactRate()*tLostHp*0.01f);	//扣自己数值血量型吸血技能,可加减血互换
				Result->setusNum(effect->getImpactRate()*tLostHp*0.01f-effect->getImpactType());																
				Result->setusType(generalType);
			}break;
		case RateNowBoold_Type:
			{
				alive->setHp(alive->getHp() - effect->getImpactType()*0.01f*alive->getHp());		
				Result->setusNum(effect->getImpactRate()*tLostHp*0.01f-effect->getImpactType()*0.01f*alive->getMaxHp());																
				Result->setusType(generalType);
			}break;
		default:
			break;
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

	float HurtCount::attributeHurt(BaseRole* AtcTarget)
	{
		const skEffectData* effect = AtcTarget->getCurrEffect();
		//属性影响类型*属性影响频率
		switch ((sProperty)effect->getImpactType())
		{
		case sProperty::eAttack:
			{
				return AtcTarget->getAtk() * effect->getImpactRate() *0.01f;
			}break;
		case sProperty::eDefense:
			{
				return AtcTarget->getDef() * effect->getImpactRate()*0.01f;
			}break;
		case sProperty::eBlood:
			{
				return AtcTarget->getHp() * effect->getImpactRate()*0.01f;
			}break;
		case sProperty::eHit:
			{
				return AtcTarget->getHit() * effect->getImpactRate()*0.01f;
			}break;
		case sProperty::eCrit:
			{
				return AtcTarget->getCrit() * effect->getImpactRate()*0.01f;
			}break;
		default:
			{
				if (effect->getImpactType())
					CCLOG("ERROR: in [ HurtCount::attribute_hurt ]");
				return 0;
			}break;
		}
	}
	//只能计算最后一个效果添加在武将身上的buff
	void HurtCount::BuffHandleLogic(BaseRole* pAlive)
	{
		const skEffectData* tKillEffect = pAlive->getCurrEffect();
		BuffManage* AtcbufManege = pAlive->getBuffManage();
		for (auto tAlive : pAlive->HittingAlive)
		{
			if (tAlive->getHp()<=0 || tAlive->getDieState())
				continue;
			vector<BuffData*>::const_iterator tItre = tKillEffect->getBuffVector().begin();
			for (;tItre != tKillEffect->getBuffVector().end();++tItre)
			{
				BuffData* buff = *tItre;
				int ranNum = CCRANDOM_0_1()*100;
				if (ranNum > buff->getTriggerRate()/*true*/)//每个buf都需要进行添加判断
				{
					//CCLOG("[ TIPS ]Buff Add Fail [BuffID= %d, userRate= %d, ranNum=%d]",buff.buffId,buff.useRate,ranNum);
					continue;		
				}
				if (buff->getBuffTarget() == usTargetType)					//自己
				{
					//CCLOG("\nAtcTargetID = %d ,AddBuff ID: %d",AtcTarget->getAliveID(),buff.buffId);
					if (buff->getTargetType() != sAttribute::eNull	 && buff->getTargetType()!=pAlive->getBaseData()->getAttribute())//判断buf的限定种族
						continue;
					AtcbufManege->AddBuff(*buff);
				}else if (buff->getBuffTarget() == hitTargetType && tAlive)				//受击目标
				{
					BuffManage* HitbufManege = tAlive->getBuffManage();
					if (buff->getTargetType() != sAttribute::eNull	 &&buff->getTargetType()!=tAlive->getBaseData()->getAttribute())
						continue;
					//CCLOG("\nHitTargetID = %d ,AddBuff ID: %d",HitTarget->getAliveID(),buff.buffId);
					HitbufManege->AddBuff(*buff);
				}else{
					CCLOG("[ ERROR ] buff.target can find bufid:%d ,aliveid:%d",buff->getBuffID(),pAlive->getAliveID());
				}
			}
		}
	}
	//属性克制
	float HurtCount::raceDispose(BaseRole* AtcTarget , BaseRole* HitTarget)
	{
		return 1;
		sAttribute hitType = HitTarget->getBaseData()->getAttribute();
		return AtcTarget->getBaseData()->judgeAttribute(hitType);
	}
};