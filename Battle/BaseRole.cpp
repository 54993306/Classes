#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
#include "Battle/RoleSkill.h"
#include "Battle/skEffectData.h"
#include "Battle/BuffData.h"
#include "Battle/BattleMessage.h"
#include "Battle/MoveObject.h"
#include "Battle/GuardArea.h"
#include "Battle/MonsterData.h"
#include "Battle/BattleDataCenter.h"
#include "Battle/StateManage/StateManager.h"

#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "model/BuffManage.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/ConstNum.h"
#include "Battle/SkillRange.h"
#include "Battle/BattleTools.h"
#include "Battle/HurtCount.h"
#include "Battle/CombatGuideManage.h"
#include "Battle/BattleLayer/BattleRoleLayer.h"

namespace BattleSpace
{
	BaseRole::BaseRole()
		:m_Enemy(false),m_Hp(0),m_MaxHp(0),mGridIndex(INVALID_GRID),m_MoveGrid(0),m_AtkDelay(0)
		,m_AtkNum(0),m_Move(true),m_CritSkill(false),m_Hrt(0),m_HrtPe(0),m_AIState(false)
		,m_initCost(0),m_Batch(0),m_CostMax(0),m_AddCost(0),m_Atk(0),m_Def(0),m_Hit(0),m_NorAtk(true)
		,m_Doge(0),m_Crit(0),m_Zoom(0),m_Renew(0),m_GroupIndex(0),m_EffectIndex(0),m_LastAlive(false)
		,m_Opposite(false),m_ExecuteCap(false),m_TerType(0),m_TerTypeNum(0),m_cloaking(false)
		,m_SortieNum(0),mRoleObject(nullptr),mBuffManage(nullptr),m_UILayout(0),m_Atktime(0)
		,m_AtkInterval(0),m_SpecialAtk(false),mBattle(false),m_MoveSpeed(0),mSkillRange(nullptr)
		,m_CritTime(0),m_FatherID(0),m_Captain(false),m_CritEffect(false),mAliveState(true)
		,m_TouchGrid(0),m_TouchState(false),mMoveObject(nullptr),mCallType(sCallType::eCommon),m_CallAliveNum(0)
		,mDelaytime(0),mLogicState(sLogicState::eNormal),mMonsterSpecies(sMonsterSpecies::eCommon),mHurtCount(nullptr)
		,m_Model(0),m_AliveID(0),mBehavior(sBehavior::eNormal),mBaseData(nullptr),mLogicData(nullptr),mGuideManage(nullptr)
		,mCommandGrid(0),mRoleLayer(nullptr),mAutoState(false),mMaxGrid(INVALID_GRID),mHasTarget(false)
	{}
	//AliveDefine
	BaseRole::~BaseRole()
	{
		CC_SAFE_RELEASE(mBuffManage);
		mBuffManage = nullptr;
		CC_SAFE_RELEASE(mSkillRange);
		mSkillRange = nullptr;
		CC_SAFE_RELEASE(mGuardArea);
		mGuardArea = nullptr;
		mRoleObject = nullptr;
		mMoveObject = nullptr;
		mStandGrids.clear();
		mSkillArea.clear();
		mAreaTargets.clear();
	}

	bool BaseRole::init()
	{
		mManage = DataCenter::sharedData()->getWar();
		mGuideManage = DataCenter::sharedData()->getCombatGuideMg();

		mBuffManage = BuffManage::create();
		mBuffManage->setAlive(this);
		mBuffManage->retain();

		mSkillRange = SkillRange::create(mManage);
		mSkillRange->retain();

		mGuardArea = GuardArea::create(mManage);
		mGuardArea->retain();

		mHurtCount = HurtCount::create();
		mHurtCount->retain();
		return true;
	}

