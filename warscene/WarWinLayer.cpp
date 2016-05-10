#include "WarWinLayer.h"
#include "Global.h"
#include "netcontrol/CPlayerControl.h"
#include "common/color.h"
#include "net/CNetClient.h"
#include "scene/loadWar.h"
#include "warscene/WarAssist.h"
#include "common/ProgressLabel.h"
#include "GMessage.h"
#include "bag/bagData.h"
#include "tollgate/Chapter.h"
#include "model/DataCenter.h"
#include "model/DataDefine.h"
#include "model/WarManager.h"
#include "warscene/RandomBusineseMan.h"
#include "guide/GuideManager.h"
#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"
#include "common/CommonFunction.h"
#include "common/CGameSound.h"
#include "tools/UICloneMgr.h"
#include "mainCity/RecruitData.h"
#include "mainCity/CNewHero.h"
#include "common/CCRollLabelAction.h"

const char WarWinLayer::WAR_WIN_RESULT[] = "war_win_show";

WarWinLayer::WarWinLayer()
	:m_pMonsterInfo(nullptr)
	,m_pItemInfo(nullptr)
	,m_iIndexForFindNewHero(0)
{
}

WarWinLayer::~WarWinLayer(){
}

bool WarWinLayer::init(){

	bool res = BaseLayer::init();
	
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	//黑底
	MaskLayer* pMaskLayer = MaskLayer::create("WarWinLayerMasklLayer");
	pMaskLayer->setContentSize(winSize);
	LayerManager::instance()->push(pMaskLayer);

	//配置内容
	m_ui = LoadComponent("warwinLayer.xaml");
	m_ui->setPosition(VCENTER);
	this->addChild(m_ui);

	m_pNewHeroEffect = CNewHero::create();
	m_pNewHeroEffect->setVisible(false);
	this->addChild(m_pNewHeroEffect, 20);

	//旋转光
	CCSprite *star = (CCSprite*)m_ui->findWidgetById("light");
	star->setTag(TAG_TITLE_STAR);
	star->runAction(CCRepeatForever::create(CCRotateBy::create(0.2f, 20)));


	//通关字样
	CCSprite *pass = (CCSprite*)m_ui->findWidgetById("finish_level");
	pass->setVisible(false);
	pass->setTag(TAG_TITLE);


	//血色底图
	CCSprite *blood = CCSprite::create("warscene2/maskbg03.png");
	blood->setPosition(pass->getPosition());
	blood->setTag(TAG_TITLE_BLOOD);
	m_ui->addChild(blood);

// 	//通关单字样
// 	CCSprite *txtLeft = CCSprite::create("warscene2/p1.png");
// 	txtLeft->setPosition(ccp(pass->getPositionX()-250, winSize.height*1.2f));
// 	txtLeft->runAction(CCMoveTo::create(0.3f, ccp(pass->getPositionX()-38, pass->getPositionY()-8)));
// 	txtLeft->setTag(TAG_TITLE_LEFT);
// 	m_ui->addChild(txtLeft, 300);
// 
// 	//通关单字样
// 	CCSprite *txtRight = CCSprite::create("warscene2/p7.png");
// 	txtRight->setPosition(ccp(pass->getPositionX()+250, winSize.height*1.2f));
// 	txtRight->runAction(CCSequence::create(CCDelayTime::create(0.4f), 
// 		CCMoveTo::create(0.3f, ccp(pass->getPositionX()+35, pass->getPositionY()-8)),
// 		CCCallFunc::create(this, callfunc_selector(WarWinLayer::afterTextAnime)),
// 		NULL));
// 	txtRight->setTag(TAG_TITLE_RIGHT);
// 	m_ui->addChild(txtRight, 300);

	afterTextAnime();

	//玩家等级进度条
	CProgressBar *bar = (CProgressBar*)m_ui->findWidgetById("level_bar1");
	bar->setMaxValue(100);
	bar->setTag(TAG_LEVEL_BAR);

	//设置玩家等级进度
	UserManager *userMan = DataCenter::sharedData()->getUser();
	UserData *user = userMan->getUserData();
	//存储之前的等级
	_iLevelBefore = user->getLevel();
	float nextExp = user->getNextExp();
	float curExp = user->getExp() * 100.0f / nextExp;
	bar->setValue(curExp); 
	
	//设置玩家等级数字
	CCString *strLv = CCString::createWithFormat("%d", user->getLevel());
	CLabel *lblValue = (CLabel *)m_ui->findWidgetById("player_level");
	lblValue->setString(strLv->getCString());
	lblValue->setTag(TAG_LEVEL_LABEL);

	//铜钱数量
	CLabel *lblMoney = (CLabel*)m_ui->findWidgetById("coin_num");
	lblMoney->setTag(TAG_MONEY_LABEL);

 	//重来按钮
 	CButton *btnTry = (CButton*)m_ui->findWidgetById("retry");
 	btnTry->setOnClickListener(this,ccw_click_selector(WarWinLayer::onRestartClick));
 	btnTry->setScale(.1f);
 	btnTry->setVisible(false);

	//退出按钮
	CButton *btnExit = (CButton*)m_ui->findWidgetById("exit");
	btnExit->setOnClickListener(this,ccw_click_selector(WarWinLayer::onExitClick));
	btnExit->setScale(.1f);
	btnExit->setVisible(false);

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_tableView = (CTableView*)m_ui->findWidgetById("scroll");
	m_tableView->setDirection(eScrollViewDirectionHorizontal);	
	m_tableView->setCountOfCell(0);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setAnchorPoint(ccp(0,0));
	m_tableView->setAutoRelocate(true);
	m_tableView->setDeaccelerateable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(WarWinLayer::tableviewDataSource));
	m_tableView->reloadData();
	
	return res;
}


