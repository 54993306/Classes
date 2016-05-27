#include "HurtCount.h"
#include "ConstNum.h"
#include "MoveRule.h"
#include "model/BattleData.h"
#include "model/BuffManage.h"
#include "Battle/BattleRole.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "warscene/BattleResult.h"
#include "common/CommonFunction.h"
#include "scene/alive/HPObject.h"

HurtCount::HurtCount():m_Manage(nullptr){}
HurtCount::~HurtCount(){}
bool HurtCount::init()
{
	m_Manage = DataCenter::sharedData()->getWar();
	return true;
}

// << 血量变化 >> 、 << 怒气值变化 >> 、<< 打击位移效果 >> 处理,目标为受击数组
BattleResult* HurtCount::AttackExcute(WarAlive* alive)
{
	BattleResult* Result = BattleResult::create();
	Result->setAlive(alive);
	if (alive->getNegate())													//为了做击退处理
	{
		vector<WarAlive*>::reverse_iterator iter = alive->AtkAlive.rbegin();//迭代器反向遍历(用下标效率是最高的)
		for (;iter != alive->AtkAlive.rend();iter++)
		{
			WarAlive* HitAlive = *iter;
			HurtExcute(Result,alive,HitAlive);
		}
	}else{
		vector<WarAlive*>::iterator iter = alive->AtkAlive.begin();
		for (;iter != alive->AtkAlive.end();iter ++)
		{
			WarAlive* HitAlive = *iter;
			HurtExcute(Result,alive,HitAlive);
		}
	}
	EffectTypeExcute(Result);												//用于计算我方受伤信息
	return Result;
}
//多人打击的情况有返回值才能确定具体移动
int HurtCount::ChangeLocation(WarAlive* AtcAlive , WarAlive* HitAlive)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	//return HitAlive->getGridIndex();
#endif
	if (HitAlive->getCallType() == FixedlyCantFlyType 
		|| HitAlive->getCallType() == NotAttack||HitAlive->getCaptain()
		|| HitAlive->getAliveType() == AliveType::WorldBoss)//不可被击飞类型武将
		return HitAlive->getGridIndex();
	SkillEffect* effect = AtcAlive->getCurrEffect();
	if (effect->batter != AtcAlive->getSortieNum())
		return HitAlive->getGridIndex();									//连击的最后一次才做击退
	bool enemy = AtcAlive->getEnemy();
	if (AtcAlive->getNegate())
		enemy = !enemy;
	int grid = MoveRule::create()->FrontBack(HitAlive,effect->repel,enemy);
	if (grid != INVALID_GRID)
	{
		if (grid >= C_BEGINGRID && grid<C_CAPTAINGRID)						//击退范围做一个边界处理
			return grid;
	}		
	return HitAlive->getGridIndex();
}
//连击的时候武将的攻击掉血帧数如果不够则无法计算完全部伤害,无法处理击退效果
void HurtCount::HurtExcute(BattleResult*Result,WarAlive*AtkAlive,WarAlive*HitAlive)
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
STR_LostHp HurtCount::hitNum(WarAlive* AtcTarget , WarAlive* HitTarget)
{
	STR_LostHp vec;
	SkillEffect* effect = AtcTarget->getCurrEffect();
	if (effect->pTarget == usTargetType)						//判断是攻击还是加血
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
	//BuffHandleLogic(AtcTarget,HitTarget);						//伤害计算完成才能添加新的BUFF
	return vec; 
}

void HurtCount::addHittingAlive( WarAlive* AtcTarget , WarAlive* HitTarget )
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

void HurtCount::woldBossHurt( WarAlive* pAlive,float pHurt )
{
	if (pAlive->getAliveType() == AliveType::WorldBoss)							//世界boss受击
	{
		pHurt *= (1+m_Manage->getBossHurtPe()*0.01f);								//鼓舞效果
		m_Manage->setBossHurtCount(pHurt);
		m_Manage->setVerifyNum(pHurt);
	}
}

