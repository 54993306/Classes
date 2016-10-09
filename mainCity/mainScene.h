/******************************************************
*文件名称:	mainScene.h
*编写日期:	2016-7-5-11:03
*编写作者:	YPF
*功能描述:	主城场景
*******************************************************/

#pragma once

#include "AppUI.h"

class CMainCityBuild;
class CPvpGateLayer;

class CMainScene:public CScene
{
public:
	CMainScene();
	DEFINE_SCENE_CLASS(CMainScene);
	virtual void onCreate();
	virtual void onEnter();
	virtual void onExit();

public:
	CC_SYNTHESIZE(CMainCityBuild*, m_pCityBuildLayer, CityBuildLayer);
	CC_SYNTHESIZE(CPvpGateLayer*, m_pPvpGateLayer, PvpGateLayer);

	CPvpGateLayer *addPvpGateLayer( CCObject *pObj=nullptr );
	void removePvpGateLayer( CCObject *pObj=nullptr );

	CMainCityBuild *addCityBuildLayer( CCObject *pObj=nullptr );
	void removeCityBuildLayer( CCObject *pObj=nullptr );

private:
};