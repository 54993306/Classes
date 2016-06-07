#include "HeroBreak.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"
#include "scene/AnimationManager.h"
#include "common/CommonFunction.h"

bool CHeroBreak::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("HeroBreak");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("HeroBreak.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}
	return false;
} 
using namespace BattleSpace;

void CHeroBreak::levelSkillUp(CHero* hero,const Hero* pHero)
{
	m_hero = hero;
	m_reshero.readData(*pHero);
	CLabel *hp = (CLabel*)(m_ui->findWidgetById("hp"));
	hp->setString(ToString(hero->hp));
	CLabel *attack = (CLabel*)(m_ui->findWidgetById("atk"));
	attack->setString(ToString(hero->atk));

	CLabel *addhp = (CLabel*)(m_ui->findWidgetById("addhp"));
	CLabel *addatk = (CLabel*)(m_ui->findWidgetById("addatk"));
	addhp->setString(ToString(pHero->hp()));
	addatk->setString(ToString(pHero->atk()));

	CProgressBar *progress = (CProgressBar *)(m_ui->findWidgetById("promote"));

	CCAnimation *garrowAnim = AnimationManager::sharedAction()->getAnimation("1005");
	CCPoint pos = ccp(progress->getPositionX()+progress->getContentSize().width/2,progress->getPositionY());
	m_garrowSpr = createAnimationSprite("skill/1005.png",pos,garrowAnim,true);
	m_ui->addChild(m_garrowSpr);

	CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("1011");
	pos = ccp(progress->getPositionX()+progress->getContentSize().width/2+5,progress->getPositionY()+12);
	m_lightSpr = createAnimationSprite("skill/1011.png",pos,lightAnim,false);
	m_lightSpr->setAnchorPoint(ccp(0.5,1));
	m_ui->addChild(m_lightSpr);

	progress->startProgressFromTo(100,0,2.0f);
	progress->setOnValueChangedListener(this,ccw_valuechanged_selector(CHeroBreak::progressChange));

	m_ui->removeChildByTag(100);
	CCSprite *di = (CCSprite*)(m_ui->findWidgetById("bg"));
	CCSprite *spr = CCSprite::create(CCString::createWithFormat("selectRole/%d.png",hero->thumb)->getCString());
	if (spr)
	{	
		spr->setScale(200.0f/spr->getContentSize().height);
		spr->setTag(100);
		CCPoint pos = ccpAdd(di->getPosition(),ccp(0,50));
		spr->setPosition(pos);
		m_ui->addChild(spr);
	}
	CCSprite *bg = (CCSprite*)m_ui->findWidgetById("bg");
	for (int i = 1; i <= 5; i++)
	{
		CCNode *star = (CCNode*)(m_ui->findWidgetById(CCString::createWithFormat("star%d",i)->getCString()));
		if (i<=hero->star)
		{
			star->setVisible(true);
		}
		else
		{
			star->setVisible(false);
		}
	}

	this->scheduleOnce(schedule_selector(CHeroBreak::runFlickerAction),2.5f);

	CCAnimation *kuangAnim = AnimationManager::sharedAction()->getAnimation("1012");
	kuangAnim->setDelayPerUnit(0.1f);
	CCSprite *kuangSpr = CCSprite::create("skill/1012.png");// createAnimationSprite("skill/1012.png",bg->getPosition(),kuangAnim,false);
	kuangSpr->setPosition(ccpAdd(bg->getPosition(),ccp(0,18)));
	kuangSpr->runAction(CCSequence::createWithTwoActions(CCRepeat::create(CCAnimate::create(kuangAnim),4),CCCallFuncN::create(this,callfuncN_selector(CHeroBreak::hideAnim))));
	m_ui->addChild(kuangSpr);
	
// 	if (pHero->star()>=2)
// 	{
// 		int animId = 1000+pHero->star()-2;
// 		CCSprite *bg = (CCSprite*)m_ui->findWidgetById("bg");
// 		CCSprite *starSpr = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%d_10000.png",animId)->getCString());
// 		starSpr->setPosition(ccpAdd(bg->getPosition(),ccp(-12,-130)));
// 		starSpr->setVisible(false);
// 		starSpr->setTag(50);
// 		m_ui->addChild(starSpr);
// 		this->scheduleOnce(schedule_selector(CHeroBreak::runStarAnim),4.0f);
// 	}
} 

void CHeroBreak::runStarAnim(float dt)
{
	CCSprite *star =(CCSprite*)(m_ui->getChildByTag(50));
	star->setVisible(true);
	CCAnimation *starAnim = AnimationManager::sharedAction()->getAnimation(ToString(1000+m_reshero.star-2));
	star->runAction(CCSequence::create(CCAnimate::create(starAnim),CCCallFuncN::create(this,callfuncN_selector(CHeroBreak::showIncrease)),nullptr));
}


void CHeroBreak::progressChange(CCObject* pSender, int nValue)
{
	if (/*nValue==95||*/nValue==80||nValue==60||nValue==40||nValue==20)
	{
		CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("1011");
		m_lightSpr->runAction(CCAnimate::create(lightAnim));

	}
	CProgressBar *progress = (CProgressBar *)pSender;
	if (m_lightSpr)
	{
		m_lightSpr->setPositionX(progress->getPositionX()-progress->getContentSize().width*(0.5-(float)nValue/100)+5);
		m_garrowSpr->setPositionX(progress->getPositionX()-progress->getContentSize().width*(0.5-(float)nValue/100));
	}
	if (nValue==0)
	{
		m_lightSpr->removeFromParent();
		m_garrowSpr->removeFromParent();
		m_lightSpr=nullptr;
		m_garrowSpr = nullptr;
		progress->setOnValueChangedListener(nullptr,nullptr);
	}
}