void WarWinLayer::onEnter()
{
	BaseLayer::onEnter();

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO,this,GameMsghandler_selector(WarWinLayer::upRoleMsg));	//接收广播消息

	setTouchPriority(-12);
}

void WarWinLayer::onExit()
{
	BaseLayer::onExit();

	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
	CNetClient::getShareInstance()->unRegisterAllMsgHandler(this);
}



void WarWinLayer::processBattleFinish(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	BattleFinishRep *res = (BattleFinishRep*)msg;
	
	CCSize size = CCDirector::sharedDirector()->getWinSize();	

	if (!res->win())return;

	//延时出现
	float fDelayTime = 1.0;
	//有信息回调了，考虑弹出商人
	if(res->has_mobieshop())
	{
		fDelayTime = 1.5f;
		runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFunc::create(this, callfunc_selector(WarWinLayer::callBackShowRandomHero))));
	}

	 //大关卡才有重来按钮
	if(DataCenter::sharedData()->getWar()->getStageType()==1)
	{
		CButton *btnTry = (CButton*)m_ui->findWidgetById("retry");
		btnTry->runAction(CCSequence::create(CCDelayTime::create(fDelayTime), CCShow::create(), CCScaleTo::create(0.2f, 1.2f), CCScaleTo::create(0.1f, 1), NULL));
	}
	
	//退出按钮
	CButton *btnExit = (CButton*)m_ui->findWidgetById("exit");
	btnExit->runAction(CCSequence::create(CCDelayTime::create(fDelayTime), CCShow::create(), CCScaleTo::create(0.2f, 1.2f), CCScaleTo::create(0.1f, 1), NULL));
	

	//CCString *strMoney = CCString::createWithFormat("%d", res->coin());
	CLabel *lblMoney = (CLabel *)m_ui->getChildByTag(TAG_MONEY_LABEL);
	//lblMoney->setString(strMoney->getCString());
	lblMoney->runAction(CCRollLabelAction::create(1.5f, 0, res->coin(), lblMoney));

	for (int i=0;i<res->star();i++)
	{
		CCNode* node = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("star%d", i+1)->getCString());
		node->setVisible(true);
		node->setScale(3.0);
		CCScaleTo* cs = CCScaleTo::create(1.0f,1.0);
		CCEaseBounceOut * es= CCEaseBounceOut::create(cs);
		node->runAction(cs);
	}

	//星星效果
	genStar(res->star());

	//英雄队伍
	//出战英雄数量
	int heroListSize = res->herolist_size();
	for (int vi = 0; vi < heroListSize; vi++)
	{
		Hero hero = res->herolist(vi);

		//获取层
		CCString* pStrLay = CCString::createWithFormat("hero%d", vi+1);
		CCString* pStrHead = CCString::createWithFormat("head%d", vi);
		CCString* pStrProgress = CCString::createWithFormat("bar_fg%d", vi);
		CCString* pStrLevel = CCString::createWithFormat("level%d", vi);
		CCString* pStrMask = CCString::createWithFormat("mask%d", vi);
		CCString* pStrExp = CCString::createWithFormat("level_num%d", vi);

		CLayout* pLayout = (CLayout*)m_ui->findWidgetById(pStrLay->getCString());
		pLayout->setVisible(true);

		//品质框
		CCSprite* pMask = (CCSprite*)pLayout->findWidgetById(pStrMask->getCString());
		pMask->setTexture(setItemQualityTexture(hero.color()));

		//添加星星
		CLayout* pStarLayout = getStarLayout(hero.quality());
		pMask->addChild(pStarLayout, 10);

		//获取头像底板
		CCSprite* pBoardHead = (CCSprite*)pLayout->findWidgetById(pStrHead->getCString());

		//生成头像
		CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero.thumb())->getCString());
		if (!head)
		{
			head = CCSprite::create("headImg/101.png");
			CCLOG("[ ERROR ]WarWinLayer::init Lost headImg/%d.png",hero.thumb());
		}
		head->setScale(pBoardHead->getContentSize().width/head->getContentSize().width);
		head->setPosition(ccp(pBoardHead->getContentSize().width*0.5f, pBoardHead->getContentSize().height*0.5f));
		pBoardHead->addChild(head);

		//英雄经验条
		CProgressBar *heroExp = (CProgressBar*)pLayout->findWidgetById(pStrProgress->getCString());
		heroExp->setTag(TAG_CHAR_BAR + vi);
		heroExp->setMaxValue(100);

		int curExp = 0;
		if(hero.nextexp() != 0)
		{
			curExp = hero.exp()  * 100.f/ hero.nextexp();
		}
		heroExp->setValue(curExp);

		//英雄等级
		CLabel *heroLv = (CLabel*)pLayout->findWidgetById(pStrLevel->getCString());
		heroLv->setTag(TAG_HEROS_LEVEL_LABEL+vi);

		//英雄获得经验,暂时用 生成效率 字段 存储
		CLabel * pHeroExp = (CLabel*)pLayout->findWidgetById(pStrExp->getCString());
		pHeroExp->setString(CCString::createWithFormat("+%d", hero.prate())->getCString());
	}
	

	//奖品列表
	itemBoxList = new CCArray();
	itemBoxList->init();
	int itemGot = res->itemlist_size();

	//筛选合并数据
	for (int vj = 0; vj < itemGot; vj++)
	{
		//缓存数据
		CPrize prize;
		prize.read(res->itemlist(vj));

		int i=0;
		for(; i<m_prizeList.size(); i++)
		{
			CPrize& prizeExist = m_prizeList.at(i);
			if(prizeExist.thumb == prize.thumb)
			{
				prizeExist.num += prize.num;
				break;
			}
		}
		if(i==m_prizeList.size())
		{
			m_prizeList.push_back(prize);
		}
	}

	int iItemSize = m_prizeList.size();
	m_tableView->setCountOfCell(iItemSize);
	m_tableView->reloadData();
	if(iItemSize<=0)
	{
		//隐藏两条白线
		m_ui->findWidgetById("line1")->setVisible(false);
		m_ui->findWidgetById("line2")->setVisible(false);
	}

	runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCCallFunc::create(this, callfunc_selector(WarWinLayer::callBackForShowNewHero))));

	//播特效
	float fTimeGap = 0.2f;
	for(unsigned int i=0; i<iItemSize; i++)
	{
		CCNode* pNode = (CCNode*)m_tableView->getContainer()->getChildren()->objectAtIndex(i);
		pNode->setAnchorPoint(ccp(0.5f, 0.5f));
		pNode->setPosition(pNode->getPosition()+ccp(pNode->getContentSize().width/2, pNode->getContentSize().height/2));
		pNode->setTag(i);
		pNode->setVisible(false);
		pNode->setScale(0.0f);
		pNode->runAction(CCSequence::create(
			CCDelayTime::create(fTimeGap*i+0.5f),
			CCShow::create(),
			CCEaseBounceOut::create(CCScaleTo::create(0.2f, 1.0f)),
			CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::callbackForScroll)),
			nullptr));
	}


	for(unsigned int i=0; i<res->herolist_size(); i++)
	{

		//获取层
		CCString* pStrLay = CCString::createWithFormat("hero%d", i+1);

		//获取头像
		CCString* pStrHead = CCString::createWithFormat("head%d", i);
		//获取品质框
		CCString* pStrMask = CCString::createWithFormat("mask%d", i);

		CLayout* pLayout = (CLayout*)m_ui->findWidgetById(pStrLay->getCString());
		pLayout->setVisible(true);

		CCNode* pHead = (CCNode*)pLayout->findWidgetById(pStrHead->getCString());
		CCNode* pMask = (CCNode*)pLayout->findWidgetById(pStrMask->getCString());

		//游戏结束后的英雄数据
		const Hero& hero = res->herolist(i);
		//先前等级
		int iLevelBefore = hero.level();
		//当前等级
		int iLevelNow = hero.maxlevel();
		//当前经验
		int iExpNow = hero.exp();
		//下一级所需经验
		int iExpNext = hero.nextexp();

		//是否升级
		int iLevelGap = iLevelBefore<iLevelNow;
		iLevelGap = iLevelGap>3?3:iLevelGap;//最多做三次

		//当前经验进度
		float fProgress = 100.0f*iExpNow/iExpNext;

		//进度条
		CProgressBar *barExp = (CProgressBar*)pLayout->getChildByTag(TAG_CHAR_BAR + i);
		//进度速度
		float fProgressSpeed = 150.0f;
		if(iLevelGap>0)
		{
			PlayEffectSound(SFX_424);
			float fTimeGap = 0;
			for(unsigned int j=0; j<=iLevelGap; j++)
			{
				barExp->runAction(
					CCSequence::createWithTwoActions(
					CCDelayTime::create(fTimeGap), 
					CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::callBackForProgressAction))));
				fTimeGap += 100.0f/fProgressSpeed;
				if(j==iLevelGap)
				{
					barExp->setTag(fProgress);
					barExp->runAction(
						CCSequence::createWithTwoActions(
						CCDelayTime::create(fTimeGap), 
						CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::callBackForProgressActionToAim))));
				}
			}

			//展示升级效果
			//获取头像
			int iZorder = 999;
			//箭头
			CCSprite* pArraow = CCSprite::create("warscene2/up.png");
			pArraow->setPosition(pMask->getPosition());
			pArraow->setOpacity(0);
			pArraow->setScale(0.2f);
			m_ui->addChild(pArraow, iZorder);
			//字样
			CCSprite* pText = CCSprite::create("warscene2/lvup.png");
			pText->setPosition(pMask->getPosition());
			pText->setOpacity(0);
			pText->setScale(0.2f);
			m_ui->addChild(pText, iZorder+1);

			pArraow->runAction(CCSequence::create(
				CCDelayTime::create(iLevelGap*0.15f+fTimeGap),
				CCSpawn::create(CCFadeIn::create(0.12f), CCScaleTo::create(0.15f, 1.0f), CCMoveBy::create(0, ccp(0, -30)), nullptr), 
				CCSpawn::createWithTwoActions(CCMoveBy::create(0.8f, ccp(0, 50)), CCFadeOut::create(1.0f)),
				CCHide::create(),
				nullptr));
			pText->runAction(CCSequence::create(
				CCDelayTime::create(iLevelGap*0.15f+fTimeGap),
				CCSpawn::create(CCFadeIn::create(0.12f), CCScaleTo::create(0.15f, 1.0f), CCMoveBy::create(0, ccp(0, -30)), nullptr), 
				CCSpawn::createWithTwoActions(CCMoveBy::create(0.8f, ccp(0, 35)), CCFadeOut::create(1.0f)),
				CCHide::create(),
				nullptr));

		}
		else
		{
			barExp->startProgressFromTo(0, fProgress, fProgress/150.0f);
		}
		
		//等级数字
		CCString *strLv = CCString::createWithFormat("%d", iLevelNow);
		CLabel *heroLv = (CLabel*)pLayout->getChildByTag(TAG_HEROS_LEVEL_LABEL+i);
		heroLv->setString(strLv->getCString());
	}
}

