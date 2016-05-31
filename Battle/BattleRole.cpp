#include "Battle/BattleRole.h"
#include "model/BuffManage.h"
#include "warscene/ConstNum.h"
#include "scene/alive/ActObject.h"
#include "Battle/TempData.h"
#include "Battle/SkillMacro.h"

WarAlive::WarAlive()
	:m_Enemy(false),m_Hp(0),m_MaxHp(0),m_GridIndex(INVALID_GRID),m_MoveGrid(0),m_AtkDelay(0)
	,m_AtkNum(0),m_Move(true),m_CritSkill(false),role(nullptr),m_Hrt(0),m_HrtPe(0),m_AIState(false)
	,m_initCost(0),m_Batch(0),m_CostMax(0),m_AddCost(0),m_Atk(0),m_Def(0),m_Hit(0),m_NorAtk(true)
	,m_Doge(0),m_Crit(0),m_Zoom(0),m_Renew(0),m_GroupIndex(0),m_EffectIndex(0),m_LastAlive(false)
	,m_Opposite(false),m_ExecuteCap(false),m_TerType(0),m_TerTypeNum(0),m_cloaking(false)
	,m_SortieNum(0),m_ActObject(nullptr),m_BuffManage(nullptr),m_UILayout(0),m_Atktime(0)
	,m_AtkInterval(0),m_TimePercent(1),m_SpecialAtk(false),m_Battle(false),m_MoveSpeed(0)
	,m_CritTime(0),m_FatherID(0),m_Captain(false),m_CritEffect(false),m_DieState(false)
	,m_TouchGrid(0),m_TouchState(false),m_MoveObj(nullptr),m_CallType(0),m_CallAliveNum(0)
	,m_Delaytime(0),m_AliveState(COMMONSTATE),m_AliveType(AliveType::Common),m_StateDelay(0)	
	,m_Model(0),m_AliveID(0),m_MstType(0)
{
	setBuffManage(nullptr);
}

WarAlive::~WarAlive()
{
	CC_SAFE_RELEASE(m_BuffManage);
	m_BuffManage = nullptr;
	m_ActObject = nullptr;
	m_MoveObj = nullptr;
	m_StandGrids.clear();
	moves.clear();
	AtkArea.clear();
	AliveS.clear();
	m_SkillArea.clear();
	m_AreaTargets.clear();
}

void WarAlive::setMoveGrid(int var)
{
	if (var == INVALID_GRID)
		CCLOG("[ TIPS ] WarAlive::setMoveGrid");
	m_MoveGrid = var;
}

void WarAlive::setBuffManage(BuffManage*)
{
	m_BuffManage = BuffManage::create();
	m_BuffManage->retain();
}

void WarAlive::setAtkNum(int var)
{
	m_AtkNum += var;
	if (m_AtkNum >= 3)					//第四次攻击为特殊攻击
	{
		m_AtkNum = 0;
		if (role->skill2.skillId)
		{
			this->ResetAttackState();
			this->setSpeAtk(true);
		}
	}
}

void WarAlive::setHp(int hp)
{
	int max = getMaxHp();
	if (hp > max)			//做最值判断
	{
		m_Hp = max;
	}else{
		m_Hp = hp;
		if (m_Hp<0)
			m_Hp = 0;
	}
}

void WarAlive::setDef(int def)
{
	m_Def = def;
	if (m_Def<=0)
		m_Def = 0;
}

void WarAlive::setNorAtk(bool var)
{
	m_NorAtk = var;
	if (!m_NorAtk)
		m_Atktime = 0;
}

