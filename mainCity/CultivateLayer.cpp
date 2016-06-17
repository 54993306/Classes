#include "CultivateLayer.h"
#include "DefendWuJiang.h"
#include "CityData.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "MainCityControl.h"
#include "mainCity/QuickProduct.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "common/CommonFunction.h"
#include "GMessage.h"
#include "model/DataDefine.h"
#include "common/CGameSound.h"
#include "Resources.h"

bool CCultivateLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("MaskLayerCult");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("Cultivate.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}
	return false;
}
using namespace BattleSpace;

void CCultivateLayer::onEnter()
{ 
	BaseLayer::onEnter();
	for (int i=1; i<=6; ++i)
	{
		if(i==4)continue;//退出按钮去掉了
		CButton *btn = dynamic_cast<CButton*>(m_ui->getChildByTag(i));
		btn->setOnClickListener(this,ccw_click_selector(CCultivateLayer::onClickButton));
	}

	m_pCoinBatch = CCSpriteBatchNode::create("common/icon_11.png");
	m_pCoinBatch->setPosition(ccp(VCENTER.x-1138/2, VCENTER.y-640/2));
	m_pCoinBatch->setContentSize(CCSizeMake(1138, 640));
	this->getParent()->addChild(m_pCoinBatch, 999);

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CCultivateLayer::onClose));
	this->addChild(pClose, 999);

	GetTcpNet->registerMsgHandler(BuildInfo, this, CMsgHandler_selector(CCultivateLayer::recBuildInfo));
	CSceneManager::sharedSceneManager()->addMsgObserver("updateHero",this, GameMsghandler_selector(CCultivateLayer::updateHero));
	CSceneManager::sharedSceneManager()->addMsgObserver(CULTIVATE_GET_MONEY,this, GameMsghandler_selector(CCultivateLayer::updateGetMoneyHeap));

	m_ui->setScale(0.0f);
	CCScaleTo *big = CCScaleTo::create(0.2f,1.0f);
	m_ui->runAction(big);
}

void CCultivateLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateHero",this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(CULTIVATE_GET_MONEY,this);

	BaseLayer::onExit();

	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
}


void CCultivateLayer::updateToAddCoin( float dt )
{
	if(m_bAddMoneyScheduleLock) return;

	//如果已经开采满了，退出
	if(m_iNow>=m_iMax) return;

	//获取金币位置
	CCNode* pCircle1 = (CCNode*)m_ui->findWidgetById("circle_1");
	CCSprite* pNew = CCSprite::create("common/icon_11.png");
	pNew->setPosition(pCircle1->getPosition());
	m_ui->addChild(pNew);
	pNew->setOpacity(0);
	pNew->runAction(CCSequence::create(
		CCSpawn::createWithTwoActions(CCSequence::create(CCFadeIn::create(0.1f), CCDelayTime::create(0.5f), CCFadeOut::create(0.1f), nullptr),CCMoveBy::create(0.65f, ccp(0, -100))),
		CCRemoveSelf::create(),
		nullptr));
}