void WarWinLayer::upRoleMsg(const TMessage& tMsg)
{
	//收到这条消息表示，领主的信息已经更新了。
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	int exp = user->getExp();
	
	float nextExp = user->getNextExp();
	float ExpPe = exp / nextExp * 100;
	
	//相差的等级
	int iLevelGap = user->getLevel()-_iLevelBefore;
	iLevelGap = iLevelGap>3?3:iLevelGap;//最多做三次
	if (iLevelGap>1)
		PlayEffectSound(SFX_432);

	//等级文字
	CCString *strLv = CCString::createWithFormat("%d", user->getLevel());
	CLabel* pLevel = (CLabel*)m_ui->getChildByTag(TAG_LEVEL_LABEL);
	pLevel->setString(strLv->getCString());

	//进度条
	CProgressBar* exp_pro = (CProgressBar*)m_ui->findWidgetById("level_bar1");
	if(!exp_pro)return;

	//进度速度
	float fProgressSpeed = 80.0f;
	if(iLevelGap>0)
	{
		float fTimeGap = 0;
		for(unsigned int j=0; j<=iLevelGap; j++)
		{
			exp_pro->runAction(
				CCSequence::createWithTwoActions(
				CCDelayTime::create(fTimeGap), 
				CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::callBackForProgressAction))));
			fTimeGap += 100.0f/fProgressSpeed;
			if(j==iLevelGap)
			{
				exp_pro->setTag(ExpPe);
				exp_pro->runAction(
					CCSequence::createWithTwoActions(
					CCDelayTime::create(fTimeGap), 
					CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::callBackForProgressActionToAim))));
			}
		}
	}
	else
	{
		exp_pro->startProgressFromTo(0, ExpPe, ExpPe/fProgressSpeed);
	}


}

