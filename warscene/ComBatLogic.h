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
	virtual void onEnter();
	virtual void onExit();
	virtual bool init();
	virtual void update(float delta);
	CREATE_FUNC(CombatLogic);
	void addEvent();
	void updateOneSecond(float dt);
	void cReturnLayer(CCObject* ob);
	void initCost();
	void initWordBossTime();
	void scheduleForRequestFinish();
	void scheduleForRequesBossFinish();
	void onPause();
	void onResume();
	CC_SYNTHESIZE(WarAssist*,m_Assist,WarAssist);
	CC_SYNTHESIZE(CCArray*,m_task,TaskArray);
	CC_SYNTHESIZE(float,m_MaxCost,MaxCost);
	CC_SYNTHESIZE(bool,m_Run,RunLogic);
	CC_SYNTHESIZE(int, m_iGameTimeCount, GameTimeCount);		//倒计时时间
	CC_SYNTHESIZE(bool, m_bCountDown, CountDown);				//倒计时开启
	void downloadPackageEnd(bool bAnswer);
	void CostCount(WarAlive* alive,float dt);
	void StoryEndEvent(CCObject* ob);
	void combatResult(CCObject* ob);
	void initMapBackground();
	float getTime() { return m_time; }
	int getCurrCost(){return m_CurrCost;}
	void changeCost(CCObject* ob);
	void costUpdate(float delta);
	void changeSpeed(CCObject* ob);
	void RoundStart(CCObject* ob);
	void TaskArray();
	void showRound();
	void runLogic(float delt);
	void ExcuteAI(float dt);
	bool IsAutoMoveType(WarAlive*alive);
	bool monsterFlee(WarAlive* alive);
	void MonsterExcuteAI(WarAlive* alive,float dt); 
	void HeroExcuteAI(WarAlive* alive);
	void AliveExcuteAI(WarAlive* alive,CCDictionary*pDic);	
	void BatterRecord(CCObject* ob);
	void AliveCritEnd(WarAlive* alive);
	void CritAtkEnd(CCObject* ob);
	bool critJudge(WarAlive* alive);
	void excuteCritEffect(WarAlive* alive,CCDictionary*pDic);
	void monsterCritEffect(WarAlive* alive,CCArray* arr);
	void heroCritEffect(WarAlive* alive,CCArray* arr);
	void attackEffect(WarAlive*alive);
	void attackDirection(WarAlive*alive);
	void critComboEffect();
	void doLostHp(CCObject* ob);
	bool walkState(WarAlive* alive);
	bool AttackJudge(WarAlive* alive);
	bool delayEntrance(WarAlive* alive,float dt);
	bool autoSkillAlive(WarAlive* alive);
	void attackTime(WarAlive* alive,float dt);
	bool aliveAttackState(WarAlive* alive);
	void displayBatchWarning();
	void displayRoundTips();
	void displayGuide();
	void monsterRemove(WarAlive* alive);
	void AliveDieDispose(CCObject* ob);
	void monsterDieDispose(WarAlive* alive);
	void ActObjectRemove(CCObject* ob);
	void NextBatch(float dt);
	bool StateDispose(WarAlive* alive,float dt);
	void battleWin();
	void battleFail();
	void beginStageFloorEffect();
	void beginStoryEnd();
	void moveStoryEnd();
	void wordBossFinish();
	void OnBattleFinish(int type, google::protobuf::Message *msg);
	void onWordBossFinish(int type, google::protobuf::Message *msg);
	
private:	
	int m_CurrCost;
	int	m_CurrBatchNum;				//记录当前战斗批次					这个参数+1的地方太多了，需要修改一下初始值
	float m_time;
	float m_speed;					//速度变化参数
	int m_BatchNum;					//当前关卡批次
	bool m_FiratBatch;				//第一批怪物提示
	bool m_Record;					//是否记录连击
	int m_RecordNum;				//点击次数
	int m_PlayerNum;				//播放次数
	bool m_bRecvFinish;				//是否接收到服务器结算信息
	float m_fCurrentCostAdd;		//当前cost变化速度
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
	GameFinishData m_finishData;
	
};
#endif