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
#include "warscene/ConstNum.h"
#include "warscene/SkillRange.h"
#include "warscene/BattleTools.h"
#include "warscene/HurtCount.h"
#include "warscene/CombatGuideManage.h"
#include "scene/layer/WarAliveLayer.h"

namespace BattleSpace{
	BaseRole::BaseRole()
		:m_Enemy(false),m_Hp(0),m_MaxHp(0),m_GridIndex(INVALID_GRID),m_MoveGrid(0),m_AtkDelay(0)
		,m_AtkNum(0),m_Move(true),m_CritSkill(false),m_Hrt(0),m_HrtPe(0),m_AIState(false)
		,m_initCost(0),m_Batch(0),m_CostMax(0),m_AddCost(0),m_Atk(0),m_Def(0),m_Hit(0),m_NorAtk(true)
		,m_Doge(0),m_Crit(0),m_Zoom(0),m_Renew(0),m_GroupIndex(0),m_EffectIndex(0),m_LastAlive(false)
		,m_Opposite(false),m_ExecuteCap(false),m_TerType(0),m_TerTypeNum(0),m_cloaking(false)
		,m_SortieNum(0),mRoleObject(nullptr),mBuffManage(nullptr),m_UILayout(0),m_Atktime(0)
		,m_AtkInterval(0),m_SpecialAtk(false),m_Battle(false),m_MoveSpeed(0),mSkillRange(nullptr)
		,m_CritTime(0),m_FatherID(0),m_Captain(false),m_CritEffect(false),m_DieState(false)
		,m_TouchGrid(0),m_TouchState(false),mMoveObject(nullptr),m_CallType(0),m_CallAliveNum(0)
		,m_Delaytime(0),m_AliveState(COMMONSTATE),m_AliveType(E_ALIVETYPE::eCommon),mHurtCount(nullptr)
		,m_Model(0),m_AliveID(0),m_MstType(0),mBaseData(nullptr),mLogicData(nullptr),mGuideManage(nullptr)
		,mTouchEndGrid(0),mRoleLayer(nullptr)
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
		setInitCost(getBaseData()->getInitCost());				//�ٻ����ĵ�cost(���������ĵ����)
		if (pFather->getAliveType() == E_ALIVETYPE::eWorldBoss)
		{
			setAtkInterval(getBaseData()->getAttackSpeed());
			setMoveSpeed(getBaseData()->getMoveSpeed());
			setMaxHp(getBaseData()->getRoleHp());
			setHp(getBaseData()->getRoleHp());					//��һ�ν�������Ѫ״̬
			setAddCost(getBaseData()->getCostSpeed());
			setAtk(getBaseData()->getRoleAttack());
			setCrit(getBaseData()->getRoleCrit());
			setDef(getBaseData()->getRoleDefense());
			setHit(getBaseData()->getRoleHit());
			setRenew(getBaseData()->getRoleRegain());
			setDoge(getBaseData()->getRoleDodge());				//��ֵ�����ٻ����佫�İٷֱ�
		}else{
			setAtkInterval((getBaseData()->getAttackSpeed()*0.01f)*pFather->getBaseData()->getAttackSpeed());
			setMoveSpeed((getBaseData()->getMoveSpeed()*0.01f)*pFather->getBaseData()->getMoveSpeed());
			setMaxHp((getBaseData()->getRoleHp()*0.01f)*pFather->getBaseData()->getRoleHp());
			setHp((getBaseData()->getRoleHp()*0.01f)*pFather->getBaseData()->getRoleHp());							//��һ�ν�������Ѫ״̬		
			setAddCost((getBaseData()->getCostSpeed()*0.01f)*pFather->getBaseData()->getCostSpeed());
			setAtk((getBaseData()->getRoleAttack()*0.01f)*pFather->getBaseData()->getRoleAttack());
			setCrit((getBaseData()->getRoleCrit()*0.01f)*pFather->getBaseData()->getRoleCrit());
			setDef((getBaseData()->getRoleDefense()*0.01f)*pFather->getBaseData()->getRoleDefense());
			setHit((getBaseData()->getRoleHit()*0.01f)*pFather->getBaseData()->getRoleHit());
			setRenew((getBaseData()->getRoleRegain()*0.01f)*pFather->getBaseData()->getRoleRegain());
			setDoge((getBaseData()->getRoleDodge()*0.01f)*pFather->getBaseData()->getRoleDodge());				//��ֵ�����ٻ����佫�İٷֱ�
		}
	}

	void BaseRole::initAliveData()
	{
		mStandGrids.clear();
		setDelaytime(getBaseData()->getDelayTime());
		setModel(getBaseData()->getRoleModel());
		if (getEnemy())															//��ָ��ת������Ӧ�����Ͷ�����ĵз�������Ӧ�ĳ�ʼ��
		{
			setGridIndex(getBaseData()->getInitGrid());
		}else{
			if (getBaseData()->getCaptain())									//�ӳ�������Ϊ����״̬(�������������ֶӳ��ı�־)
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
		setCallType(getBaseData()->getCallType());
		setAtkInterval(getBaseData()->getAttackSpeed());
		setMoveSpeed(getBaseData()->getMoveSpeed());
		if (getAliveType() == E_ALIVETYPE::eWorldBoss)
		{
			setMaxHp(getBaseData()->getMaxHp());
		}else{
			setMaxHp(getBaseData()->getRoleHp());
		}
		setHp(getBaseData()->getRoleHp());									//��һ�ν�������Ѫ״̬
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
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		if (this->getEnemy())
		{

		}else{		
			setAtk(500000);
			setMaxHp(500000);
			setHp(500000);		//��һ�ν�������Ѫ״̬
		}
#endif
	}

	void BaseRole::setMoveGrid(int var)
	{
		if (var == INVALID_GRID)
			CCLOG("[ TIPS ] WarAlive::setMoveGrid");
		m_MoveGrid = var;
	}

	void BaseRole::setAtkNum(int var)
	{
		m_AtkNum += var;
		if (m_AtkNum >= 3)					//���Ĵι���Ϊ���⹥��
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
		if (hp > max)			//����ֵ�ж�
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
		if (m_Def<=0)
			m_Def = 0;
	}

	void BaseRole::setNorAtk(bool var)
	{
		m_NorAtk = var;
		if (!m_NorAtk)
			m_Atktime = 0;
	}

	void BaseRole::setAtktime(float var)
	{
		//���ӹ��١����ٹ���(�ٷֱȡ�ֱ����ֵ����)
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
	//�����ͷű�ɱ��ʱ��
	void BaseRole::setCritTime(float var)
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

	void BaseRole::setAliveStat(int var)
	{
		m_AliveState = var;
	}

	void BaseRole::setGridIndex(int var)
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
			{
				for (int j =0;j<getBaseData()->getRoleCol();j++)
				{
					if (var+j*C_GRID_ROW+i>C_ENDGRID)
						continue;
					mStandGrids.push_back(var+j*C_GRID_ROW+i);
				}	
			}
			sort(mStandGrids.begin(),mStandGrids.end());
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
		int row = mTouchGrids.at(0)%C_GRID_ROW;				//��С���ӵ�վλ
		if (row+getBaseData()->getRoleRow()>C_GRID_ROW)						//�佫��ռ����,���ܳ�����ͼ��
		{
			m_TouchGrid=INVALID_GRID;
			mTouchGrids.clear();
		}
	}

	void BaseRole::setCallType(int var)
	{
		if (var == AutoSkill)							//����ս�����ͷż���(��ʯ��)
			setCriAtk( true );							//ֻ�б�ɱ����û�й���Ҳ���ͷŵ�
		m_CallType = var;
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
		setSortieNum(0);																//��ǰЧ����������
		setOpposite(false);																//���÷��򹥻�
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
		for (auto alive:HittingAlive)
		{
			if (alive == this)
				continue;
			if (!alive->getDieState()&&alive->getHp()<=0 && alive->getRoleObject())
				alive->getRoleObject()->AliveDie();
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

	skEffectData* BaseRole::getCurrEffect()
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

	bool BaseRole::NextEffect()
	{
		int effId = getEffIndex() + 1;
		int ranNum = CCRANDOM_0_1()*100;								//0��100����
		if (getGroupIndex() >= getCurrSkill()->getListSize())
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

	bool BaseRole::standInGrid( int pGrid )
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

	bool BaseRole::critJudge()
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

	vector<BaseRole*>* BaseRole::getSkillTargets()
	{
		int tTarget = getCurrEffect()->getTargetType();
		if ((getEnemy()&&tTarget == eUsType)	|| 
			(!getEnemy()&&tTarget == eEnemyType) )			/*�з��Լ����ҷ�����*/
			return mManage->getVecMonsters(true);
		if ((getEnemy()&&tTarget == eEnemyType) ||
			(!getEnemy()&&tTarget == eUsType))				/*�з����ˣ��ҷ��Լ�*/
			return mManage->getVecHeros(true);
		return mManage->getAliveRoles(true); 
	}

	void BaseRole::excuteLogic(float pTime)
	{
		if (!getRoleObject())return;
		getRoleObject()->updateFrameEvent(pTime);									//ˢ���佫֡�¼�
		if (stateDispose(pTime))
			return;										
		if (!AttackJudge())
			return;
		if (getEnemy())
		{
			MonsterExcuteAI(pTime);
		}else{
			HeroExcuteAI();			
		}
	}

	bool BaseRole::stateDispose(float pTime)
	{
		if (getCallType()  == NotAttack		||								//ʯͷ���佫���������жϴ���
			getAliveStat() == UNATK			||
			getAliveStat() == INVINCIBLE	||							//�����߽���佫���ٽ��м��ܴ���
			getDieState()					|| 
			getHp()<=0						)
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
			return false;
		if (getBaseData()->getDelayTime())
		{
			if (getDelaytime()<=0)
			{
				setGridIndex(getBaseData()->getInitGrid());
				getRoleLayer()->roleWantIntoBattle(this);
			}else{
				setDelaytime(getDelaytime() - pTime);
			}
		}
		if (getAtkDelay()>0)
			setAtkDelay(getAtkDelay() - pTime);									//�з��佫�ͷű�ɱ�������ӳ�ʱ��
		return true;
	}

	bool BaseRole::autoSkillAlive()
	{
		if ((getBaseData()->getCallType() == AutoSkill || getBaseData()->getMonsterType() == MST_SKILL)&&!getCriAtk())						//����ս�����ͷż���(��ʯ��)
		{
			if (getAliveStat()==COMMONSTATE)
			{
				if (getBaseData()->getMonsterType() == MST_SKILL)
					getBaseData()->setMonsterType(MST_COMMON);
				setCriAtk( true );
				return true;
			}
		}
		return false;
	}

	void BaseRole::attackTime( float pTime )
	{
		E_StateCode ActionCode = getRoleObject()->getCurrActionCode();
		if (ActionCode == E_StateCode::eStandState	|| 
			ActionCode == E_StateCode::eHitState	|| 
			ActionCode == E_StateCode::eWalkState	)//����ļ����ͷ����ɹ���������ʱ��ȷ����
			setAtktime(pTime);
		if (!getCriAtk()&&getEnemy())
			setCritTime(pTime);
		getBuffManage()->upDateBuff(pTime);									// BUFFӦ����ÿһ֡���Զ������
	}

	bool BaseRole::AttackJudge()
	{
		E_StateCode ActionCode = getRoleObject()->getCurrActionCode();
		if (ActionCode == E_StateCode::eWalkState)
			return walkState();

		if (ActionCode == E_StateCode::eDizzyState			||
			ActionCode == E_StateCode::eActiveAttackState	||
			ActionCode == E_StateCode::eSpecialAttackState	||
			ActionCode == E_StateCode::eNormalAttackState	||
			ActionCode == E_StateCode::eVictoryState		||
			ActionCode == E_StateCode::eDeathState)
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
		if (getEnemy()||getCallType()==AutoMoveType)		//�ҷ��Զ��ƶ����佫
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
		if (monsterFlee())												//�������ܴ���,��AIִ�еĶ���������ط����������
			return;
		E_StateCode ActionCode = getRoleObject()->getCurrActionCode();
		if (getGridIndex()<C_BEGINGRID)							//@@ÿ�����ﶼ�еķ���
		{
			if (getRoleObject()->getMoveState() == E_StateCode::eNullState
				&& MonstMoveExcute())
				getRoleObject()->setMoveState(E_StateCode::eWalkState);
			return ;
		}
		mSkillRange->initAttackInfo(this);
		if (mAreaTargets.size())
		{
			if (ActionCode == E_StateCode::eWalkState)
			{
				getRoleObject()->setMoveState(E_StateCode::eNullState);
				getRoleObject()->TurnStateTo(E_StateCode::eStandState);
			}
			AliveExcuteAI();
		}else{
			if (getSkillType() == eCallAtk)
			{
				getRoleObject()->setMoveState(E_StateCode::eNullState);
				getRoleObject()->TurnStateTo(E_StateCode::eStandState);
				AliveExcuteAI(); 
			}else{
				if (getCriAtk())
					setCriAtk(false);			
				if (getRoleObject()->getMoveState() == E_StateCode::eNullState && MonstMoveExcute() )
					getRoleObject()->setMoveState(E_StateCode::eWalkState);
			}
		}
	}
	/****************************** Move ****************************************/
	bool BaseRole::monsterFlee()
	{
		if (mManage->getStageID())				//��ǰ��Ϊ���������ؿ�,���Գ����һ�����������ж��Ƿ�Ϊ���ֹؿ�
			return false;
		if (getModel() == 1056  
			&& ((getHp()*1.0f/getMaxHp()) < 0.60f))					//�佫��Ҫһ���õ���ǰѪ���ٷֱȵķ���
		{
			if (getRoleObject()->getMoveState() == E_StateCode::eNullState)
			{
				setMoveGrid(4);
				getRoleObject()->setMoveState(E_StateCode::eWalkState);
				setMoveSpeed(0.5f);
				setDef(1000000);
			}
			if (getGridIndex()<= C_BEGINGRID-C_GRID_ROW*4)
			{		
				getRoleObject()->AliveDie();
				getRoleObject()->setVisible(false);
				NOTIFICATION->postNotification(B_ActObjectRemove,this);		//����Ԥ����Χִ����������
			}		
			return true;
		}
		return false;
	}

	bool BaseRole::MonstMoveExcute()
	{
		if (!getMove()||getAliveStat() == INVINCIBLE)
			return false;
		int grid = monsterMove();
		if( grid!= INVALID_GRID && grid < C_CAPTAINGRID)	
		{		
			if (getMoveGrid() != grid)
				setMoveGrid(grid);
			return true;
		}
		return false;
	}

	int BaseRole::monsterMove()//������ƶ������ǣ��ĸ����ȷ���ֵ����INVALID_GRID�����ĸ�Ϊ��׼������Ĳ���������
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		if (getRoleLayer()->gettestState())
			return INVALID_GRID;
#endif
		if (getAliveType() == E_ALIVETYPE::eWorldBoss)
			return INVALID_GRID;
		int index = getGridIndex();
		if (index/C_GRID_ROW >= C_GRID_COL-1)
			return INVALID_GRID;
		return getMonsterMoveGrid();
	}

	int BaseRole::getMonsterMoveGrid()
	{
		if (getEnemy())
		{
			int row = getGridIndex() % C_GRID_ROW;
			int col = getGridIndex() / C_GRID_ROW;
			int Index = INVALID_GRID;
			if (!getMove())
				return INVALID_GRID;
			for (int i = 1;i <= 1;i++)			//һ�����������ȥ�����жϣ��ж�ÿһ��������ƶ���Χ
			{
				col += 1;
				if(col >= C_GRID_COL-1||col < 0) 
					break;
				Index = MoveJudge(col * C_GRID_ROW + row);
				if (Index == INVALID_GRID)
					break;
			}
			return Index;
		}
		return INVALID_GRID;
	}

	int BaseRole::MoveJudge( int pGrid )
	{
		BaseRole* tRole = mManage->getAliveByGrid(pGrid);
		if( tRole == nullptr || tRole == this ||tRole->getEnemy())
		{
			return CountMoveGrid( pGrid );	
		}else{
			return INVALID_GRID;
		}
	}

	int BaseRole::CountMoveGrid( int grid )
	{
		if (mStandGrids.size() <= 1)										//���ж϶���������
			return grid;
		int tRowOffs = grid % C_GRID_ROW - getGridIndex() % C_GRID_ROW;		//�б仯��
		int tColOffs = grid / C_GRID_ROW - getGridIndex() / C_GRID_ROW;		//�б仯��
		int tGridOffs = grid - getGridIndex();								//��Ǹ��ӱ仯��
		for (int j=1;j < mStandGrids.size(); j++)
		{
			int tStandGrid = mStandGrids.at(j);
			int tStandRow = tStandGrid % C_GRID_ROW;				
			int tStandCol = tStandGrid / C_GRID_ROW;
			if (tStandRow+tRowOffs >= C_GRID_ROW || 
				tStandRow+tRowOffs < 0 ||
				tStandCol+tColOffs >= C_GRID_COL-1 ||
				tStandCol+tColOffs < 0)
			{
				return INVALID_GRID;		//�����ƶ��˳�ѭ���������ж�
			}
			BaseRole* tRole = mManage->getAliveByGrid(tStandGrid+tGridOffs);
			if (getEnemy())
			{
				if (getGridIndex()<C_BEGINGRID)
					return grid;
				if(tRole&&tRole->getHp()>0&&!tRole->getEnemy())//�ĸ���һ��Ϊ�ظ�
				{
					return INVALID_GRID;		//�����ƶ��˳�ѭ���������ж�
				}		
			}else{
				if(tRole&&tRole->getHp()>0&&tRole != this)//�ĸ���һ��Ϊ�ظ�
				{
					return INVALID_GRID;		//�����ƶ��˳�ѭ���������ж�
				}
			}
		}
		return grid;
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

	void BaseRole::SkillActionAndEffect(int pActionIndex,int pEffectID)							//�����佫��ǰ����Ч�����������佫������Ч�Ͷ����л�(���Գ����һ��������)
	{
		getRoleObject()->setAtkEffect(pEffectID);
		getRoleObject()->TurnStateTo((E_StateCode)pActionIndex);
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
			tAlive->getRoleObject()->setMoveState(E_StateCode::eNullState);
			tAlive->getRoleObject()->TurnStateTo(E_StateCode::eStandState);
		}
		getRoleLayer()->changeLight(true);
		NOTIFICATION->postNotification(B_RoleSkill,this);
	}

	void BaseRole::monsterCritEffect()
	{
		getRoleObject()->TurnStateTo(E_StateCode::eStandState);		
		NOTIFICATION->postNotification(B_RoleSkill,this);
	}

	void BaseRole::attackDirection()
	{
		if (getOpposite())									//���򹥻�����(����ȡ����������)
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
		E_StateCode ActionCode = getRoleObject()->getCurrActionCode();	
		mSkillRange->initAttackInfo(this);
		if (mAreaTargets.size() || getCriAtk())
		{
			if (ActionCode == E_StateCode::eWalkState)
			{
				getRoleObject()->setMoveState(E_StateCode::eNullState);
				getRoleObject()->TurnStateTo(E_StateCode::eStandState);
				setAIState(false);
			}
			AliveExcuteAI();
		}else{		
			//@@��Щ�߼���Ӧ�ö���װ���佫���ڲ��������佫ȥ����������������
			if (IsAutoMoveType() || ActionCode == E_StateCode::eWalkState)//�Զ��ƶ���,���Ѿ���ʼִ��AI״̬
				return;
			if (getCallType() != CommonType)				//ֻ�п��տ��Ƶ��佫���к����Ĵ���(���������佫û��MoveObject)
				return;
			if ( !getCaptain() && getGridIndex() != getMoveObject()->getgrid())					//�佫��ǰ�Ƿ��ڹ̶�λ��,����������ƶ��ع̶�λ��(ִ��AI���״̬)
			{			
				setAIState(false);
				setMoveGrid(getMoveObject()->getgrid());							//@@�佫��һ���ص���ʼλ�õķ���
				getRoleObject()->setMoveState(E_StateCode::eWalkState);
			}else{
				int grid = INVALID_GRID;
				if (getCaptain())
				{
					grid = mSkillRange->CaptainGuard(this);
				}else{
					grid = mGuardArea->getAliveGuard(this);
				}
				setAIState(true);
				if(grid)
				{
					setMoveGrid(grid);
					getRoleObject()->setMoveState(E_StateCode::eWalkState);
				}else{
					setAIState(false);
				}	
			}
		}
	}

	bool BaseRole::IsAutoMoveType()
	{
		if (getCallType() != AutoMoveType)
			return false;
		if (getRoleObject()->getMoveState()  != E_StateCode::eNullState || !getMove())
			return true;
		if (getGridIndex() <= C_BEGINGRID-C_GRID_ROW*3)			
		{
			getRoleObject()->AliveDie();							//����Ԥ����Χִ����������
			return true;
		}			
		setMoveGrid(getGridIndex()-C_GRID_ROW);			//������ƶ���Χ��д���˵�
		getRoleObject()->setMoveState(E_StateCode::eWalkState);
		return true;
	}

	void BaseRole::AliveCritEnd()
	{
		if ( getHp()>0 && getBattle() && !getCaptain() )				//�ҷ��佫�ͷż���ʱ����Լ�Ѫ���Լ���ɱ
		{
			getRoleObject()->setMoveState(E_StateCode::eWalkState);	
			getRoleObject()->setUpdateState(true);
			if (getBaseData()->getAlertType())
				setAIState(true);
		}
	}

	BaseRole* BaseRole::getAbsentCallAlive()
	{
		const Members* tRoleMap = mManage->getMembers();
		Members::const_iterator iter = tRoleMap->begin();
		for(; iter != tRoleMap->end();++iter)			//�ж��Ƿ����Ѵ�����δ������ٻ��佫
		{
			BaseRole* tRole = iter->second;
			if (tRole->getHp()<=0 || tRole->getEnemy() != getEnemy())
				continue;
			if (tRole->getBaseData()->getCallRole() && 
				!tRole->getBattle() && 
				tRole->getFatherID()==getAliveID())
				return tRole;
		}
		return nullptr;
	}

	BaseRole* BaseRole::getCallAlive( RoleSkill* skill )
	{
		BaseRole* alive = getAbsentCallAlive();					
		if (alive)
			return alive;
		if (captainCallNumberJudge())
			return nullptr;
		const skEffectData* effect = skill->getSummonEffect();
		if (!effect)
		{
			CCLOG("[ *ERROR ] WarManager::getCallAlive Skill Effect NULL");
			return nullptr;
		}
		return getNewCallAlive(effect->getTargetType());
	}

	BaseRole* BaseRole::getNewCallAlive( int CallId )
	{
		const vector<MonsterData*>tVector = BattleData->getCallRoleVector();
		for (MonsterData*tBaseData :  tVector )
		{
			if (tBaseData->getCallID() != CallId)
				continue;
			BaseRole* child = BaseRole::create();
			child->setBaseData(tBaseData);
			child->setCallType(tBaseData->getCallType());
			child->setEnemy(getEnemy());
			if ( getEnemy() )
			{
				child->setAliveID(C_CallMonst);
				if (child->getBaseData()->getMonsterType() == MST_HIDE)
					child->setCloaking(true);
				child->setDelaytime(tBaseData->getDelayTime());
				if (tBaseData->getInitGrid())
				{
					CallAliveByFixRange(this,child);
				}else{
					int ran = CCRANDOM_0_1()*(mStandGrids.size()-1);
					int grid = mManage->getCurrRandomGrid(mStandGrids.at(ran));	//�õ���ǰ�佫���ӵĸ�����Χ����
					child->setGridIndex(grid);
				}
				child->setMstType(child->getBaseData()->getMonsterType());
				child->setMove(tBaseData->getMoveState());
			}else{
				child->setAliveID(C_CallHero);
				child->setGridIndex(INVALID_GRID);
			}
			child->initAliveByFather(this);
			mManage->addBattleRole(child);
			child->setFatherID(getAliveID());
			return child;
		}
		CCLOG("[ *ERROR ]WarManager::getNewCallAlive  CallId =%d ",CallId);
		return nullptr;
	}

	void BaseRole::attackEventLogic()
	{
		if( soriteNumberEnd() )										//����Ѫ֡����ʵ���߼�ֵ������ʵ���߼�ֵ�������
			return;	
		switch ( getSkillType() )
		{
		case eNorAtk:
		case eSpeAtk:
		case eCriAtk:
			{
				setSortieNum(getSortieNum()+1);											//��ʾִ����һ�ι����߼�
				BattleResult* tResult = mHurtCount->AttackExcute(this);					//ʵ�ʽ����˺�����ĵط�����Ӧ�ɶ��������Ƶģ��������Կ��Ʋ��š�
				if ( getCriAtk() && !getEffIndex())										//��ɱ������һ����Ч��
					NOTIFICATION->postNotification(B_AttactNull,tResult);
				if (!tResult->m_HitTargets.empty())
					NOTIFICATION->postNotification(B_AttackResult,tResult);
			}break;
		case eCallAtk:
			{
				setSortieNum(getSortieNum()+1);											//һ���Կ��ٻ�����佫
				BaseRole* tRole = getCallAlive(getCurrSkill());							//�õ����ٻ����佫	
				if (tRole)
					NOTIFICATION->postNotification(B_SkillCreateRole,tRole);
			}break;
		}
		if( soriteNumberEnd() )
		{
			mHurtCount->BuffHandleLogic(this);											//�˺�������ɲ�������µ�BUFF
			clearHitAlive();
		}
	}

	bool BaseRole::soriteNumberEnd()
	{
		if(getSortieNum() >= getCurrEffect()->getBatter() )										//����Ѫ֡����ʵ���߼�ֵ������ʵ���߼�ֵ�������
			return true;
		return false;
	}

	void BaseRole::setTouchEndGrid(int pGrid)
	{
		mTouchEndGrid = pGrid;
		moveToTouchEndGrid();
	}

	void BaseRole::moveToTouchEndGrid()
	{
		if (unCommonAlive() || movePrecondition())
			return;
		getRoleObject()->showThis();
		if ( !mManage->inMoveArea(mTouchEndGrid)			||
			WorldBossJudge()								||
			!mGuideManage->moveGuideJudge(mTouchEndGrid)	||
			!aliveMoveJudge())		//��ǰλ���Ƿ���Է���Ӣ��
			return;
		getRoleObject()->setActMoveGrid(mTouchEndGrid);
		mGuideManage->moveGuideJudge(mTouchEndGrid,true);
		setAIState(false);
	}

	bool BaseRole::unCommonAlive()
	{
		if ( getCallType() != CommonType		&& 
			mGuideManage->moveGuideJudge(mTouchEndGrid) )
		{
			setMoveGrid(mTouchEndGrid);
			getRoleObject()->setMoveState(E_StateCode::eWalkState);
			mGuideManage->moveGuideJudge(mTouchEndGrid,true);
			return true;
		}
		return false;
	}

	bool BaseRole::movePrecondition()
	{
		if (	!getMove()			|| 
			!getRoleObject()		|| 
			!getMoveObject()	||
			mTouchEndGrid == getMoveObject()->getgrid() )
			return true;
		return false;
	}

	bool BaseRole::WorldBossJudge()
	{
		if (!mManage->getNormal() && mTouchEndGrid < 92)		//��Ӣ�ؿ������佫ֻ���Ұ���
			return true;
		if (mManage->getWorldBoss())
		{
			if (getMoveObject()->getgrid() > 108 && mTouchEndGrid < 108)				//д���ĸ�����
			{
				return true;
			}else if (getMoveObject()->getgrid() < 80 && mTouchEndGrid > 80)
			{
				return true;
			}
		}
		return false;
	}

	bool BaseRole::aliveMoveJudge()
	{
		vector<int> tDestinations = getDestinations(this,mTouchEndGrid);		
		if (!tDestinations.size())
			return false;				//û��Ŀ��λ��(�����߽�)
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

	vector<int> BaseRole::getDestinations(BaseRole* pAlive,int pGrid)
	{
		vector<int> tDestinations ;
		for (int i=0;i<pAlive->getBaseData()->getRoleRow();i++)
			for (int j =0;j<pAlive->getBaseData()->getRoleCol();j++)
				tDestinations.push_back(pGrid+j*C_GRID_ROW+i);
		sort(tDestinations.begin(),tDestinations.end());
		if (borderJudge(pAlive,tDestinations))					//���߽��ж�
			tDestinations.clear();
		return tDestinations;
	}

	bool BaseRole::borderJudge( BaseRole* pAlive,vector<int>& pVector )
	{
		for (auto i : pVector)									//��˧λ�ò����滻
			if (i>=C_CAPTAINGRID||i<C_GRID_ROW+C_BEGINGRID)		//�ҷ��佫��Ե����
				return true;
		int row = pVector.at(0)%C_GRID_ROW;						//��С���ӵ�վλ
		if (row+pAlive->getBaseData()->getRoleRow()>C_GRID_ROW)					//�佫��ռ����,���ܳ�����ͼ��
			return true;
		return false;
	}

	bool BaseRole::swappingRule( vector<int>& pDestinations )
	{
		vector<BaseRole*> tAreaAlives = getAliveInArea(pDestinations);
		int tOffs = getMoveObject()->getgrid() - mTouchEndGrid;
		for (auto tSwappingAlive:tAreaAlives)
		{
			if (tSwappingAlive == this)
				continue;
			int tSwappingGrid = tSwappingAlive->getMoveObject()->getgrid()+tOffs;
			vector<int> tAliveDes = getDestinations(tSwappingAlive,tSwappingGrid);
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

	bool BaseRole::vectorIntersection( vector<int>& pVector,vector<int>& ptVector )
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
			if (mGuideManage->isRest())		//����״̬���ҷ�δ�����佫�����ص�
			{
				return true;
			}else{
				if (tDestinationAlive->getCallType() == CommonType)
					return false;
			}
		}
		return true;
	}

	void BaseRole::roleIntoBattle()
	{
		if (getEnemy())
		{
			setAliveStat(INVINCIBLE);
			getRoleObject()->TurnStateTo(E_StateCode::eEnterState);
		}
	}

};