void CHeroBreak::onEnter()
{
	BaseLayer::onEnter();
	CProgressBar *progress = (CProgressBar *)(m_ui->findWidgetById("promote"));
// 	CCAnimation *garrowAnim = AnimationManager::sharedAction()->getAnimation("1005");
// 	CCPoint pos = ccp(progress->getPositionX()+progress->getContentSize().width/2,progress->getPositionY());
// 	m_garrowSpr = createAnimationSprite("skill/1005.png",pos,garrowAnim,true);
// 	m_ui->addChild(m_garrowSpr);
// 
// 	CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("1011");
// 	pos = ccp(progress->getPositionX()+progress->getContentSize().width/2+5,progress->getPositionY()+12);
// 	m_lightSpr = createAnimationSprite("skill/1011.png",pos,lightAnim,false);
// 	m_lightSpr->setAnchorPoint(ccp(0.5,1));
// 	m_ui->addChild(m_lightSpr);
// 
// 	progress->startProgressFromTo(100,0,1.5f);
// 	progress->setOnValueChangedListener(this,ccw_valuechanged_selector(CHeroBreak::progressChange));
// 
// 	this->scheduleOnce(schedule_selector(CHeroBreak::runFlickerAction),2.5f);
// 
// 	CCSprite *di = (CCSprite*)(m_ui->findWidgetById("bg"));
// 	CCSprite *spr = CCSprite::create(CCString::createWithFormat("selectRole/%d.png",541)->getCString());
// 	if (spr)
// 	{	
// 		spr->setScale(200.0f/spr->getContentSize().height);
// 		spr->setTag(100);
// 		CCPoint pos = ccpAdd(di->getPosition(),ccp(0,50));
// 		spr->setPosition(pos);
// 		m_ui->addChild(spr);
// 	}
// 
// 	CCSprite *bg = (CCSprite*)m_ui->findWidgetById("bg");
// 	CCAnimation *kuangAnim = AnimationManager::sharedAction()->getAnimation("1012");
// 	kuangAnim->setDelayPerUnit(0.2f);
// 	CCSprite *kuangSpr = CCSprite::create("skill/1012.png");// createAnimationSprite("skill/1012.png",bg->getPosition(),kuangAnim,false);
// 	kuangSpr->setPosition(ccpAdd(bg->getPosition(),ccp(0,25)));
// 	kuangSpr->runAction(CCSequence::createWithTwoActions(CCRepeat::create(CCAnimate::create(kuangAnim),4),CCCallFuncN::create(this,callfuncN_selector(CHeroBreak::hideAnim))));
// 	m_ui->addChild(kuangSpr);
// 
// 	int animId = 1003;
// 	CCAnimation *starAnim = AnimationManager::sharedAction()->getAnimation(ToString(animId));
// 	CCSprite *starSpr = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%d_10000.png",animId)->getCString());
// 	starSpr->setPosition(ccpAdd(bg->getPosition(),ccp(0,-130)));
// 	starSpr->runAction(CCSequence::create(CCDelayTime::create(4.0f),CCAnimate::create(starAnim),CCCallFuncN::create(this,callfuncN_selector(CHeroBreak::showIncrease)),nullptr));
// 	m_ui->addChild(starSpr);
}

void CHeroBreak::hideflicker(CCNode* node)
{
	if (m_reshero.id>0)
	{
		CCSprite *hero = (CCSprite*)(m_ui->getChildByTag(100));
		hero->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("selectRole/%d.png",m_reshero.thumb)->getCString()));
	}
	node->removeFromParent();
	CCSprite *chain = (CCSprite*)(m_ui->findWidgetById("chain"));
	//chain->runAction(CCShake::create(0.2f,20.0f));
	runEffect_ShakeY(chain,15.0f,1.2f,6);
	//runEffect_ShakeX(chain,20.0f,1.0f,3);
}

void CHeroBreak::showIncrease(CCNode* node)
{
	CLayout *layout = (CLayout*)(m_ui->findWidgetById("increase"));
	layout->setVisible(true);
	layout->setOpacity(150);
	layout->runAction(CCFadeTo::create(0.5f,255));
}

void CHeroBreak::hideAnim(CCNode* node) 
{
	node->removeFromParent();
}

void CHeroBreak::runFlickerAction(float dt)
{
	CCSprite *bg = (CCSprite*)m_ui->findWidgetById("bg");
	CCAnimation *flickAnim = AnimationManager::sharedAction()->getAnimation("1013");
	CCSprite *flickSpr = CCSprite::createWithSpriteFrameName("1013_10000.png");//,bg->getPosition(),flickAnim,false);
	flickSpr->setPosition(bg->getPosition());
	flickSpr->runAction(CCSequence::create(CCAnimate::create(flickAnim),CCCallFuncN::create(this,callfuncN_selector(CHeroBreak::hideflicker)),nullptr));
	m_ui->addChild(flickSpr);
	for (int i = 1; i <= 5; i++)
	{
		CCNode *star = (CCNode*)(m_ui->findWidgetById(CCString::createWithFormat("star%d",i)->getCString()));
		star->setVisible(false);
	}
}

void CHeroBreak::onExit()
{
	BaseLayer::onExit();
}

bool CHeroBreak::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");
	CCSprite *bgSpr1 = (CCSprite*)m_ui->findWidgetById("bg1");

	if( !res && !bgSpr->boundingBox().containsPoint(pTouch->getLocation())&&!bgSpr1->boundingBox().containsPoint(pTouch->getLocation()))
	{
		res = true;
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
	}
	return res;
}
