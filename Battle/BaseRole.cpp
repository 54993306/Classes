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
#include "Battle/BattleModel.h"
#include "Battle/StateManage/StateManager.h"

#include "battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "Battle/BuffManage.h"
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
		:m_Enemy(false),m_Hp(0),m_MaxHp(0),mGridIndex(INVALID_GRID),m_MoveGrid(0),m_AtkDelay(0),mFirstInit(true)
		,m_AtkNum(0),m_Move(true),m_CritSkill(false),m_Hrt(0),m_HrtPe(0),m_AIState(false),mUpdateSkill(false)
		,m_initCost(0),m_Batch(0),mCostMax(0),m_AddCost(0),m_Atk(0),m_Def(0),m_Hit(0),m_NorAtk(true)
		,m_Doge(0),m_Crit(0),m_Zoom(0),m_Renew(0),m_GroupIndex(0),m_EffectIndex(0),m_LastAlive(false)
		,m_Opposite(false),m_ExecuteCap(false),m_TerType(0),m_TerTypeNum(0),m_cloaking(false),mSkillTime(0)
		,m_SortieNum(0),mRoleObject(nullptr),mBuffManage(nullptr),m_UILayout(0),m_Atktime(0),mOtherCamp(false)
		,m_AtkInterval(0),m_SpecialAtk(false),mBattle(false),m_MoveSpeed(0),mSkillRange(nullptr),m_Model(0)
		,m_CritTime(0),m_FatherID(0),m_Captain(false),m_CritEffect(false),mAliveState(true),mBattleModel(nullptr)
		,m_TouchGrid(0),m_TouchState(false),mMoveObject(nullptr),mCallType(sCallType::eCommon),m_CallAliveNum(0)
		,mDelaytime(0),mLogicState(sLogicState::eNormal),mMonsterSpecies(sMonsterSpecies::eCommon),mHurtCount(nullptr)
		,m_AliveID(0),mBehavior(sBehavior::eNormal),mBaseData(nullptr),mLogicData(nullptr),mGuideManage(nullptr)
		,mCommandGrid(0),mRoleLayer(nullptr),mAutoState(false),mMaxGrid(INVALID_GRID),mHasTarget(false),mEnterTime(0)
		,mSkillEffect(nullptr),mClearState(true)
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
		CC_SAFE_RELEASE(mSkillEffect);
		mSkillEffect = nullptr;
		mRoleObject = nullptr;
		mMoveObject = nullptr;
		mStandGrids.clear();
		mSkillArea.clear();
		mAreaTargets.clear();
	}
	//���Կ�һ��������������Ӧ���ⲿ���ܷ�װ��һ��������
	bool BaseRole::init()
	{
		mManage = BattleManage;
		mBattleModel = BattleData->getBattleModel();
		mGuideManage = ManageCenter->getCombatGuideMg();

		mBuffManage = BuffManage::create();
		mBuffManage->setAlive(this);
		mBuffManage->retain();

		mSkillRange = SkillRange::create(mManage);
		mSkillRange->retain();

		mGuardArea = GuardArea::create(mManage);
		mGuardArea->retain();

		mHurtCount = HurtCount::create();
		mHurtCount->retain();

		mSkillEffect = skEffectData::create();			//���ݴ��Դ���
		mSkillEffect->retain();
		return true;
	}

	void BaseRole::initAliveByFather( BaseRole*pFather )
	{
		setModel(getBaseData()->getRoleModel());
		setZoom(getBaseData()->getRoleZoom()*0.01f);
		setInitCost(getBaseData()->getInitCost());				//�ٻ����ĵ�cost(���������ĵ����)
		if (pFather->getMonsterSpecies() == sMonsterSpecies::eWorldBoss || 
			pFather->getCallType() == sCallType::eBoxHaveRole )
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
		setOtherCamp(getBaseData()->getOtherCamp());
		if (getOtherCamp())									//��ָ��ת������Ӧ�����Ͷ�����ĵз�������Ӧ�ĳ�ʼ��
		{
			if (getEnemy())
			{
				setGridIndex(getBaseData()->getInitGrid());
			}else{
				if (!getBaseData()->getInitGrid())
				{
					setGridIndex(C_OtherCaptain);
					setCaptain(true);
				}else{
					if (getFirstInit())
					{
						setFirstInit(false);
						if (getBaseData()->getInitGrid() < C_PVEStopGrid)
						{
							for (int tGrid = C_PVEStopGrid; tGrid < C_PVEOtherArea ; tGrid++) 
							{
								if (mManage->getAliveByGrid(tGrid))
									continue;
								setGridIndex(tGrid);								//(���ݴ����ݴ���)
								CCLOG("[ *ERROR ] BaseRole::initAliveData");
								break;
							}
						}else{
							setGridIndex(getBaseData()->getInitGrid());
						}
					}else{
						setGridIndex(INVALID_GRID);
					}
				}
			}
		}else{
			if (getBaseData()->getCaptain())									//�ӳ�������Ϊ����״̬(�������������ֶӳ��ı�־)
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
		ResetAttackState();
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
		setHp(getBaseData()->getRoleHp());									//��һ�ν�������Ѫ״̬
		setCostMax(getBaseData()->getMaxCost());
		setInitCost(getBaseData()->getInitCost());			
		setAddCost(getBaseData()->getCostSpeed());
		setAtk(getBaseData()->getRoleAttack());
		setCrit(getBaseData()->getRoleCrit());
		setDef(getBaseData()->getRoleDefense());
		setHit(getBaseData()->getRoleHit());
		setRenew(getBaseData()->getRoleRegain());
		setDoge(getBaseData()->getRoleDodge());
		setZoom(getBaseData()->getRoleZoom()*0.01f);
#if BATTLE_TEST
		if (getOtherCamp())
		{
			if (getModel() == 709)
			{
				setModel(2342);
				setZoom(1.4f);
			}
		}else{
			setAtk(1000900);
		}
#endif
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
				ResetAttackState();
				setCriAtk(true);
			}
			m_CritTime = 0;
		}
	}

	void BaseRole::setBattle(bool pBattle)
	{
		if (pBattle != mBattle)
			mBattle = pBattle;
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
		if (mGridIndex <= INVALID_GRID)
			return;
		setBattle(true);
		if (getCaptain())
		{
			mStandGrids.clear();		//��������Χ��Ҫ��
			for (int tRow=0;tRow<getBaseData()->getRoleRow();tRow++)
			{
				if (pGrid + tRow>C_ENDGRID)
					continue;
				mStandGrids.push_back(pGrid + tRow);
			}
			sort(mStandGrids.begin(),mStandGrids.end());
		}else{
			initContainGrid(mGridIndex,mStandGrids);
			bNotification->postNotification(MsgRoleGridChange,this);			//�������Ч�ʺܵͣ�Ƶ�ʸߣ�����ȫ����Ϣ���飩
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
		if (row+getBaseData()->getRoleRow() > C_GRID_ROW)						//�佫��ռ����,���ܳ�����ͼ��
		{
			m_TouchGrid=INVALID_GRID;
			mTouchGrids.clear();
		}
	}

	void BaseRole::setCallType(sCallType var)
	{
		if (var == sCallType::eAutoSkill)							//����ս�����ͷż���(��ʯ��)
			setCriAtk( true );										//ֻ�б�ɱ����û�й���Ҳ���ͷŵ�
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
	//���ܻ������е��佫���Ǳ����е��佫
	void BaseRole::clearHitAlive()
	{
		if (mClearState)
		{
			mClearState = false;					//��ֹ��forѭ���г��ֶ�ε��õ������ը������ը�����˵��ܻ�����������ը��������ֱ��������
			for (auto tRole:HittingAlive)
			{
				if (tRole == this)
					continue;						//�Ա�����������ط�������,��ɫ�������кܶ������,��Ѫ����Ѫ��������ɫҲ���Լ�������
				if (tRole->getAliveState() && 
					tRole->getHp()<=0 && 
					tRole->getRoleObject())
				{
					tRole->byOtherKill(this);				//����ɱ�����
				}
					
			}
			HittingAlive.clear();
			mClearState = true;
		}
	}

	RoleSkill* BaseRole::getCurrSkill()
	{
		if (this->getCriAtk())
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
		if (getGroupIndex() < getCurrSkill()->getListSize())
			if ( getCurrSkill()->getEffectSize(getGroupIndex())>getEffIndex())
				return getCurrSkill()->getIndexEffect(getGroupIndex(),getEffIndex());
			else
				CCLOG("[ *ERROR ] BaseRole::getCurrEffect EffIndex > EffectListSize  AliveID=%d mode=%d",getAliveID(),getBaseData()->getRoleModel());
		else{
			CCLOG("[ *ERROR ]BaseRole::getCurrEffect NULL Role=%d,Mode=%d,Index=%d,Skill=%d",getAliveID(),getBaseData()->getRoleModel(),getGroupIndex(),getCurrSkill()->getSkillID());
			this->setGroupIndex(0);
		}	
		return mSkillEffect;
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
	//��PVP�еĵз�boss����֪�����佫���ܻ�����͹����ж������ǲ�һ����
	bool BaseRole::inStandGrid( int pGrid )
	{
		if (getCaptain())
		{
			if (getOtherCamp())
			{
				if (mBattleModel->isPvEBattle() && pGrid < C_PVEStopGrid)
					return true;
			}else{
				if ( pGrid >= C_CAPTAINGRID )
					return true;
			}	
		}
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

	vector<BaseRole*>* BaseRole::getCurrSkillTargets()
	{
		int tTarget = getCurrEffect()->getTargetType();
		if ((getOtherCamp()&&tTarget == eUsType)	|| 
			(!getOtherCamp()&&tTarget == eEnemyType) )			/*�з��Լ����ҷ�����*/
			return mManage->inBattleMonsters(true);
		if ((getOtherCamp()&&tTarget == eEnemyType) ||
			(!getOtherCamp()&&tTarget == eUsType))				/*�з����ˣ��ҷ��Լ�*/
			return mManage->inBattleHeros(true);
		return mManage->inBattleRoles(true); 
	}

	void BaseRole::excuteLogic(float pTime)
	{
		updateRole(pTime);
		if (!getRoleObject())return;
		this->getRoleObject()->updateFrameEvent(pTime);									//ˢ���佫֡�¼�
		if (stateDispose(pTime) || !AttackJudge())
			return;										
		if (getEnemy())
		{
			this->MonsterExcuteAI();
		}else{
			this->HeroExcuteAI();			
		}
	}

	bool BaseRole::stateDispose(float pTime)
	{
		if (getCallType()   == sCallType::eNotAttack	||							//ʯͷ���佫���������жϴ���
			getCallType()	== sCallType::eBoxHaveRole	||
			getLogicState() == sLogicState::eUnAttack	||
			getLogicState() == sLogicState::eInvincible	||							//�����߽���佫���ٽ��м��ܴ���
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
				setAtkDelay(getAtkDelay() - pTime);									//�з��佫�ͷű�ɱ�������ӳ�ʱ��
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
				if (converCol(getGridIndex()) > C_GRID_COL_MIN)	//�ж��Ƿ����ս��
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
			getBaseData()->getBehavior() == sBehavior::eAutoSkill) && !getCriAtk())						//����ս�����ͷż���(��ʯ��)
		{
			if (getLogicState()==sLogicState::eNormal)
			{
				if (getBaseData()->getBehavior() == sBehavior::eAutoSkill)
					getBaseData()->setBehavior(sBehavior::eNormal);					//�ı����߼�����ֵ   �������Ӧ���ǲ��ɱ��
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
			ActionCode == sStateCode::eWalkState	)//����ļ����ͷ����ɹ���������ʱ��ȷ����
		{
			setAtktime(pTime);
			if (!getCriAtk()&&getEnemy())
				setCritTime(pTime);
		}
		getBuffManage()->upDateBuff(pTime);									// BUFFӦ����ÿһ֡���Զ������
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
		if (getEnemy() || getCallType()==sCallType::eAutoMove)		//�ҷ��Զ��ƶ����佫
		{
			return true;
		}else{
			if (getAIState())
			{
				return true;
			}else{
				setHasTarget(false);
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

	void BaseRole::MonsterExcuteAI()
	{
		if (monsterFlee())												//�������ܴ���,��AIִ�еĶ���������ط����������
			return;
		sStateCode ActionCode = getRoleObject()->getCurrActionCode();
		if (getGridIndex()<C_BEGINGRID)							//@@ÿ�����ﶼ�еķ���
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
		if (mManage->getStageID())				//��ǰ��Ϊ���������ؿ�,���Գ����һ�����������ж��Ƿ�Ϊ���ֹؿ�
			return false;
		if (getModel() == 1056  
			&& ((getHp()*1.0f/getMaxHp()) < 0.60f))					//�佫��Ҫһ���õ���ǰѪ���ٷֱȵķ���
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
				roleDie();
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
	//����ʵ��ִ����һ���ƶ������ж�
	int BaseRole::getMonsterMove()
	{
#if BATTLE_TEST
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

	void BaseRole::SkillActionAndEffect(int pActionIndex,int pEffectID)							//�����佫��ǰ����Ч�����������佫������Ч�Ͷ����л�(���Գ����һ��������)
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
		bNotification->postNotification(B_RoleSkill,this);
		if ( !mBattleModel->battlePause() )
			return;
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
		bNotification->postNotification(MsgChangeLayerLight,CCBool::create(true));
		mRoleLayer->removeEvent();
	}

	void BaseRole::monsterCritEffect()
	{
		getRoleObject()->TurnStateTo(sStateCode::eStandState);		
		bNotification->postNotification(B_RoleSkill,this);
	}

	void BaseRole::attackDirection()
	{
		if (getOpposite())									//���򹥻�����(����ȡ����������)
		{
			RoleObject* pActObject = getRoleObject();
			if (pActObject->getEnemy())
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
				getCallType() != sCallType::eCommon		)	//ֻ�п��տ��Ƶ��佫���к����Ĵ���(���������佫û��MoveObject))								//�Զ��ƶ���,���Ѿ���ʼִ��AI״̬		
				return;
			if (!getCaptain() && getMoveObject() && 
				getGridIndex() != getMoveObject()->getgrid())					//�佫��ǰ�Ƿ��ڹ̶�λ��,����������ƶ��ع̶�λ��(ִ��AI���״̬)
			{			
				setMoveGrid(getMoveObject()->getgrid());											//@@�佫��һ���ص���ʼλ�õķ���
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
			tGrid = mSkillRange->CaptainGuard(this);				//��һ����һֱ��ͣ��Ѳ��
		}else{
			tGrid = mGuardArea->getAliveGuard(this);
		}
		setAIState(true);
		if(tGrid)
		{
			setHasTarget(true);									//�����ǲ�û�������ã���Ϊ����ʼ�ƶ�ʱ������Χ�ڻ�������Ϊfalse
			setMoveGrid(tGrid);
			getRoleObject()->setMoveState(sStateCode::eWalkState);
		}else{
#if BATTLE_TEST
			if (getOtherCamp())
			{
				//�Է���Ӫִ��AI��λ��
				setAIState(false);
			}else{
				if (!getRoleLayer()->gettestState())
				{
					if (!mBattleModel->heroMove(this))				//�佫���״̬�Ǳ�ʾ���ֿ��У��ж�ս��ģʽ���佫AI������Ӱ��Ӧ�ô��������ȥ
						setAIState(false);
				}
			}
			return;
#endif
			if (getOtherCamp()||getCaptain())
			{
				setAIState(false);
			}else{
				if (!mBattleModel->heroMove(this))					//�佫���״̬�Ǳ�ʾ���ֿ��У��ж�ս��ģʽ���佫AI������Ӱ��Ӧ�ô��������ȥ
					setAIState(false);
			}
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
			roleDie();							//����Ԥ����Χִ����������
			return true;
		}			
		if (mManage->inUnDefineArea(getGridIndex()-C_GRID_ROW))
		{
			getRoleObject()->setMoveState(sStateCode::eStandState);
		}else{
			if (getOtherCamp())
			{
				setMoveGrid(getGridIndex()+C_GRID_ROW);					//������ƶ���Χ��д���˵�
			}else{
				setMoveGrid(getGridIndex()-C_GRID_ROW);					//������ƶ���Χ��д���˵�
			}
			getRoleObject()->setMoveState(sStateCode::eWalkState);
		}
		return true;
	}

	void BaseRole::AliveCritEnd()
	{
		if ( getBattle() && !getCaptain() )				//�ҷ��佫�ͷż���ʱ����Լ�Ѫ���Լ���ɱ
		{
			getRoleObject()->setMoveState(sStateCode::eWalkState);	
			getRoleObject()->setUpdateState(true);
			setHasTarget(false);
			if (getBaseData()->getAlertType())
				setAIState(true);	
			const RoleSkill* skill = getBaseData()->getActiveSkill();
			float tDirectorTime = CCDirector::sharedDirector()->getScheduler()->getTimeScale();
			setSkillTime(skill->getCooldown()/tDirectorTime);
		}
		mRoleLayer->addEvent();
	}

	BaseRole* BaseRole::getAbsentCallRole()
	{
		const RolesMap* tRoleMap = mManage->getRolesMap();
		RolesMap::const_iterator iter = tRoleMap->begin();
		for(; iter != tRoleMap->end();++iter)			//�ж��Ƿ����Ѵ�����δ������ٻ��佫
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
		MonsterData* tBaseData = (MonsterData*)BattleData->getChildRoleData(pRoleID);
		if ( !tBaseData)
		{
			CCLOG("[ *ERROR ]WarManager::getNewCallAlive  CallId =%d ",pRoleID);
			return nullptr;
		}
		BaseRole* tChild = BaseRole::create();
		tChild->setBaseData(tBaseData);
		tChild->setCallType(tBaseData->getCallType());
		tChild->setEnemy(getEnemy());
		tChild->setOtherCamp(getOtherCamp());
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
				if (mStandGrids.empty())
				{
					CCLOG("[ *ERROR ] BaseRole::createCallRole ");
					tChild->setGridIndex(getGridIndex());
				}else{
					int ran = CCRANDOM_0_1()*(mStandGrids.size()-1);
					int grid = mManage->getCurrRandomGrid(mStandGrids.at(ran));	//�õ���ǰ�佫���ӵĸ�����Χ����
					tChild->setGridIndex(grid);
				}
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
					bNotification->postNotification(B_AttactNull,tResult);
				if (!tResult->m_HitTargets.empty())
					bNotification->postNotification(B_AttackResult,tResult);
			}break;
		case eCallAtk:
			{
				setSortieNum(getSortieNum()+1);											//һ���Կ��ٻ�����佫
				BaseRole* tRole = getCallRole(getCurrSkill());							//�õ����ٻ����佫	
				bNotification->postNotification(MsgCreateRoleObject,tRole);
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
		if (!moveToGrid()								||
			!mGuideManage->moveGuideJudge(mCommandGrid)	||
			!moveJudge())		//��ǰλ���Ƿ���Է���Ӣ��
			return false;
		mGuideManage->moveGuideJudge(mCommandGrid,true);
		getRoleObject()->setActMoveGrid(mCommandGrid);
		setAIState(false);
		return true;
	}

	bool BaseRole::moveToGrid()
	{
		if (getBattle())
		{
			if (mBattleModel->isPvEBattle() && mCommandGrid < C_PVEStopGrid)
				return false;
			if (!getOtherCamp() && !mBattleModel->moveJudge(this,mCommandGrid))
				return false;
			return mManage->inMoveArea(mCommandGrid);
		}else{
			if (getOtherCamp())
			{
				return mManage->inOtherEnter(mCommandGrid);
			}else{
				return mManage->inEnterArea(mCommandGrid);		//�Է����볡����ʵ��
			}
		}
	}

	bool BaseRole::unCommonAlive()
	{
		if (!getRoleObject())
			return true;
		if ( getCallType() != sCallType::eCommon		&& 
			mGuideManage->moveGuideJudge(mCommandGrid)	&&
			moveToGrid()			)
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
			!getMoveObject()	||
			getLogicState() == sLogicState::eInvincible||
			!getAliveState()	||
			mCommandGrid == getMoveObject()->getgrid() )
			return true;
		return false;
	}

	bool BaseRole::moveJudge()
	{
		vector<int> tDestinations;
		initContainGrid(getCommandGrid(),tDestinations);
		if (tDestinations.empty())
			return false;				//û��Ŀ��λ��(�����߽�)
		//if (!haveOtherRole(tDestinations))
		//	return true;
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

	bool BaseRole::currGridHasTarget( )
	{
		vector<int> tArea;							//��ģ�⼼��״̬���жϡ�
		int tGroupIndex = getGroupIndex();
		int tEffectIndex = getEffIndex();
		setGroupIndex(0);
		setEffIndex(0);
		mSkillRange->initSkillArea(this,tArea);
		vector<BaseRole*>* tVec = getCurrSkillTargets();
		for (auto tGrid : tArea)
		{
			BaseRole* tRole = mManage->getAliveByGrid(tGrid);
			if (!tRole) continue;
			for (auto t2Role : * tVec)
			{
				if (t2Role == tRole)		//�ڹ�����Χ���м��ܶ�ӦĿ��
				{
					setGroupIndex(tGroupIndex);
					setEffIndex(tEffectIndex);
					return true;
				}
			}
		}
		setGroupIndex(tGroupIndex);
		setEffIndex(tEffectIndex);
		return false;
	}

	bool BaseRole::borderJudge( vector<int>& pVector )
	{
		if (pVector.empty())return true;
		for (auto pGrid : pVector)									//��˧λ�ò����滻
		{
			if ( pGrid >= C_CAPTAINGRID )							//�ҷ��佫��Ե����
				return true;
			if ( !getEnemy() && !getFatherID() &&
				pGrid<C_GRID_ROW+C_BEGINGRID )
				return true;
		}
		int tRow = pVector.at(0) % C_GRID_ROW;							//��С���ӵ�վλ
		if (tRow+getBaseData()->getRoleRow() > C_GRID_ROW)				//�佫��ռ����,���ܳ�����ͼ��
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
			if (tSwappingAlive->getOtherCamp() != this->getOtherCamp())
				return false;
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
			BaseRole* tRole = mRoleLayer->getAliveByMoveGrid(tGrid);
			if (tRole && !tRole->getOtherCamp())
			{
				bool tAddAlive = true;
				for (auto atAlive:tAreaAlives)
				{
					if (atAlive == tRole)
					{
						tAddAlive = false;
						break;
					}
				}
				if (tAddAlive)
					tAreaAlives.push_back(tRole);
			}
		}
		return tAreaAlives;
	}

	bool BaseRole::vectorIntersection( vector<int>& pVector,vector<int>& ptVector )			//�����佫���վλ���������һ�����Ż��ķ���
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
			BaseRole* tRole = mRoleLayer->getAliveByMoveGrid(tGrid);
			if (!tRole || tRole->getOtherCamp() != getOtherCamp())
				continue;
			if (mGuideManage->isRest())		//����״̬���ҷ�δ�����佫�����ص�
			{
				return true;
			}else{
				if (tRole->getCallType() == sCallType::eCommon)
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

	bool BaseRole::skillAreaHasTarget()
	{
		vector<int> tArea;							//��ģ�⼼��״̬���жϡ�
		this->setCriAtk(true);
		int tGroupIndex = getGroupIndex();
		int tEffectIndex = getEffIndex();
		setGroupIndex(0);
		setEffIndex(0);
		mSkillRange->initSkillArea(this,tArea);
		vector<BaseRole*>* tVec = getCurrSkillTargets();
		for (auto tGrid : tArea)
		{
			BaseRole* tRole = mManage->getAliveByGrid(tGrid);
			if (!tRole)continue;
			for (auto t2Role : * tVec)
			{
				if (t2Role == tRole)		//�ڹ�����Χ���м��ܶ�ӦĿ��
				{
					setGroupIndex(tGroupIndex);
					setEffIndex(tEffectIndex);
					setCriAtk(false);
					return true;
				}
			}
		}
		setGroupIndex(tGroupIndex);
		setEffIndex(tEffectIndex);
		setCriAtk(false);
		return false;
	}

	void BaseRole::updateRole( float dt )
	{
		if (getEnemy() || !getOtherCamp())
			return ;
		if (mEnterTime > 0)
		{
			mEnterTime -= dt;
			if (mEnterTime < 0)
			{
				mEnterTime = 0;
				bNotification->postNotification(MsgButtonStateUpdate);
			}
		}
		if (!getCriAtk() && mSkillTime > 0)
		{
			mSkillTime -= dt;
			if (mSkillTime < 0)
			{
				mSkillTime = 0;
				bNotification->postNotification(MsgButtonStateUpdate);
			}
		}
	}

	void BaseRole::roleDie()
	{
		if (this->getAliveState())
		{
			setAliveState(false);
			setFirstInit(false);
			mRoleObject->AliveDie();
			clearHitAlive();
			setHp(0);
			setGridIndex(INVALID_GRID);
			if (getMoveObject())getMoveObject()->removeFromParentAndCleanup(true);
			setMoveObject(nullptr);
			getBuffManage()->Buffclear();
			float tDirectorTime = CCDirector::sharedDirector()->getScheduler()->getTimeScale();
			setEnterTime(getBaseData()->getColdDown()/tDirectorTime);
			setSkillTime(0);
			setBattle(false);
			bNotification->postNotification(MsgRoleDie,this);
		}
	}
	//��ɫ����ɱ����ʱ����е�һЩ����
	void BaseRole::byOtherKill(BaseRole* pRole)
	{
		//���ٻ������Ľ�ɫ�ĳ�ʼ�����ݸ�����ȥ����
		this->roleDie();
		if (getCallType() == sCallType::eBoxHaveRole)
		{
			if (getBaseData()->hasActiveSkill())
			{
				BaseRole* tRole = getCallRole(getBaseData()->getActiveSkill());							//����ɱ�����ó��ٻ���ļ��� 	
				tRole->setGridIndex(getBaseData()->getInitGrid());
				tRole->setOtherCamp(pRole->getOtherCamp());
				tRole->setEnemy(pRole->getEnemy());
				bNotification->postNotification(MsgCreateRoleObject,tRole);								//ʯͷ�ɱ�˫�����й������ж��Ƿ�Ϊ�ж���Ӫ�ļ��ɡ�
			}else{
				CCLOG("[ *ERROR ] BaseRole::getCurrSkill CritSKILL IS NULL");
			}
		}
	}

	//�������һ�����õķ��������õ�,���Ƿ���ƶ��жϱ����жϵ�
	bool BaseRole::hasOtherRole( int pGrid )
	{
		BaseRole* tRole = mRoleLayer->getAliveByMoveGrid(pGrid);
		if (!tRole || tRole->getOtherCamp() != getOtherCamp())
			return false;
		return true;
	}

	bool BaseRole::inMoveObjectArea( int pGrid )
	{
		if ( !mMoveObject )
			return false;
		if (getCaptain())
		{
			if (getOtherCamp())
			{
				if (mBattleModel->isPvEBattle() && pGrid < C_PVEStopGrid)
					return true;
			}else{
				if ( pGrid >= C_CAPTAINGRID )
					return true;
			}	
		}
		for (auto tGrid : mMoveObject->grids)
		{
			if (tGrid == pGrid)
				return true;
		}
		return false;
	}

};