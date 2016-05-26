#include "Battle/BattleRole.h"
#include "model/BuffManage.h"
#include "warscene/ConstNum.h"
#include "scene/alive/ActObject.h"

WarAlive::~WarAlive()
{
	CC_SAFE_RELEASE(m_BuffManage);
	m_BuffManage = nullptr;
	m_ActObject = nullptr;
	m_MoveObj = nullptr;
	grids.clear();
	moves.clear();
	AtkArea.clear();
	AliveS.clear();
	AtkGrid.clear();
	AtkAlive.clear();
}

WarAlive::WarAlive()
	:m_Enemy(false)
	,m_Hp(0)
	,m_MaxHp(0)
	,m_GridIndex(INVALID_GRID)
	,m_MoveGrid(0)					//�ƶ�״̬�µ�ǰλ��
	,m_AtkNum(0)					//��������
	,m_Move(true)					//�Ƿ���ƶ�
	,m_CritSkill(false)				//�Ƿ��ͷű�ɱ��
	,role(nullptr)					//�洢��������Ϣ
	,m_Hrt(0)						//�˺�ֵ
	,m_HrtPe(0)						//�˺��ٷֱ�
	,m_initCost(0)					//ŭ��ֵ
	,m_Batch(0)						//����
	,m_CostMax(0)					//���ŭ��ֵ
	,m_AddCost(0)					//�غ�ŭ��ֵ�仯
	,m_Atk(0)						//����		//����ʵ�ʵ��˺�����Ϊbuff�ͷ��������ݼ�����ֵ
	,m_Def(0)						//����
	,m_Hit(0)						//����
	,m_Doge(0)						//����
	,m_Crit(0)						//����
	,m_Zoom(0)						//���ű�
	,m_Renew(0)						//�ظ�
	,m_EfGroup(1)					//Ĭ��ִ��Ч����0
	,m_EffectIndex(0)				//��¼��ǰ����Ч��
	,m_Negate(false)				//����Ƿ���
	,m_ExecuteCap(false)			//ִ�жӳ������
	,m_TerType(0)					//����Ӱ������
	,m_TerTypeNum(0)				//����Ӱ����ֵ
	,m_cloaking(false)				//�Ƿ�Ϊ�����佫
	,m_SortieNum(0)					//��ǰ�غϳ�������
	,m_ActObject(nullptr)			//��Ӧ����ʾ����
	,m_BuffManage(nullptr)			//buf������
	,m_UILayout(0)					//��Ӧ��UI�ؼ�ID
	,m_Atktime(0)					//����ʱ��
	,m_AtkInterval(0)				//�ɹ���ʱ��
	,m_TimePercent(1)				//�������ٰٷֱ�
	,m_SpecialAtk(false)			//���⹥��
	,m_Battle(false)				//Ĭ��Ϊ������״̬
	,m_MoveSpeed(0)					//�ƶ��ٶ�
	,m_CritTime(0)					//�ͷű�ɱ��ʱ��
	,m_NorAtk(true)					//��ͨ����
	,m_FatherID(0)					//���ٻ����佫���д���Ϣ
	,m_Captain(false)				//�Ƿ�Ϊ�ӳ�
	,m_CritEffect(false)			//�Ƿ񲥷Ź���ɱ����Ч
	,m_TouchGrid(0)					//�����ƶ�����
	,m_AIState(false)				//�佫�Ƿ���AI״̬
	,m_TouchState(false)			//�佫�Ƿ񱻴���
	,m_MoveObj(nullptr)				//�洢�佫�ƶ�����
	,m_CallType(0)					//�佫����
	,m_Delaytime(0)					//�佫�����ӳ�ʱ��
	,m_AtkDelay(0)					//�佫�����ӳ�ʱ��
	,m_StateDelay(0)				//�佫״̬�ӳ�ʱ��
	,m_AliveState(COMMONSTATE)		//�佫״̬
	,m_AliveType(AliveType::Common)	//��ɫƷ�ʵȼ�
	,m_DieState(false)
	,m_CallAliveNum(0)				//record can claa alive number
	,m_LastAlive(false)
{
	setBuffManage(nullptr);
}

void WarAlive::setMoveGrid(int var)
{
	if (var == INVALID_GRID)
		CCLOG("[ TIPS ] WarAlive::setMoveGrid");
	m_MoveGrid = var;
}
int WarAlive::getMoveGrid(){ return m_MoveGrid; }

void WarAlive::setBuffManage(BuffManage*)
{
	m_BuffManage = BuffManage::create();
	m_BuffManage->retain();
}
BuffManage* WarAlive::getBuffManage() { return m_BuffManage; }

