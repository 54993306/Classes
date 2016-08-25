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


namespace BattleSpace
{
	class BufExp;
	class BattleScene; 
	class BaseRole;
	class WarAssist;
	class WarControl;
	class StoryLayer;
	class WarManager;
	class BattleMapLayer;
	class CombatEffect;
	class BattleRoleLayer;
	class CombatGuideManage;
	class TotalStrategy;
	class CombatLogic : public CCNode
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
		void onPause();
		void onResume();
		CC_SYNTHESIZE_READONLY(CCArray*,mTask,TaskArray);
		CC_SYNTHESIZE_READONLY(float,mTime,Time);

		void CostCount(BaseRole* alive,float dt);
		void StoryEndEvent(CCObject* ob);
		void initMapBackground();
		void updateTask();
		void showRound();
		void runLogic(float delt);
		void ExcuteAI(float dt);
		void BatterRecord(CCObject* ob);
		void CritAtkEnd(CCObject* ob);
		void critComboEffect();
		void beginStageFloorEffect();
		void beginStoryEnd();
		void moveStoryEnd(CCObject* ob);
		void rolePlyaSkill(CCObject* ob);
	private:	
		float mInterval;
		bool mFirstBatch;				//第一批怪物提示
		bool m_Record;					//是否记录连击
		int m_RecordNum;				//点击次数
		int m_PlayerNum;				//播放次数
		bool mRecvFinish;				//是否接收到服务器结算信息
		BaseRole* mCritRole;				//存储释放技能的武将信息
		BufExp* mbufExp;
		WarAssist* mAssist;
		BattleScene* mBattleScene;
		WarManager* mManage;
		WarControl* mControlLayer;
		BattleMapLayer* mMapLayer;
		CombatEffect* mCombatEffect;
		CombatGuideManage* mGuideManage;
		TotalStrategy* mTotalStrategy;
	};
};
#endif