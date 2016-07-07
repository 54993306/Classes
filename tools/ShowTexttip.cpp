// ***************************************************************
//  FileName: ShowTexttip.cpp
//  Version:  1.0   
//  Date: 
//  Author: linxiancheng
//  Copyright (C) 2014 U1
//  --------------------------------------------------------------
//  Description:  文字提示
//  -------------------------------------------------------------
//  History: 
//  -------------------------------------------------------------
// ***************************************************************

#include "ShowTexttip.h"
#include "texttip.h"
#include "commonDef.h"

#include "CCTK/base.h"
#include "CCTK/scenemanager.h"
#include "CCTK/scene.h"

#include "CocoStudio/GUI/System/CocosGUI.h"

#include "scene/CPopTip.h"

#include "bag/ItemTip.h"
#include "bag/StuffTip.h"


void ShowTexttip(const char * content,
                 const ccColor3B & color,
				 int type/* = MTB_WARNING*/,
                 const CCPoint & startPoint /*= ccp(480, 320)*/,
                 float startFlyTime /*= 1.0f*/,
                 float flyingTime /*= 1.0f*/,
                 float offsetx /*= 0.0f*/,
                 float offsety /*= 50.0f*/,
                 const char * fontName /*= FONT_NAME*/,
                 int fontSize /*= 25*/ )
{
#ifdef WIN32
	//printf("[ShowTexttip] [%s]\n",content);
#endif
	
	//CCScene * scene = World::getInstance()->getScene();
	CScene* scene = CSceneManager::sharedSceneManager()->getRunningScene();
    if (scene)
    {
        texttip * text = texttip::create();
        CCSize winSize = winSize;
        scene->addChild(text, 9998);
        text->setContent(content);
		text->setPosition(startPoint);
		text->setFontColor(color);
        if (type == MTB_TEXT)
		{
			text->setFlyState(startFlyTime, flyingTime, ccp(offsetx, offsety),type,fontSize);
			TextTipManager::GetInstance().AddTextTip( text );
		}
        else
		{
            text->setFlyState(0.5, 0.5, ccp(offsetx, offsety),type);
			text->doProcess();
		}
		
    }
}

void ShowTexttip( CCNode * content, 
				 const ccColor3B & color, 
				 int type /*= MTB_OK*/, 
				 const CCPoint & startPoint /*= ccp(480, 320)*/, 
				 float startFlyTime /*= 0.5f*/, 
				 float flyingTime /*= 0.5f*/, 
				 float offsetx /*= 0.0f*/, 
				 float offsety /*= 50.0f*/ )
{
    CScene* scene = CSceneManager::sharedSceneManager()->getRunningScene();

    if (scene)
    {
        texttip * text = texttip::create();
        CCSize winSize = winSize;
        scene->addChild(text, 9998);
        text->setContent(content);
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		//text->setPosition(startPoint);
		text->setPosition(ccp(size.width/2,size.height/2));
		text->setFontColor(color);
        if (type == MTB_TEXT)
		{
			text->setFlyState(startFlyTime, flyingTime, ccp(offsetx, offsety),type);
			TextTipManager::GetInstance().AddTextTip( text );
		}
        else
		{
            text->setFlyState(0.5, 0.5, ccp(offsetx, offsety),type);
			text->doProcess();
		}

    }
}

void showScrollTip(const char* str, CCNode * _parent)                      
{
	/*
	m_scroll->setDirection(eScrollViewDirectionVertical);
	m_scroll->setPosition(VCENTER);
	m_scroll->setColor(ccWHITE);
	_parent->addChild(m_scroll);

	CCLabelTTF *label = CCLabelTTF::create("saaaaaaaa25aaaaaaaarrrt767433aaaaaaaawwe2344444454t656576yttyrwdrytytytswtyuy",
		"Arial",15,CCSizeMake(400,300),kCCTextAlignmentLeft);
	label->setPosition(ccp(0,50));
	m_scroll->getContainer()->addChild(label);

	CCLabelTTF *label1 = CCLabelTTF::create("saaaaaaaa25aaaaaaaarrrt767433aaaaaaaawwe2344444454t656576yttyrwdrytytytswtyuy",
		"Arial",15,CCSizeMake(400,300),kCCTextAlignmentLeft);
	m_scroll->getContainer()->addChild(label1);
	label1->setPosition(ccp(100,50));
	
	CScrollView *scrollView = CScrollView::create(CCSizeMake(500,400));
	scrollView->setContainerSize(CCSizeMake(500,400));*/

	CCLabelTTF *label = CCLabelTTF::create("saaaaaaaa25aaaaaaaarrrt767433aaaaaaaawwe2344444454t656576yttyrwdrytytytswtyuy",
		FONT_NAME,15,CCSizeMake(400,300),kCCTextAlignmentLeft);

}

CPopTip* ShowPopTextTip(const char* str,const ccColor3B& color, bool isAutoClose)
{
	CPopTip *pop = CPopTip::create();
	pop->setVisible(true);
	pop->addContentTip(str);
	pop->setTouchEnabled(true);
	pop->setTouchPriority(-100);
	pop->showConfirmOnly();
 	pop->setButtonLisener(pop,ccw_click_selector(CPopTip::onClose));

	LayerManager::instance()->push(pop, true);

	//CScene* scene = CSceneManager::sharedSceneManager()->getRunningScene();
	//scene->addChild(pop,50,2);
	//pop->runScaleAction();
	if (isAutoClose)
	{	
		CCDelayTime *delay = CCDelayTime::create(1.5f);
		CCScaleTo *smal = CCScaleTo::create(0.2f,0.0f);
		CCSequence *seque = CCSequence::create(delay,smal, CCCallFuncO::create(pop,callfuncO_selector(CPopTip::onClose), nullptr), nullptr);
		pop->runActionWithUi(seque);
	}
	return pop;
}

void ShowConfirmTextTip(const char* str, CCObject *pSender, SEL_ClickHandler pHandler, const ccColor3B& color /*=ccWHITE*/)
{
// 	CPopTip *pop = CPopTip::create("tips/bg.png");
// 	pop->addBeforeImage("tips/blackbg.png",0,20);
// 	pop->addButton("public/btn_yellow_befor.png","public/btn_yellow_after.png",nullptr,2,1);
// 	pop->addButton("public/btn_yellow_befor.png","public/btn_yellow_after.png",nullptr,1,1);
// 	pop->buttonAddImage("public/font_queiding.png",1);
// 	pop->buttonAddImage("public/font_quxiao.png",2);
// 	pop->addTextRichTip(str,color,"Arail",18.0f);

	CPopTip *pop = CPopTip::create();
	pop->setVisible(true);
	pop->addContentTip(str);
	pop->setTouchEnabled(true);
	pop->setTouchPriority(-100);
	pop->setButtonLisener(pSender,pHandler);
	LayerManager::instance()->push(pop, true);

	//CScene* scene = CSceneManager::sharedSceneManager()->getRunningScene();
	//scene->addChild(pop,50,2);

	pop->runScaleAction();
}

void showItemTip(const CItem *item)
{
	if (item->itemType==2)
	{
		LayerManager::instance()->push(MaskLayer::create("armor"));
	    CItemTip *tip = CItemTip::create();
		tip->showItemProperty((CItem*)item);
		LayerManager::instance()->push(tip);
	}
	else
	{
		LayerManager::instance()->push(MaskLayer::create("stuff"));
		CStuffTip *tip = CStuffTip::create();
		tip->showItemProperty((CItem*)item);
		LayerManager::instance()->push(tip);
	}
}
