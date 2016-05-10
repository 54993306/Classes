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
	void initAliveBtn();
	void initTipsEffect(CCObject* ob);
	void hideEffect(CCNode* node);
	void AddEvent();
	void RemoveEvent();
	void OnClick(CCObject* ob);						//按钮类
	void CheckBoxCallBack(CCObject* ob,bool check);
	void UpdateBtn(CCObject* ob);						
	void ChangeBoxGoldNum(CCObject* ob);			//箱子金币
	CCNode* getboxGold(bool box,bool label);			
	bool LongClick(CCObject* pSender, CCTouch* pTouch);
	void onlongClickEnd(CCObject* pSender, CCTouch* pTouch, float fDuration);
	bool judgeSceneTrap();							//判断场景效果启动条件		
	CLayout* getLaout();
	void upContinuousState(float dt);				//
	void upContinuousNum(CCObject* ob);				//
	void AliveProCallBack(CCObject* ob);
	void AliveBattleDispose(CCObject* ob);
	void CallAliveBattle(WarAlive*alive);
	void upAliveButtonState(int cost);
	CWidgetTouchModel AtkButtonClick(CCObject* ob,CCTouch* pTouch);
	void AtkBtnClick(CCObject* ob);
	void CaptainHit(CCObject* ob);
	void SkillMask(CCObject* ob);
	void showCostAddOrReduceEffect(int iCostChange);
	void showFlyCostToBar(CCPoint pStartPos);
	void showFlyCostToBarCallBack(CCNode* pSender);
	CC_SYNTHESIZE(CCPoint, m_goldIconPos, GoldIconPos);
	CC_SYNTHESIZE(CCPoint, m_boxIconPos, BoxIconPos);
	CC_SYNTHESIZE(CCSpriteBatchNode*, m_batchNodeEffect, BatchNodeEffect);
	void update(float dt);
	void upButtonState(CCObject* ob);
	void flyCostToHeroBtn(CCNode* pNode);
	void updateTimeCountUI(int iCount);
	void updateWorldBossUI();
	void updateWorldBossDamage(int iDamage);
	void updateWorldBossBloodBar(int iValue);
	void initArmatureTips();
	void frameEvent(CCBone *pBone, const char *str, int a, int b);

	void hideUpUiPart();	//隐藏UI的上面部分

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
	GU_openGuide = 250,
	TEST_BattleData,		//重载关卡数据按钮
	CL_OpenGameEditorBtn =300,
};
#endif