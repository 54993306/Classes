#include "DataCenter.h"
#include "TerrainManager.h"
#include "WarManager.h"
#include "MapManager.h"
#include "warscene/CombatGuideManage.h"
#include "warscene/CHeroSoundData.h"
#include "cocoa/CCArray.h"
#include "common/CommonFunction.h"
#include "DataPool.h"

DataCenter* DataCenter::m_dataCenter = nullptr;

DataCenter::DataCenter()
	:m_war(nullptr),m_map(nullptr),m_ter(nullptr),m_user(nullptr),m_wuJiang(nullptr)
	,m_battle(nullptr),m_skill(nullptr),m_tollgate(nullptr),m_task(nullptr)
	,m_heroInfo(nullptr),m_itemDesc(nullptr),m_CombatguideMG(nullptr),m_stageData(nullptr)
	,m_rewardData(nullptr), m_cityActionType(CA_None),m_RoleData(nullptr)
{}
DataCenter::~DataCenter()
{
	CC_SAFE_RELEASE(m_war);
	CC_SAFE_RELEASE(m_map);
	CC_SAFE_RELEASE(m_ter);
	CC_SAFE_RELEASE(m_user);
	CC_SAFE_DELETE(m_wuJiang);
	CC_SAFE_DELETE(m_battle);
	CC_SAFE_DELETE(m_skill);
	CC_SAFE_DELETE(m_tollgate);
	CC_SAFE_DELETE(m_task);
	CC_SAFE_DELETE(m_heroInfo);
	CC_SAFE_RELEASE(m_CombatguideMG);
	CC_SAFE_DELETE(m_stageData);
	CC_SAFE_DELETE(m_rewardData);
	CC_SAFE_DELETE(m_RoleData);

	m_map = nullptr;
	m_war = nullptr;
	m_ter = nullptr;
	m_user = nullptr;
	m_wuJiang = nullptr;
	m_battle = nullptr;
	m_skill = nullptr;
	m_tollgate = nullptr;
	m_task = nullptr;
	m_heroInfo = nullptr;
	m_CombatguideMG = nullptr;
	m_rewardData = nullptr;
}
DataCenter* DataCenter::sharedData()
{
	if( m_dataCenter == nullptr )
	{
		m_dataCenter = new DataCenter();
		m_dataCenter->init();
	}
	return m_dataCenter;
}
bool DataCenter::init()
{
	m_user = UserManager::create();
	m_user->retain();

	m_dataPool = new CDataPool;
	if (!initWar())
	{
		CCLOG("[ *ERROR ] DataCenter::init");
		return false;
	}
	return true;
}

MapManager* DataCenter::getMap()
{
	return m_map;
}

WarManager* DataCenter::getWar()
{
	return m_war;
}
TerrainManager* DataCenter::getTer()
{
	return m_ter;
}
UserManager* DataCenter::getUser()
{
	return m_user;
}
WuJiangGM* DataCenter::getWuJiang()
{
	if(!m_wuJiang)
	{
		m_wuJiang = new WuJiangGM();
	}
	return m_wuJiang;
}
EmBattleModel* DataCenter::getBattle()
{
	if(!m_battle)
	{
		m_battle = new EmBattleModel();
	}
	return m_battle;
}
SkillGM* DataCenter::getSkill()
{
	if(!m_skill)
	{
		m_skill = new SkillGM();
	}
	return m_skill;
}


TollgateGM* DataCenter::tollgate()
{
	if(!m_tollgate)
	{
		m_tollgate = new TollgateGM();
	}
	return m_tollgate;
}

//战斗数据初始化
bool DataCenter::initWar()
{
	if(!m_RoleData)
	{
		m_RoleData = new CRroleData;
		m_RoleData->initData();
	}
	if (!m_map)
	{
		m_map = MapManager::create();
		m_map->retain();
	}
	if (! m_war)
	{
		m_war= WarManager::create();			
		m_war->retain();
	}
	if (! m_ter)
	{
		m_ter = TerrainManager::create();
		m_ter->retain();
	}
	if (! m_CombatguideMG)
	{
		m_CombatguideMG = CombatGuideManage::create();
		m_CombatguideMG->retain();
	}
	if (m_war && m_ter && m_map)
	{
		return true;
	}else{
		return false;
	}
}

TaskGM* DataCenter::getTask(int taskType)
{
	if (!m_task)
	{
		m_task = new TaskGM();
	}
	if (taskType == Evole_Task)
	{
		m_task->parseEvolve();
	}
	else
	{
		m_task->parseTask();
	}
	return m_task;
}

HeroInfoGM* DataCenter::getHeroInfo()
{
	if (!m_heroInfo)
	{
		m_heroInfo = new HeroInfoGM();
	}
	return m_heroInfo;
}

ItemGM* DataCenter::getItemDesc()
{
	if (!m_itemDesc)
	{
		m_itemDesc = new ItemGM();
	}
	return m_itemDesc;
}

CombatGuideManage* DataCenter::getCombatGuideMg()
{
	return m_CombatguideMG;
}

CStageData* DataCenter::getStageData()
{
	if (!m_stageData)
	{
		m_stageData = new CStageData();
	}
	return m_stageData;
}

RewardDataGM* DataCenter::getRewardDataGM()
{
	if (!m_rewardData)
	{
		m_rewardData = new RewardDataGM();
	}
	return m_rewardData;
}

void DataCenter::destroy()
{
	LGPause(CCDirector::sharedDirector()->getRunningScene(), true);
	CC_SAFE_DELETE(m_dataCenter);
}

CDataPool* DataCenter::getDataPool()
{
	return m_dataPool;
}
