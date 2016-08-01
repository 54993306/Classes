/******************************************************
*文件名称:	mainScene.cpp
*编写日期:	2016-7-5-11:03
*编写作者:	YPF
*功能描述:	主城场景
*******************************************************/

#include "mainCity/mainScene.h"
#include "pvp_ui/PvpGateLayer.h"
#include "mainCity/mainCityBuild.h"
#include "scene/layer/BackLayer.h"
#include "mainCity/TopLayer.h"

CMainScene::CMainScene()
	:m_pCityBuildLayer(nullptr), m_pPvpGateLayer(nullptr)
{

}

void CMainScene::onCreate()
{
	CScene::onCreate();

	//初始化
	addCityBuildLayer();

	CTopLayer *pTopLay = CTopLayer::create();
	pTopLay->setTouchPriority(-2);
	this->addChild(pTopLay, 100, 2000);


	BackLayer *pBackLayer = BackLayer::create();
	pBackLayer->setZOrder(10);
	this->addChild(pBackLayer);

}

void CMainScene::onEnter()
{
	CScene::onEnter();

	//绑定场景隐藏和显示的消息
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainScene::addCityBuildLayer), "CMainScene::addCityBuildLayer", nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainScene::removeCityBuildLayer), "CMainScene::removeCityBuildLayer", nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainScene::addPvpGateLayer), "CMainScene::addPvpGateLayer", nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainScene::removePvpGateLayer), "CMainScene::removePvpGateLayer", nullptr);
}

void CMainScene::onExit()
{
	CScene::onExit();

	NOTIFICATION->removeAllObservers(this);
}


CPvpGateLayer *CMainScene::addPvpGateLayer( CCObject *pObj )
{
	if(m_pPvpGateLayer == nullptr)
	{
		m_pPvpGateLayer = CPvpGateLayer::create();
		this->addChild(m_pPvpGateLayer, -1);
	}
	return m_pPvpGateLayer;
}

void CMainScene::removePvpGateLayer( CCObject *pObj )
{
	if(m_pPvpGateLayer != nullptr)
	{
		this->removeChild(m_pPvpGateLayer, true);
		m_pPvpGateLayer = nullptr;
	}
}

CMainCityBuild *CMainScene::addCityBuildLayer( CCObject *pObj )
{
	if(m_pCityBuildLayer == nullptr)
	{
		m_pCityBuildLayer = CMainCityBuild::create();
		this->addChild(m_pCityBuildLayer, -2);
	}
	return m_pCityBuildLayer;
}

void CMainScene::removeCityBuildLayer( CCObject *pObj )
{
	if(m_pCityBuildLayer != nullptr)
	{
		this->removeChild(m_pCityBuildLayer, true);
		m_pCityBuildLayer = nullptr;
	}
}