void WarWinLayer::touchDispose()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,0,true);
}

void WarWinLayer::afterTextAnime(){

	CCNode *node = m_ui->getChildByTag(TAG_TITLE_BLOOD);
	node->setVisible(false);
// 	node = m_ui->getChildByTag(TAG_TITLE_LEFT);
// 	node->setVisible(false);
// 	node = m_ui->getChildByTag(TAG_TITLE_RIGHT);
// 	node->setVisible(false);
	node = m_ui->getChildByTag(TAG_TITLE);
	node->setVisible(true);
	
	passTextAnime();
}

void WarWinLayer::passTextAnime(){

	CCAnimation *txtAni = CCAnimation::create();
	txtAni->addSpriteFrameWithFileName("warscene2/pass_1.png");
	txtAni->addSpriteFrameWithFileName("warscene2/pass_2.png");
	txtAni->addSpriteFrameWithFileName("warscene2/pass_3.png");
	txtAni->addSpriteFrameWithFileName("warscene2/pass_4.png");
	txtAni->addSpriteFrameWithFileName("warscene2/pass_5.png");
	txtAni->addSpriteFrameWithFileName("warscene2/pass_6.png");
	txtAni->setDelayPerUnit(0.1f);
	CCSprite *node = (CCSprite*)m_ui->getChildByTag(TAG_TITLE);
	node->runAction(CCRepeatForever::create(
		CCSequence::create(CCAnimate::create(txtAni), CCDelayTime::create(1.f), NULL)
		));
}