	void BaseRole::initAliveByFather( BaseRole*pFather )
	{
		setModel(getBaseData()->getRoleModel());
		setZoom(getBaseData()->getRoleZoom()*0.01f);
		setInitCost(getBaseData()->getInitCost());				//召唤消耗的cost(根技能消耗的相等)
		if (pFather->getMonsterSpecies() == sMonsterSpecies::eWorldBoss)
		{
			setAtkInterval(getBaseData()->getAttackSpeed());
			setMoveSpeed(getBaseData()->getMoveSpeed());
			setMaxHp(getBaseData()->getRoleHp());
			setHp(getBaseData()->getRoleHp());					//第一次进来是满血状态
			setAddCost(getBaseData()->getCostSpeed());
			setAtk(getBaseData()->getRoleAttack());
			setCrit(getBaseData()->getRoleCrit());
			setDef(getBaseData()->getRoleDefense());
			setHit(getBaseData()->getRoleHit());
			setRenew(getBaseData()->getRoleRegain());
			setDoge(getBaseData()->getRoleDodge());				//数值型是召唤它武将的百分比
		}else{
			setAtkInterval((getBaseData()->getAttackSpeed()*0.01f)*pFather->getBaseData()->getAttackSpeed());
			setMoveSpeed((getBaseData()->getMoveSpeed()*0.01f)*pFather->getBaseData()->getMoveSpeed());
			setMaxHp((getBaseData()->getRoleHp()*0.01f)*pFather->getBaseData()->getRoleHp());
			setHp((getBaseData()->getRoleHp()*0.01f)*pFather->getBaseData()->getRoleHp());							//第一次进来是满血状态		
			setAddCost((getBaseData()->getCostSpeed()*0.01f)*pFather->getBaseData()->getCostSpeed());
			setAtk((getBaseData()->getRoleAttack()*0.01f)*pFather->getBaseData()->getRoleAttack());
			setCrit((getBaseData()->getRoleCrit()*0.01f)*pFather->getBaseData()->getRoleCrit());
			setDef((getBaseData()->getRoleDefense()*0.01f)*pFather->getBaseData()->getRoleDefense());
			setHit((getBaseData()->getRoleHit()*0.01f)*pFather->getBaseData()->getRoleHit());
			setRenew((getBaseData()->getRoleRegain()*0.01f)*pFather->getBaseData()->getRoleRegain());
			setDoge((getBaseData()->getRoleDodge()*0.01f)*pFather->getBaseData()->getRoleDodge());				//数值型是召唤它武将的百分比
		}
	}

