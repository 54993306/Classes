#ifndef _COMBATLOGINC_
#define _COMBATLOGINC_
/************************************************************* 
 *
 *
 *		Data : 2016.6.2
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
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "update/CDownloadPackageDelegate.h"
namespace BattleSpace
{
	struct GameFinishData
	{
		int reason;
		bool res;
		int roundNum;
	};

	class BufExp;
	class BattleScene; 
	class BaseRole;
	class WarAssist;
	class WarControl;
	class StoryLayer;
	class WarManager;
	class WarMapData;
	class BattleMapLayer;
	class CombatEffect;
	class BattleRoleLayer;
	class CombatGuideManage;
	class TotalStrategy;

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
		void initWordBossTime();
		void scheduleForRequestFinish();
		void scheduleForRequesBossFinish();
		void onPause();
		void onResume();
		CC_SYNTHESIZE_READONLY(WarAssist*,mAssist,WarAssist);
		CC_SYNTHESIZE_READONLY(CCArray*,mTask,TaskArray);
		CC_SYNTHESIZE_READONLY(int, m_iGameTimeCount, GameTimeCount);		//倒计时时间
		CC_SYNTHESIZE_READONLY(bool, m_bCountDown, CountDown);				//倒计时开启
		CC_SYNTHESIZE_READONLY(float,mTime,Time);

		void downloadPackageEnd(bool bAnswer);
		void CostCount(BaseRole* alive,float dt);
		void StoryEndEvent(CCObject* ob);
		void combatResult(CCObject* ob);
		void initMapBackground();
		void updateTask();
		void showRound();
		void runLogic(float delt);
		void ExcuteAI(float dt);
		void BatterRecord(CCObject* ob);
		void CritAtkEnd(CCObject* ob);
		void critComboEffect();
		void displayRoundTips();
		void displayGuide();
		void AliveDieDispose(CCObject* ob);
		void monsterDieDispose(BaseRole* alive);
		void ActObjectRemove(CCObject* ob);
		void NextBatch(float dt);
		void battleWin();
		void battleFail();
		void beginStageFloorEffect();
		void beginStoryEnd();
		void moveStoryEnd();
		void wordBossFinish();
		void OnBattleFinish(int type, google::protobuf::Message *msg);
		void onWordBossFinish(int type, google::protobuf::Message *msg);
		void rolePlyaSkill(CCObject* ob);
	private:	
		int	m_CurrBatchNum;				//记录当前战斗批次					这个参数+1的地方太多了，需要修改一下初始值
		bool m_FiratBatch;				//第一批怪物提示
		bool m_Record;					//是否记录连击
		int m_RecordNum;				//点击次数
		int m_PlayerNum;				//播放次数
		bool m_bRecvFinish;				//是否接收到服务器结算信息
		BaseRole* mCritRole;				//存储释放技能的武将信息
		BufExp* mbufExp;
		BattleScene* mBattleScene;
		WarMapData* m_MapData;
		WarManager* mManage;
		WarControl* mControlLayer;
		BattleMapLayer* mMapLayer;
		BattleRoleLayer* mRoleLayer;
		CombatEffect* mCombatEffect;
		GameFinishData m_finishData;
		CombatGuideManage* mGuideManage;
		TotalStrategy* mTotalStrategy;
	};
};
#endif