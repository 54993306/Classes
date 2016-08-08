#pragma once

/******************************************************
*文件名称:	PvpGateLayer.h
*编写日期:	2016-6-13-17:15
*编写作者:	YPF
*功能描述:	竞技场入口
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include <spine/spine-cocos2dx.h>

using namespace spine;

USING_NS_CC;

class CPvpGateLayer: public BaseLayer
{
public:
	CPvpGateLayer();
	~CPvpGateLayer();
	
	CREATE_LAYER(CPvpGateLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	void onClose(CCObject* pSender);

	void setGateLevel( int iRank );

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

public:
	//特效入场
	void showEffectIn();
	//特效出场
	void showEffectOut();

	//异步pvp
	void showAsynPvp();

private:
	//初始化过场云
	void initCloundyTranstion();
	void showCloundyTranstion( bool bFlipY=false );
	void hideCloundyTranstion();

	void SpineComplete( int trackIndex, int loopCount );


	void initBuilding();
	void showBuilding();
	void hideBuilding();

	void postMsgToCleanMainCityRes();
	void postMsgToRebuildMainCityRes();

	//前端血雾
	void runBloodFogAction();
	void relocation(CCNode* pSender);

	//后端云
	void runCloud();

	//UI面板出场
	void showUI();
	void callbackForSpineAnimate( CCNode *pSender );
private:
	//同步PVP
	void pvpSynchronization(CCObject* pSender);
	//异步PVP
	void pvpAsynchronization(CCObject* pSender);


private:
	CLayout										*m_ui;
	SkeletonAnimation						*m_pCloudyTranstion;
	CCPoint										m_cloudPos;
	CCPoint										m_cloudAPos;
	CLayout										*m_fogLay;
	CLayout										*m_fogLay1;
	CLayout										*m_fMiddleLayer;
	int												m_iRank;				//排名
	int												m_iGateLevel;		//建筑等级
	int												m_iRoleRank;		//角色阶段
};		