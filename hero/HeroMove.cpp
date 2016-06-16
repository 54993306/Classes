#include "HeroMove.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"

bool CHeroMove::init()
{
	if (BaseLayer::init())
	{
		this->setIsShowBlack(false);
		m_ui = LoadComponent("HeroMove.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CHeroMove::onClickButton(CCObject* pSender)
{

}

void CHeroMove::showMoveDirection(CHero* hero)
{
	CCSprite *head1 = (CCSprite *)(m_ui->findWidgetById("hero1"));
	head1->removeChildByTag(1);
	CCSprite *her1 = CCSprite::create(CCString::createWithFormat("headImg/%d.png",hero->thumb)->getCString());
	if (!her1)
	{
		her1 = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CHeroMove::showMoveDirection Lost Image = %d",hero->thumb);
	}
	her1->setPosition(ccp(head1->getContentSize().width/2,head1->getContentSize().height/2));
	her1->setScale(0.9f);
	her1->setTag(1);
	head1->addChild(her1);

	CCSprite *head2 = (CCSprite *)(m_ui->findWidgetById("hero2"));
	head2->removeChildByTag(1);
	CCSprite *her2 = CCSprite::create(CCString::createWithFormat("headImg/%d.png",hero->thumb)->getCString());
	if (!her2)
	{
		her2 = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CHeroMove::showMoveDirection Lost Image = %d",hero->thumb);
	}
	her2->setPosition(ccp(head2->getContentSize().width/2,head2->getContentSize().height/2));
	her2->setScale(0.9f);
	her2->setTag(1);
	head2->addChild(her2);

	m_hero = hero;

	CLayout *upLay = (CLayout*)(m_ui->findWidgetById("upMove"));
    CCArray *childArray = upLay->getChildren();
	for (int i = 0; i < upLay->getChildrenCount(); i++)
	{
		CCNode *child = (CCNode*)(childArray->objectAtIndex(i));
		child->setVisible(false);
	}

	//displayMove(upLay, hero->move1, hero->move2, hero->move3, hero->move4, hero->move5);

	CLayout *downLay = (CLayout*)(m_ui->findWidgetById("downMove"));
	childArray = downLay->getChildren();
	for (int i = 0; i < downLay->getChildrenCount(); i++)
	{
		CCNode *child = (CCNode*)(childArray->objectAtIndex(i));
		child->setVisible(false);
	}

	if (m_hero->quality<m_hero->maxQuality)
	{
		//displayMove(downLay, hero->nMove1, hero->nMove2, hero->nMove3, hero->nMove4, hero->nMove5);
	}
	else
	{
		downLay->setVisible(false);
	}
} 

void CHeroMove::onEnter()
{
	BaseLayer::onEnter();
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO,this,GameMsghandler_selector(CHeroMove::updateHeroInfo));
}

void CHeroMove::updateHeroInfo(const TMessage& tMsg)
{
	CHero *hero = (CHero*)tMsg.lParam;
	showMoveDirection(hero);
}

void CHeroMove::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
}

void CHeroMove::displayMove(CLayout * upLay, int move1,int move2, int move3, int move4, int move5)
{
	for (int i = 0; i < move1&&i<2; i++)
	{
		CCSprite *spr = (CCSprite*)(upLay->getChildByTag(10+i+1));
		if (spr)spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag((10+i+1)*10));
		spr->setVisible(true);
	}

	for (int i = 0; i < move2&&i<2; i++)
	{
		CCSprite *spr = (CCSprite*)(upLay->getChildByTag(20+2*i+1));
		if (spr)spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag((20+2*i+1)*10));
		spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag(20+2*i+2));
		spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag((20+2*i+2)*10));
		spr->setVisible(true);
	}

	for (int i = 0; i < move3&&i<2; i++)
	{
		CCSprite *spr = (CCSprite*)(upLay->getChildByTag(30+2*i+1));
		if (spr)spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag((30+2*i+1)*10));
		spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag(30+2*i+2));
		if (spr)spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag((30+2*i+2)*10));
		spr->setVisible(true);
	}

	for (int i = 0; i < move4&&i<2; i++)
	{
		CCSprite *spr = (CCSprite*)(upLay->getChildByTag(40+i+1));
		if (spr)spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag((40+i+1)*10));
		spr->setVisible(true);
	}

	for (int i = 0; i <move5&&i<2; i++)
	{
		CCSprite *spr = (CCSprite*)(upLay->getChildByTag(50+2*i+1));
		if (spr)spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag((50+2*i+1)*10));
		spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag(50+2*i+2));
		if (spr)spr->setVisible(true);

		spr = (CCSprite*)(upLay->getChildByTag((50+2*i+2)*10));
		spr->setVisible(true);
	}
}
