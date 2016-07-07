#ifndef _WAR_MANAGER_H_
#define _WAR_MANAGER_H_
/************************************************************* 
 *
 *
 *		Data : 2016.6.22
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "AppUI.h"
#include "Battle/BattleMacro.h"
#include "Battle/RoleMacro.h"
#include <spine/spine-cocos2dx.h>
class ArmatureEventDataMgr;
namespace BattleSpace
{
	class BaseRole;
	class EffectData;
	class EffectInfo;
	class CombatLogic;
	class BuffConfig;
	class StoryData;
	using namespace std;
	typedef map<unsigned int,BaseRole*> RolesMap;	
	typedef std::pair<spSkeletonData*,spAtlas*> SpData;
	typedef map<std::string,SpData> MapSkeletonData;

	class WarManager : public CCObject
	{
	public:
		WarManager();
		virtual ~WarManager(){};
		bool init(){return true;}
		CREATE_FUNC(WarManager);	
	public:
		void addBattleRole(BaseRole* alive);
		BaseRole* getAlive(unsigned int aliveID);
		BaseRole* getAliveByGrid(int grid);
		BaseRole* getAliveByType(sMonsterSpecies type,bool Monster = true);
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
		const RolesMap* getRolesMap();
		void updateAlive();
		bool isSpine(int id);
		SpData* getSpineData(string Name);
		MapSkeletonData& getSpineMap(){return mSpineData;}
		/*************************************************/
		vector<int>* getBossHurtVec(){return &m_VecBossHurt;}
		vector<int>* getMoveVec(){return &mMoveArea;}
		vector<int>* getNndefinedArea(){return &mUndefinedArea;}
		vector<int>* getAddcostVec(){return &mCostArea;}

		vector<BaseRole*>* inBattleHeros(bool pSort = false);
		vector<BaseRole*>* inBattleMonsters(bool pSort = false);
		vector<BaseRole*>* inBattleRoles(bool pSort = false);

		CCArray* getRoles(bool pEnemy,bool isAlive = false);
		void sortRole(CCArray* pRoles);
		CCArray* getHeros(bool pIsAlive = false);
		CCArray* getMonsters(bool pIsAlive = false);

		void initHeros(vector<BaseRole*>& pHeros,bool pAlive = false);
		void initMonsters(vector<BaseRole*>& pMonsters,bool pAlive = false);
		void initRoles(vector<BaseRole*>& pRoles,bool pAlive,bool pEnemy);

		int getCurrRandomGrid(int grid,bool hasAlive = false);
		void initCommonData();
		void clearOldData();
		void initRoleSkillInfo(int pEffectID,BaseRole* pRole);
		bool inAddCostArea(int pGrid);
		bool inMoveArea(int pGrid);
		bool inUnDefineArea(int pGrid);
	public:
		CC_SYNTHESIZE(int, m_iWorldBossRank, WorldBossRank);			//世界BOSS狂暴状态
		CC_SYNTHESIZE(int,m_LoadImage,LoadImage);						//用于记录加载图片的id
		CC_SYNTHESIZE(int,mStageID,StageID);							//关卡ID
		CC_SYNTHESIZE(int,mBatch,Batch);								//批次ID
		CC_SYNTHESIZE(int, m_iLastStageId, LastStageId);				//上一次关卡信息(-1标记悬赏府)
		CC_SYNTHESIZE(int, m_StageType, StageType);						//大关卡还是小关卡
		CC_SYNTHESIZE(bool, m_bNormal, Normal);							//精英还是普通
		PROPERTY_CONSTREAD(bool, m_BossModel, WorldBoss);				//世界BOSS模型
		CC_SYNTHESIZE(int, m_ChapterIndex, ChapterIndex);				//在打哪一章节
		CC_SYNTHESIZE(int, m_ChapterCount, ChapterCount);				//章节个数
		CC_SYNTHESIZE(int, mBossHurtPe,BossHurtPe);					//世界boss伤害加成百分比
		PROPERTY_CONSTREAD(int, m_VerifyNum,VerifyNum);					//验证所有武将造成的伤害信息(验证伤害是否被恶意修改)
		PROPERTY_CONSTREAD(int, m_BossHurtCount,BossHurtCount);			//造成的总伤害
		CC_SYNTHESIZE(bool, mBattleOver, battleOver);					//战斗结束
		CC_SYNTHESIZE(bool, mFirstStage, FirstStage);					//首次进入关卡
		CC_SYNTHESIZE(bool, mAutoPlay, AutoPlay);						//开启自动战斗
	public:
		CC_SYNTHESIZE(float,mMaxCost,MaxCost);
		CC_SYNTHESIZE(float,mCurrCost,CurrCost);
		CC_SYNTHESIZE(float,mCostSpeed,CostSpeed);
		CC_SYNTHESIZE(bool,mLogicState,LogicState);
		void costUpdate(float delta);
		void changeCost(float pCost);
	protected:
		vector<int> mUndefinedArea;								//未定义区域,任意对象都不可进入
		vector<int> mMoveArea;									//可移动格子存储
		vector<int> mCostArea;									//可增加cost格子存储
		vector<int> m_VecBossHurt;								//服务器验证伤害,每5秒存储一次当前造成总伤害信息
		MapSkeletonData mSpineData;								//
		vector<int> m_SpineID;									//记录spine的ID
		RolesMap mBattleRole;									//战场上活着的英雄
		EffectData* m_efdata;
		BuffConfig* mBuffData;
		StoryData* mStoryData;
		ArmatureEventDataMgr* m_armatureEventDataMgr;				
		vector<BaseRole*> mBattleHeros;
		vector<BaseRole*> mBattleMonsters;
		vector<BaseRole*> mBattleRoles;
	};
};
#endif // !_WAR_MANAGER_H_