	void BaseRole::initAliveData()
	{
		mStandGrids.clear();
		setDelaytime(getBaseData()->getDelayTime());
		setModel(getBaseData()->getRoleModel());
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
					setCallAliveNum(effect->getCallNumber());
			}else{
				setGridIndex(INVALID_GRID);
			}
		}
		setHasTarget(false);
		setNorAtk(true);
		setExecuteCap(false);
		setAliveState(true);
		setCallType(getBaseData()->getCallType());
		setAtkInterval(getBaseData()->getAttackSpeed());
		setMoveSpeed(getBaseData()->getMoveSpeed());
		if (getMonsterSpecies() == sMonsterSpecies::eWorldBoss)
		{
			setMaxHp(getBaseData()->getMaxHp());
		}else{
			setMaxHp(getBaseData()->getRoleHp());
		}
		setHp(getBaseData()->getRoleHp());									//第一次进来是满血状态
		setCostmax(getBaseData()->getMaxCost());
		setInitCost(getBaseData()->getInitCost());			
		setAddCost(getBaseData()->getCostSpeed());
		setAtk(getBaseData()->getRoleAttack());
		setCrit(getBaseData()->getRoleCrit());
		setDef(getBaseData()->getRoleDefense());
		setHit(getBaseData()->getRoleHit());
		setRenew(getBaseData()->getRoleRegain());
		setDoge(getBaseData()->getRoleDodge());
		setZoom(getBaseData()->getRoleZoom()*0.01f);
	}

	void BaseRole::setAtkNum(int var)
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

	void BaseRole::setHp(int hp)
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

	void BaseRole::setDef(int def)
	{
		m_Def = def;
		if ( m_Def <= 0 )
			m_Def = 0;
	}

	void BaseRole::setNorAtk(bool var)
	{
		m_NorAtk = var;
		if ( !m_NorAtk )
			m_Atktime = 0;
	}

	void BaseRole::setAtktime(float var)
	{
		//增加攻速、减少攻速(百分比、直接数值两类)
		if (m_NorAtk == true)
			return;
		m_Atktime += var;
		if (m_Atktime >= m_AtkInterval)
		{
			m_Atktime = m_AtkInterval;
			this->ResetAttackState();
			this->setNorAtk(true);
		}
		char time[60] = {0};
		sprintf(time, "%.2f / %.2f",m_Atktime,m_AtkInterval);
		mRoleObject->setNickName(time);
	}
	//怪物释放必杀技时间
	void BaseRole::setCritTime(float var)
	{
		m_CritTime += var;
		if ( m_CritTime >= getBaseData()->getCritTime())
		{
			if (getBaseData()->hasActiveSkill())
			{
				ResetAttackState();
				setCriAtk(true);
			}
			m_CritTime = 0;
		}
	}

	void BaseRole::setBattle(bool pBattle)
	{
		if (pBattle != mBattle)
		{
			mBattle = pBattle;
			bNotification->postNotification(MsgBattleStateChange,this);
		}
		if (!pBattle)
			setGridIndex(INVALID_GRID);
	}

	bool BaseRole::getBattle()
	{
		if (getGridIndex() > INVALID_GRID && 
			getAliveState() &&
			getHp() > 0)
		{
			return true;
		}
		return false;
	}

	int BaseRole::getMaxGrid(void) const
	{
		return mStandGrids.at(mStandGrids.size()-1);
	}

	void BaseRole::initContainGrid( int pGrid,vector<int>& pVector )
	{
		pVector.clear();
		for (int tRow=0;tRow<getBaseData()->getRoleRow();tRow++)
		{
			for (int tCol =0;tCol<getBaseData()->getRoleCol();tCol++)
			{
				if (pGrid+tCol*C_GRID_ROW+tRow>C_ENDGRID)
					continue;
				pVector.push_back(pGrid+tCol*C_GRID_ROW+tRow);
			}	
		}
		sort(pVector.begin(),pVector.end());
		if (borderJudge(pVector))
			pVector.clear();
	}

	void BaseRole::setGridIndex(int pGrid)
	{
		if (pGrid == mGridIndex || mDelaytime>0)
			return;
		if (mGridIndex < C_BEGINGRID && pGrid >= C_BEGINGRID)
			bNotification->postNotification(MsgRoleEnterBattle,this);
		mGridIndex = pGrid;
		if (mGridIndex > INVALID_GRID && !getCaptain())
		{
			initContainGrid(mGridIndex,mStandGrids);
			setBattle(true);
			bNotification->postNotification(MsgRoleGridChange,this);
		}
	}

	void BaseRole::setTouchGrid(int var)
	{
		m_TouchGrid = var;
		mTouchGrids.clear();
		for (int i=0;i<getBaseData()->getRoleRow();i++)
			for (int j =0;j<getBaseData()->getRoleCol();j++)
				mTouchGrids.push_back(var+j*C_GRID_ROW+i);
		sort(mTouchGrids.begin(),mTouchGrids.end());
		int row = mTouchGrids.at(0)%C_GRID_ROW;				//最小格子的站位
		if (row+getBaseData()->getRoleRow() > C_GRID_ROW)						//武将所占格子,不能超出地图外
		{
			m_TouchGrid=INVALID_GRID;
			mTouchGrids.clear();
		}
	}

	void BaseRole::setCallType(sCallType var)
	{
		if (var == sCallType::eAutoSkill)							//进入战场就释放技能(陨石类)
			setCriAtk( true );							//只有必杀技是没有怪物也会释放的
		mCallType = var;
	}

	void BaseRole::ResetAttackState()
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

	void BaseRole::ExcuteNextEffect()
	{
		setSortieNum(0);																//当前效果攻击次数
		setOpposite(false);																//重置反向攻击
		mAreaTargets.clear();
		mSkillArea.clear();
		clearHitAlive();
	}

	bool BaseRole::canSummonAlive()
	{
		if (getBaseData()->getActiveSkillType() == eCallAtk)
		{
			if (getCaptain() && getCallAliveNum() < 1)
				return false;
			return true;
		}
		return false;
	}

	void BaseRole::clearHitAlive()
	{
		for (auto tRole:HittingAlive)
		{
			if (tRole == this)
				continue;
			if (tRole->getAliveState()&&tRole->getHp()<=0 && tRole->getRoleObject())
				tRole->getRoleObject()->AliveDie();
		}
		HittingAlive.clear();
	}

	RoleSkill* BaseRole::getCurrSkill()
	{
		if (getCriAtk())
		{
			if (getBaseData()->hasActiveSkill())
			{
				return getBaseData()->getActiveSkill();
			}else{
				CCLOG("[ *ERROR ] BaseRole::getCurrSkill CritSKILL IS NULL");
				return getBaseData()->getNormalSkill();
			}
		}else if (getSpeAtk())
		{
			if (getBaseData()->hasSpecialSkill())
			{
				return getBaseData()->getSpecialSkill();
			}else{
				CCLOG("[ *ERROR ] BaseRole::getCurrSkill SpecialSKILL IS NULL");
				return getBaseData()->getNormalSkill();
			}
		}else{
			return getBaseData()->getNormalSkill();
		}
	}

	skEffectData* BaseRole::getCurrEffect()
	{
		if (this->getGroupIndex() < this->getCurrSkill()->getListSize())
			if ( this->getCurrSkill()->getEffectSize(getGroupIndex())>getEffIndex())
				return this->getCurrSkill()->getIndexEffect(getGroupIndex(),getEffIndex());
			else
				CCLOG("[ *ERROR ] BaseRole::getCurrEffect EffIndex > EffectListSize  AliveID=%d mode=%d",getAliveID(),getBaseData()->getRoleModel());
		else{
			CCLOG("[ *ERROR ]BaseRole::getCurrEffect NULL  AliveID=%d mode=%d EffectGroup=%d",getAliveID(),getBaseData()->getRoleModel(),getGroupIndex());
			this->setGroupIndex(0);
		}	
		return nullptr;
		CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
	}

	bool BaseRole::NextEffect()
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

	int BaseRole::getSkillType()
	{
		return getCurrSkill()->getSkillType();
	}

	bool BaseRole::captainCallNumberJudge()
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

	bool BaseRole::hasAliveByTargets( BaseRole* pAlive )
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

	void BaseRole::cloakingTarget()
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

	bool BaseRole::inStandGrid( int pGrid )
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

	bool BaseRole::inStandVector( vector<int>& pGrids )
	{
		if (!getMoveObject())return false;
		for (auto tGrid : pGrids)
		{
			if (tGrid >= C_CAPTAINGRID || tGrid < C_BEGINGRID)
				return true;
			for (auto tStandGrid : getMoveObject()->grids)
			{
				if (tGrid == tStandGrid)
					return true;
			}
		}
		return false;
	}

	bool BaseRole::critJudge()
	{
		if (!getCaptain() && getCriAtk()&& !getCritEffect() 
			&& getCallType() != sCallType::eAutoSkill )
		{
			setCritEffect(true);
			return true;
		}
		return false;
	}

	vector<BaseRole*>* BaseRole::getSkillTargets()
	{
		int tTarget = getCurrEffect()->getTargetType();
		if ((getEnemy()&&tTarget == eUsType)	|| 
			(!getEnemy()&&tTarget == eEnemyType) )			/*敌方自己，我方敌人*/
			return mManage->inBattleMonsters(true);
		if ((getEnemy()&&tTarget == eEnemyType) ||
			(!getEnemy()&&tTarget == eUsType))				/*敌方敌人，我方自己*/
			return mManage->inBattleHeros(true);
		return mManage->inBattleRoles(true); 
	}

	void BaseRole::excuteLogic(float pTime)
	{
		if (!getRoleObject())return;
		this->getRoleObject()->updateFrameEvent(pTime);									//刷新武将帧事件
		if (stateDispose(pTime) || !AttackJudge())
			return;										
		if (getEnemy())
		{
			this->MonsterExcuteAI(pTime);
		}else{
			this->HeroExcuteAI();			
		}
	}

	bool BaseRole::stateDispose(float pTime)
	{
		if (getCallType()   == sCallType::eNotAttack	||							//石头类武将不做攻击判断处理
			getLogicState() == sLogicState::eUnAttack	||
			getLogicState() == sLogicState::eInvincible	||							//超出边界的武将不再进行技能处理
			getHp() <= 0 || !this->getAliveState()		)
			return true;
		if (delayEntrance(pTime))
			return true;
		if (autoSkillAlive())
			return false;
		attackTime(pTime);
		return false;
	}

	bool BaseRole::delayEntrance( float pTime )
	{
		if ( getBattle() )
		{
			if (getAtkDelay()>0)
			{
				setAtkDelay(getAtkDelay() - pTime);									//敌方武将释放必杀技攻击延迟时间
				return true;
			}
			return false;
		}
		if (getBaseData()->getDelayTime())
		{
			if (getDelaytime()<=0)
			{
				setGridIndex(getBaseData()->getInitGrid());
				getRoleLayer()->AddActToGrid(getRoleObject(),getGridIndex());
				if (converCol(getGridIndex()) > C_GRID_COL_MIN)	//判断是否进入战区
					getRoleObject()->TurnStateTo(sStateCode::eEnterState);
			}else{
				setDelaytime(getDelaytime() - pTime);
			}
		}
		return true;
	}

	bool BaseRole::autoSkillAlive()
	{
		if ((getBaseData()->getCallType() == sCallType::eAutoSkill  || 
			getBaseData()->getBehavior() == sBehavior::eAutoSkill) && !getCriAtk())						//进入战场就释放技能(陨石类)
		{
			if (getLogicState()==sLogicState::eNormal)
			{
				if (getBaseData()->getBehavior() == sBehavior::eAutoSkill)
					getBaseData()->setBehavior(sBehavior::eNormal);					//改变了逻辑的数值   这份数据应该是不可变的
				setCriAtk( true );
				return true;
			}
		}
		return false;
	}

	void BaseRole::attackTime( float pTime )
	{
		sStateCode ActionCode = getRoleObject()->getCurrActionCode();
		if (ActionCode == sStateCode::eStandState	|| 
			ActionCode == sStateCode::eHitState	|| 
			ActionCode == sStateCode::eWalkState	)//怪物的技能释放是由攻击次数和时间确定的
		{
			setAtktime(pTime);
			if (!getCriAtk()&&getEnemy())
				setCritTime(pTime);
		}
		getBuffManage()->upDateBuff(pTime);									// BUFF应该是每一帧都自动处理的
	}

	bool BaseRole::AttackJudge()
	{
		sStateCode ActionCode = getRoleObject()->getCurrActionCode();
		if (ActionCode == sStateCode::eWalkState)
			return this->walkState();

		if (ActionCode == sStateCode::eDizzyState			||
			ActionCode == sStateCode::eActiveAttackState	||
			ActionCode == sStateCode::eSpecialAttackState	||
			ActionCode == sStateCode::eNormalAttackState	||
			ActionCode == sStateCode::eVictoryState			||
			ActionCode == sStateCode::eDeathState)
		{
			if (getCloaking())
			{
				setCloaking(false);
				((CCArmature*)getRoleObject()->getArmature())->setOpacity(255);
			}
			return false;
		}
		return aliveAttackState();
	}

	bool BaseRole::walkState()
	{
		setHasTarget(false);
		if (getEnemy() || getCallType()==sCallType::eAutoMove)		//我方自动移动类武将
		{
			return true;
		}else{
			if (getAIState())
			{
				return true;
			}else{
				return false;
			}
		}
	}

	bool BaseRole::aliveAttackState()
	{
		if (getSpeAtk())
		{
			return true;
		}else if (getNorAtk())
		{
			return true;
		}else if (getCriAtk())
		{
			return true;
		}else{
			return false;
		}
	}

	void BaseRole::MonsterExcuteAI( float dt )
	{
		if (monsterFlee())												//骸兽逃跑处理,将AI执行的都放在这个地方后面好整理
			return;
		sStateCode ActionCode = getRoleObject()->getCurrActionCode();
		if (getGridIndex()<C_BEGINGRID)							//@@每个怪物都有的方法
		{
			if (getRoleObject()->getMoveState() == sStateCode::eNullState
				&& MonstMoveExcute())
				getRoleObject()->setMoveState(sStateCode::eWalkState);
			return ;
		}
		mSkillRange->initAttackInfo(this);
		if (mAreaTargets.size())
		{
			if (ActionCode == sStateCode::eWalkState)
			{
				getRoleObject()->setMoveState(sStateCode::eNullState);
				getRoleObject()->TurnStateTo(sStateCode::eStandState);
			}
			AliveExcuteAI();
		}else{
			if (getSkillType() == eCallAtk)
			{
				getRoleObject()->setMoveState(sStateCode::eNullState);
				getRoleObject()->TurnStateTo(sStateCode::eStandState);
				AliveExcuteAI(); 
			}else{
				if (getCriAtk())
					setCriAtk(false);			
				if (getRoleObject()->getMoveState() == sStateCode::eNullState && MonstMoveExcute() )
					getRoleObject()->setMoveState(sStateCode::eWalkState);
			}
		}
	}
	/****************************** Move ****************************************/
	bool BaseRole::monsterFlee()
	{
		if (mManage->getStageID())				//当前是为新手引导关卡,可以抽象出一个方法用于判断是否为新手关卡
			return false;
		if (getModel() == 1056  
			&& ((getHp()*1.0f/getMaxHp()) < 0.60f))					//武将需要一个得到当前血量百分比的方法
		{
			if (getRoleObject()->getMoveState() == sStateCode::eNullState)
			{
				setMoveSpeed(0.5f);
				setMoveGrid(4);
				getRoleObject()->setMoveState(sStateCode::eWalkState);
				setDef(1000000);
			}
			if (getGridIndex()<= C_BEGINGRID-C_GRID_ROW*4)
			{		
				getRoleObject()->AliveDie();
				getRoleObject()->setVisible(false);
			}		
			return true;
		}
		return false;
	}

	bool BaseRole::MonstMoveExcute()
	{
		if ( !getMove() )
			return false;
		int tGrid = getMonsterMove();
		if( tGrid != INVALID_GRID)	
		{		
			if (getMoveGrid() != tGrid)
				setMoveGrid(tGrid);
			return true;
		}
		return false;
	}

	int BaseRole::getMonsterMove()//怪物的移动处理是，哪个最先返回值不是INVALID_GRID就以哪个为标准，后面的不做处理了
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		if (getRoleLayer()->gettestState())
			return INVALID_GRID;
