#ifndef _WAR_MANAGER_H_
#define _WAR_MANAGER_H_
#include "AppUI.h"
#include <map>
//#include "model/BattleData.h"
#include "Battle/TempData.h"
#include "tollgate/Chapter.h"
#include <spine/spine-cocos2dx.h>
#include "Battle/BattleMacro.h"
#include "Battle/BattleTrap.h"
#include "protos/boss_protocol.pb.h"
#include "protos/stage_protocol.pb.h"
class WarAlive;
class EffectData;
class ArmatureEventDataMgr;
class SpecialEffectData;
class EffectInfo;
class SpecialEffectInfo;
class terData;
class CombatLogic;
class CritSkillImage;
class MoveObject;
class BuffData;
class StoryData;
using namespace std;
typedef map<unsigned int,WarAlive*> Members;	
typedef map<unsigned int,unsigned int> Movemap;			//存储当前回合可移动位置
typedef map<unsigned int,vector<TempMonster>> BatchMonster;
typedef std::pair<spSkeletonData*,spAtlas*> SpData;
typedef map<std::string,SpData> MapSkeletonData;

/************************************************************************/
/*
				战场信息分发管理							//  [9/9/2014 xc_lin]
*/
/************************************************************************/
//战斗数据初始化
struct BattleServerData 
{
	vector<TempMonster> MonsterList;
	vector<TempHero> HeroList;
	vector<BattleTrap> TrapList;
};

class WarManager : public CCObject
{
public:
	WarManager();
	virtual ~WarManager();
	bool init(){return true;}
	CREATE_FUNC(WarManager);	
public:
	void addAlive(WarAlive* alive);
	WarAlive* getAlive(unsigned int aliveID);
	CCArray* getAlives(bool isAlive = false);
	WarAlive* getAliveByGrid(int grid);
	WarAlive* getCallAlive(WarAlive* pAlive,TempSkill* skill);
	WarAlive* getAliveByType(AliveType type,bool Monster = true);
	WarAlive* getAbsentCallAlive(WarAlive* fatherAlive);
	void BattleDataClear();
	void ReleaseSpineData();
	void initData();
	bool checkMonstOver();
	void initBatchData(int batch);
	void initAlive(WarAlive* alive);
	void initCallAlive(WarAlive* alive,WarAlive*cAlive);	//召唤类武将初始化
	BattleServerData* getBattleData();						//得到战场数据
	vector<TempMonster*>* getCallMonst();
	EffectData* getEffData();
	BuffData* getBuffData();
	StoryData* getStoryData();
	ArmatureEventDataMgr* getArmatureDataMgr();
	terData* getTerData(){return m_terData;};
	void setTollgate(CStage& tag);
	CStage* getTollgete();
	Members* getMembers();
	void updateAlive();
	vector<WarAlive*>* getVecHeros(){return &m_Heros;}
	vector<WarAlive*>* getVecMonsters(){return &m_Monsters;}
	vector<WarAlive*>* getSkillTargets(WarAlive* pAlive);
	bool isSpine(int id);
	SpData* getSpineData(std::string Name);
	MapSkeletonData& getSpineMap(){return m_MapSpineData;}
	vector<int>* getBossHurtVec(){return &m_VecBossHurt;}
	vector<int>* getMoveVec(){return &m_CantMoveGrid;}
	vector<int>* getAddcostVec(){return &m_AddCostGrid;}
	/*************************************************/
	void sortArrayByGridIndex(CCArray* arr);
	CCArray* getHeros(bool isAlive = false,bool sort = true);
	CCArray* getMonsts(bool isAlive = false,bool sort = true);
	CCArray* getAlivesByCamp(bool enemy,bool isAlive,bool sort);
	WarAlive* getNewCallAlive(WarAlive* Father,int CallId);
	BattleServerData* getServerData(){return &m_ServerData;}

	void initBattleData( BattleResponse*batRes );
	void initWordBossData( WarResponse*batRes );
	void initCommonData();
	void clearBeforeData();
public:
	CC_SYNTHESIZE(int, m_iWorldBossRank, WorldBossRank);			//世界BOSS狂暴状态

	CC_SYNTHESIZE_READONLY(CritSkillImage*,m_CritImage,CritImage)
	CC_SYNTHESIZE(int,m_LoadImage,LoadImage);						//用于记录加载图片的id
	CC_SYNTHESIZE(int,m_StageID,StageID);							//关卡ID
	CC_SYNTHESIZE(int,m_Batch,Batch);								//批次ID
	CC_SYNTHESIZE(CombatLogic*,m_Logic,LogicObj);					//存储一个逻辑对象
	CC_SYNTHESIZE(CChapter, m_chapter, Chapter);					//存储章节信息
	CC_SYNTHESIZE(int, m_iLastStageId, LastStageId);				//上一次关卡信息(-1标记悬赏府)
	CC_SYNTHESIZE(int, m_StageType, StageType);						//大关卡还是小关卡
	CC_SYNTHESIZE(bool, m_bNormal, Normal);							//精英还是普通
	CC_PROPERTY(bool, m_BossModel, WorldBoss);						//世界BOSS模型
	CC_SYNTHESIZE(int, m_ChapterIndex, ChapterIndex);				//在打哪一章节
	CC_SYNTHESIZE(int, m_ChapterCount, ChapterCount);				//章节个数
	CC_SYNTHESIZE(int, m_iReliveNeedGoldNum, ReliveNeedGoldNum);	//续关需要的元宝数量
	CC_SYNTHESIZE(int, m_BossHurtPe,BossHurtPe);					//世界boss伤害加成百分比
	CC_PROPERTY(int, m_VerifyNum,VerifyNum);						//验证所有武将造成的伤害信息(验证伤害是否被恶意修改)
	CC_PROPERTY(int, m_BossHurtCount,BossHurtCount);				//造成的总伤害
	CC_SYNTHESIZE(bool, m_BattleOver, battleOver);					//战斗结束
protected:
	vector<int> m_CantMoveGrid;									//可移动格子存储
	vector<int> m_AddCostGrid;									//可增加cost格子存储
	vector<int> m_VecBossHurt;									//服务器验证伤害,每5秒存储一次当前造成总伤害信息
	MapSkeletonData m_MapSpineData;								//
	vector<int> m_SpineID;										//记录spine的ID
	Members m_members;											//战场上活着的英雄
	BattleServerData m_ServerData;								//存储战场信息
	vector<TempMonster*> m_CallRole;								//存储召唤类型武将
	EffectData* m_efdata;
	BuffData* m_BuffData;
	StoryData* m_StoryData;
	ArmatureEventDataMgr* m_armatureEventDataMgr;
	terData* m_terData;
	CStage tollgateInfo;
	int m_SceneTarpID;											//场景效果类型记录
	vector<WarAlive*> m_Heros;
	vector<WarAlive*> m_Monsters;
};
#endif // !_WAR_MANAGER_H_