void CCultivateLayer::updateHero(const TMessage& tMsg)
{
	//清空
	if (tMsg.nMsg == -1)
	{
		CButton *btn = (CButton*)(m_ui->getChildByTag(m_selectHero));
		btn->removeChildByTag(1);
		CHero hero;
		hero.id = 0;
// 		memset(&hero,0,sizeof(CHero));
		if(m_city.heroList.size()>m_selectHero-1)
		{
			m_city.heroList[m_selectHero-1] = hero;
		}

	    CCNode *lab = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("lab%d",m_selectHero)->getCString());
		lab->setVisible(true);
		CCNode *ing = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("ing_%d",m_selectHero)->getCString());
		ing->setVisible(false);
	}
	else
	{
		CButton *btn = (CButton*)(m_ui->getChildByTag(m_selectHero));
		CHero *hero = (CHero*)tMsg.lParam;
		string icon = ToString(hero->thumb);

		btn->removeChildByTag(1);
		CCNode *lab = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("lab%d",m_selectHero)->getCString());
		lab->setVisible(false);
		CCNode *ing = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("ing_%d",m_selectHero)->getCString());
		ing->setVisible(true);
		CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png",hero->thumb)->getCString());
		if (!head)
		{
			head = CCSprite::create("headImg/101.png");
			CCLOG("[ ERROR ] CCultivateLayer::updateHero Lost Image = %d",hero->thumb);
		}
		head->setTag(1);
		btn->addChild(head);
		NodeFillParent(head);
		btn->setEnabled(false);
		head->setColor(RGB_GRAY_COLOR);

		if (m_selectHero>m_city.heroList.size())
		{
			m_city.heroList.push_back(*hero);
		}
		else
		{
			m_city.heroList[m_selectHero-1] = *hero;
		}
		delete hero;
	}

	CLabel *extx = (CLabel*)(m_ui->findWidgetById("baseXl"));
	extx->setString(ToString(tMsg.intParam));

	CMainCityControl::getInstance()->updateCityHero(m_city.cityId,m_city);


	//轮子
	updateCircle(m_city.heroList.size());

}

void CCultivateLayer::onClickButton(CCObject* pSender)
{
	CButton *btn = dynamic_cast<CButton*>(pSender);

	switch (btn->getTag())
	{
	case 1:
	case 2:
	case 3:
		{
			m_selectHero = btn->getTag();
			vector<CHero> &heroList = m_city.heroList;
			//此位置已经驻守
			if (heroList.size()>=btn->getTag())
			{
				CHero hero = heroList[btn->getTag()-1];
				CDefendWuJiang *layer =CDefendWuJiang::create();
				layer->setCityId(m_city.id,m_city.cityId);
				layer->setReplaceId(hero.id);
				LayerManager::instance()->push(layer);
				CPlayerControl::getInstance().sendHeroList(3);
			}
			//此位置还未驻守
			else
			{
				CDefendWuJiang *layer =CDefendWuJiang::create();
				layer->setCityId(m_city.id,m_city.cityId);
				layer->setReplaceId(0);
				LayerManager::instance()->push(layer);
				CPlayerControl::getInstance().sendHeroList(3);
			}
		}
		break;
	case 5:
		{
			//获取
			CMainCityControl::getInstance()->sendGetCityResource(m_city.id);
			btn->setEnabled(false);
			//CSceneManager::sharedSceneManager()->PostMessageA(CULTIVATE_GET_MONEY, -1, nullptr, nullptr, 999);
		}
		break;

		//快速生产
	case 6:
		{
			//快速获取
 			//CQuickProduct *quick = CQuickProduct::create(this);
 			//this->addChild(quick);
			ShowPopTextTip(GETLANGSTR(191));
		}
		break;
	default:
		break;
	}
}