#endif
		if (getMonsterSpecies() == sMonsterSpecies::eWorldBoss)
			return INVALID_GRID;
		int tCol = getGridIndex()/C_GRID_ROW;
		tCol += 1;
		if(tCol >= C_GRID_COL-1||tCol < 0) 
			return INVALID_GRID;
		return tCol * C_GRID_ROW + getGridIndex()%C_GRID_ROW;
	}
	/****************************** Move ****************************************/

	void BaseRole::AliveExcuteAI()
	{
		if (critJudge())
		{
			excuteCritEffect();
			return;
		}
		attackDirection();
		mManage->initRoleSkillInfo(getCurrEffect()->getEffectID(),this);
	}

	void BaseRole::SkillActionAndEffect(int pActionIndex,int pEffectID)							//根据武将当前技能效果，播放在武将身上特效和动作切换(可以抽出来一个对象处理)
	{
		getRoleObject()->setAtkEffect(pEffectID);
		getRoleObject()->TurnStateTo((sStateCode)pActionIndex);
	}

	void BaseRole::excuteCritEffect()
	{
		if (getEnemy())
		{
			monsterCritEffect();
		}else{
			heroCritEffect();
		}
	}

	void BaseRole::heroCritEffect()
	{
		RoleObject* pActObject = getRoleObject();
		pActObject->setUserObject(CCBool::create(true));
		pActObject->setZOrder(0);
		for (auto tAlive:mAreaTargets)
		{
			tAlive->getRoleObject()->setUserObject(CCBool::create(true));
			tAlive->getRoleObject()->setZOrder(0);
			tAlive->getRoleObject()->pauseSchedulerAndActions();
			tAlive->getRoleObject()->setMoveState(sStateCode::eNullState);
			tAlive->getRoleObject()->TurnStateTo(sStateCode::eStandState);
		}
		getRoleLayer()->changeLight(true);
		bNotification->postNotification(B_RoleSkill,this);
		mRoleLayer->removeEvent();
	}

	void BaseRole::monsterCritEffect()
	{
		getRoleObject()->TurnStateTo(sStateCode::eStandState);		
		bNotification->postNotification(B_RoleSkill,this);
	}

	void BaseRole::attackDirection()
	{
		if (getOpposite())									//反向攻击处理(怪物取消反向处理了)
		{
			RoleObject* pActObject = getRoleObject();
			if (getEnemy())
			{
				pActObject->setRoleDirection(Ditection_Left);
			}else{
				pActObject->setRoleDirection(Ditection_Right);
			}
		}
	}

	void BaseRole::HeroExcuteAI()
	{
		sStateCode ActionCode = getRoleObject()->getCurrActionCode();	
		mSkillRange->initAttackInfo(this);
		if (mAreaTargets.size() || getCriAtk())
		{
			if (ActionCode == sStateCode::eWalkState)
			{
				getRoleObject()->setMoveState(sStateCode::eNullState);
				getRoleObject()->TurnStateTo(sStateCode::eStandState);
				setAIState(false);
			}
			AliveExcuteAI();
		}else{		
			if (IsAutoMoveType()						||
				ActionCode == sStateCode::eWalkState	||
				getCallType() != sCallType::eCommon	)			//只有可收控制的武将才有后续的处理(其他类型武将没有MoveObject))								//自动移动类,或已经开始执行AI状态
				return;
			if ( !getCaptain() && getGridIndex() != getMoveObject()->getgrid())					//武将当前是否在固定位置,如果不在则移动回固定位置(执行AI完毕状态)
			{			
				setAIState(false);
				setMoveGrid(getMoveObject()->getgrid());							//@@武将有一个回到初始位置的方法
				getRoleObject()->setMoveState(sStateCode::eWalkState);
			}else{
				heroGuard();
			}
		}
	}

	void BaseRole::heroGuard()
	{
		int tGrid = INVALID_GRID;
		if ( getCaptain() )
		{
			tGrid = mSkillRange->CaptainGuard(this);
		}else{
			tGrid = mGuardArea->getAliveGuard(this);
		}
		setAIState(true);
		if(tGrid)
		{
			setMoveGrid(tGrid);
			getRoleObject()->setMoveState(sStateCode::eWalkState);
		}else{
			setAIState(false);
		}
	}

	bool BaseRole::IsAutoMoveType()
	{
		if (getCallType() != sCallType::eAutoMove)
			return false;
		if (getRoleObject()->getMoveState()  != sStateCode::eNullState || !getMove())
			return true;
		if (getGridIndex() <= C_BEGINGRID-C_GRID_ROW*3)			
		{
			getRoleObject()->AliveDie();							//超出预定范围执行死亡操作
			return true;
		}			
		if (mManage->inUnDefineArea(getGridIndex()-C_GRID_ROW))
		{
			getRoleObject()->setMoveState(sStateCode::eStandState);
		}else{
			setMoveGrid(getGridIndex()-C_GRID_ROW);					//这里的移动范围是写死了的
			getRoleObject()->setMoveState(sStateCode::eWalkState);
		}
		return true;
	}

	void BaseRole::AliveCritEnd()
	{
		if ( getBattle() && !getCaptain() )				//我方武将释放技能时会扣自己血将自己击杀
		{
			getRoleObject()->setMoveState(sStateCode::eWalkState);	
			getRoleObject()->setUpdateState(true);
			setHasTarget(false);
			if (getBaseData()->getAlertType())
				setAIState(true);		
		}
		mRoleLayer->addEvent();
	}

	BaseRole* BaseRole::getAbsentCallRole()
	{
		const RolesMap* tRoleMap = mManage->getRolesMap();
		RolesMap::const_iterator iter = tRoleMap->begin();
		for(; iter != tRoleMap->end();++iter)			//判断是否有已创建但未上阵的召唤武将
		{
			BaseRole* tRole = iter->second;
			if (!tRole->getFatherID() || tRole->getHp()<=0 
				|| tRole->getEnemy() != getEnemy() )
				continue;
			if (!tRole->getBattle() && tRole->getFatherID() == getAliveID() )
				return tRole;
		}
		return nullptr;
	}

	BaseRole* BaseRole::getCallRole( RoleSkill* skill )
	{
		BaseRole* tRole = getAbsentCallRole();					
		if (tRole) return tRole;
		if (captainCallNumberJudge())
			return nullptr;
		const skEffectData* tEffect = skill->getSummonEffect();
		if (!tEffect)
		{
			CCLOG("[ *ERROR ] WarManager::getCallAlive Skill Effect NULL");
			return nullptr;
		}
		return createCallRole(tEffect->getTargetType());
	}

	BaseRole* BaseRole::createCallRole( int pRoleID )
	{
		MonsterData* tBaseData = (MonsterData*)BattleData->getCallRoleData(pRoleID);
		if ( !tBaseData)
		{
			CCLOG("[ *ERROR ]WarManager::getNewCallAlive  CallId =%d ",pRoleID);
			return nullptr;
		}
		BaseRole* tChild = BaseRole::create();
		tChild->setBaseData(tBaseData);
		tChild->setCallType(tBaseData->getCallType());
		tChild->setEnemy(getEnemy());
		if ( getEnemy() )
		{
			tChild->setAliveID(C_CallMonst);
			if (tChild->getBaseData()->getBehavior() == sBehavior::eHide)
				tChild->setCloaking(true);
			tChild->setDelaytime(tBaseData->getDelayTime());
			if (tBaseData->getInitGrid())
			{
				CallAliveByFixRange(this,tChild);
			}else{
				int ran = CCRANDOM_0_1()*(mStandGrids.size()-1);
				int grid = mManage->getCurrRandomGrid(mStandGrids.at(ran));	//得到当前武将格子的附近范围格子
				tChild->setGridIndex(grid);
			}
			tChild->setBehavior(tChild->getBaseData()->getBehavior());
			tChild->setMove(tBaseData->getMoveState());
		}else{
			tChild->setAliveID(C_CallHero);
			tChild->setGridIndex(INVALID_GRID);
		}
		tChild->initAliveByFather(this);
		mManage->addBattleRole(tChild);
		tChild->setFatherID(getAliveID());
		return tChild;
	}

	void BaseRole::attackEventLogic()
	{
		if( soriteNumberEnd() )										//当掉血帧多于实际逻辑值，少于实际逻辑值情况处理
			return;	
		switch ( getSkillType() )
		{
		case eNorAtk:
		case eSpeAtk:
		case eCriAtk:
			{
				setSortieNum(getSortieNum()+1);											//表示执行了一次攻击逻辑
				BattleResult* tResult = mHurtCount->AttackExcute(this);					//实际进行伤害计算的地方，不应由动作来控制的，动作可以控制播放。
				if ( getCriAtk() && !getEffIndex())										//必杀技多释一个空效果
					bNotification->postNotification(B_AttactNull,tResult);
				if (!tResult->m_HitTargets.empty())
					bNotification->postNotification(B_AttackResult,tResult);
			}break;
		case eCallAtk:
			{
				setSortieNum(getSortieNum()+1);											//一次性可召唤多个武将
				BaseRole* tRole = getCallRole(getCurrSkill());							//得到被召唤的武将	
				bNotification->postNotification(MsgCreateRoleObject,tRole);
			}break;
		}
		if( soriteNumberEnd() )
		{
			mHurtCount->BuffHandleLogic(this);											//伤害计算完成才能添加新的BUFF
			clearHitAlive();
		}
	}

	bool BaseRole::soriteNumberEnd()
	{
		if(getSortieNum() >= getCurrEffect()->getBatter() )										//当掉血帧多于实际逻辑值，少于实际逻辑值情况处理
			return true;
		return false;
	}

	void BaseRole::setCommandGrid(int pGrid)
	{
		mCommandGrid = pGrid;
		if ( !moveToTouchEndGrid())
			mCommandGrid = INVALID_GRID;
	}

	bool BaseRole::moveToTouchEndGrid()
	{
		if (unCommonAlive() || movePrecondition())
			return false;
		getRoleObject()->showThis();
		if (!mManage->inMoveArea(mCommandGrid)			||
			!mGuideManage->moveGuideJudge(mCommandGrid)	||
			!moveJudge())		//当前位置是否可以放置英雄
			return false;
		mGuideManage->moveGuideJudge(mCommandGrid,true);
		getRoleObject()->setActMoveGrid(mCommandGrid);
		setAIState(false);
		return true;
	}

	bool BaseRole::unCommonAlive()
	{
		if ( getCallType() != sCallType::eCommon		&& 
			 mGuideManage->moveGuideJudge(mCommandGrid)	&&
			 mManage->inMoveArea(mCommandGrid)			)
		{
			mGuideManage->moveGuideJudge(mCommandGrid,true);
			setMoveGrid(mCommandGrid);
			getRoleObject()->setMoveState(sStateCode::eWalkState);
			return true;
		}
		return false;
	}

	bool BaseRole::movePrecondition()
	{
		if (!getMove()			|| 
			!getRoleObject()	|| 
			!getMoveObject()	||
			getLogicState() == sLogicState::eInvincible||
			mCommandGrid == getMoveObject()->getgrid() )
			return true;
		return false;
	}

	bool BaseRole::moveJudge()
	{
		vector<int> tDestinations;
		initContainGrid(getCommandGrid(),tDestinations);
		if (!tDestinations.size())
			return false;				//没有目标位置(超出边界)
		if (getBattle())
		{
			if (!swappingRule(tDestinations))
				return false;	
		}else{
			if (!callAliveJudge(tDestinations))
				return false;
		}
		return true;
	}

	bool BaseRole::borderJudge( vector<int>& pVector )
	{
		for (auto pGrid : pVector)									//主帅位置不可替换
		{
			if ( pGrid >= C_CAPTAINGRID )							//我方武将边缘处理
				return true;
			if ( !getEnemy() && !getFatherID() &&
				 pGrid<C_GRID_ROW+C_BEGINGRID )
				return true;
		}
		int tRow = pVector.at(0) % C_GRID_ROW;							//最小格子的站位
		if (tRow+getBaseData()->getRoleRow() > C_GRID_ROW)				//武将所占格子,不能超出地图外
			return true;
		return false;
	}

	bool BaseRole::swappingRule( vector<int>& pDestinations )
	{
		vector<BaseRole*> tAreaAlives = getAliveInArea(pDestinations);
		int tOffs = getMoveObject()->getgrid() - mCommandGrid;
		for (auto tSwappingAlive:tAreaAlives)
		{
			if (tSwappingAlive == this)
				continue;
			int tSwappingGrid = tSwappingAlive->getMoveObject()->getgrid()+tOffs;
			vector<int> tAliveDes;
			tSwappingAlive->initContainGrid(tSwappingGrid,tAliveDes);
			if (tAliveDes.size() && !vectorIntersection(pDestinations,tAliveDes) )
			{
				for (auto atGrid : tAliveDes)
				{
					BaseRole* atAlive = mRoleLayer->getAliveByMoveGrid(atGrid);
					if (atAlive && atAlive != this && atAlive != tSwappingAlive)
						return false;
				}
			}else{
				return false;
			}
		}
		moveSwappingAlives(tAreaAlives,tOffs);
		return true;
	}

	vector<BaseRole*> BaseRole::getAliveInArea( vector<int>& pAreas )
	{
		vector<BaseRole*> tAreaAlives;
		for (auto tGrid:pAreas)
		{
			BaseRole* tAlive = mRoleLayer->getAliveByMoveGrid(tGrid);
			if (tAlive)
			{
				bool tAddAlive = true;
				for (auto atAlive:tAreaAlives)
				{
					if (atAlive == tAlive)
					{
						tAddAlive = false;
						break;
					}
				}
				if (tAddAlive)
					tAreaAlives.push_back(tAlive);
			}
		}
		return tAreaAlives;
	}

	bool BaseRole::vectorIntersection( vector<int>& pVector,vector<int>& ptVector )			//关于武将间的站位情况处理是一个可优化的方法
	{
		for (auto i : pVector)
		{
			for (auto j: ptVector)
			{
				if (i==j)
					return true;
			}
		}
		return false;
	}

	void BaseRole::moveSwappingAlives( vector<BaseRole*>& pVector,int pOffs )
	{
		for (auto tAlive:pVector)
		{
			int tGrid = tAlive->getMoveObject()->getgrid() + pOffs;
			tAlive->getRoleObject()->setActMoveGrid(tGrid);
		}
	}

	bool BaseRole::callAliveJudge( vector<int>& pDestinations )
	{
		for (auto tGrid:pDestinations)
		{
			BaseRole* tDestinationAlive = mRoleLayer->getAliveByMoveGrid(tGrid);
			if (!tDestinationAlive)
				continue;
			if (mGuideManage->isRest())		//重置状态下我方未上阵武将可以重叠
			{
				return true;
			}else{
				if (tDestinationAlive->getCallType() == sCallType::eCommon)
					return false;
			}
		}
		return true;
	}

	bool BaseRole::singleGrid()
	{
		if (getBaseData()->getRoleRow() >1 || getBaseData()->getRoleCol() > 1)
			return false;
		return true;
	}
};