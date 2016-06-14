#ifndef _WAR_MANAGER_H_
#define _WAR_MANAGER_H_
/************************************************************************/
/*
				战场信息分发管理							//  [9/9/2014 xc_lin]
*/
/************************************************************************/
#include "AppUI.h"
#include "Battle/BattleMacro.h"
#include "Battle/RoleMacro.h"
#include <spine/spine-cocos2dx.h>
class ArmatureEventDataMgr;

namespace BattleSpace{
	class BaseRole;
	class EffectData;
	class SpecialEffectData;
	class EffectInfo;
	class SpecialEffectInfo;
	class terData;
	class CombatLogic;
	class MoveObject;
	class BuffConfig;
	class StoryData;
	class RoleSkill;
	class ActObject;
	using namespace std;
	typedef map<unsigned int,BaseRole*> Members;	
	typedef std::pair<spSkeletonData*,spAtlas*> SpData;
	typedef map<std::string,SpData> MapSkeletonData;

	class WarManager : public CCObject
	{
	public:
		WarManager();
		virtual ~WarManager();
		bool init(){return true;}
		CREATE_FUNC(WarManager);	
	public:
		void addAlive(BaseRole* alive);
		BaseRole* getAlive(unsigned int aliveID);
		BaseRole* getAliveByGrid(int grid);
		BaseRole* getCallAlive(BaseRole* pAlive,const RoleSkill* skill);
		BaseRole* getAliveByType(E_ALIVETYPE type,bool Monster = true);
		BaseRole* getAbsentCallAlive(BaseRole* fatherAlive);
		void BattleDataClear();
		void ReleaseSpineData();
		void initData();
		bool checkMonstOver();
		void initMonsterByBatch(int batch);
		void initHeroData();
		EffectData* getEffData();
		BuffConfig* getBuffData();
		StoryData* getStoryData();
		ArmatureEventDataMgr* getArmatureDataMgr();
		terData* getTerData(){return m_terData;};
		const Members* getMembers();
		void updateAlive();
		vector<BaseRole*>* getVecHeros(bool pSort = false);
		vector<BaseRole*>* getVecMonsters(bool pSort = false);
		vector<BaseRole*>* getAliveRoles(bool pSort = false);
		bool isSpine(int id);
		SpData* getSpineData(string Name);
		MapSkeletonData& getSpineMap(){return m_MapSpineData;}
		vector<int>* getBossHurtVec(){return &m_VecBossHurt;}
		vector<int>* getMoveVec(){return &m_CantMoveGrid;}
		vector<int>* getAddcostVec(){return &m_AddCostGrid;}
		/*************************************************/
		void sortArrayByGridIndex(CCArray* arr);
		CCArray* getAlivesByCamp(bool enemy = true,bool isAlive = false,bool sort = true);
		BaseRole* getNewCallAlive(BaseRole* Father,int CallId);
		void initCommonData();
		void clearOldData();
		void initRoleSkillInfo(int pEffectID,BaseRole* pRole);
		bool inAddCostArea(int pGrid);
	public:
		CC_SYNTHESIZE(int, m_iWorldBossRank, WorldBossRank);			//世界BOSS狂暴状态
		CC_SYNTHESIZE(int,m_LoadImage,LoadImage);						//用于记录加载图片的id
		CC_SYNTHESIZE(int,m_StageID,StageID);							//关卡ID
		CC_SYNTHESIZE(int,m_Batch,Batch);								//批次ID
		CC_SYNTHESIZE(CombatLogic*,m_Logic,LogicObj);					//存储一个逻辑对象
		CC_SYNTHESIZE(int, m_iLastStageId, LastStageId);				//上一次关卡信息(-1标记悬赏府)
		CC_SYNTHESIZE(int, m_StageType, StageType);						//大关卡还是小关卡
		CC_SYNTHESIZE(bool, m_bNormal, Normal);							//精英还是普通
		CC_PROPERTY_CONSTREAD(bool, m_BossModel, WorldBoss);			//世界BOSS模型
		CC_SYNTHESIZE(int, m_ChapterIndex, ChapterIndex);				//在打哪一章节
		CC_SYNTHESIZE(int, m_ChapterCount, ChapterCount);				//章节个数
		CC_SYNTHESIZE(int, m_BossHurtPe,BossHurtPe);					//世界boss伤害加成百分比
		CC_PROPERTY_CONSTREAD(int, m_VerifyNum,VerifyNum);				//验证所有武将造成的伤害信息(验证伤害是否被恶意修改)
		CC_PROPERTY_CONSTREAD(int, m_BossHurtCount,BossHurtCount);		//造成的总伤害
		CC_SYNTHESIZE(bool, m_BattleOver, battleOver);					//战斗结束
		CC_SYNTHESIZE(bool, m_FirstStage, FirstStage);					//首次进入关卡
	protected:
		vector<int> m_CantMoveGrid;									//可移动格子存储
		vector<int> m_AddCostGrid;									//可增加cost格子存储
		vector<int> m_VecBossHurt;									//服务器验证伤害,每5秒存储一次当前造成总伤害信息
		MapSkeletonData m_MapSpineData;								//
		vector<int> m_SpineID;										//记录spine的ID
		Members m_members;											//战场上活着的英雄
		EffectData* m_efdata;
		BuffConfig* m_BuffData;
		StoryData* m_StoryData;
		ArmatureEventDataMgr* m_armatureEventDataMgr;
		terData* m_terData;
		int m_SceneTarpID;											//场景效果类型记录
		vector<BaseRole*> m_Heros;
		vector<BaseRole*> m_Monsters;
		vector<BaseRole*> m_AliveRoles;
	};
};
#endif // !_WAR_MANAGER_H_