void CCultivateLayer::showBuildInfo(const CCity& ct)
{
	//开采量/总量
	CLabel *storge = (CLabel*)(m_ui->findWidgetById("storLab"));
	CCString *str = CCString::createWithFormat("%d/%d",ct.cap, ct.maxCap);
	storge->setString(str->getCString());

	//基础开采速度
	CLabel *basex = (CLabel*)(m_ui->findWidgetById("baseXl"));
	basex->setString(ToString(ct.basePE));

	//额外开采速度
	CLabel *extx = (CLabel*)(m_ui->findWidgetById("ExtXl"));
	extx->setString(ToString(ct.extPE));

	//开采进度条
	CProgressBar *proStorge = (CProgressBar*)(m_ui->findWidgetById("ProgressSto"));
	proStorge->setMinValue(0);
	proStorge->setMaxValue(ct.maxCap);
	proStorge->startProgress(ct.cap, 0.2f);

	m_iCurrentMoneyRank = calculateMoneyHeapRank((100*ct.cap/ct.maxCap));

	m_iNow = ct.cap;
	m_iMax = ct.maxCap;
	m_fSpeed = ct.basePE + ct.extPE;

	//更新金币堆
	showMoneyHeap(m_iCurrentMoneyRank);

	//轮子
	updateCircle(ct.heroList.size());

	//动画
	CCNode* pCircle1 = (CCNode*)m_ui->findWidgetById("circle_1");
	CCPoint culPos = ccp(pCircle1->getPosition().x, pCircle1->getPosition().y+50);
	CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("9006");
	culAnim->setDelayPerUnit(0.1f);
	CCSprite *culSpr = createAnimationSprite("skill/9006.png",culPos,culAnim,true);
	m_ui->addChild(culSpr);


	//开采英雄列表
	int i=0;
	for (; i<ct.heroList.size()&&i<3; i++)
	{
		CButton *btn = (CButton*)(m_ui->getChildByTag(i+1));
		const CHero &hero = ct.heroList[i];
		if (hero.thumb!=0)
		{
			//头像
			string icon = ToString(hero.thumb);
			CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png",hero.thumb)->getCString());
			if (!head)
			{
				head = CCSprite::create("headImg/101.png");
				CCLOG("[ ERROR ] CCultivateLayer::showBuildInfo Lost Image = %d",hero.thumb);
			}
			head->setPosition(ccp(btn->getContentSize().width/2, btn->getContentSize().height/2));
			head->setTag(1);
			btn->addChild(head);
			NodeFillParent(head);

			//边框
			CImageView* pQuality = (CImageView*)m_ui->findWidgetById(CCString::createWithFormat("rect_%d",i+1)->getCString());
			pQuality->setTexture(setItemQualityTexture(hero.iColor));
			//添加星级
			CLayout* pStarLayout = getStarLayout(hero.quality);
			pQuality->addChild(pStarLayout);

			//驻守标记（有人开采则隐藏掉）
			CCNode *lab = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("lab%d",i+1)->getCString());
			lab->setVisible(false);
			CCNode *ing = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("ing_%d",i+1)->getCString());
			ing->setVisible(true);

			//如果英雄还没有开采完
			if (!hero.remove)
			{
				btn->setEnabled(false);
				head->setColor(RGB_GRAY_COLOR);
			}
		}
	}

	for (i=ct.heroNum;i<3;i++)
	{
		CButton *btn = (CButton*)(m_ui->getChildByTag(i+1));
		btn->setEnabled(false);
		CCNode *lab = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("lab%d",i+1)->getCString());
		lab->setVisible(false);
		//加锁
		CCSprite* pLock = CCSprite::create("common/icon_bg3.jpg");
		btn->addChild(pLock);
		NodeFillParent(pLock);
	}

	runFadeAction(ct);
}

void CCultivateLayer::recBuildInfo(int type, google::protobuf::Message *msg)
{
	if(!isVisible())
	{
		this->setVisible(true);
		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
	}
	
	City* ct = (City*)msg;
	m_city.read(*ct);
	showBuildInfo(m_city);
}

void CCultivateLayer::runFadeAction(const CCity &ct)
{
	//闪动动画 
	CLabel *label = (CLabel*)(m_ui->findWidgetById("ExtXl"));
	CCFadeTo *out = CCFadeTo::create(0.5f,100);
	CCFadeTo *in = CCFadeTo::create(0.5f,255);
	label->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(out,in)));

	for (int i=0;i<ct.heroNum;i++)
	{
		CCNode *lab = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("lab%d",i+1)->getCString());
		CCFadeTo *out = CCFadeTo::create(0.5f,100);
		CCFadeTo *in = CCFadeTo::create(0.5f,255);
		lab->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(out,in)));
	}
}