void WarWinLayer::changeStar(CCNode *pSender){
	//CCAnimation *starAni = CCAnimation::create();
	//starAni->addSpriteFrameWithFileName("warscene2/a3.png");
	//starAni->addSpriteFrameWithFileName("warscene2/a2.png");
	//starAni->setDelayPerUnit(0.1f);

	//CCSprite *star = (CCSprite*) pSender;
	//star->runAction(CCAnimate::create(starAni));
}

void WarWinLayer::genStar(int numOfStar)
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	//中间的星星
	CCSprite *grade2 = (CCSprite*)m_ui->findWidgetById("star2");
	if (DataCenter::sharedData()->getWar()->getNormal())
	{
		grade2->setTexture(CCTextureCache::sharedTextureCache()->addImage("warscene2/starlight.png"));
	}
	else
	{
		grade2->setTexture(CCTextureCache::sharedTextureCache()->addImage("tollgate/star.png"));
	}
	CCPoint basePos2 = grade2->getPosition();
	grade2->setPositionY(size.height*1.05f);
	grade2->runAction(CCSequence::create(CCDelayTime::create(0.25f),
		CCSpawn::createWithTwoActions(CCMoveTo::create(0.3f, basePos2), CCRotateBy::create(0.3f, 360)),
		CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::changeStar)), NULL));
	grade2->setTag(TAG_GRADE_STAR2);

	//左边的星星
	CCSprite *grade1 = (CCSprite*)m_ui->findWidgetById("star1");
	if (DataCenter::sharedData()->getWar()->getNormal())
	{
		grade1->setTexture(CCTextureCache::sharedTextureCache()->addImage("warscene2/starlight.png"));
	}
	else
	{
		grade1->setTexture(CCTextureCache::sharedTextureCache()->addImage("tollgate/star.png"));
	}
	CCPoint basePos1 = grade1->getPosition();
	grade1->setPositionY(size.height*1.05f);
	grade1->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCMoveTo::create(0.3f, basePos1), CCRotateBy::create(0.3f, 360)),
		CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::changeStar)), NULL));
	grade1->setTag(TAG_GRADE_STAR1);

	//右边的星星
	CCSprite *grade3 = (CCSprite*)m_ui->findWidgetById("star3");
	if (DataCenter::sharedData()->getWar()->getNormal())
	{
		grade3->setTexture(CCTextureCache::sharedTextureCache()->addImage("warscene2/starlight.png"));
	}
	else
	{
		grade3->setTexture(CCTextureCache::sharedTextureCache()->addImage("tollgate/star.png"));
	}
	CCPoint basePos3 = grade3->getPosition();
	grade3->setPositionY(size.height*1.05f);
	grade3->runAction(CCSequence::create(CCDelayTime::create(0.55f), 
		CCSpawn::createWithTwoActions(CCMoveTo::create(0.3f, basePos3), CCRotateBy::create(0.3f, 360)),
		CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::changeStar)), NULL));
	grade3->setTag(TAG_GRADE_STAR3);

	switch(numOfStar){		
	case 0:
		grade1->setVisible(false);
	case 1:
		grade2->setVisible(false);
	case 2:
		grade3->setVisible(false);
	case 3:	
		break;
	}

	this->runAction(CCSequence::create(CCDelayTime::create(1.2f),
		CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::openItem)), NULL));
}

