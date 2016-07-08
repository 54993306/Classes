#ifndef _COMBATGUIDEMANAGE_
#define _COMBATGUIDEMANAGE_

#include "AppUI.h"

/************************************************************************/
//
//							战斗引导信息管理
//
/************************************************************************/
namespace BattleSpace{
	class CombatGuideStep;
	class CombatGuideLayer;

	typedef map<unsigned int,CombatGuideStep*> MapCombatGuideData;	

	class CombatGuideManage : public CCObject
	{
	public:
		CombatGuideManage();
		virtual ~CombatGuideManage();
		virtual bool init(){ return true;}
		CREATE_FUNC(CombatGuideManage);
	public:
		void clearGuideData(bool quit = false);
		void addGuideStep(CombatGuideStep* step);
		CombatGuideStep* getGuideStep(int id);
		CombatGuideStep* getCurrStep();
		bool moveGuideJudge(int pGrid,bool pNextStep = false);
		bool isRest();
		void setCurrBatchGuide(const char* FilePath);		//(特殊)引导不是第一步开始走的
		bool IsGuide(){ return mGuideState; }				//当前是否在引导的状态
		int getCurrGuideIndex(){return m_CurrStepID;}		//得到当前引导id
		void EnterGuide();
		void NextStep();
		void NextStepExcute(CombatGuideStep* Curr_step);
		void ExitGuide();
		bool LoadJsonFile(const char* path);
		CC_SYNTHESIZE(CombatGuideLayer*,m_GuideLayer,GuideLayer);//保存一个GuideLayer层
	protected:
		int m_CurrStepID;
		bool mGuideState;											//当然是否为引导状态
		MapCombatGuideData m_GuideData;							//存储引导数据信息
	};
};
#endif