void CCultivateLayer::timeDelay(float delt)
{	
	int minute = m_second/60;
	int second = m_second%60;
	m_time->setString(CCString::createWithFormat("0%d:%d%d",minute,second/10, second%10)->getCString());
	if (m_second==0)
	{
		this->unschedule(schedule_selector(CCultivateLayer::timeDelay));
		CCSprite *getRes =(CCSprite *)(m_ui->findWidgetById("getRes"));
		CCSprite *waitGet =(CCSprite *)(m_ui->findWidgetById("waitGet"));
		getRes->setVisible(true);
		waitGet->setVisible(false);
		m_time->setVisible(false);
		CButton *btn = (CButton*)m_ui->getChildByTag(5);
		btn->setEnabled(true);
	}
	m_second--;
}

void CCultivateLayer::waitTime()
{
	m_time =(CLabel *)(m_ui->findWidgetById("timeLab"));
	CCSprite *getRes =(CCSprite *)(m_ui->findWidgetById("getRes"));
	CCSprite *waitGet =(CCSprite *)(m_ui->findWidgetById("waitGet"));
	getRes->setVisible(false);
	waitGet->setVisible(true);
	m_time->setVisible(true);
	m_second = 300;
	this->schedule(schedule_selector(CCultivateLayer::timeDelay),1.0f,300,0.0f);
}

void CCultivateLayer::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->close("MaskLayerCult");
}


float CCultivateLayer::calculateCircleActionTime( int iHeroSize )
{
	switch (iHeroSize)
	{
	case 0:
		{
			return 4.0f;
		}
		break;
	case 1:
		{
			return 3.0f;
		}
		break;
	case 2:
		{
			return 2.0f;
		}
		break;
	case 3:
		{
			return 1.0f;
		}
		break;
	default:
		break;
	}
	return 0;
}


void CCultivateLayer::updateCircle( int iHeroSize )
{
	float fTimeCircle = calculateCircleActionTime(iHeroSize);
	CCNode* pCircle1 = (CCNode*)m_ui->findWidgetById("circle_1");
	pCircle1->stopAllActions();
	pCircle1->runAction(CCRepeatForever::create(CCRotateBy::create(fTimeCircle, 360)));
	CCNode* pCircle2 = (CCNode*)m_ui->findWidgetById("circle_2");
	pCircle2->stopAllActions();
	pCircle2->runAction(CCRepeatForever::create(CCRotateBy::create(fTimeCircle/2, 360)));

	//开启产金币效果
	schedule(schedule_selector(CCultivateLayer::updateToAddCoin), 3600/m_fSpeed);
	schedule(schedule_selector(CCultivateLayer::updateTime));
	schedule(schedule_selector(CCultivateLayer::updateLabel), 1.0f);
}


int CCultivateLayer::calculateMoneyHeapRank( int iProgress )
{
	return iProgress/30;
}

void CCultivateLayer::showMoneyHeap( int iRank )
{
	for(unsigned int i=0; i<3; i++)
	{
		CCNode* pNode = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("coin_%d", i+1)->getCString());
		if(iRank>=i+1)
		{
			pNode->setVisible(true);
		}
		else
		{
			pNode->setVisible(false);
		}
	}
}


void CCultivateLayer::updateGetMoneyHeap( const TMessage& tMsg )
{
	m_iCurrentGetMoney = tMsg.intParam;

	m_bAddMoneyScheduleLock = true;

	m_fCultivateTime = 0;

	showGetMoneyHeapEffect();
}

void CCultivateLayer::showGetMoneyHeapEffect()
{
	PlayEffectSound(SFX_415);

	//屏蔽层
	MaskLayer* lay = MaskLayer::create("HammerEffect");
	lay->setContentSize(CCSizeMake(1138,640));
	lay->setAnchorPoint(ccp(0.5f, 0.5f));
	lay->setPosition(VCENTER);
	lay->setOpacity(0);
	LayerManager::instance()->push(lay);

	//金钱堆动画
	if(m_iCurrentMoneyRank>0)
	{
		for(unsigned int i=0; i<m_iCurrentMoneyRank; i++)
		{
			CCNode* pNode = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("coin_%d", i+1)->getCString());
			if(pNode->isVisible())
			{
				pNode->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f*(m_iCurrentMoneyRank-1-i)), CCFadeOut::create(0.5f)));
			}
		}
	}
	

	//参考位置
	CCPoint pStartPos = ((CCNode*)(m_ui->findWidgetById("coin_1")))->getPosition();

	showGiftWave(CCString::createWithFormat("common/icon_11.png"), pStartPos, ccp(723, 610));

	int iGapIndex = m_iCurrentMoneyRank<1?1:m_iCurrentMoneyRank;
	lay->runAction(CCSequence::create(CCDelayTime::create(iGapIndex*0.5f+0.2f),CCCallFunc::create(this, callfunc_selector(CCultivateLayer::showGetMoneyHeapEffectCallBack)),nullptr));
}

