#include "QuickProduct.h"
#include "scene/CPopTip.h"
#include "Global.h"
#include "tools/ShowTexttip.h"
CQuickProduct* CQuickProduct::create(CCNode *parent)
{
	CQuickProduct *pRet = new CQuickProduct(); 
	
	if (pRet && pRet->init()) 
	{ 
		pRet->m_parent = parent;
		pRet->autorelease(); 
		return pRet; 
	} 
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}

bool CQuickProduct::init()
{
	if (CCNode::init())
	{
// 		CPopTip *tip = CPopTip::create("tips/bg.png");
// 		tip->addBeforeImage("tips/blackbg.png",0,20);
// 		tip->addButton("public/btn_yellow_befor.png","public/btn_yellow_after.png",nullptr,1,1);
// 		tip->addButton("public/btn_yellow_befor.png","public/btn_yellow_after.png",nullptr,2,1);//font_kuaisushengchan
// 		tip->addBeforeImage("public/font_kuaisushengchan.png",0,145);
// 
// 		tip->setBottomOffset(-10);
// 		tip->setTouchEnabled(true);
// 		tip->setTouchPriority(-100);
// 
// 		tip->buttonAddImage("public/font_queiding.png",1);
// 		tip->buttonAddImage("public/font_quxiao.png",2);
// 
// 		//tip->addContentTip("使用一粒鑽石，可以即時獲得2小時內的資源，確定要使用鑽石嗎？",ccRED,"Arail",50.0f,18.0f);
// 		tip->addTextRichTip("使用一粒",ccWHITE,"Arail",18.0f);
// 		tip->addRichImage("mainCity/icon_17.png");
// 		tip->addTextRichTip(",可以即時獲得2小時內的資源,",ccWHITE,"Arail",18.0f);
// 		tip->addTextRichTip("\n確定要使用嗎？",ccWHITE,"Arail",18.0f);
// 		tip->reloadTextRich(ccp(VCENTER.x,VCENTER.y+30));
// 		tip->setButtonLisener(this,ccw_click_selector(CQuickProduct::onUserStone));
// 		this->addChild(tip,10,1);
// 
// 		tip->runScaleAction();
		ShowConfirmTextTip(GETLANGSTR(111),this,ccw_click_selector(CQuickProduct::onUserStone));
		return true;
	}
	return false;
}

void CQuickProduct::onUserStone(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	CPopTip *tip = (CPopTip*)btn->getParent()->getParent();
	
	CCScaleTo *smal = CCScaleTo::create(0.2f,0.0f);
	CCSequence *seque = CCSequence::createWithTwoActions(smal, CCCallFuncN::create(this,callfuncN_selector(CQuickProduct::removeTip)));
	tip->runAction(seque);

	if (btn->getTag()==1)
	{
		/*tip->removeFromParent();*/
// 		CPopTip *pop = CPopTip::create("tips/bg.png");
// 		pop->addBeforeImage("tips/blackbg.png",0,20);
// 		pop->addButton("public/btn_yellow_befor.png","public/btn_yellow_after.png",nullptr,3,1);
// 		pop->buttonAddImage("public/font_queiding.png",3);
// 		pop->addTextRichTip("沒有足夠的鑽石",ccWHITE,"Arail",18.0f);
// 		/*pop->addRichImage("mainCity/icon_11.png");*/
// 		pop->reloadTextRich(ccp(VCENTER.x,VCENTER.y+50));
// 
// 		pop->setBottomOffset(-10);
// 		pop->setTouchEnabled(true);
// 		pop->setTouchPriority(-100);
// 		pop->setButtonLisener(this,ccw_click_selector(CQuickProduct::onUserStone));
// 		this->addChild(pop,10,2);
// 		pop->runScaleAction();
//		ShowPopTextTip(GETLANGSTR(263));
	}	
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CQuickProduct::removeTip(CCNode * node)
{
	//快速生产
	if (node->getParent()->getTag()==1)
	{
		node->getParent()->removeFromParent();
	}
// 	//取消生产
// 	else if (node->getTag()==2)
// 	{
// 		this->removeFromParent();
// 	}
	//获取资源
	else 
	{
		this->removeFromParent();
	}
	
}