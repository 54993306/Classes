#pragma once

#include "AppUI.h"
#include "mainCityUI.h"
#include "model/DataCenter.h"

#include "MainCityControl.h"
#include "task/TaskControl.h"

enum CityBtn
{
	Btn_Yekuang=2,
	Btn_Arena=3,
	Btn_Strengthen=4,
	Btn_Cultivate=6,
	Btn_Nongchang=5,
	
	Btn_Jituan=7,
	Btn_Zhihuifu,
	Btn_Binying,
	Btn_Recruit,
	Btn_Zahuodian,
	Btn_CompaseTa=12,
	Btn_Rank,
	Btn_jingjichang,
	Btn_School,
	Btn_Monon,
	Btn_Tower,
};

class CTopLayer;

class CMainCityBuild:public BaseLayer
{
public:
	CMainCityBuild():m_isLoadFinish(false),m_isInterTouch(false),m_isTouchShop(false),m_armature(nullptr),m_iIndexForShow(0), m_bTouchLock(false),m_iBgLayerMaxLeftBasePos(1822)
	{}
// 	virtual ~CMainCityBuild();
	CREATE_FUNC(CMainCityBuild);
	virtual bool init();
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onEnter();

	void cityHeroMovePoint();

	void runShake(CCNode * lay);

	virtual void onExit();
	void update(float delta);
	void onClickButton(CCObject* pSender);
	//收获按钮
	void onGetButton(CCObject* pSender);

	void removeCoin(CCNode * node);

	void showCityInfo(CityData& cityData);	

	void addMoveHero(CCity * cb, int i);

	void updateCityCap(CCity &city, CLayout * lay, CButton * btn);

	void onEnterTransitionDidFinish();
	void updateGarrsionHero(const TMessage& tMsg);
	void updateCityBuild(const TMessage& tMsg);

	void updateBuildTexture(CCity * city);

	void recvWorldBossNotice(int type, google::protobuf::Message *msg);

	void show(CCObject* pObj);
	void hide(CCObject* pObj);

	void resetPosToTower( CCObject *pSender );

	void showPvpActionToSky(CCObject* pObj);

public:
	void onTimeWaitCityAction(CCNode* pSender);
	void moveFrontByPos(CCPoint pos);
	void updateFuctionOpen(const TMessage& tMsg);
	void updateGameTip(const TMessage& tMsg);

	void showNoticeTip(CGameTips * ct);

	void hideTips(int tag);
	void resetPos(CCNode* node);

protected:
	void moveHero(float delta);
	void blinkShow(CCNode *node);
	void walkStop(CCNode *node);
	void runCloudAction();
	void relocation(CCNode* pSender);
	void runFogAction();
	void relocationFog(CCNode* pSender);
	void repeatMove(CCNode *node);
	void updateCityGarrsion(CHero *hero, int cityId);
	void createBuildEffect();

	void addComposeAnim(CCPoint lightPos1, CCPoint lightPos2, CLayout * effLay);
	void addStrengthenAnim(CCPoint firePos, CLayout * effLay);
	void addCulAnim(CLayout * effLay);

	void runGuideStep(float dt);
	void openFrameEvent(CCBone *bone, const char *name, int orginIndex, int currendtIndex);
	void moveEvent(CCArmature *armature, MovementEventType movementType, const char *movementID);
	void removeMobieShop(float dt);
	void updateMobileShop(const TMessage& tMsg);
	void unGarrisonAndFindNew();

private:
	//点击塔上天
	void showPvp();
	void showPvpCallbackForResetAnchorPoint();
	void showPvpCallBack();

	//从天上返回
	void backFromPvp( CCObject *pSender );
	void backFromPvpForResetAnchorPoint();
	void backFormPvpCallBack();

	//自动上天
	void autoToPvp();

private:
	float m_fmoveTime;
	float  m_fMoveSpeed;

	CLayout *m_ui;
	CMainCityUI *m_cityUi;
	CLayout *m_fogLay;
	CLayout *m_fogLay1;

	CCPoint m_preMovePos;  //场景移动中坐标
	CCPoint m_beginPos;
	float m_minX;
	float m_maxX;
	float m_bgMinX;
	float m_bgMaxX;
	bool m_isInterTouch;

	CCNode *m_hero[4];

	CCPoint m_movePos;
	CCPoint m_real;
	CCPoint m_buildPos[17];
	CityData m_cityData;
	map<int,CCity> m_cityMap;
	CCPoint m_movePoint[4];
	bool m_isLoadFinish;

	CCPoint m_cloudPos;
	CCPoint m_cloudAPos;
	CCPoint m_fogPos;
	CCPoint m_fogAPos;

	CCPoint m_foodPos;
	CCPoint m_coinPos;

	CCPoint m_tieQiangPos[3];
	CCPoint m_fieldPos[3];
	CCPoint m_goldPos[3];
	CCPoint m_composePos[3];
	map<int,CCPoint*> m_garrsionPointMap;  //武將駐守點
	CLayout *m_frontLay;
	CLayout *m_bgLay;
	CCArmature *m_armature;
	bool m_isTouchShop;
	int m_iIndexForShow;
	bool m_bTouchLock;
	int m_iBgLayerMaxLeftBasePos;
	float		m_fUiActionTime;				//上天时用
};