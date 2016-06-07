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



class CRroleData;
class CDataPool;

enum CityActionType{
	CA_None = 0,
	CA_GetHero,
	CA_BlackSmith,
	CA_Levelup,
	CA_Combine,
	CA_GoToChapater,
	CA_GoToStage,
	CA_GoToReward,
	CA_GoToWorldBoss
};

enum Task_Type
{
	Evole_Task,
	Normal_Task,
};

namespace BattleSpace{
	class WarManager;
	class MapManager;
	class TerrainManager;
	class CombatGuideManage;
};

USING_NS_CC;
class DataCenter : public CCObject
{
public:
	DataCenter();
	virtual ~DataCenter();
	static DataCenter* sharedData();
	static void destroy();
public:
	BattleSpace::WarManager* getWar();
	BattleSpace::TerrainManager* getTer();
	BattleSpace::MapManager* getMap();
	UserManager* getUser();
	SkillGM* getSkill();
	TaskGM* getTask(int taskType);
	TollgateGM* tollgate();
	HeroInfoGM* getHeroInfo();
	ItemGM* getItemDesc();
	CStageData* getStageData();
	BattleSpace::CombatGuideManage* getCombatGuideMg();
	bool initWar();
	RewardDataGM* getRewardDataGM();
	CDataPool* getDataPool();

	CC_SYNTHESIZE(CityActionType, m_cityActionType, CityActionType);
	CC_SYNTHESIZE(CRroleData*, m_RoleData, RoleData);
protected:
	virtual bool init();	
protected:
	BattleSpace::MapManager* m_map;
	BattleSpace::WarManager* m_war;
	BattleSpace::TerrainManager* m_ter;
	UserManager* m_user;
	SkillGM* m_skill;
	TaskGM*  m_task;
	TollgateGM* m_tollgate;
	HeroInfoGM* m_heroInfo;
	ItemGM* m_itemDesc;
	BattleSpace::CombatGuideManage* m_CombatguideMG;
	CStageData *m_stageData;
	RewardDataGM *m_rewardData;
	CDataPool *m_dataPool;
private:
	static DataCenter* m_dataCenter;
};
#define DATAPOOL DataCenter::sharedData()->getDataPool()
#endif // !_DATA_CENTER_H_
