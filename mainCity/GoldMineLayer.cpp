#include "GoldMineLayer.h"
#include "DefendWuJiang.h"
#include "CityData.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "MainCityControl.h"

#include "mainCity/QuickProduct.h"
#include "common/color.h"

bool CGlodMineLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("MaskLayerGold");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		
		m_ui = LoadComponent("jinkuang.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CGlodMineLayer::onEnter()
{ 
	BaseLayer::onEnter();
	for (int i=1; i<=6; ++i)
	{
		CButton *btn = dynamic_cast<CButton*>(m_ui->getChildByTag(i));
		btn->setOnClickListener(this,ccw_click_selector(CGlodMineLayer::onClickButton));
	}

	m_ui->setScale(0.0f);

	CCScaleTo *big = CCScaleTo::create(0.2f,1.0f);
// 	CCScaleTo *smal= CCScaleTo::create(0.0f,0.0f);
// 	CCSequence *seque = CCSequence::createWithTwoActions(smal,big);
	m_ui->runAction(big);

	GetTcpNet->registerMsgHandler(BuildInfo, this, CMsgHandler_selector(CGlodMineLayer::recBuildInfo));
	CSceneManager::sharedSceneManager()->addMsgObserver("updateHero",this, GameMsghandler_selector(CGlodMineLayer::updateHero));
}

void CGlodMineLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateHero",this);
	BaseLayer::onExit();
}

void CGlodMineLayer::updateHero(const TMessage& tMsg)
{
	//清空
	if (tMsg.nMsg ==0)
	{
		CButton *btn = (CButton*)(m_ui->getChildByTag(m_selectHero));
		btn->removeChildByTag(1);
		CHero hero;
		hero.id= 0;
// 		memset((void*)&hero,0,sizeof(CHero));
		m_city.heroList[m_selectHero-1] = hero;
// 
		CCNode *lab = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("lab%d",m_selectHero)->getCString());
		lab->setVisible(true);

	}
	else
	{
		CButton *btn = (CButton*)(m_ui->getChildByTag(m_selectHero));
		CHero *hero = (CHero*)tMsg.lParam;
		string icon = ToString(hero->thumb);

		btn->removeChildByTag(1);
		CCNode *lab = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("lab%d",m_selectHero)->getCString());
		lab->setVisible(false);

		CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png",hero->thumb)->getCString());
		if (!head)
		{
			head = CCSprite::create("headImg/101.png");
			CCLOG("[ ERROR ] CGlodMineLayer::updateHero Lost Image = %d",hero->thumb);
		}
		head->setPosition(ccp(btn->getContentSize().width/2, btn->getContentSize().height/2));
		head->setTag(1);
		head->setScale(82.0f/100.0f);
		btn->addChild(head);

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

	CLabel *extx = (CLabel*)(m_ui->findWidgetById("ExtXl"));
	extx->setString(ToString(tMsg.intParam));

	CMainCityControl::getInstance()->updateCityHero(m_city.cityId,m_city);
}

void CGlodMineLayer::onClickButton(CCObject* pSender)
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
			if (heroList.size()>=btn->getTag())
			{
				CHero hero = heroList[btn->getTag()-1];
				CDefendWuJiang *layer =CDefendWuJiang::create();
				layer->setCityId(m_city.id,m_city.cityId);
				layer->setReplaceId(hero.id);
				LayerManager::instance()->push(layer);
				CPlayerControl::getInstance().sendHeroList(3);
			}
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
	//关闭
	case 4:
		{
			LayerManager::instance()->pop();
			LayerManager::instance()->close("MaskLayerGold");
		}
		break;
	//获取资源
	case 5:
		{
			CMainCityControl::getInstance()->sendGetCityResource(m_city.id);
// 			LayerManager::instance()->pop();
// 			LayerManager::instance()->close("MaskLayer");
			btn->setEnabled(false);
			waitTime();

			CProgressBar *proSto = (CProgressBar*)(m_ui->findWidgetById("ProgressSto"));
			proSto->startProgress(0,0.3f);
		}
		break;

    //快速生产
	case 6:
		{
			CQuickProduct *quick = CQuickProduct::create(this);
			this->addChild(quick);
		}
		break;
	default:
		break;
	}
}


void CGlodMineLayer::showBuildInfo(const CCity& ct)
{
	CLabelAtlas *level = (CLabelAtlas*)(m_ui->findWidgetById("level"));
	level->setString(ToString(ct.level));

	CLabel *storge = (CLabel*)(m_ui->findWidgetById("storLab"));
	CCString *str = CCString::createWithFormat("%d/%d",ct.cap, ct.maxCap);
	storge->setString(str->getCString());

	CLabel *basex = (CLabel*)(m_ui->findWidgetById("baseXl"));
	basex->setString(ToString(ct.basePE));

	CLabel *extx = (CLabel*)(m_ui->findWidgetById("ExtXl"));
	extx->setString(ToString(ct.extPE));


	CProgressBar *proStorge = (CProgressBar*)(m_ui->findWidgetById("ProgressSto"));
	proStorge->setMinValue(0);
	proStorge->setMaxValue(ct.maxCap);
	proStorge->startProgress(ct.cap,0.2f);
    int i=0;
	for (; i<ct.heroList.size()&&i<3; i++)
	{
		CButton *btn = (CButton*)(m_ui->getChildByTag(i+1));
		const CHero &hero = ct.heroList[i];
		if (hero.thumb!=0)
		{
			CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png",hero.thumb)->getCString());
			if (!head)
			{
				head = CCSprite::create("headImg/101.png");
				CCLOG("[ ERROR ] CGlodMineLayer::showBuildInfo Lost Image = %d",hero.thumb);
			}
			head->setPosition(ccp(btn->getContentSize().width/2, btn->getContentSize().height/2));
			head->setTag(1);		
			head->setScale(82.0f/100.0f);
			btn->addChild(head);

			if (!hero.remove)
			{
				btn->setEnabled(false);
				head->setColor(RGB_GRAY_COLOR);
			}
		}
		CCNode *lab = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("lab%d",i+1)->getCString());
		lab->setVisible(false);
	}

	for (i=ct.heroNum;i<3;i++)
	{
		CButton *btn = (CButton*)(m_ui->getChildByTag(i+1));
		btn->setEnabled(false);
		CCNode *lab = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("lab%d",i+1)->getCString());
		lab->setVisible(false);
	}

	runFadeAction(ct);
}

void CGlodMineLayer::recBuildInfo(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	City* ct = (City*)msg;
	m_city.read(*ct);
	showBuildInfo(m_city);
}

void CGlodMineLayer::runFadeAction(const CCity &ct)
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

void CGlodMineLayer::timeDelay(float delt)
{	
	int minute = m_second/60;
	int second = m_second%60;
	m_time->setString(CCString::createWithFormat("0%d:%d",minute,second)->getCString());
	if (m_second==0)
	{
		this->unschedule(schedule_selector(CGlodMineLayer::timeDelay));
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

void CGlodMineLayer::waitTime()
{
	m_time =(CLabel *)(m_ui->findWidgetById("timeLab"));
	CCSprite *getRes =(CCSprite *)(m_ui->findWidgetById("getRes"));
	CCSprite *waitGet =(CCSprite *)(m_ui->findWidgetById("waitGet"));
	getRes->setVisible(false);
	waitGet->setVisible(true);
	m_time->setVisible(true);
	m_second = 300;
	this->schedule(schedule_selector(CGlodMineLayer::timeDelay),1.0f,300,0.0f);
}