void WarWinLayer::openItem(CCNode* pSender){

	int iCount = itemBoxList->count();
	iCount = iCount>7?7:iCount;//最多显示七个
	if ( iCount > 0)
	{
		float delayTime = 0.2f;
		for (int vi = 0; vi  < iCount; vi ++)
		{
			CCSprite *box = (CCSprite*)itemBoxList->objectAtIndex(vi);
			box->runAction(CCSequence::create(CCDelayTime::create(delayTime*vi+0.3f),
				CCFadeOut::create(delayTime), NULL));
		}	
		itemBoxList->removeAllObjects();
	}
}

void WarWinLayer::onExitClick(CCObject *pSender){
	CCMenuItem *item = (CCMenuItem*)pSender;
	item->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.2f), 
		CCScaleTo::create(0.1f, 1.f), 
		CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::onBackClick)),
		NULL));
}

void WarWinLayer::onRestartClick(CCObject *pSender){
	CCMenuItem *item = (CCMenuItem*)pSender;
	item->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.2f), 
		CCScaleTo::create(0.1f, 1.f), 
		CCCallFuncN::create(this, callfuncN_selector(WarWinLayer::stageRestart)),
		NULL));
}

void WarWinLayer::onBackClick(CCNode* ob)
{
	//返回章节
	if(DataCenter::sharedData()->getWar()->getLastStageId()!=-1)
	{
		DataCenter::sharedData()->setCityActionType(CA_GoToChapater);

		//预请求关卡信息
		if(DataCenter::sharedData()->getWar()->getNormal())
		{
			CPlayerControl::getInstance().sendChapterList(0);
		}
		else
		{
			CPlayerControl::getInstance().sendChapterList(1);
		}
	}
	else
	{
		DataCenter::sharedData()->setCityActionType(CA_GoToReward);		
	}
	CScene* scene = GETSCENE(LoadWar);
	((LoadWar*)scene)->setRelease(true, SkipcityScene);
	CSceneManager::sharedSceneManager()->replaceScene(scene);
}

