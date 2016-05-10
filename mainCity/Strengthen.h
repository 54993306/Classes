#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "CityData.h"
#include "bag/bagData.h"

class CLevelUpItem;
class CSmeltArmor;
class CStarUpItem;

enum StrengthenType
{
	StrengthenLevelUp = 1,		//升级
	StrengthenSmelt,				//洗练
	StrengthenStarUp				//升星
};

//强化类
class CStrengthen:public BaseLayer
{
public:
	CStrengthen();
	~CStrengthen();

	CREATE_LAYER(CStrengthen);

	virtual bool init();

	void onEnter();

	void onExit();

	void showRedTip();
	

	//切换到
	void changeToStrengthenWithType(StrengthenType iType);

protected:	
	void onClose(CCObject *pSender);
	void onSwitchBtn(CCObject *pSender, bool bChecked);

private:
	CLayout *m_pUi;

	CLevelUpItem*	m_pLevelUpLay;   //升级层
	CSmeltArmor*	m_pSmeltLay;   //洗练层
	CStarUpItem*	m_pStarUpLay;   //升星层

	StrengthenType m_iType; //选择的类型
};

//公共特效
inline float ShowTVLight(CCNode* pNode, CCNode* pParent)
{
	CCClippingNode* pClip = CCClippingNode::create();
	pClip->setAnchorPoint(ccp(0.0f, 0.0f));
	pClip->setContentSize(pNode->getContentSize());
	pClip->setPosition(pNode->getPosition()-ccp(pNode->getContentSize().width/2, pNode->getContentSize().height/2));


	int iMaxHeight = pNode->getContentSize().height;
	int iMaxWidth = pNode->getContentSize().width;

	float fGap = 35;
	CCDrawNode *stencil = CCDrawNode::create();
	CCPoint rectangle[4];
	rectangle[0] = ccp(fGap, fGap);
	rectangle[1] = ccp(iMaxWidth-fGap, fGap);
	rectangle[2] = ccp(iMaxWidth-fGap, iMaxHeight-fGap);
	rectangle[3] = ccp(fGap, iMaxHeight-fGap);
	ccColor4F white = {1,1,1,1};
	stencil->drawPolygon(rectangle, 4, white, 1, white);
	pClip->setStencil(stencil);
	pParent->addChild(pClip);
	//光影
	CCSprite* pLight = CCSprite::create("strengthen/lightline.png");
	pLight->setScaleY(1.3f);
	pLight->setRotation(40);
	pLight->setPosition(rectangle[3]);
	pClip->addChild(pLight);

	//执行
	pLight->runAction(CCSequence::create(
		CCMoveTo::create(0.5f, rectangle[1]),
		CCRemoveSelf::create(true),
		NULL));

	return 0.5f;
}