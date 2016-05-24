#include "DataDefine.h"
#include "model/BuffManage.h"
#include "warscene/ConstNum.h"
#include "scene/alive/ActObject.h"
#include "common/CommonFunction.h"
#include "roleImg/RoleUpdate.h"
#include "Battle/MoveObject.h"
#include "Battle/BattleMessage.h"

AliveBase::AliveBase()
	:m_AliveID(0)
	,m_Model(0)
{}

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
	:m_IsEnemy(false)
	,m_Hp(0)
	,m_MaxHp(0)
	,m_GridIndex(INVALID_GRID)
	,m_MoveGrid(0)					//移动状态下当前位置
	,m_AtkNum(0)					//攻击次数
	,m_Move(true)					//是否可移动
	,m_CritSkill(false)				//是否释放必杀技
	,role(nullptr)					//存储服务器信息
	,m_Hrt(0)						//伤害值
	,m_HrtPe(0)						//伤害百分比
	,m_initCost(0)					//怒气值
	,m_Batch(0)						//批次
	,m_CostMax(0)					//最大怒气值
	,m_AddCost(0)					//回合怒气值变化
	,m_Atk(0)						//攻击		//用于实际的伤害计算为buff和服务器数据计算后的值
	,m_Def(0)						//防御
	,m_Hit(0)						//命中
	,m_Doge(0)						//闪避
	,m_Crit(0)						//暴击
	,m_Zoom(0)						//缩放比
	,m_Renew(0)						//回复
	,m_EfGroup(1)					//默认执行效果组0
	,m_EffectIndex(0)				//记录当前处理效果
	,m_Negate(false)				//标记是否反向
	,m_ExecuteCap(false)			//执行队长技标记
	,m_TerType(0)					//地形影响类型
	,m_TerTypeNum(0)				//地形影响数值
	,m_cloaking(false)				//是否为隐身武将
	,m_SortieNum(0)					//当前回合出击次数
	,m_ActObject(nullptr)			//对应的显示对象
	,m_BuffManage(nullptr)			//buf管理器
	,m_UILayout(0)					//对应的UI控件ID
	,m_Atktime(0)					//蓄气时间
	,m_AtkInterval(0)				//可攻击时间
	,m_TimePercent(1)				//增减攻速百分比
	,m_SpecialAtk(false)			//特殊攻击
	,m_Battle(false)				//默认为不上阵状态
	,m_MoveSpeed(0)					//移动速度
	,m_CritTime(0)					//释放必杀技时间
	,m_NorAtk(true)					//普通攻击
	,m_FatherID(0)					//被召唤的武将才有此信息
	,m_Captain(false)				//是否为队长
	,m_CritEffect(false)			//是否播放过必杀技特效
	,m_TouchGrid(0)					//触摸移动格子
	,m_AIState(false)				//武将是否在AI状态
	,m_TouchState(false)			//武将是否被触摸
	,m_MoveObj(nullptr)				//存储武将移动对象
	,m_CallType(0)					//武将类型
	,m_Delaytime(0)					//武将出现延迟时间
	,m_AtkDelay(0)					//武将攻击延迟时间
	,m_StateDelay(0)				//武将状态延迟时间
	,m_AliveState(COMMONSTATE)		//武将状态
	,m_AliveType(AliveType::Common)	//角色品质等级
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
int WarAlive::getAtkNum(){return m_AtkNum;}

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
float WarAlive::getAtktime(){ return m_Atktime; }
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
	int row = TouchGrids.at(0)%C_GRID_ROW;				//最小格子的站位
	if (row+role->row>C_GRID_ROW)						//武将所占格子,不能超出地图外
	{
		m_TouchGrid=INVALID_GRID;
		TouchGrids.clear();
	}
}
int WarAlive::getTouchGrid(){return m_TouchGrid;}

void WarAlive::setCallType(int var)
{
	if (var == AutoSkill)							//进入战场就释放技能(陨石类)
		setCriAtk( true );							//只有必杀技是没有怪物也会释放的
	m_CallType = var;
}

int WarAlive::getCallType(){return m_CallType;}

void WarAlive::ResetAttackState()
{
	if (!m_IsEnemy && m_CritSkill)
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
	setSortieNum(0);																//当前效果攻击次数
	setNegate(false);																//重置反向攻击
	AtkArea.clear();																//清除随机固定格子(好像还是逻辑的)
	AliveS.clear();																	//清除随机固定武将
	AtkAlive.clear();
	AtkGrid.clear();
	clearHitAlive();
}

bool WarAlive::canSummonAlive()
{
	if (role->skill3.skillType ==CallAtk)
	{
		if (m_Captain && m_CallAliveNum < 1)
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

void UserData::read(const protos::common::Role& role)
{
	if (this->m_level<role.rolelv()&&role.roleid()==this->getRoleID())
	{
		CRoleUpdate *roleUpdate = CRoleUpdate::create();
		roleUpdate->setZOrder(10);
		LayerManager::instance()->push(roleUpdate);
		roleUpdate->updateRole(this,&role);
	}

	if (role.has_rolename())
	{
		this->m_roleName = role.rolename();
	}
	if (role.has_roleid())
	{
		this->m_roleId = role.roleid();
	}
	if (role.has_rolelv())
	{
		this->m_level = role.rolelv();
	}
	if (role.has_rolecoin())
	{
		this->m_coin = role.rolecoin();
	}
	if (role.has_rolegold())
	{
		this->m_roleGold = role.rolegold();
	}
	if (role.has_roleexp())
	{
		this->m_exp = role.roleexp();
	}
	if (role.has_nextexp())
	{
		this->m_nextExp = role.nextexp();
	}
	if (role.has_roleaction())
	{
		this->m_roleAction = role.roleaction();
	}
	if (role.has_rolefood())
	{
		this->m_roleFood = role.rolefood();
	}
	if (role.has_rolefcoin())
		this->m_Friends = role.rolefcoin();

	if (role.has_actionlimit())
	{
		this->m_actionLimit = role.actionlimit();
	}

	if (role.has_actiontime())
	{
		this->m_actionTime = role.actiontime();
	}
	if (role.has_interval())
	{
		this->m_interval = role.interval();
	}

	if (role.has_vip())
	{
		this->m_vip = role.vip();
	}
	if (role.has_thumb())
	{
		this->m_thumb = role.thumb();
	}
	if (role.has_combat())
	{
		this->m_combat = role.combat();
	}
	if (role.has_newerstep())
	{
		this->m_newStep = role.newerstep();
	}
	else
	{
		this->m_newStep = 0;
	}

	if (role.has_mobileshop())
	{
		this->m_mobileShop = role.mobileshop();
	}
// 	else
// 	{
// 		this->m_mobileShop = false;
// 	}

	if (role.has_invitefriend())
	{
		this->m_inviteFriend = role.invitefriend();
	}
	if (role.has_fbid())
	{
		this->m_fbId = role.fbid();
	}
	if(role.has_firstlogin())
	{
		this->m_bFirstLogin = role.firstlogin();
	}
// 	this->m_thumb = 0;
// 	this->m_fbId = "677158805720606";
}

UserData::UserData()
	:m_raceType(0),m_level(0),m_roleFood(0),m_Friends(0),m_newStep(0),m_roleId(0)
	,m_coin(0),m_exp(0),m_nextExp(0),m_roleAction(0),m_mapId(0),m_roleGold(0),m_roleName("")
	,m_actionLimit(0),m_interval(0),m_vip(0),m_combat(0),m_thumb(0),m_inviteFriend(0)
	,m_isFBLogin(false),m_mobileShop(false),m_bFirstLogin(false)
{}