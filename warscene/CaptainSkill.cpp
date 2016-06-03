#include "CaptainSkill.h"
#include "model/DataCenter.h"
#include "Battle/BattleRole.h"
#include "SkillRange.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "warscene/ConstNum.h"
CaptainSkill::CaptainSkill(){}

CaptainSkill::~CaptainSkill(){}

CaptainSkill* CaptainSkill::create()
{
	CaptainSkill* pRec = new CaptainSkill();
	if (pRec && pRec->init())
	{
		pRec->autorelease();
		return pRec;
	}else{
		delete pRec;
		pRec = NULL;
		return NULL;
	}
}

bool CaptainSkill::init()
{
	return true;
}

void CaptainSkill::ExecuteCaptainSkill()
{
	WarAlive* alive  = DataCenter::sharedData()->getWar()->getAliveByGrid(C_CAPTAINGRID);
	CCArray* arr = DataCenter::sharedData()->getWar()->getHeros(true);
	if (!alive||!arr||alive->role->skCaptain.getSkillID() == 0)
		return;
	RoleSkill skill = alive->role->skCaptain;							//需同时满足种族限制且满足属性限制
	CCArray* RaceArr = RaceJudgeCap(arr,skill.getTargetType());			//种族限制判定
	if (!RaceArr||!RaceArr->count())
		return;																//不满足种族限制条件
	//CCArray* AttributeArr=AttributeJudgeArrCap(_CaptainSkill,RaceArr);	//属性限制判定(暂时取消)
	//if (!AttributeArr||!AttributeArr->count())
	//	return;																//满足种族限制未满足属性限制
	CCArray* AttributeArr = RaceArr;
	CCArray* executeArr = CCArray::create();
	for (int i=0;i<skill.getEffectSize(0);i++)							//触发队长技能效果
	{
		if (!skill.getIndexEffect(0,i)->getEffectID())
			continue;
		CCArray* _arr = getTargetArrCap(skill.getIndexEffect(0,i),AttributeArr,arr);			
		AddArr(executeArr,_arr);
	}
	CCObject* ob = nullptr;
	CCARRAY_FOREACH(executeArr,ob)
	{
		WarAlive* alive = (WarAlive*)ob;									//只触发一次队长技,但是需要触发完队长技的效果才行	
		alive->setExecuteCap(true);
	}
}

