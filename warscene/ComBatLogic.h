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
	CREATE_FUNC(CombatLogic);

	void updateOneSecond(float dt);
	void initMember();
	
	void AliveDieDispose(CCObject* ob);
	void ActRemove(CCObject* ob);
	void NextBatch(float dt);
	void CBackLayer(CCObject* ob);
	void CombatResult(CCObject* ob);
	
	
	
	void StoryEndEvent(CCObject* ob);
	
	
	
	void CostCount(WarAlive* alive,float dt);
	

	
	
	bool StateDispose(WarAlive* alive,float dt);
	void OnBattleFinish(int type, google::protobuf::Message *msg);
	
	void startCountDown(int iTime);		//开始倒计时
	int getCurrCost(){return m_CurrCost;}
	CC_SYNTHESIZE(WarAssist*,m_Assist,WarAssist);
	CC_SYNTHESIZE(CCArray*,m_task,TaskArray);
	CC_SYNTHESIZE(float,m_MaxCost,MaxCost);
	CC_SYNTHESIZE(bool,m_Run,RunLogic);
	CC_SYNTHESIZE(int, m_iGameTimeCount, GameTimeCount);		//倒计时时间
	CC_SYNTHESIZE(bool, m_bCountDown, CountDown);				//倒计时开启

	float getTime() { return m_time; }
	void scheduleForRequestFinish();
	void scheduleForRequesBossFinish();
	
	void onPause();
	void onResume();
	void reliveSuccess();

	void downloadPackageEnd(bool bAnswer);
	/*****************************************************************************/
	void ChangeCost(CCObject* ob);
	void ChangeSpeed(CCObject* ob);
	void RoundStart(CCObject* ob);
	void TaskArray();
	void showRound();
	void RunLogic(float delt);
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
	void attackEnd(WarAlive* alive);
	void doLostHp(CCObject* ob);
	bool walkState(WarAlive* alive);
	bool AttackJudge(WarAlive* alive);
	bool delayEntrance(WarAlive* alive,float dt);
	bool autoSkillAlive(WarAlive* alive);
	void attackTime(WarAlive* alive,float dt);
	bool aliveAttackState(WarAlive* alive);
	void displayBatchTips();
	void displayRoundTips();
	void displayGuide();
	void heroRemove(WarAlive* alive);
	void monsterRemove(WarAlive* alive);
protected:
	void onClickDownloadPackage(CCObject* pSender);
private:	
	int m_CurrCost;
	float m_FrameTime;				//累计时间
	int m_send;						//记录以请求的消息批次
	int	m_CurrBatchNum;				//记录当前战斗批次					这个参数+1的地方太多了，需要修改一下初始值
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