void CCultivateLayer::showGetMoneyHeapEffectCallBack()
{
	LayerManager::instance()->pop();

	CCString *str = CCString::createWithFormat(GETLANGSTR(255), m_iCurrentGetMoney);
	ShowPopTextTip(str->getCString());
	waitTime();
	CProgressBar *prostro = (CProgressBar*)(m_ui->findWidgetById("ProgressSto"));
	prostro->startProgress(0, 0.3f);
	CLabel* pLabel = (CLabel*)m_ui->findWidgetById("storLab");
	CCString *labelStr = CCString::createWithFormat("0/%d", m_city.maxCap);
	pLabel->setString(labelStr->getCString());

	m_bAddMoneyScheduleLock = false;

	m_iNow = 0;
}

void CCultivateLayer::showGiftWave( CCString* pStr, CCPoint pBasePos, CCPoint pAimPos )
{
	int iWave = m_iCurrentMoneyRank*5;
	iWave = iWave<5?5:iWave;
	for(int i=0; i<iWave; i++){
		for(int j=0; j<4; j++){
			CCSprite* pSprite =  CCSprite::create(pStr->getCString());
			pSprite->setPosition(ccp(pBasePos.x+(CCRANDOM_0_1()-0.5f)*100, pBasePos.y-5+(CCRANDOM_0_1()-0.5f)*40));
			m_pCoinBatch->addChild(pSprite);
			pSprite->setScale(0.85f);
			ccBezierConfig bezierConfig;
			bezierConfig.controlPoint_1 = pBasePos+ccp(40, 70);
			bezierConfig.controlPoint_2 = pAimPos+ccp(40, -70);
			bezierConfig.endPosition = pAimPos;
			pSprite->runAction(CCSequence::create(
				CCHide::create(),
				CCDelayTime::create(i*0.1f+CCRANDOM_0_1()*0.05f),
				CCShow::create(),
				CCSpawn::createWithTwoActions(CCBezierTo::create(0.5f, bezierConfig), CCRotateBy::create(0.5f, 360)),
				CCScaleTo::create(0.15f, 1.1f),
				CCFadeOut::create(0.1f),
				CCRemoveSelf::create(),
				NULL));
		}
	}
}

void CCultivateLayer::updateTime( float dt )
{
	if(m_bAddMoneyScheduleLock) return;

	//如果已经开采满了，退出
	if(m_iNow>=m_iMax) return;

	m_fCultivateTime += dt;
}

void CCultivateLayer::updateLabel( float dt )
{
	if(m_bAddMoneyScheduleLock) return;

	//如果已经开采满了，退出
	if(m_iNow>=m_iMax) return;

	//进来界面后，开采了多少
	int iAdd = m_fCultivateTime*m_fSpeed/3600;
	iAdd += m_iNow;

	//开采量/总量
	CLabel *storge = (CLabel*)(m_ui->findWidgetById("storLab"));
	CCString *str = CCString::createWithFormat("%d/%d", iAdd, m_iMax);
	storge->setString(str->getCString());

	//开采进度条
	CProgressBar *proStorge = (CProgressBar*)(m_ui->findWidgetById("ProgressSto"));
	proStorge->startProgress(iAdd, 0.2f);

}

