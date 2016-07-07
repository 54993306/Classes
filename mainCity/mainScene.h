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

//重置锚点并且保持位置
void inline ResetAnchorPointAndKeepSamePos( CCNode *pNode, CCPoint anchorpoint )
{
	//重置锚点
	int iWidth = pNode->getContentSize().width*pNode->getScaleX();
	int iHeight = pNode->getContentSize().height*pNode->getScaleY();
	CCPoint oldAnchor = pNode->getAnchorPoint();

	CCPoint offPos = ccp(
		(anchorpoint.x-oldAnchor.x)*iWidth, 
		(anchorpoint.y-oldAnchor.y)*iHeight);

	CCPoint newPos = pNode->getPosition() + offPos;
	pNode->setPosition(newPos);
	pNode->setAnchorPoint(anchorpoint);
}

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