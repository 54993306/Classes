#include "DataCenter.h"
#include "common/CommonFunction.h"
#include "DataPool.h"

DataCenter* DataCenter::m_dataCenter = nullptr;

DataCenter::DataCenter()
:m_user(nullptr),m_skill(nullptr),m_tollgate(nullptr),m_task(nullptr)
,m_heroInfo(nullptr),m_itemDesc(nullptr),m_stageData(nullptr)
,m_rewardData(nullptr), m_cityActionType(CA_None),m_shareData(nullptr)
{}
DataCenter::~DataCenter()
{
	CC_SAFE_RELEASE(m_user);
	CC_SAFE_DELETE(m_skill);
	CC_SAFE_DELETE(m_tollgate);
	CC_SAFE_DELETE(m_task);
	CC_SAFE_DELETE(m_heroInfo);
	CC_SAFE_DELETE(m_stageData);
	CC_SAFE_DELETE(m_rewardData);
	CC_SAFE_DELETE(m_shareData);
	m_user = nullptr;
	m_skill = nullptr;
	m_tollgate = nullptr;
	m_task = nullptr;
	m_heroInfo = nullptr;
	m_rewardData = nullptr;
	m_shareData = nullptr;
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

	return true;
}
UserManager* DataCenter::getUser()
{
	return m_user;
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

ShareGM* DataCenter::getShareData()
{
	if (!m_shareData)
	{
		m_shareData = new ShareGM();
	}
	return m_shareData;
}

VsAnimateData& DataCenter::getVsAnimateData()
{
	return m_VsAnimateData;
}