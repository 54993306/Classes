#include "WarFailLayer.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "Battle/WarAssist.h"
#include "tools/CCShake.h"
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "protos/ProtoDefine.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "GMessage.h"
#include "common/ShaderDataHelper.h"
#include "mainCity/MainCityControl.h"
#include "tools/ShowTexttip.h"
#include "model/DataCenter.h"


WarFailLayer::WarFailLayer(void)
{
}

 using namespace BattleSpace;
WarFailLayer::~WarFailLayer(void)
{
}

bool WarFailLayer::init(){
	bool res = BaseLayer::init();
	PlayEffectSound(SFX_Fail);
	setVisible(true);

	CCSize size = CCDirector::sharedDirector()->getWinSize();

	//黑底
	MaskLayer* pMaskLayer = MaskLayer::create("WarFailLayerMaskLayer");
	pMaskLayer->setContentSize(size);
	LayerManager::instance()->push(pMaskLayer);

	//配置内容
	m_ui = LoadComponent("warFailLayer.xaml");
	m_ui->setPosition(VCENTER);
	this->setTouchPriority(-12);
	this->addChild(m_ui);

	//章节信息
	CCString* pChapter = CCString::createWithFormat("tollgate/chapter_%d.png", 101/*DataCenter::sharedData()->getWar()->getChapter().id*/);
	CCSprite* pTitle = CCSprite::create(pChapter->getCString());
	if(!pTitle)
	{
		CCLOG("ERROR WarFailLayer::init");
		pTitle = CCSprite::create("tollgate/chapter_1.png");
	}

	pTitle->setAnchorPoint(ccp(0, 0.5f));
	CLabel *titleBg = (CLabel*)m_ui->findWidgetById("title");
	titleBg->setOpacity(0);
	//更换图片
	pTitle->setPosition(ccp(0, pTitle->getContentSize().height*0.5f));
	titleBg->addChild(pTitle);

	//旋转光
	CCSprite *star = (CCSprite*)m_ui->findWidgetById("light");
	star->setColor(ccc3(30, 30, 30));
	star->setOpacity(225);
	star->runAction(CCRepeatForever::create(CCRotateBy::create(0.2f, 20)));

	//失败字样
	{
// 		CCSprite *title_left1 = (CCSprite*)m_ui->findWidgetById("left_2");
// 		title_left1->setTag(TAG_F_TITLE_LEFT_1);
// 		title_left1->setVisible(false);
// 
// 		CCSprite *title_left2 = (CCSprite*)m_ui->findWidgetById("left_1");
// 		title_left2->setTag(TAG_F_TITLE_LEFT_2);
// 		title_left2->setVisible(false);
// 
// 		CCSprite *title_right = (CCSprite*)m_ui->findWidgetById("right_1");
// 		title_right->setTag(TAG_F_TITLE_RIGHT);
// 		title_right->setVisible(false);

// 		CCSprite *txtLeft = (CCSprite*)m_ui->findWidgetById("left");
// 		txtLeft->setPosition(ccp(title_left1->getPositionX()-250, size.height*1.2f));
// 		txtLeft->runAction(CCSequence::create(CCMoveTo::create(0.25f, title_left1->getPosition()),
// 			CCDelayTime::create(0.3f),
// 			CCCallFuncN::create(this, callfuncN_selector(WarFailLayer::afterTextEnd)), 
// 			NULL));
// 		this->runAction(CCSequence::create(CCDelayTime::create(0.5f),
// 			CCShake::create(0.1f, 10.f), CCShake::create(0.1f, 10.f),
// 			NULL));

// 		CCSprite *txtRight = (CCSprite*)m_ui->findWidgetById("right");
// 		txtRight->setPosition(ccp(title_right->getPositionX()+250, size.height*1.2f));
// 		txtRight->runAction(CCSequence::create(CCDelayTime::create(0.2f), 
// 			CCMoveTo::create(0.25f, title_right->getPosition()),
// 			CCDelayTime::create(0.1f),
// 			CCCallFuncN::create(this, callfuncN_selector(WarFailLayer::afterTextEnd)),
// 			CCCallFuncN::create(this, callfuncN_selector(WarFailLayer::showDropText)),
// 			NULL));

		//血色底图
// 		CCSprite *blood = CCSprite::create("warscene2/maskbg03.png");
// 		blood->setPosition(star->getPosition());
// 		txtRight->getParent()->addChild(blood, txtRight->getZOrder()-1);
	}

	//选择按钮
	for(unsigned int i=0; i<3; i++)
	{
		CButton * hint = (CButton*)m_ui->findWidgetById(CCString::createWithFormat("hint%d", i+1)->getCString());
		hint->setOnClickListener(this,ccw_click_selector(WarFailLayer::onHintClick));
		hint->setTag(CA_GetHero+i);
		hint->setScale(0.8f);
		hint->runAction(CCSequence::create(CCScaleTo::create(0.2f, 1.2f), CCScaleTo::create(0.1f, 1), NULL));
	}

	//复活按钮
	CButton *btnTry = (CButton*)m_ui->findWidgetById("relive");
	btnTry->setOnClickListener(this,ccw_click_selector(WarFailLayer::onRetryClick));
	btnTry->setScale(.1f);
	btnTry->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCScaleTo::create(0.2f, 1.2f), CCScaleTo::create(0.1f, 1), NULL));
	btnTry->setVisible(false);

	//退出按钮
	CButton *btnExit = (CButton*)m_ui->findWidgetById("exit");
	btnExit->setOnClickListener(this,ccw_click_selector(WarFailLayer::onExitClick));
	btnExit->setScale(.1f);
	btnExit->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCScaleTo::create(0.2f, 1.2f), CCScaleTo::create(0.1f, 1), NULL));

	return res;
}