void WarAlive::setAtktime(float var)
{
	//增加攻速、减少攻速(百分比、直接数值两类)
	if (m_NorAtk == true)
		return;
	m_Atktime += var * m_TimePercent;				//攻速增减参数使用
	if (m_Atktime >= m_AtkInterval)
	{
		m_Atktime = m_AtkInterval;
		this->ResetAttackState();
		this->setNorAtk(true);
	}
	char time[60] = {0};
	sprintf(time, "%.2f / %.2f",m_Atktime,m_AtkInterval);
	m_ActObject->setNickName(time);
}
//怪物释放必杀技时间
void WarAlive::setCritTime(float var)
{
	m_CritTime += var;
	if ( m_CritTime >= role->mCritTime)
	{
		if (role->skill3.skillId)
		{
			this->ResetAttackState();
			this->setCriAtk(true);
		}
		m_CritTime = 0;
	}
}

void WarAlive::setDelaytime(float var) { m_Delaytime -= var; }

void WarAlive::setAtkDelay(float var){m_AtkDelay = var;}

void WarAlive::setAliveStat(int var)
{
	m_AliveState = var;
}

void WarAlive::setStatDelay(float var)
{
	if (var <0)
	{
		m_StateDelay = 0;
		m_AliveState = COMMONSTATE;
	}else{
		m_StateDelay = var;
	}
}

void WarAlive::setGridIndex(int var)
{
	if (var == m_GridIndex||m_Delaytime>0)
		return;
	m_GridIndex = var;
	if (var <= INVALID_GRID )
	{
		m_Battle = false;
	}else{
		m_Battle =true;
		m_StandGrids.clear();
		for (int i=0;i<role->row;i++)
			for (int j =0;j<role->col;j++)
			{
				if (var+j*C_GRID_ROW+i>C_ENDGRID)
					continue;
				m_StandGrids.push_back(var+j*C_GRID_ROW+i);
			}			
			sort(m_StandGrids.begin(),m_StandGrids.end());
	}
}

void WarAlive::setTouchGrid(int var)
{
	if (var == m_TouchGrid)
		return;
	m_TouchGrid = var;
	TouchGrids.clear();
	for (int i=0;i<role->row;i++)
		for (int j =0;j<role->col;j++)
			TouchGrids.push_back(var+j*C_GRID_ROW+i);
	sort(TouchGrids.begin(),TouchGrids.end());
	int row = TouchGrids.at(0)%C_GRID_ROW;				//最小格子的站位
	if (row+role->row>C_GRID_ROW)						//武将所占格子,不能超出地图外
	{
		m_TouchGrid=INVALID_GRID;
		TouchGrids.clear();
	}
}

void WarAlive::setCallType(int var)
{
	if (var == AutoSkill)							//进入战场就释放技能(陨石类)
		setCriAtk( true );							//只有必杀技是没有怪物也会释放的
	m_CallType = var;
}

void WarAlive::ResetAttackState()
{
	if (!m_Enemy && m_CritSkill)
	{
		setNorAtk(true);
	}else{
		setNorAtk(false);
	}
	setCritEffect(false);
	setCriAtk(false);
	setSpeAtk(false);
	setEffIndex(0);
	setGroupIndex(0);
	ExcuteNextEffect();
}

void WarAlive::ExcuteNextEffect()
{
	setSortieNum(0);																//当前效果攻击次数
	setOpposite(false);																//重置反向攻击
	AtkArea.clear();																//清除随机固定格子(好像还是逻辑的)
	AliveS.clear();																	//清除随机固定武将
	m_AreaTargets.clear();
	m_SkillArea.clear();
	clearHitAlive();
}

bool WarAlive::canSummonAlive()
{
	if (role->skill3.skillType ==CallAtk)
	{
		if (getCaptain() && getCallAliveNum() < 1)
			return false;
		return true;
	}
	return false;
}

void WarAlive::clearHitAlive()
{
	for (auto alive:HittingAlive)
	{
		if (alive == this)
			continue;
		if (!alive->getDieState()&&alive->getHp()<=0 && alive->getActObject())
			alive->getActObject()->AliveDie();
	}
	HittingAlive.clear();
}