// 同一戰鬥重新開始
void WarWinLayer::stageRestart(CCNode* pSender)
{
	//返回关卡
	if(DataCenter::sharedData()->getWar()->getLastStageId()!=-1)
	{
		DataCenter::sharedData()->setCityActionType(CA_GoToStage);
	}
	else
	{
		DataCenter::sharedData()->setCityActionType(CA_GoToReward);		
	}
	CScene* scene = GETSCENE(LoadWar);
	((LoadWar*)scene)->setRelease(true, SkipcityScene);
	CSceneManager::sharedSceneManager()->replaceScene(scene);

	////预请求关卡信息
	//CStage* stage = DataCenter::sharedData()->getWar()->getTollgete(); 
	//CPlayerControl::getInstance().sendStageInfo(stage->id);
}

void WarWinLayer::callBackForProgressAction( CCNode* pSender )
{
	CProgressBar* barExp = (CProgressBar*)pSender;
	barExp->startProgressFromTo(0, 100, 100/150.0f);
}

void WarWinLayer::callBackForProgressActionToAim( CCNode* pSender )
{
	CProgressBar* barExp = (CProgressBar*)pSender;
	barExp->startProgressFromTo(0, barExp->getTag(), barExp->getTag()/150.0f);
}

void WarWinLayer::callBackShowRandomHero()
{
	RandomBusineseMan* pRandomBusineseMan = RandomBusineseMan::create();
	LayerManager::instance()->push(pRandomBusineseMan);
}

bool WarWinLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	BaseLayer::ccTouchBegan(pTouch,pEvent);

	return true;
}

void WarWinLayer::onPress( CCObject* pSender, CTouchPressState iState )
{
	CImageView* pImage = (CImageView*)pSender;
	CPrize* prize = &m_prizeList.at(pImage->getTag());
	int iType = pImage->getZOrder();

	//1.物品 2.英雄
	switch (iType)
	{
	case 1:
		{
			if(m_pItemInfo==nullptr)
			{
				m_pItemInfo = CItemInfo::create();
				this->addChild(m_pItemInfo);
			}

			switch (iState)
			{
			case cocos2d::cocoswidget::CTouchPressOn:
				{
					PlayEffectSound(SFX_429);

					m_pItemInfo->setInfo(prize);
					m_pItemInfo->bindPos(pImage);
					m_pItemInfo->changeForSign(prize);
					m_pItemInfo->show();
					CCLOG("show");
				}
				break;
			case cocos2d::cocoswidget::CTouchPressOff:
				{
					m_pItemInfo->hide();
					CCLOG("hide");
				}
				break;
			default:
				break;
			}

		}break;
	case 2:
		{
			if(m_pMonsterInfo==nullptr)
			{
				m_pMonsterInfo = CMonsterInfo::create();
				this->addChild(m_pMonsterInfo);
			}

			switch (iState)
			{
			case cocos2d::cocoswidget::CTouchPressOn:
				{
					PlayEffectSound(SFX_429);

					CHero pHero;
					pHero.heroid = prize->id;
					pHero.thumb = prize->thumb;
					pHero.quality = prize->quality;
					pHero.iColor = prize->color;
					pHero.level = 1;
					m_pMonsterInfo->setHero(&pHero);
					m_pMonsterInfo->bindPos(pImage);
					m_pMonsterInfo->show();
					CCLOG("show");
				}
				break;
			case cocos2d::cocoswidget::CTouchPressOff:
				{
					m_pMonsterInfo->hide();
					CCLOG("hide");
				}
				break;
			default:
				break;
			}

		}break;
	default:
		break;
	}


}