//对一个武将造成伤害计算
STR_LostHp HurtCount::lostCount(WarAlive* AtcTarget , WarAlive* HitTarget)
{
	HitTarget->setCloaking(false);														//伤害击中取消隐身状态		
	STR_LostHp hp;
	float race_hurt = raceDispose(AtcTarget,HitTarget);									//属性伤害
	int crit_pe = critJudge(AtcTarget,HitTarget);										//暴伤修正百分比
	hp.hitType = lostType(race_hurt,crit_pe);											//得到掉血类型
	//普通伤害 =(攻击力*(1+百分比))^2/(攻击力*(1+百分比)+目标防御))*暴击伤害*属性伤害
	SkillEffect* effect = AtcTarget->getCurrEffect();
	float tAttackNum = pow(AtcTarget->getAtk()*(1+effect->damage*0.01f),2);
	float tDefenseNum = AtcTarget->getAtk()*(1+effect->damage*0.01f)+HitTarget->getDef();
	float BaseHurt = tAttackNum / tDefenseNum;
	float erange = effect->erange * 0.01f;												//伤害浮动值
	if (!erange)erange = 0.05f;
	float base_hurt_max = BaseHurt * (1 + erange);										//基础伤害max
	float base_hurt_min = BaseHurt * (1 - erange);										//基础伤害min
	BaseHurt = CCRANDOM_0_1()*(base_hurt_max-base_hurt_min) + base_hurt_min;			//浮动后的基础伤害
	float TotlaHurt = BaseHurt * crit_pe * race_hurt + effect->hurt;
	woldBossHurt(HitTarget,TotlaHurt);
	if (TotlaHurt <= 0)
		TotlaHurt = 1;
	hp.hitNum = -TotlaHurt;
	HitTarget->setHp(HitTarget->getHp() - TotlaHurt);									//实际扣血
	return hp;
}

STR_LostHp HurtCount::gainCount(WarAlive* AtcTarget, WarAlive* HitTarget)
{
	STR_LostHp str;
	float addNum = 0;	//加血只与加血方有关
	SkillEffect* effect = AtcTarget->getCurrEffect();	
	float erange = effect->erange * 0.01f;							//伤害浮动值
	float hurt   = effect->hurt;									//真实伤害
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
	if (HitTarget->getCallType() != UNAddHP)						//不可被加血类型武将
		HitTarget->setHp(HitTarget->getHp() + str.hitNum);			//血量实际变化的位置
	return str;
}
//命中=命中/(命中+目标闪避）* 100
bool HurtCount::hitJudge(WarAlive* AtcTarget , WarAlive* HitTarget)
{
	int ranNum = CCRANDOM_0_1()*100;									//0到100的数
	float atc_hit = AtcTarget->getHit();
	float hit_dodge = HitTarget->getDoge();
	int hitNum = atc_hit/(atc_hit+hit_dodge)*100;
	if (ranNum > hitNum)
		return true;
	return false;
}