TempSkill* WarAlive::getCurrSkill()
{
	if (getCriAtk())
	{
		if (role->skill3.skillId)
		{
			return &role->skill3;
		}else{
			CCLOG("[ *ERROR ] WarManager::getSkill CritSKILL IS NULL");
			return &role->skill1;
		}
	}else if (getSpeAtk())
	{
		if (role->skill2.skillId)
		{
			return &role->skill2;
		}else{
			CCLOG("[ *ERROR ] WarManager::getSkill SpecialSKILL IS NULL");
			return &role->skill1;
		}
	}else{
		return &role->skill1;
	}
}

SkillEffect* WarAlive::getCurrEffect()
{
	if (getGroupIndex() < getCurrSkill()->EffectList.size())
		if ( getCurrSkill()->EffectList.at(getGroupIndex()).size()>getEffIndex())
			return & getCurrSkill()->EffectList.at(getGroupIndex()).at(getEffIndex());
		else
			CCLOG("[ *ERROR ] EffIndex > EffectListSize  AliveID=%d mode=%d",getAliveID(),role->thumb);
	else{
		CCLOG("[ *ERROR ]Skill Effect Is NULL  AliveID=%d mode=%d EffectGroup=%d",getAliveID(),role->thumb,getGroupIndex());
		setGroupIndex(0);
	}	
	return nullptr;
	CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
}

bool WarAlive::NextEffect()
{
	int effId = getEffIndex() + 1;
	int ranNum = CCRANDOM_0_1()*100;								//0到100的数
	if (getGroupIndex() >= getCurrSkill()->EffectList.size())
		return false;
	if (getCurrSkill()->skillId&&effId < getCurrSkill()->EffectList.at(getGroupIndex()-1).size())			//判断是否为最后的一个特效
	{
		if (getCurrSkill()->EffectList.at(getGroupIndex()).at(effId).userRate > ranNum)
		{
			setEffIndex(effId);
			return true;
		}
	}
	return false;
}

int WarAlive::getSkillType()
{
	return getCurrSkill()->skillType;
}

int WarAlive::getEffectType()
{
	if (getCurrEffect())
		return getCurrEffect()->Efftype;
	else{
		CCLOG("[ *ERROR ] BattleRole::getEffectType ");
		return 0;
	}
}

bool WarAlive::captainCallNumberJudge()
{
	if (getCaptain() )
	{
		if (getCallAliveNum()<1)
		{
			return true;
		}else{
			setCallAliveNum(getCallAliveNum() - 1);
		}
	}
	return false;
}

bool WarAlive::hasAliveByTargets( WarAlive* pAlive )
{
	bool tHasAlive = false;
	for (auto tAlive:m_AreaTargets)
	{
		if (tAlive->getAliveID() == pAlive->getAliveID())
		{
			tHasAlive = true;
			break;
		}
	}
	return tHasAlive;
}

bool WarAlive::pierceJudge()
{
	if (getCurrEffect()->mode == ePuncture&&m_AreaTargets.size())//贯穿与非贯穿类处理
	{
		WarAlive* alive = m_AreaTargets.at(0);
		if (!alive->getCloaking())									//潜行类怪物处理
			return true;
		m_AreaTargets.clear();
	}
	return false;
}

void WarAlive::cloakingTarget()
{
	if (!getCriAtk()&& m_AreaTargets.size())						//判断受击目标是否为潜行类怪物							
	{
		for (auto tAlive:m_AreaTargets)
		{
			if ( !tAlive->getCloaking() )
				return;
		}
		m_AreaTargets.clear();										//非主动技能状态无法攻击全部潜行怪物		
	}
}

bool WarAlive::standInGrid( int pGrid )
{
	if (pGrid >= C_CAPTAINGRID && getCaptain() )
		return true;
	for (auto tGrid : m_StandGrids)
	{
		if (tGrid == pGrid)
			return true;
	}
	return false;
}
