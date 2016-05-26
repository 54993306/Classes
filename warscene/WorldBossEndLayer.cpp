#include "WorldBossEndLayer.h"
#include "Global.h"
#include "netcontrol/CPlayerControl.h"
#include "common/color.h"
#include "net/CNetClient.h"
#include "scene/loadWar.h"
#include "warscene/WarAssist.h"
#include "common/ProgressLabel.h"
#include "bag/bagData.h"
#include "tollgate/Chapter.h"
#include "model/DataCenter.h"
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
#include "common/CCMixLabelAction.h"

WorldBossEndLayer::WorldBossEndLayer()
{
}

WorldBossEndLayer::~WorldBossEndLayer(){
}

bool WorldBossEndLayer::init(){

	bool res = BaseLayer::init();
	
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	//黑底
	MaskLayer* pMaskLayer = MaskLayer::create("WorldBossEndLayerMasklLayer");
	pMaskLayer->setContentSize(winSize);
	LayerManager::instance()->push(pMaskLayer);

	//配置内容
	m_ui = LoadComponent("worldBossEndLayer.xaml");
	m_ui->setPosition(VCENTER);
	this->addChild(m_ui);

	
	return res;
}


void WorldBossEndLayer::onEnter()
{
	BaseLayer::onEnter();


	//旋转光
	CCSprite *star = (CCSprite*)m_ui->findWidgetById("light");
	star->runAction(CCRepeatForever::create(CCRotateBy::create(0.2f, 20)));

	//通关字样
	CCSprite *pass = (CCSprite*)m_ui->findWidgetById("finish_level");
	pass->setVisible(true);
	pass->setScale(4.0);
	pass->runAction(CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)));
	PlayEffectSound(SFX_425);

	//退出按钮
	CButton *btnExit = (CButton*)m_ui->findWidgetById("exit");
	btnExit->setOnClickListener(this,ccw_click_selector(WorldBossEndLayer::onExitClick));
	btnExit->setScale(.1f);
	btnExit->setVisible(false);

	setTouchPriority(-12);
}

void WorldBossEndLayer::onExit()
{
	BaseLayer::onExit();

	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}



