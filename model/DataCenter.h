#ifndef _DATA_CENTER_H_
#define _DATA_CENTER_H_
#include "cocos2d.h"
#include "UserManager.h"
#include "SkillGM.h"
#include "TollgateGM.h"
#include "TaskGM.h"
#include "HeroInfoGM.h"
#include "ItemGM.h"
#include "tollgate/StageData.h"
#include "RewardGM.h"
#include "DataPool.h"
#include "ShareGM.h"

enum CityActionType
{
	CA_None = 0,
	CA_GetHero,
	CA_BlackSmith,
	CA_Levelup,
	CA_Combine,
	CA_GoToChapater,
	CA_GoToStage,
	CA_GoToReward,
	CA_GoToWorldBoss,
	CA_PVP
};

enum Task_Type
{
	Evole_Task,
	Normal_Task,
};

struct VsAnimateData
{
	std::string sEnemyRoleName;
	std::string sSelfRoleName;
	int			iEnemyLeader;
	int			iSelfLeader;
	int			iEnemyHead;
	int			iSelfHead;
	std::string	sEnemyFacebookId;
	std::string	sSelfFacebookId;
public:
	VsAnimateData():sEnemyRoleName(""),sSelfRoleName(""),iEnemyLeader(0),iSelfLeader(0),
		iEnemyHead(0),iSelfHead(0),sEnemyFacebookId(""),sSelfFacebookId("")
	{}
};

class CDataPool;

USING_NS_CC;

class DataCenter : public CCObject
{
public:
	DataCenter();
	virtual ~DataCenter();
	static DataCenter* sharedData();
	static void destroy();
public:
	UserManager* getUser();
	SkillGM* getSkill();
	TaskGM* getTask(int taskType);
	TollgateGM* tollgate();
	HeroInfoGM* getHeroInfo();
	ItemGM* getItemDesc();
	CStageData* getStageData();
	RewardDataGM* getRewardDataGM();
	CDataPool* getDataPool();
	ShareGM* getShareData();

	CC_SYNTHESIZE(CityActionType, m_cityActionType, CityActionType);
	VsAnimateData& getVsAnimateData();
protected:
	virtual bool init();	
protected:
	UserManager* m_user;
	SkillGM* m_skill;
	TaskGM*  m_task;
	TollgateGM* m_tollgate;
	HeroInfoGM* m_heroInfo;
	ItemGM* m_itemDesc;
	CStageData *m_stageData;
	RewardDataGM *m_rewardData;
	CDataPool *m_dataPool;
	ShareGM *m_shareData;
	VsAnimateData m_VsAnimateData;
private:
	static DataCenter* m_dataCenter;
};
#define DATAPOOL DataCenter::sharedData()->getDataPool()
#endif // !_DATA_CENTER_H_
