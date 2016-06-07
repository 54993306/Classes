#include "Battle/BattleRole.h"
#include "model/BuffManage.h"
#include "warscene/ConstNum.h"
#include "scene/alive/ActObject.h"
#include "Battle/RoleBaseData.h"
#include "Battle/SkillMacro.h"
#include "Battle/RoleSkill.h"
namespace BattleSpace{
	WarAlive::WarAlive()
		:m_Enemy(false),m_Hp(0),m_MaxHp(0),m_GridIndex(INVALID_GRID),m_MoveGrid(0),m_AtkDelay(0)
		,m_AtkNum(0),m_Move(true),m_CritSkill(false),m_Hrt(0),m_HrtPe(0),m_AIState(false)
		,m_initCost(0),m_Batch(0),m_CostMax(0),m_AddCost(0),m_Atk(0),m_Def(0),m_Hit(0),m_NorAtk(true)
		,m_Doge(0),m_Crit(0),m_Zoom(0),m_Renew(0),m_GroupIndex(0),m_EffectIndex(0),m_LastAlive(false)
		,m_Opposite(false),m_ExecuteCap(false),m_TerType(0),m_TerTypeNum(0),m_cloaking(false)
		,m_SortieNum(0),m_ActObject(nullptr),m_BuffManage(nullptr),m_UILayout(0),m_Atktime(0)
		,m_AtkInterval(0),m_TimePercent(1),m_SpecialAtk(false),m_Battle(false),m_MoveSpeed(0)
		,m_CritTime(0),m_FatherID(0),m_Captain(false),m_CritEffect(false),m_DieState(false)
		,m_TouchGrid(0),m_TouchState(false),m_MoveObj(nullptr),m_CallType(0),m_CallAliveNum(0)
		,m_Delaytime(0),m_AliveState(COMMONSTATE),m_AliveType(E_ALIVETYPE::eCommon),m_StateDelay(0)	
		,m_Model(0),m_AliveID(0),m_MstType(0),mBaseData(nullptr)
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
		mSkillArea.clear();
		mAreaTargets.clear();
	}

	void WarAlive::initAliveByFather( WarAlive*pFather )
	{
		this->setModel(getBaseData()->getRoleModel());
		this->setZoom(this->getBaseData()->getRoleZoom()*0.01f);
		this->setInitCost(this->getBaseData()->getInitCost());									//召唤消耗的cost(根技能消耗的相等)
		if (pFather->getAliveType() == E_ALIVETYPE::eWorldBoss)
		{
			this->setAtkInterval(this->getBaseData()->getAttackSpeed());
			this->setMoveSpeed(this->getBaseData()->getMoveSpeed());
			this->setMaxHp(this->getBaseData()->getRoleHp());
			this->setHp(this->getBaseData()->getRoleHp());					//第一次进来是满血状态
			this->setAddCost(this->getBaseData()->getCostSpeed());
			this->setAtk(this->getBaseData()->getRoleAttack());
			this->setCrit(this->getBaseData()->getRoleCrit());
			this->setDef(this->getBaseData()->getRoleDefense());
			this->setHit(this->getBaseData()->getRoleHit());
			this->setRenew(this->getBaseData()->getRoleRegain());
			this->setDoge(this->getBaseData()->getRoleDodge());				//数值型是召唤它武将的百分比
		}else{
			this->setAtkInterval((this->getBaseData()->getAttackSpeed()*0.01f)*pFather->getBaseData()->getAttackSpeed());
			this->setMoveSpeed((this->getBaseData()->getMoveSpeed()*0.01f)*pFather->getBaseData()->getMoveSpeed());
			this->setMaxHp((this->getBaseData()->getRoleHp()*0.01f)*pFather->getBaseData()->getRoleHp());
			this->setHp((this->getBaseData()->getRoleHp()*0.01f)*pFather->getBaseData()->getRoleHp());							//第一次进来是满血状态		
			this->setAddCost((this->getBaseData()->getCostSpeed()*0.01f)*pFather->getBaseData()->getCostSpeed());
			this->setAtk((this->getBaseData()->getRoleAttack()*0.01f)*pFather->getBaseData()->getRoleAttack());
			this->setCrit((this->getBaseData()->getRoleCrit()*0.01f)*pFather->getBaseData()->getRoleCrit());
			this->setDef((this->getBaseData()->getRoleDefense()*0.01f)*pFather->getBaseData()->getRoleDefense());
			this->setHit((this->getBaseData()->getRoleHit()*0.01f)*pFather->getBaseData()->getRoleHit());
			this->setRenew((this->getBaseData()->getRoleRegain()*0.01f)*pFather->getBaseData()->getRoleRegain());
			this->setDoge((this->getBaseData()->getRoleDodge()*0.01f)*pFather->getBaseData()->getRoleDodge());				//数值型是召唤它武将的百分比
		}
	}

	void WarAlive::initAliveData()
	{
		mStandGrids.clear();
		if (getEnemy())															//把指针转化成相应的类型对特殊的敌方进行相应的初始化
		{
			setGridIndex(getBaseData()->getInitGrid());
		}else{
			if (getBaseData()->getCaptain())									//队长进来则为上阵状态(服务器用于区分队长的标志)
			{
				setCaptain(true);
				setGridIndex(C_CAPTAINSTAND);
				const skEffectData* effect = getBaseData()->getActiveSkill()->getSummonEffect();
				if (effect)
					setCallAliveNum(effect->getImpactType());
			}else{
				setGridIndex(INVALID_GRID);
			}
		}
		setExecuteCap(false);
		setDieState(false);
		setModel(getBaseData()->getRoleModel());
		this->setCallType(getBaseData()->getCallType());
		this->setAtkInterval(getBaseData()->getAttackSpeed());
		this->setMoveSpeed(getBaseData()->getMoveSpeed());
		if (this->getAliveType() == E_ALIVETYPE::eWorldBoss)
		{
			this->setMaxHp(getBaseData()->getMaxHp());
		}else{
			this->setMaxHp(getBaseData()->getRoleHp());
		}
		this->setDelaytime(getBaseData()->getDelayTime());
		this->setHp(getBaseData()->getRoleHp());									//第一次进来是满血状态
		this->setCostmax(getBaseData()->getMaxCost());
		this->setInitCost(getBaseData()->getInitCost());			
		this->setAddCost(getBaseData()->getCostSpeed());
		this->setAtk(getBaseData()->getRoleAttack());
		this->setCrit(getBaseData()->getRoleCrit());
		this->setDef(getBaseData()->getRoleDefense());
		this->setHit(getBaseData()->getRoleHit());
		this->setRenew(getBaseData()->getRoleRegain());
		this->setDoge(getBaseData()->getRoleDodge());
		this->setZoom(getBaseData()->getRoleZoom()*0.01f);
		this->setMove(true); 
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		if (this->getEnemy())
		{

		}else{		
			//alive->setAtk(500000);
			this->setMaxHp(500000);
			this->setHp(500000);		//第一次进来是满血状态
		}
#endif
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
			if (getBaseData()->hasSpecialSkill())
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
		if ( m_CritTime >= getBaseData()->getCritTime())
		{
			if (getBaseData()->hasActiveSkill())
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
			for (int i=0;i<getBaseData()->getRoleRow();i++)
				for (int j =0;j<getBaseData()->getRoleCol();j++)
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
		for (int i=0;i<getBaseData()->getRoleRow();i++)
			for (int j =0;j<getBaseData()->getRoleCol();j++)
				mTouchGrids.push_back(var+j*C_GRID_ROW+i);
		sort(mTouchGrids.begin(),mTouchGrids.end());
		int row = mTouchGrids.at(0)%C_GRID_ROW;				//最小格子的站位
		if (row+getBaseData()->getRoleRow()>C_GRID_ROW)						//武将所占格子,不能超出地图外
		{
			m_TouchGrid=INVALID_GRID;
			mTouchGrids.clear();
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
		mAreaTargets.clear();
		mSkillArea.clear();
		clearHitAlive();
	}

	bool WarAlive::canSummonAlive()
	{
		if (getBaseData()->getActiveSkillType() ==eCallAtk)
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

	const RoleSkill* WarAlive::getCurrSkill()
	{
		if (getCriAtk())
		{
			if (getBaseData()->hasActiveSkill())
			{
				return getBaseData()->getActiveSkill();
			}else{
				CCLOG("[ *ERROR ] WarAlive::getCurrSkill CritSKILL IS NULL");
				return getBaseData()->getNormalSkill();
			}
		}else if (getSpeAtk())
		{
			if (getBaseData()->hasSpecialSkill())
			{
				return getBaseData()->getSpecialSkill();
			}else{
				CCLOG("[ *ERROR ] WarAlive::getCurrSkill SpecialSKILL IS NULL");
				return getBaseData()->getNormalSkill();
			}
		}else{
			return getBaseData()->getNormalSkill();
		}
	}

	const skEffectData* WarAlive::getCurrEffect()
	{
		if (this->getGroupIndex() < this->getCurrSkill()->getListSize())
			if ( this->getCurrSkill()->getEffectSize(getGroupIndex())>getEffIndex())
				return this->getCurrSkill()->getIndexEffect(getGroupIndex(),getEffIndex());
			else
				CCLOG("[ *ERROR ] WarAlive::getCurrEffect EffIndex > EffectListSize  AliveID=%d mode=%d",getAliveID(),getBaseData()->getRoleModel());
		else{
			CCLOG("[ *ERROR ]WarAlive::getCurrEffect NULL  AliveID=%d mode=%d EffectGroup=%d",getAliveID(),getBaseData()->getRoleModel(),getGroupIndex());
			this->setGroupIndex(0);
		}	
		return nullptr;
		CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
	}

	bool WarAlive::NextEffect()
	{
		int effId = getEffIndex() + 1;
		int ranNum = CCRANDOM_0_1()*100;								//0到100的数
		if (getGroupIndex() >= getCurrSkill()->getListSize())
			return false;
		if (getCurrSkill()->getSkillID()&&effId < getCurrSkill()->getEffectSize(getGroupIndex()))			//判断是否为最后的一个特效
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
		if (getCurrEffect()->getAreaType() == ePuncture&&mAreaTargets.size())//贯穿与非贯穿类处理
		{
			WarAlive* alive = mAreaTargets.at(0);
			if (!alive->getCloaking())									//潜行类怪物处理
				return true;
			mAreaTargets.clear();
		}
		return false;
	}

	void WarAlive::cloakingTarget()
	{
		if (!getCriAtk()&& mAreaTargets.size())						//判断受击目标是否为潜行类怪物							
		{
			for (auto tAlive:mAreaTargets)
			{
				if ( !tAlive->getCloaking() )
					return;
			}
			mAreaTargets.clear();										//非主动技能状态无法攻击全部潜行怪物		
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
};