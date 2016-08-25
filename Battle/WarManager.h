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
 *		Description : 向角色管理中心转变,对它的只能进行拆解
 *
 *
 *************************************************************/

#include "AppUI.h"
#include "Battle/BattleMacro.h"
#include "Battle/RoleMacro.h"
namespace BattleSpace
{
	class BaseRole;
	class EffectData;
	class EffectInfo;
	class CombatLogic;
	class BuffConfig;
	class StoryData;
	class CaptainSkill;
	typedef map<unsigned int,BaseRole*> RolesMap;	
	class WarManager : public CCObject
	{
	public:
		WarManager();
		virtual bool init(){return true;}
		CREATE_FUNC(WarManager);	
	public:
		void addBattleRole(BaseRole* alive);
		BaseRole* getRole(unsigned int aliveID);
		BaseRole* getAliveByGrid(int grid);
		BaseRole* getAliveByType(sMonsterSpecies type,bool Monster = true);
		void BattleDataClear();
		void initData();
		bool checkBatchOver();
		void initMonsterByBatch(int batch);
		void initHeroData();

		const RolesMap* getRolesMap() const;
		void updateAlive();
		void saveWordBossHurt();
		vector<int>* getBossHurtVec(){return &m_VecBossHurt;}

		vector<BaseRole*>& inBattleHeros(bool pSort = false);
		vector<BaseRole*>& inBattleMonsters(bool pSort = false);
		vector<BaseRole*>& inBattleRoles(bool pSort = false);

		CCArray* getRoles(bool pEnemy,bool isAlive = false);
		void sortRole(CCArray* pRoles);
		CCArray* getHeros(bool pIsAlive = false);
		CCArray* getMonsters(bool pIsAlive = false);

		void initHeros(vector<BaseRole*>& pHeros,bool pAlive = false);
		void initMonsters(vector<BaseRole*>& pMonsters,bool pAlive = false);
		void initOtherCamp(vector<BaseRole*>& pOthers,bool pAlive = false);
		void initRoles(vector<BaseRole*>& pRoles,bool pAlive,bool pEnemy);

		void initCommonData();
		void clearOldData();

		bool lastBatch();
		void executeCaptainSkill(bool mOther = false);
	public:
		CC_SYNTHESIZE(int, m_iWorldBossRank, WorldBossRank);			//世界BOSS狂暴状态			   这些记录的信息应该放到战斗模式的子类中去进行实现
		CC_SYNTHESIZE(int, m_LoadImage,LoadImage);						//用于记录加载图片的id
		CC_SYNTHESIZE(int, mStageID,StageIndex);						//关卡ID
		CC_SYNTHESIZE(int, mBatch,Batch);								//关卡批次(从0开始计数)
		CC_SYNTHESIZE(int, mCurrBatch,CurrBatch);						//当前批次(从0开始计数)
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
		CC_SYNTHESIZE(int, m_iStageTimeCountDown, StageTimeCountDown);	//关卡倒计时
	public:
		CC_SYNTHESIZE(float, mMaxCost,MaxCost);
		CC_SYNTHESIZE(float, mCurrCost,CurrCost);
		CC_SYNTHESIZE(float, mCostSpeed,CostSpeed);
		CC_SYNTHESIZE(bool, mLogicState,LogicState);
		void costUpdate(float delta);
		void changeCost(float pCost);
	protected:
		vector<int> m_VecBossHurt;								//服务器验证伤害,每5秒存储一次当前造成总伤害信息
		RolesMap mBattleRole;									//战场上活着的英雄	
		CaptainSkill* mCaptainSkill;
		vector<BaseRole*> mBattleHeros;
		vector<BaseRole*> mBattleMonsters;
		vector<BaseRole*> mBattleRoles;
	};
};
#endif // !_WAR_MANAGER_H_
