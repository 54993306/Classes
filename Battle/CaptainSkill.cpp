﻿#include "Battle/CaptainSkill.h"
#include "Battle/BattleCenter.h"
#include "Battle/BaseRole.h"
#include "Battle/SkillRange.h"
#include "Battle/WarManager.h"
#include "Battle/CoordsManage.h"
#include "Battle/ConstNum.h"
#include "Battle/RoleSkill.h"
#include "Battle/BaseRoleData.h"
#include "Battle/skEffectData.h"
namespace BattleSpace
{
	void CaptainSkill::ExecuteSkill(bool pOther /*=false*/)
	{
		BaseRole* tRole  = nullptr;
		CCArray* tArray = nullptr;
		if (pOther)
		{
			tRole  = BattleManage->getAliveByGrid(C_OtherCaptain);
			tArray = BattleManage->getMonsters(true);
		}else{
			tRole  = BattleManage->getAliveByGrid(C_CAPTAINGRID);
			tArray = BattleManage->getHeros(true);
		}
		if (!tRole||!tArray|| !tArray->count()||
			!tRole->getBaseData()->hasCaptainSkill())
			return;
		const RoleSkill* skill = tRole->getBaseData()->getCaptainSkill();							//需同时满足种族限制且满足属性限制
		CCArray* RaceArr = RaceJudgeCap(tArray,skill->getTargetType());			//种族限制判定
		if (!RaceArr||!RaceArr->count())
			return;																//不满足种族限制条件
		//CCArray* AttributeArr=AttributeJudgeArrCap(_CaptainSkill,RaceArr);	//属性限制判定(暂时取消)
		//if (!AttributeArr||!AttributeArr->count())
		//	return;																//满足种族限制未满足属性限制
		CCArray* AttributeArr = RaceArr;
		CCArray* executeArr = CCArray::create();
		for (int i=0;i<skill->getEffectSize(0);i++)							//触发队长技能效果
		{
			if (!skill->getIndexEffect(0,i)->getEffectID())
				continue;
			CCArray* _arr = getTargetArrCap(skill->getIndexEffect(0,i),AttributeArr,tArray);			
			AddArr(executeArr,_arr);
		}
		CCObject* ob = nullptr;
		CCARRAY_FOREACH(executeArr,ob)
		{
			BaseRole* alive = (BaseRole*)ob;									//只触发一次队长技,但是需要触发完队长技的效果才行	
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
			BaseRole* alive = (BaseRole*)ob;
			CCARRAY_FOREACH(pTargetArray,obj)
			{
				BaseRole* _alive = (BaseRole*)obj;
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
				sRoleNature type	= sRoleNature::eNull;
				sRoleNature type2	= sRoleNature::eNull;
				if (arr->count() < 3)break;
				CCARRAY_FOREACH(arr,obj)
				{
					BaseRole* alive = (BaseRole*)obj;
					if (alive->getExecuteCap())continue;
					if (type == sRoleNature::eNull || type2 == sRoleNature::eNull)
					{
						if (type == sRoleNature::eNull)
						{
							type = alive->getBaseData()->getProperty();
							continue;
						}
						type2 = alive->getBaseData()->getProperty();
						continue;
					}
					if (type != sRoleNature::eNull && type2 != sRoleNature::eNull)
					{
						if (alive->getBaseData()->getProperty() != type&&alive->getBaseData()->getProperty()!=type2)
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
						BaseRole* alive = (BaseRole*)ob;
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
			BaseRole* alive = (BaseRole*)obj;
			if (alive->getExecuteCap())continue;
			if (exclude)
			{
				if (alive->getBaseData()->getProperty() == (sRoleNature)type)
					continue;
				targetArr->addObject(obj);
			}else{
				if (alive->getBaseData()->getProperty() != (sRoleNature)type)
					continue;
				targetArr->addObject(obj);
			}
		}

		if (!exclude || !And)return targetArr;
		sRoleNature secondType = sRoleNature::eNull;
		bool fail = true;
		CCARRAY_FOREACH(targetArr,obj)
		{
			BaseRole* tRole = (BaseRole*)obj;
			if (secondType == sRoleNature::eNull)
			{
				secondType = tRole->getBaseData()->getProperty();
				continue;
			}
			if (secondType != sRoleNature::eNull&&secondType != tRole->getBaseData()->getProperty())
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
			BaseRole* alive = (BaseRole*)obj;
			if (AttributeJudge(alive,skill.getAffectType(),skill.getAffectRatio()))
				_arr->addObject(obj);
		}
		return _arr;
	}
	//属性判定
	bool CaptainSkill::AttributeJudge(BaseRole* alive,int type,int rate)
	{
		switch ((sAttribute)type)
		{
		case sAttribute::eNull:
			{
				return true;
			}break;
		case sAttribute::DefRate:		//除去NedHpRate是少于规定触发外其他都是进入战斗立马处理，扣减完后触发队长技处理
			{ 
				alive->setDef(alive->getBaseData()->getRoleDefense()*rate*0.01f);
				return true;	
			}break;
		case sAttribute::AtkRate:		
			{ 
				alive->setAtk(alive->getBaseData()->getRoleAttack()*rate*0.01f);			
				return true;	
			}break;
		case sAttribute::NedHpRate:			//血量少于某值触发效果
			{ 
				if (alive->getHp() <= alive->getBaseData()->getRoleHp()*rate*0.01f)
					return true;	
			}break;
		case sAttribute::RenewRate:		
			{ 
				alive->setRegain(alive->getBaseData()->getRoleRegain()*rate*0.01f);
				return true;	
			}break;
		case sAttribute::DogeRate:		
			{ 
				alive->setDoge(alive->getBaseData()->getRoleDodge()*rate*0.01f);	
				return true;	
			}break;
		case sAttribute::HitRate:		
			{ 
				alive->setHit(alive->getBaseData()->getRoleHit()*rate*0.01f);		
				return true;	
			}break;
		case sAttribute::CritRate:		
			{ 
				if (alive->getCrit() >= alive->getBaseData()->getRoleCrit()*rate*0.01f)			
					return true;	
			}break;
		case sAttribute::HpMaxRate:		
			{ 
				alive->setHp(alive->getBaseData()->getRoleHp()*rate*0.01f);				
				return true;	
			}break;
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
			BaseRole* alive = (BaseRole*)obj;
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
			BaseRole* alive = (BaseRole*)obj;
			if (alive->getExecuteCap())continue;	//执行过主帅技
			AliveExecute(pEffect,alive);
		}
	}
	//队长技效果武将属性变化
	void CaptainSkill::AliveExecute(const skEffectData*pEffect,BaseRole*pAlive)
	{
		switch (pEffect->getImpactType())
		{
		case sAttribute::DefRate:
			{
				pAlive->setDef(pAlive->getDef()+(pAlive->getBaseData()->getRoleDefense() * (pEffect->getImpactRate()*0.01f-1)));
			}break;
		case sAttribute::AtkRate:
			{
				pAlive->setAtk(pAlive->getAtk()+(pAlive->getBaseData()->getRoleAttack() * (pEffect->getImpactRate()*0.01f-1)));
			}break;
		case sAttribute::AgilityRate:
			{
				pAlive->setAtkInterval(pAlive->getAtkInterval()+(pEffect->getImpactRate()*0.01f-1)*pAlive->getBaseData()->getAttackSpeed());
			}break;
		case sAttribute::RenewRate:
			{
				pAlive->setRegain(pAlive->getRegain()+(pAlive->getBaseData()->getRoleRegain() * (pEffect->getImpactRate()*0.01f-1)));
			}break;
		case sAttribute::DogeRate:
			{
				pAlive->setDoge(pAlive->getDoge() + (pAlive->getBaseData()->getRoleDodge() * (pEffect->getImpactRate()*0.01f-1)));
			}break;
		case sAttribute::HitRate:
			{
				pAlive->setHit(pAlive->getHit() + (pAlive->getBaseData()->getRoleHit() * (pEffect->getImpactRate()*0.01f-1)));
			}break;
		case sAttribute::CritRate:
			{
				pAlive->setCrit(pAlive->getCrit() + (pAlive->getBaseData()->getRoleCrit() * (pEffect->getImpactRate()*0.01f-1)));
			}break;
		case sAttribute::HpMaxRate:
			{
				pAlive->setMaxHp(pAlive->getMaxHp()+(pAlive->getBaseData()->getRoleHp() * (pEffect->getImpactRate()*0.01f-1)));
				pAlive->setHp(pAlive->getMaxHp());
			}break;
		default:
			break;
		}
	}

};