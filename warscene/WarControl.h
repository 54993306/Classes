#ifndef _WARCONTROL_
#define _WARCONTROL_
#include "AppUI.h"
#include "Global.h"
#include "scene/layer/LayerManager.h"

class WarAlive;
class WarScene;
class WarManager;
class CSpecialProgress;

class WarControl : public BaseLayer
{
public:
	WarControl();
	~WarControl();
	virtual bool init();
	CREATE_FUNC(WarControl);
	virtual void onEnter();
	virtual void onExit();
public:
	void initAliveButtons();
	void initTipsEffect(CCObject* ob);
	void hideEffect(CCNode* node);
	void AddEvent();
	void RemoveEvent();
	void OnClick(CCObject* ob);						//按钮类
	void CheckBoxCallBack(CCObject* ob,bool check);
	void UpdateBtn(CCObject* ob);						
	void ChangeBoxGoldNum(CCObject* ob);			//箱子金币
	CCNode* getboxGold(bool box,bool label);			
	bool AliveButtonLongClick(CCObject* pSender, CCTouch* pTouch);
	void onlongClickEnd(CCObject* pSender, CCTouch* pTouch, float fDuration);	
	CLayout* getLaout();
	void upContinuousState(float dt);				//
	void upContinuousNum(CCObject* ob);				//
	void AliveBarFullCallBack(CCObject* ob);
	void AliveBattleDispose(CCObject* ob);
	void CallAliveBattle(WarAlive*alive);
	
	CWidgetTouchModel AliveButtonClickBegin(CCObject* ob,CCTouch* pTouch);
	void AliveButtonClick(CCObject* ob);
	void CaptainHit(CCObject* ob);
	void SkillMask(CCObject* ob);
	void showCostAddOrReduceEffect(int iCostChange);
	void showFlyCostToBar(CCPoint pStartPos);
	void showFlyCostToBarCallBack(CCNode* pSender);
	CC_SYNTHESIZE(CCPoint, m_goldIconPos, GoldIconPos);
	CC_SYNTHESIZE(CCPoint, m_boxIconPos, BoxIconPos);
	CC_SYNTHESIZE(CCSpriteBatchNode*, m_batchNodeEffect, BatchNodeEffect);

	
	void upButtonState(CCObject* ob);

	void flyCostToHeroBtn(CCNode* pNode);
	void updateTimeCountUI(int iCount);
	
	void iniCaptainHurtTips();
	void frameEvent(CCBone *pBone, const char *str, int a, int b);

/*******************************************************************************************************************/
	CCNode* getMoveLayout(int index);							//得到武将移动的UI部分

	void hideUpUiPart();										//隐藏UI的上面部分

	void iniTestUi();											//初始化一些用于测试的控件按钮
	
	void initUIAbove();											//初始化UI上部分

	void initUIEffect();										//初始化添加在UI上的效果

	void updateWorldBossDamage();								//刷新时间boss伤害表现

	void updateWorldBossBloodBar(CCObject* ob);					//刷新世界boss血量条

	void initNormalAbove();										//初始化非世界boss上部分

	void initWorldBossAbove(WarAlive* boss);					//世界boss情况

	void initCostBar();											//初始化costbar

	void initAliveButton(CCNode* Layout,WarAlive* alive);		//初始化单个武将按钮

	void initAliveButtonBar(CCNode* Layout,int model);			//初始化武将进度条

	void initButtonBackImage(CButton* button,int index );		//根据召唤数目初始化按钮图

	void updateUiLayerCostNumber(int cost);						//刷新显示层上Cost值

	void updateAliveButtonEffect();								//刷新武将按钮特效

protected:
	WarManager* m_Manage;
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
	float		m_interval;
	int			m_boxNum;
	int			m_goldNum;
	int			m_ContinuousNum;
	bool		m_openEye;
	bool		m_WindResume;
	int m_iAimCost;
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
	GU_openGuide = 250,		//引导测试按钮
	TEST_BattleData,		//重载关卡数据按钮
	CL_OpenGameEditorBtn =300,
};
#endif