void WarWinLayer::addTableCell( unsigned int uIdx, CTableViewCell* pCell )
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);		

	CPrize& prize = m_prizeList.at(uIdx);

	string str = GetImageName(prize.type, prize.color, prize.thumb);

	for (int i = 1; i <= 3; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		switch (i)
		{
		case 1:
			{
				CImageView *mask= (CImageView *)child;
				CImageView *itemSpr = CImageView::create(str.c_str());
				if (!itemSpr)
				{
					itemSpr = CImageView::create("prop/32003.png");
					CCLOG("[ ERROR ] CPopItem::addTableCell Lost Image = %d");
				}
				itemSpr->setPosition(ccp(mask->getContentSize().width/2,mask->getContentSize().height/2));
				mask->addChild(itemSpr);
			}
			break;
		case 2:
			{
				CImageView *mask= (CImageView *)child;
				mask->setTexture(setItemQualityTexture(prize.color));
				mask->setTag(uIdx);
				mask->setZOrder(prize.type);
				mask->setTouchEnabled(true);
				mask->setOnPressListener(this, ccw_press_selector(WarWinLayer::onPress));

				//添加星星
				if(prize.quality >0)
				{
					CLayout* pStarLayout = getStarLayout(prize.quality);
					mask->addChild(pStarLayout);
				}

			}
			break;
		case 3:
			{
				int iPrizeNum = prize.num;

				if (iPrizeNum>0)
				{
					CLabel *num = (CLabel*)child;
					num->setVisible(false);
					pCell->removeChild(num);
					CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
					haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
					haveNum->setPosition(ccp(83, 8));
					pCell->addChild(haveNum, 99);
					haveNum->setString(ToString(iPrizeNum));
				}
				else
				{
					child->setVisible(false);
				}
			}
			break;
		}
	}
}

CCObject* WarWinLayer::tableviewDataSource( CCObject* pConvertCell, unsigned int uIdx )
{
	CTableViewCell* pCell = (CTableViewCell*)pConvertCell;
	if (!pCell)
	{
		pCell = new CTableViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		addTableCell(uIdx,pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx,pCell);
	}
	return pCell;
}

void WarWinLayer::callbackForScroll(CCNode* pSender)
{
	CCNode* pNode = (CCNode*)pSender;
	int iTag = pNode->getTag();
	m_tableView->setContentOffset(ccp(iTag>5?(iTag-5)*m_tableView->getSizeOfCell().width:0, 0));
}

void WarWinLayer::callBackForShowNewHero()
{
	for(; m_iIndexForFindNewHero<m_prizeList.size(); m_iIndexForFindNewHero++)
	{
		CPrize& prize = m_prizeList.at(m_iIndexForFindNewHero);

		//获得新英雄出场
		if(prize.type == 2)
		{
			HeroLotteryData data;
			data.heroType = prize.type;
			data.thumb = prize.thumb;
			data.quality = prize.quality;
			m_pNewHeroEffect->showNewHeroEffect(&data);
			m_pNewHeroEffect->runAction(CCSequence::createWithTwoActions(
				CCDelayTime::create(3.0f), 
				CCCallFunc::create(this, callfunc_selector(WarWinLayer::callBackForShowNewHero))));
			m_iIndexForFindNewHero++;
			return;
		}
	}

	m_pNewHeroEffect->setVisible(false);
}