void CaptainSkill::AddArr(CCArray* pTargetArray,CCArray*arr)
{
	if (!arr)return;
	CCObject* ob = nullptr;
	CCObject* obj = nullptr;
	bool _add = true;
	CCARRAY_FOREACH(arr,ob)
	{
		_add = true;
		WarAlive* alive = (WarAlive*)ob;
		CCARRAY_FOREACH(pTargetArray,obj)
		{
			WarAlive* _alive = (WarAlive*)obj;
			if (alive == _alive)
			{
				_add = false;
				break;
			}
		}
		if (_add)
			pTargetArray->addObject(ob);
	}
}
//种族判定通过数组
CCArray* CaptainSkill::RaceJudgeCap(CCArray* arr,int type)
{
	CCArray* aliveArr = CCArray::create();
	switch (type)
	{
	case Fire:				{  aliveArr = RaceJudge(arr,Fire);				}break;
	case Water:				{  aliveArr = RaceJudge(arr,Water);				}break;
	case Wood:				{  aliveArr = RaceJudge(arr,Wood);				}break;
	case WaterAndWood:		{  aliveArr = RaceJudge(arr,Fire,true);			}break;
	case FireAndWater:		{  aliveArr = RaceJudge(arr,Wood,true);			}break;
	case FireAndWodd:		{  aliveArr = RaceJudge(arr,Water,true);		}break;
	case WoodAndWater:		{  aliveArr = RaceJudge(arr,Fire,true,true);	}break;
	case WaterAndFire:		{  aliveArr = RaceJudge(arr,Wood,true,true);	}break;
	case WoodAndFire:		{  aliveArr = RaceJudge(arr,Water,true,true);	}break;
	case AllAliveType:
		{
			CCObject* obj = nullptr;
			int type	= 0;
			int type2	= 0;
			if (arr->count() < 3)break;
			CCARRAY_FOREACH(arr,obj)
			{
				WarAlive* alive = (WarAlive*)obj;
				if (alive->getExecuteCap())continue;
				if (!type || !type2)
				{
					if (!type)
					{
						 type = alive->role->roletype;
						continue;
					}
					type2 = alive->role->roletype;
					continue;
				}
				if (type&&type2)
				{
					if (alive->role->roletype != type&&alive->role->roletype!=type2)
					{
						aliveArr = arr;
						break;
					}
				}
			}
		}break;
	case AnyType:
		{
			if (arr->count())
			{
				CCObject* ob = nullptr;
				CCARRAY_FOREACH(arr,ob)
				{
					WarAlive* alive = (WarAlive*)ob;
					if (alive->getExecuteCap())continue;
					aliveArr->addObject(ob);
				}
			}	
		}break;
	default:
		break;
	}
	return aliveArr;
}
//种族判定
CCArray* CaptainSkill::RaceJudge(CCArray* arr,int type,bool exclude/*= false*/,bool And/*= false*/)
{
	CCArray* targetArr = CCArray::create();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		if (alive->getExecuteCap())continue;
		if (exclude)
		{
			if (alive->role->roletype == type)continue;
			targetArr->addObject(obj);
		}else{
			if (alive->role->roletype != type)continue;
			targetArr->addObject(obj);
		}
	}

	if (!exclude || !And)return targetArr;
	int secondType = 0;
	bool fail = true;
	CCARRAY_FOREACH(targetArr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		if (!secondType)
		{
			secondType = alive->role->roletype;
			continue;
		}
		if (secondType&&secondType!=alive->role->roletype)
		{
			fail = false;
			break;
		}
	}
	if (fail)targetArr->removeAllObjects();
	return targetArr;
}
//属性判定通过数组
CCArray* CaptainSkill::AttributeJudgeArrCap(RoleSkill& skill,CCArray* arr)
{
	CCArray* _arr = CCArray::create();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		if (AttributeJudge(alive,skill.getAffectType(),skill.getAffectRatio()))
			_arr->addObject(obj);
	}
	return _arr;
}
//属性判定
bool CaptainSkill::AttributeJudge(WarAlive* alive,int type,int rate)
{
	switch (type)
	{
	case allAtb: { return true; }
	case DefRate:		//除去NedHpRate是少于规定触发外其他都是进入战斗立马处理，扣减完后触发队长技处理
		{ 
			alive->setDef(alive->role->def*rate*0.01f);
			return true;	
		}break;
	case AtkRate:		
		{ 
			alive->setAtk(alive->role->atk*rate*0.01f);			
			return true;	
		}break;
	case NedHpRate:			//血量少于某值触发效果
		{ 
			if (alive->getHp() <= alive->role->hp*rate*0.01f)
				return true;	
		}break;
	case RenewRate:		
		{ 
			alive->setRenew(alive->role->renew*rate*0.01f);
			return true;	
		}break;
	case DogeRate:		
		{ 
			alive->setDoge(alive->role->dodge*rate*0.01f);	
			return true;	
		}break;
	case HitRate:		
		{ 
			alive->setHit(alive->role->hit*rate*0.01f);		
			return true;	
		}break;
	case CritRate:		
		{ 
			if (alive->getCrit() >= alive->role->crit*rate*0.01f)			
				return true;	
		}break;
	case HpMaxRate:		
		{ 
			alive->setHp(alive->role->hp*rate*0.01f);				
			return true;	
		}break;
	default:
		break;
	}
	return false;
}
//方位判定通过数组
CCArray* CaptainSkill::PositionJudgeArrCap(CCArray* arr,int type)
{
	CCArray* targetArr = CCArray::create();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		if (PositionJudge(alive->getEnemy(),alive->getGridIndex(),type))
		targetArr->addObject(obj);
	}
	return targetArr;
}
//方位判定
bool CaptainSkill::PositionJudge(bool Enemy,int grid,int type)
{
	//SkillRange* skillrange = SkillRange::create();
	//switch (type)
	//{
	//case CapfrontOne:
	//	{
	//		return skillrange->BossAreaJudge(Front,grid,Enemy,ustarget);
	//	}break;
	//case CapcenterOne:
	//	{
	//		return skillrange->BossAreaJudge(Centen,grid,Enemy,ustarget);
	//	}break;
	//case CapbackOne:
	//	{
	//		return skillrange->BossAreaJudge(Back,grid,Enemy,ustarget);
	//	}break;
	//case CapfrontArea:
	//	{
	//		return skillrange->BossAreaJudge(Front,grid,Enemy,ustarget,Disperse,1);
	//	}break;
	//case CapcenterArea:
	//	{
	//		return skillrange->BossAreaJudge(Centen,grid,Enemy,ustarget,Disperse,1);
	//	}break;
	//case CapbackArea:
	//	{
	//		return skillrange->BossAreaJudge(Back,grid,Enemy,ustarget,Disperse,1);
	//	}break;
	//case CapfrontRowArea:
	//	{
	//		return skillrange->BossAreaJudge(Front,grid,Enemy,ustarget,Row,1);
	//	}break;
	//case CapbackRowArea:
	//	{
	//		return skillrange->BossAreaJudge(Centen,grid,Enemy,ustarget,Row,1);
	//	}break;
	//case CapcenterRowArea:
	//	{
	//		return skillrange->BossAreaJudge(Front,grid,Enemy,ustarget,Row,1);
	//	}break;
	//default:
	//	break;
	//}
	return false;
}
//目标数组获得
CCArray* CaptainSkill::getTargetArrCap(const skEffectData*pEffect,CCArray*targetArr,CCArray*AllArr)
{
	CCArray* tArray = nullptr;
	switch (pEffect->getTargetType())
	{
	case Fire:
	case Water:
	case Wood:
	case WaterAndWood:
	case FireAndWater:
	case FireAndWodd:
	case WoodAndWater:
	case WaterAndFire:
	case WoodAndFire:
	case AllAliveType:
	case AnyType:
		{
			tArray = RaceJudgeCap(AllArr,pEffect->getTargetType());
			ExecuteArrCap(pEffect , tArray);							//从所有的我方阵营中选取目标数组
		}break;
	case usAll:
		{
			tArray = targetArr;
			ExecuteArrCap(pEffect , tArray);							//从满足释放条件的的目标中选取目标数组
		}break;
	case usFire:
		{
			tArray = RaceJudgeCap(targetArr,Fire);
			ExecuteArrCap(pEffect , tArray);
		}break;
	case usWood:
		{
			tArray = RaceJudgeCap(targetArr,Wood);
			ExecuteArrCap(pEffect , tArray);
		}break;
	case usWater:
		{
			tArray = RaceJudgeCap(targetArr,Water);
			ExecuteArrCap(pEffect , tArray);
		}break;
	case CapfrontOne:												//根据方位选取目标数组
	case CapcenterOne:
	case CapbackOne:
	case CapfrontArea:
	case CapcenterArea:
	case CapbackArea:
	case CapfrontRowArea:
	case CapbackRowArea:
	case CapcenterRowArea:
		{
			tArray = PositionJudgeArrCap(AllArr,pEffect->getTargetType());
			ExecuteArrCap(pEffect,tArray);
		}
	default:
		break;
	}
	return tArray;
}
//目标武将执行效果
void CaptainSkill::ExecuteArrCap(const skEffectData*pEffect,CCArray* pArray)
{
	if (!pArray)
	{
		CCLOG("[ Tips ] CaptainSkill::ExecuteAliveCap");
		return;
	}
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(pArray,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		if (alive->getExecuteCap())continue;	//执行过主帅技
		AliveExecute(pEffect,alive);
	}
}
//队长技效果武将属性变化
void CaptainSkill::AliveExecute(const skEffectData*pEffect,WarAlive*pAlive)
{
	switch (pEffect->getImpactType())
	{
	case DefRate:
		{
			pAlive->setDef(pAlive->getDef()+(pAlive->role->def * (pEffect->getImpactRate()*0.01f-1)));
		}break;
	case AtkRate:
		{
			pAlive->setAtk(pAlive->getAtk()+(pAlive->role->atk * (pEffect->getImpactRate()*0.01f-1)));
		}break;
	case AgilityRate:
		{
			pAlive->setAtkInterval(pAlive->getAtkInterval()+(pEffect->getImpactRate()*0.01f-1)*pAlive->role->atkInterval);
		}break;
	case RenewRate:
		{
			pAlive->setRenew(pAlive->getRenew()+(pAlive->role->renew * (pEffect->getImpactRate()*0.01f-1)));
		}break;
	case DogeRate:
		{
			pAlive->setDoge(pAlive->getDoge() + (pAlive->role->dodge * (pEffect->getImpactRate()*0.01f-1)));
		}break;
	case HitRate:
		{
			pAlive->setHit(pAlive->getHit() + (pAlive->role->hit * (pEffect->getImpactRate()*0.01f-1)));
		}break;
	case CritRate:
		{
			pAlive->setCrit(pAlive->getCrit() + (pAlive->role->crit * (pEffect->getImpactRate()*0.01f-1)));
		}break;
	case HpMaxRate:
		{
			pAlive->setMaxHp(pAlive->getMaxHp()+(pAlive->role->hp * (pEffect->getImpactRate()*0.01f-1)));
			pAlive->setHp(pAlive->getMaxHp());
		}break;
	default:
		break;
	}
}