void WarFailLayer::afterTextEnd(CCNode *pSender){
	pSender->removeFromParent();
}

void WarFailLayer::updateNewStep(const TMessage& tMsg)
{
	int step = DataCenter::sharedData()->getUser()->getUserData()->getNewStep();
	if (step>0&&step<100)
	{
		for(unsigned int i=0; i<3; i++)
		{
			CButton * hint = (CButton*)m_ui->findWidgetById(CCString::createWithFormat("hint%d", i+1)->getCString());
			hint->setEnabled(false);
			//设置灰度
			hint->getDisabledImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
		}
	}
}

void WarFailLayer::showDropText(CCNode *pSender){
	CCNode *base = m_ui;
// 	CCNode *txt = base->getChildByTag(TAG_F_TITLE_LEFT_1);
// 	txt->setVisible(true);
// 	
// 	txt = base->getChildByTag(TAG_F_TITLE_LEFT_2);
// 	txt->setVisible(true);
// 	
// 	txt = base->getChildByTag(TAG_F_TITLE_RIGHT);
// 	txt->setVisible(true);
}

void WarFailLayer::onHintClick(CCObject * pSender){
	CCMenuItem *item = (CCMenuItem*)pSender;
	item->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.2f), 
		CCScaleTo::create(0.1f, 1.f), 
		CCCallFuncN::create(this, callfuncN_selector(WarFailLayer::onExitWithMode)),
		NULL));
}

void WarFailLayer::onExitWithMode(CCNode* pSender)
{
	int iTag = pSender->getTag();
	bool isOpen = true;
	if(iTag == CA_BlackSmith || iTag == CA_Combine)
	{
		int iCityId = 0;
		if(iTag==CA_BlackSmith) iCityId = 4;
		if(iTag==CA_Combine) iCityId = 12;

		isOpen = CMainCityControl::getInstance()->isCityOpen(iCityId);
	}

	//是否开启//调和塔 12  铁匠捕 4  培植区6  酒馆10   悬赏府15   竞技场3  排行榜13  千层塔17 历练谷16
	if(isOpen)
	{
		DataCenter::sharedData()->setCityActionType((CityActionType)iTag);
		CScene* scene = GETSCENE(LoadBattleResource);
		((LoadBattleResource*)scene)->setRelease(true,SkipcityScene);
		CSceneManager::sharedSceneManager()->replaceScene(scene);
	}
	else
	{
		ShowPopTextTip(GETLANGSTR(191));
	}
}

void WarFailLayer::onExitClick(CCObject* pSender){
	CCMenuItem *item = (CCMenuItem*)pSender;
	item->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.2f), 
		CCScaleTo::create(0.1f, 1.f), 
		CCCallFuncN::create(this, callfuncN_selector(WarFailLayer::onExitWait)),
		NULL));
}
void WarFailLayer::onExitWait(CCNode *pSender){	

	if(BattleManage->getLastStageId()!=-1)
	{
		//预请求关卡信息
		if(BattleManage->getNormal())
		{
			CPlayerControl::getInstance().sendChapterList(0);
		}
		else
		{
			CPlayerControl::getInstance().sendChapterList(1);
		}
		DataCenter::sharedData()->setCityActionType(CA_GoToChapater);
	}
	else
	{
		DataCenter::sharedData()->setCityActionType(CA_GoToReward);		
	}

	CScene* scene = GETSCENE(LoadBattleResource);
	((LoadBattleResource*)scene)->setRelease(true,SkipcityScene);
	CSceneManager::sharedSceneManager()->replaceScene(scene);
}

void WarFailLayer::onRetryClick(CCObject *pSender){
	CCMenuItem *item = (CCMenuItem*)pSender;
	item->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.2f), 
		CCScaleTo::create(0.1f, 1.f), 
		CCCallFuncN::create(this, callfuncN_selector(WarFailLayer::onRetryWait)),
		NULL));
	CNetClient::getShareInstance()->sendDataType(ContinueStageMsg);
}

//复活
void WarFailLayer::onRetryWait(CCNode *pSender)
{

}

void WarFailLayer::onEnter()
{
	BaseLayer::onEnter();
	setTouchPriority(-12);
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO,this,GameMsghandler_selector(WarFailLayer::updateNewStep));
}

bool WarFailLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	BaseLayer::ccTouchBegan(pTouch,pEvent);

	return true;
}

void WarFailLayer::onExit()
{
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
	BaseLayer::onExit();
}
