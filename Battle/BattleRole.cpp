#include "Battle/BattleRole.h"
#include "model/BuffManage.h"
#include "warscene/ConstNum.h"
#include "scene/alive/ActObject.h"
#include "Battle/RoleBaseData.h"
#include "Battle/SkillMacro.h"
#include "Battle/RoleSkill.h"
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
	,m_Delaytime(0),m_AliveState(COMMONSTATE),m_AliveType(E_ALIVETYPE::Common),m_StateDelay(0)	
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
	mStandGrids.clear();
	moves.clear();
	mSkillArea.clear();
	mAreaTargets.clear();
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
	if (m_AtkNum >= 3)					//���Ĵι���Ϊ���⹥��
	{
		m_AtkNum = 0;
		if (role->skSpecial.getSkillID())
		{
			this->ResetAttackState();
			this->setSpeAtk(true);
		}
	}
}

void WarAlive::setHp(int hp)
{
	int max = getMaxHp();
	if (hp > max)			//����ֵ�ж�
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
	//���ӹ��١����ٹ���(�ٷֱȡ�ֱ����ֵ����)
	if (m_NorAtk == true)
		return;
	m_Atktime += var * m_TimePercent;				//������������ʹ��
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
//�����ͷű�ɱ��ʱ��
void WarAlive::setCritTime(float var)
{
	m_CritTime += var;
	if ( m_CritTime >= role->mCritTime)
	{
		if (role->skActive.getSkillID())
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
		mStandGrids.clear();
		for (int i=0;i<role->row;i++)
			for (int j =0;j<role->col;j++)
			{
				if (var+j*C_GRID_ROW+i>C_ENDGRID)
					continue;
				mStandGrids.push_back(var+j*C_GRID_ROW+i);
			}			
			sort(mStandGrids.begin(),mStandGrids.end());
	}
}

void WarAlive::setTouchGrid(int var)
{
	m_TouchGrid = var;
	mTouchGrids.clear();
	for (int i=0;i<role->row;i++)
		for (int j =0;j<role->col;j++)
			mTouchGrids.push_back(var+j*C_GRID_ROW+i);
	sort(mTouchGrids.begin(),mTouchGrids.end());
	int row = mTouchGrids.at(0)%C_GRID_ROW;				//��С���ӵ�վλ
	if (row+role->row>C_GRID_ROW)						//�佫��ռ����,���ܳ�����ͼ��
	{
		m_TouchGrid=INVALID_GRID;
		mTouchGrids.clear();
	}
}

void WarAlive::setCallType(int var)
{
	if (var == AutoSkill)							//����ս�����ͷż���(��ʯ��)
		setCriAtk( true );							//ֻ�б�ɱ����û�й���Ҳ���ͷŵ�
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
	setSortieNum(0);																//��ǰЧ����������
	setOpposite(false);																//���÷��򹥻�
	mAreaTargets.clear();
	mSkillArea.clear();
	clearHitAlive();
}

bool WarAlive::canSummonAlive()
{
	if (role->skActive.getSkillType() ==eCallAtk)
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

RoleSkill* WarAlive::getCurrSkill()
{
	if (getCriAtk())
	{
		if (role->skActive.getSkillID())
		{
			return &role->skActive;
		}else{
			CCLOG("[ *ERROR ] WarManager::getSkill CritSKILL IS NULL");
			return &role->skNormal;
		}
	}else if (getSpeAtk())
	{
		if (role->skSpecial.getSkillID())
		{
			return &role->skSpecial;
		}else{
			CCLOG("[ *ERROR ] WarManager::getSkill SpecialSKILL IS NULL");
			return &role->skNormal;
		}
	}else{
		return &role->skNormal;
	}
}

const skEffectData* WarAlive::getCurrEffect()
{
	if (getGroupIndex() < getCurrSkill()->getListSize())
		if ( getCurrSkill()->getEffectSize(getGroupIndex())>getEffIndex())
			return getCurrSkill()->getIndexEffect(getGroupIndex(),getEffIndex());
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
	int ranNum = CCRANDOM_0_1()*100;								//0��100����
	if (getGroupIndex() >= getCurrSkill()->getListSize)
		return false;
	if (getCurrSkill()->getSkillID()&&effId < getCurrSkill()->getEffectSize(getGroupIndex()))			//�ж��Ƿ�Ϊ����һ����Ч
	{
		if (getCurrSkill()->getIndexEffect(getGroupIndex(),effId)->getuserRate() > ranNum)
		{
			setEffIndex(effId);
			return true;
		}
	}
	return false;
}

int WarAlive::getSkillType()
{
	return getCurrSkill()->getSkillType();
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
	for (auto tAlive:mAreaTargets)
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
	if (getCurrEffect()->getAreaType() == ePuncture&&mAreaTargets.size())//�ᴩ��ǹᴩ�ദ��
	{
		WarAlive* alive = mAreaTargets.at(0);
		if (!alive->getCloaking())									//Ǳ������ﴦ��
			return true;
		mAreaTargets.clear();
	}
	return false;
}

void WarAlive::cloakingTarget()
{
	if (!getCriAtk()&& mAreaTargets.size())						//�ж��ܻ�Ŀ���Ƿ�ΪǱ�������							
	{
		for (auto tAlive:mAreaTargets)
		{
			if ( !tAlive->getCloaking() )
				return;
		}
		mAreaTargets.clear();										//����������״̬�޷�����ȫ��Ǳ�й���		
	}
}

bool WarAlive::standInGrid( int pGrid )
{
	if (pGrid >= C_CAPTAINGRID && getCaptain() )
		return true;
	for (auto tGrid : mStandGrids)
	{
		if (tGrid == pGrid)
			return true;
	}
	return false;
}

bool WarAlive::critJudge()
{
	if (!getCaptain()&&
		getCriAtk()&&
		!getCritEffect()&&
		getCallType() != AutoSkill)
	{
		setCritEffect(true);
		return true;
	}
	return false;
}