void WarAlive::setAtkNum(int var)
{
	m_AtkNum += var;
	if (m_AtkNum >= 3)					//���Ĵι���Ϊ���⹥��
	{
		m_AtkNum = 0;
		if (role->skill2.skillId)
		{
			this->ResetAttackState();
			this->setSpeAtk(true);
		}
	}
}
int WarAlive::getAtkNum(){return m_AtkNum;}

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
int WarAlive::getHp()	{ return m_Hp; }

void WarAlive::setDef(int def)
{
	m_Def = def;
	if (m_Def<=0)
		m_Def = 0;
}
int WarAlive::getDef()	{ return m_Def; }

void WarAlive::setNorAtk(bool var)
{
	m_NorAtk = var;
	if (!m_NorAtk)
		m_Atktime = 0;
}
bool WarAlive::getNorAtk(){return m_NorAtk;}

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
float WarAlive::getAtktime(){ return m_Atktime; }
//�����ͷű�ɱ��ʱ��
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
float WarAlive::getCritTime(){return m_CritTime;}

void WarAlive::setDelaytime(float var) { m_Delaytime -= var; }
float WarAlive::getDelaytime(){return m_Delaytime;}

void WarAlive::setAtkDelay(float var){m_AtkDelay = var;}
float WarAlive::getAtkDelay(){return m_AtkDelay;}

void WarAlive::setAliveStat(int var)
{
	m_AliveState = var;
}
int WarAlive::getAliveStat(){return m_AliveState;}

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
float WarAlive::getStatDelay(){return m_StateDelay;}

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
		grids.clear();
		for (int i=0;i<role->row;i++)
			for (int j =0;j<role->col;j++)
			{
				if (var+j*C_GRID_ROW+i>C_ENDGRID)
					continue;
				grids.push_back(var+j*C_GRID_ROW+i);
			}			
			sort(grids.begin(),grids.end());
	}
}

int WarAlive::getGridIndex(){return m_GridIndex;}

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
	int row = TouchGrids.at(0)%C_GRID_ROW;				//��С���ӵ�վλ
	if (row+role->row>C_GRID_ROW)						//�佫��ռ����,���ܳ�����ͼ��
	{
		m_TouchGrid=INVALID_GRID;
		TouchGrids.clear();
	}
}
int WarAlive::getTouchGrid(){return m_TouchGrid;}

void WarAlive::setCallType(int var)
{
	if (var == AutoSkill)							//����ս�����ͷż���(��ʯ��)
		setCriAtk( true );							//ֻ�б�ɱ����û�й���Ҳ���ͷŵ�
	m_CallType = var;
}

int WarAlive::getCallType(){return m_CallType;}

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
	setEffectGroup(1);
	ExcuteNextEffect();
}

void WarAlive::ExcuteNextEffect()
{
	setSortieNum(0);																//��ǰЧ����������
	setNegate(false);																//���÷��򹥻�
	AtkArea.clear();																//�������̶�����(�������߼���)
	AliveS.clear();																	//�������̶��佫
	AtkAlive.clear();
	AtkGrid.clear();
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

CSkill* WarAlive::getCurrSkill()
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

CEffect* WarAlive::getCurrEffect()
{
	if (getEffectGroup()<= getCurrSkill()->EffectList.size())
		if ( getCurrSkill()->EffectList.at(getEffectGroup()-1).size()>getEffIndex())
			return & getCurrSkill()->EffectList.at(getEffectGroup()-1).at(getEffIndex());
		else
			CCLOG("[ *ERROR ] EffIndex > EffectListSize  AliveID=%d mode=%d",getAliveID(),role->thumb);
	else{
		CCLOG("[ *ERROR ]Skill Effect Is NULL  AliveID=%d mode=%d EffectGroup=%d",getAliveID(),role->thumb,getEffectGroup());
		setEffectGroup(1);
	}	
	return nullptr;
	CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
}

bool WarAlive::NextEffect()
{
	int effId = getEffIndex() + 1;
	int ranNum = CCRANDOM_0_1()*100;								//0��100����
	if (getEffectGroup() > getCurrSkill()->EffectList.size())
		return false;
	if (getCurrSkill()->skillId&&effId < getCurrSkill()->EffectList.at(getEffectGroup()-1).size())			//�ж��Ƿ�Ϊ����һ����Ч
	{
		if (getCurrSkill()->EffectList.at(getEffectGroup()-1).at(effId).userRate > ranNum)
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
