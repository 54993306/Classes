#ifndef _COMBATLOGINC_
#define _COMBATLOGINC_
/************************************************************************/
//                                                                      
//						处理战斗流程逻辑维护任务数组
//
//
//  [8/12/2015 Lxc]
/************************************************************************/
#include "AppUI.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "update/CDownloadPackageDelegate.h"

struct GameFinishData
{
	int reason;
	bool res;
	int roundNum;
	int aliveNum;
};

struct CMonster;
class BufExp;
class MoveRule;
class WarScene; 
class WarAlive;
class WarAssist;
class ActObject;
class HurtCount;
class TerrainExp;
class WarControl;
class StoryLayer;
class WarManager;
class SkillRange;
class WarMapData;
class WarMapLayer;
class TerrainLayer;
class BattleResult;
class CombatEffect;
class WarAliveLayer;
class CombatGuideLayer;

class CombatLogic : public CCNode, public DownloadPackageDelegate
{
public:
	CombatLogic(); 
	~CombatLogic();
	virtual void onExit();
	virtual bool init();
	virtual void update(float delta);
	void updateOneSecond(float dt);
	CREATE_FUNC(CombatLogic);
	void TaskArray();
	void showRound();
	void RunLogic(float delt);
	void initMember();
	void ExcuteAI(float dt);
	void AliveDieDispose(CCObject* ob);
	void ActRemove(CCObject* ob);
	void NextBatch(float dt);
	void CBackLayer(CCObject* ob);
	void CombatResult(CCObject* ob);
	void RoundStart(CCObject* ob);
	void CritAtkEnd(CCObject* ob);
	void ChangeSpeed(CCObject* ob);
	void StoryEndEvent(CCObject* ob);
	void BatterRecord(CCObject* ob);
	void AtkLogic(CCObject* ob);
	void ChangeCost(CCObject* ob);
	void CostCount(WarAlive* alive,float dt);
	void AliveExcuteAI(WarAlive* alive,CCDictionary*pDic);	
	void MonsterExcuteAI(WarAlive* alive,float dt); 
	void HeroExcuteAI(WarAlive* alive);
	bool AttackJudge(WarAlive* alive);
	void AliveCritEnd(WarAlive* alive);
	//bool getHitAlive(WarAlive* alive);
	bool StateDispose(WarAlive* alive,float dt);
	void ContinueStage(int type, google::protobuf::Message *msg);
	void OnBattleFinish(int type, google::protobuf::Message *msg);
	bool IsAutoMoveType(WarAlive*alive);
	void startCountDown(int iTime);		//开始倒计时
	CC_SYNTHESIZE(WarAssist*,m_Assist,WarAssist);
	CC_SYNTHESIZE(CCArray*,m_task,TaskArray);
	CC_SYNTHESIZE(float,m_MaxCost,MaxCost);
	CC_SYNTHESIZE(bool,m_Run,RunLogic);
	CC_SYNTHESIZE(bool,m_CostChange,CostChange);
	CC_PROPERTY(float,m_CurrCost,CurrCost);
	CC_SYNTHESIZE(int, m_iGameTimeCount, GameTimeCount);		//倒计时时间
	CC_SYNTHESIZE(bool, m_bCountDown, CountDown);				//倒计时开启

	float getTime() { return m_time; }
	void scheduleForRequestFinish();
	void scheduleForRequesBossFinish();
	bool monsterFlee(WarAlive* alive);
	void onPause();
	void onResume();
	void reliveSuccess();

	void downloadPackageEnd(bool bAnswer);
protected:
	void onClickDownloadPackage(CCObject* pSender);
private:			
	float m_FrameTime;				//累计时间
	int m_send;						//记录以请求的消息批次
	int	m_CurrBatchNum;				//记录当前战斗批次
	float m_time;
	float m_speed;					//速度变化参数
	int m_BatchNum;					//当前关卡批次
	bool m_FiratBatch;				//第一批怪物提示
	bool m_Record;					//是否记录连击
	int m_RecordNum;				//点击次数
	int m_PlayerNum;				//播放次数
	WarAlive* m_Alive;				//存储释放技能的武将信息
	BufExp* m_bufExp;
	WarScene* m_Scene;
	MoveRule* m_MoveRule;
	TerrainExp* m_terExp;
	WarMapData* m_MapData;
	WarManager* m_Manage;
	WarControl* m_UILayer;
	HurtCount* m_HurtCount;
	WarMapLayer* m_MapLayer;
	SkillRange* m_SkillRange;
	StoryLayer* m_StoryLayer;
	WarAliveLayer* m_AliveLayer;
	TerrainLayer* m_TerrainLayer;
	CombatEffect* m_CombatEffect;
	CombatGuideLayer* m_GuideLayer;
	bool m_bFinish;
	bool m_bGameOver;
	GameFinishData m_gameFinishData;
	float m_fCurrentCostAdd;
};
#endif