void WorldBossEndLayer::processBattleFinish(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	BossFinishRes *res = (BossFinishRes*)msg;

	float fDelayTime = 1.0;

	//退出按钮
	CButton *btnExit = (CButton*)m_ui->findWidgetById("exit");
	btnExit->runAction(CCSequence::create(CCDelayTime::create(fDelayTime), CCShow::create(), CCScaleTo::create(0.2f, 1.2f), CCScaleTo::create(0.1f, 1), NULL));

	int iRank = res->rank();
	//UP
	CLayout* pLayerUp = (CLayout*)m_ui->findWidgetById("up");
	////DOWN
	CLayout* pLayerDown = (CLayout*)m_ui->findWidgetById("down");
	bool bIsFirst = iRank==1?true:false;

	if(bIsFirst)
	{
		pLayerUp->setVisible(true);
		pLayerDown->setVisible(false);
		pLayerUp->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fDelayTime), CCMoveBy::create(0.3f, ccp(0, -80))));
	}
	else
	{
		pLayerUp->setVisible(true);
		pLayerDown->setVisible(true);
		CLayout* pCell = (CLayout*)m_ui->findWidgetById("Cell");
		pCell->setPositionY(pCell->getPositionY()-300);
		pCell->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fDelayTime), CCMoveBy::create(0.3f, ccp(0, 300))));
	}

	//排名+伤害信息
	//排名
	CCSprite* pRankIcon = (CCSprite*)(m_ui->findWidgetById("rank_icon"));
	if(iRank<=3)
	{
		CCString* pStr = CCString::createWithFormat("worldBoss/rank_%d.png", iRank);
		std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pStr->getCString());
		if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		{
			pRankIcon->setTexture(CCTextureCache::sharedTextureCache()->addImage(pStr->getCString()));
		}
	}
	else
	{
		pRankIcon->setVisible(false);
	}
	//label
	CLabelAtlas* pAtlas = CLabelAtlas::create("", "worldBoss/no_04.png", 26, 43, 48);
	pAtlas->setString(CCString::createWithFormat("%d", iRank)->getCString());
	pAtlas->setPosition(pRankIcon->getPosition()+ccp(0, 25));
	pRankIcon->getParent()->addChild(pAtlas, pRankIcon->getZOrder()+1);
	pAtlas->runAction(MixLabelAction::create(2.0f, pAtlas));

	//累计伤害
	int iHurt = res->totalhurt();
	CCSprite* pDamageBg= (CCSprite*)(m_ui->findWidgetById("text"));
	pDamageBg->setVisible(false);
	CLabelAtlas* pDamage = CLabelAtlas::create("", "worldBoss/no_03.png", 32, 49, 48);
	pDamage->setString(CCString::createWithFormat("%d", iHurt)->getCString());
	pDamage->setPosition(pDamageBg->getPosition()+ccp(0, 26));
	pDamage->setAnchorPoint(ccp(0.5f, 0.5f));
	pDamageBg->getParent()->addChild(pDamage, pDamageBg->getZOrder()+1);
	pDamage->runAction(MixLabelAction::create(1.5f, pDamage));

	//目标玩家
	if(!bIsFirst)
	{
		int iRank = res->next().rank();
		//名次
		CCSprite* pRankIcon = (CCSprite*)(m_ui->findWidgetById("aim_rank"));
		if(iRank<=3)
		{
			CCString* pStr = CCString::createWithFormat("worldBoss/rank_%d.png", iRank);
			std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pStr->getCString());
			if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
			{
				pRankIcon->setTexture(CCTextureCache::sharedTextureCache()->addImage(pStr->getCString()));
			}
		}
		else
		{
			pRankIcon->setVisible(false);
		}
		//label
		CLabelAtlas* pAtlas = CLabelAtlas::create("", "worldBoss/no_04.png", 26, 43, 48);
		pAtlas->setString(CCString::createWithFormat("%d", iRank)->getCString());
		pAtlas->setPosition(pRankIcon->getPosition()+ccp(0, 0));
		pRankIcon->getParent()->addChild(pAtlas, pRankIcon->getZOrder()+1);
		pAtlas->runAction(MixLabelAction::create(2.0f, pAtlas));


		//头像
		CButton* pHeadBg = (CButton*)(m_ui->findWidgetById("head"));
		CCSprite* head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", res->next().thumb())->getCString());
		if (!head)
		{
			head = CCSprite::create("headImg/101.png");
			CCLOG("[ ERROR ] WorldBossEndLayer::processBattleFinish Lost Image = %d", res->next().thumb());
		}
		pHeadBg->addChild(head);
		NodeFillParent(head);

		//名字
		CLabel* pName = (CLabel*)(m_ui->findWidgetById("name"));
		pName->setString(res->next().name().c_str());

		//等级
		CLabel* pLevel = (CLabel*)(m_ui->findWidgetById("level"));
		pLevel->setString(ToString(res->next().level()));

		//伤害
		CLabelAtlas* pHurt = (CLabelAtlas*)(m_ui->findWidgetById("hurt"));
		pHurt->setString(ToString(res->next().hurt()));
	}
}

void WorldBossEndLayer::onExitClick(CCObject *pSender)
{
	CCMenuItem *item = (CCMenuItem*)pSender;
	item->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.2f), 
		CCScaleTo::create(0.1f, 1.f), 
		CCCallFuncN::create(this, callfuncN_selector(WorldBossEndLayer::onBackClick)),
		NULL));
}

void WorldBossEndLayer::onBackClick(CCNode* ob)
{
	//返回章节
	DataCenter::sharedData()->setCityActionType(CA_GoToWorldBoss);

	CScene* scene = GETSCENE(LoadWar);
	((LoadWar*)scene)->setRelease(true, SkipcityScene);
	CSceneManager::sharedSceneManager()->replaceScene(scene);
}


bool WorldBossEndLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	BaseLayer::ccTouchBegan(pTouch,pEvent);

	return true;
}