int  HurtCount::critJudge(WarAlive* AtcTarget , WarAlive* HitTarget)
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
	WarAlive *alive = Result->getAlive();
	SkillEffect* effect = alive->getCurrEffect();
	int lostHp = getAllTargetLostHp(Result);
	switch (effect->Efftype)
	{
	case Suckblood_Type:	//吸血型效果
		{		
			alive->setHp(alive->getHp() + effect->pro_Rate*lostHp*0.01f);
			Result->setusNum(effect->pro_Rate*lostHp*0.01f);																
			Result->setusType(gainType);
		}break;
	case Boom_Type:			//自爆型效果
		{
			Result->setusNum(- effect->pro_Rate);											//爆炸中用于计算自身伤害的血量
			Result->setusType(generalType);
			alive->setHp(alive->getHp() - effect->pro_Rate);
		}break;
	case RateBlood_Type:	//扣自己最大血量百分比型吸血技能,可加减血互换
		{
			alive->setHp(alive->getHp() - effect->pro_Type*0.01f*alive->getMaxHp()+effect->pro_Rate*lostHp*0.01f);		
			Result->setusNum(effect->pro_Rate*lostHp*0.01f-effect->pro_Type*0.01f*alive->getMaxHp());																
			Result->setusType(generalType);
		}break;
	case NumBoold_Type:
		{
			alive->setHp(alive->getHp() - effect->pro_Type+effect->pro_Rate*lostHp*0.01f);	//扣自己数值血量型吸血技能,可加减血互换
			Result->setusNum(effect->pro_Rate*lostHp*0.01f-effect->pro_Type);																
			Result->setusType(generalType);
		}break;
	case RateNowBoold_Type:
		{
			alive->setHp(alive->getHp() - effect->pro_Type*0.01f*alive->getHp());		
			Result->setusNum(effect->pro_Rate*lostHp*0.01f-effect->pro_Type*0.01f*alive->getMaxHp());																
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

float HurtCount::attributeHurt(WarAlive* AtcTarget)
{
	SkillEffect* effect = AtcTarget->getCurrEffect();
	//属性影响类型*属性影响频率
	switch (effect->pro_Type)
	{
	case atb_attak:
		{
			return AtcTarget->getAtk() * effect->pro_Rate *0.01f;
		}break;
	case atb_def:
		{
			return AtcTarget->getDef() * effect->pro_Rate*0.01f;
		}break;
	case atb_hp:
		{
			return AtcTarget->getHp() * effect->pro_Rate*0.01f;
		}break;
	case atb_hit:
		{
			return AtcTarget->getHit() * effect->pro_Rate*0.01f;
		}break;
	case atb_crit:
		{
			return AtcTarget->getCrit() * effect->pro_Rate*0.01f;
		}break;
	default:
		{
			if (effect->pro_Type)
				CCLOG("ERROR: in [ HurtCount::attribute_hurt ]");
			return 0;
		}break;
	}
}
//只能计算最后一个效果添加在武将身上的buff
void HurtCount::BuffHandleLogic(WarAlive* pAlive)
{
	SkillEffect* effect = pAlive->getCurrEffect();
	BuffManage* AtcbufManege = pAlive->getBuffManage();
	for (auto tAlive : pAlive->HittingAlive)
	{
		if (tAlive->getHp()<=0 || tAlive->getDieState())
			continue;
		for (vector<RoleBuffData>::iterator itre = effect->buffList.begin();itre != effect->buffList.end();++itre)
		{
			RoleBuffData buff = *itre;
			int ranNum = CCRANDOM_0_1()*100;
			if (ranNum > buff.getTriggerRate()/*true*/)//每个buf都需要进行添加判断
			{
				//CCLOG("[ TIPS ]Buff Add Fail [BuffID= %d, userRate= %d, ranNum=%d]",buff.buffId,buff.useRate,ranNum);
				continue;		
			}
			if (buff.getBuffTarget() == usTargetType)					//自己
			{
				//CCLOG("\nAtcTargetID = %d ,AddBuff ID: %d",AtcTarget->getAliveID(),buff.buffId);
				if (buff.getTargetType()&&buff.getTargetType()!=pAlive->role->roletype)//判断buf的限定种族
					continue;
				AtcbufManege->AddBuff(buff);
			}else if (buff.getBuffTarget() == hitTargetType && tAlive)				//受击目标
			{
				BuffManage* HitbufManege = tAlive->getBuffManage();
				if (buff.getTargetType()&&buff.getTargetType()!=tAlive->role->roletype)
					continue;
				//CCLOG("\nHitTargetID = %d ,AddBuff ID: %d",HitTarget->getAliveID(),buff.buffId);
				HitbufManege->AddBuff(buff);
			}else{
				CCLOG("[ ERROR ] buff.target can find bufid:%d ,aliveid:%d",buff.getBuffID(),pAlive->getAliveID());
			}
		}
	}
}
//属性克制
float HurtCount::raceDispose(WarAlive* AtcTarget , WarAlive* HitTarget)
{
	return 1;
	//火1>木3; 木3>水2; 水2>火1
	int atkType = AtcTarget->role->roletype;
	int hitType = HitTarget->role->roletype;
	if (atkType == hitType)
	{
		return 1.0f;
	}else if ( atkType == FireType )
	{
		if (hitType == WoodType)
			return 1.3f;
		if (hitType == WaterType)
			return 0.7f;
	}else if ( atkType == WoodType )
	{
		if (hitType == WaterType)
			return 1.3f;
		if (hitType == FireType)
			return 0.7f;
	}else if( atkType == WaterType )
	{
		if (hitType == FireType)
			return 1.3f;
		if (hitType == WoodType)
			return 0.7f;
	}
	return 1.0f;
}