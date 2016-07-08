#ifndef _WARCONTROL_
#define _WARCONTROL_
/************************************************************* 
*
*
*		Data : 2016.5.13
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : 可分为几个部分进行处理，按钮的特效，其他连击效果的显示，UI上半部分的处理可拆分为多个类，将tag的定义抽出到共用的位置进行处理，创建一个ui的文件夹管理相关的类
*
*
*************************************************************/
#include "AppUI.h"
#include "Global.h"
#include "scene/layer/LayerManager.h"
class CSpecialProgress;

namespace BattleSpace{
	class BaseRole;
	class BattleScene;
	class WarManager;
	class WarControl : public BaseLayer
	{
	public:
		WarControl();
		~WarControl();
		virtual bool init();
		CREATE_FUNC(WarControl);
		virtual void onEnter();
		virtual void onExit();
	public:												//
		void AddEvent(CCObject* ob = nullptr);
		void RemoveEvent(CCObject* ob = nullptr);
		CLayout* getLaout();
		void initAliveButtons();
		void showMonsterTips(CCObject* ob);

		void OnClick(CCObject* ob);						//按钮类					
		void ChangeBoxGoldNum(CCObject* ob);			//箱子金币
		bool AliveButtonLongClick(CCObject* pSender, CCTouch* pTouch);
		void onlongClickEnd(CCObject* pSender, CCTouch* pTouch, float fDuration);	

		void upContinuousNodeState(float dt);				//
		void upContinuousNumber(CCObject* ob);				//
		void AliveBarFullCallBack(CCObject* ob);
		void AliveBattleDispose(CCObject* ob);
		void CallRoleEntranceBattle(BaseRole*alive);

		CWidgetTouchModel AliveButtonBeginClick(CCObject* ob,CCTouch* pTouch);
		CWidgetTouchModel onTouchBegin(CCObject* ob,CCTouch* pTouch);
		void AliveButtonClick(CCObject* ob);
		void CaptainHit(CCObject* ob);
		void SkillMask(CCObject* ob);

		void showFlyCostToBarCallBack(CCNode* pSender);

		void ResetButtonState(CCObject* ob);
		void updateTimeCountUI(int iCount);
		 
		void iniCaptainHurtTips();

		/*******************************************************************************************************************/
		CCNode* getMoveLayout(int index);							//get role button move part

		void hideUiUpPart(CCObject* ob);							//hide ui panel above part

		void iniTestUi();											//init use to test button

		void initUIAbove();											//init ui panel above part

		void initUIEffect();										//init add ui panel effect

		void updateWorldBossDamage();								//update word boss hurt number

		void updateWorldBossBloodBar(CCObject* ob);					//update word boss blood bar

		void initNormalAbove();										//init normal panel above part

		void initWorldBossAbove(BaseRole* boss);					//have word init panel above part

		void initCostBar();											//init cost bar

		void initAliveButton(CCNode* Layout,BaseRole* alive);		//init single role button

		void initAliveButtonBar(CCNode* Layout,int model);			//init role bar

		void initCaptinButtonBar(CCNode* Layout,BaseRole* pRole);

		void initButtonBackImage(CButton* button,int index );		//reason call number init button back image

		void updateCostNumber();									//update cost number

		void updateCostSpeed(float dt);										//update cost speed

		void updateAliveButtonEffect();								//update role button effect

		bool guideStateButtonEffect(CCNode* layout,bool pClick);	//guide state button dispose

		void AliveBattlefield(BaseRole* alive);						//role log in battlefield or leave

		void updateBatchNumber(CCObject* ob);						//update batch number

		void postButtonState(CCObject* ob);

		void changeSpeed();

		void upAutoSkillState(float dt);

		void showFlyCostToBar(CCObject* ob);

		void battleBegin();
		void updateOneSecond(float dt);
		CC_SYNTHESIZE(CCPoint, m_goldIconPos, GoldIconPos);
		CC_SYNTHESIZE(CCPoint, m_boxIconPos, BoxIconPos);
		CC_SYNTHESIZE(CCSpriteBatchNode*, m_batchNodeEffect, BatchNodeEffect);
	protected:
		WarManager* mManage;
		CLayout*	m_ControLayer;
		CCNode*		m_warLayer;
		float		m_fdetal;
		float		m_costTime;//战斗倒计时
		CSpecialProgress* m_pCostBar;
		CLabel*  m_pCostNum;
		CLabel*  m_pCostChange;
		CCLabelAtlas*  m_pAllDamage;
		CProgressBar* m_pBossBar1;
		CProgressBar* m_pBossBar2;
		CCLayerColor* m_LayerColor;
		CCArmature* m_ArmatureTips;
	private:
		int			m_boxNum;
		int			m_goldNum;
		int			m_ContinuousNum;
		int			m_iAimCost;
		bool		mAutoState;
		int			m_iGameTimeCount;
		bool		m_bCountDown;
	};
	enum TrapResumeType
	{
		Restart_Type = 1,		//重新计时
		Resume_Type,			//继续
		RestartAddTime_Type,	//加时重新计时
	};
	enum ControlLayer_Tag
	{
		CL_Menu = 1,			//返回菜单按钮
		CL_GoldNum,				//金币lab
		CL_BoxNum,				//宝箱lab
		CL_TrapPro,				//场景效果进度条
		CL_StarBtn,				//开始战斗按钮
		CL_Batch,				//批次节点
		CL_CostBar,				//cost进度条	
		CL_CostEffect,			//cost特效底框
		CL_AddSpeedBtn,			//加速按钮
		CL_BoxNumPro,			//封装后的宝箱数字
		CL_GoldNumPro,			//封装后的金币数字
		CL_ContinuousNum,		//连击次数
		CL_CostNum,				//显示cost值
		CL_MAXcostTips,			//满cost标记
		CL_BtnLayout1	= 50,	
		CL_BtnLayout2,
		CL_BtnLayout3,
		CL_BtnLayout4,
		CL_BtnLayout5,
		CL_ContinuousLayou,		//连击节点
		CL_HeroNode = 100,		//武将移动节点
		CL_Btn,					//技能按钮
		CL_HeroIcon,			//武将头像
		CL_HeroPro,				//CD进度条
		CL_NeedCost,			//Cost值
		CL_BtnCallEff,			//召唤状态
		CL_BtnSkillEff,			//技能状态
		CL_TipsEffect1 = 200,	//进击特效
		CL_TipsEffect2,
		CL_TipsEffect3,
		CL_TipsEffect4,
		CL_AutoPlay,
		GU_openGuide = 250,		//引导测试按钮
		TEST_BattleData,		//重载关卡数据按钮
		TEST_MoveState,
		TEST_Tips,
		TEST_Role,
	};
};
#endif
