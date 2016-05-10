#include "ReloadResCoverLayer.h"

ReloadResCoverLayer::ReloadResCoverLayer()
{

}

ReloadResCoverLayer::~ReloadResCoverLayer()
{

}

bool ReloadResCoverLayer::init()
{
	BaseLayer::init();

	CCTextureCache::sharedTextureCache()->removeTextureForKey("loginScene/sogreat.png");
	CCSprite* pSprite = CCSprite::create("loginScene/sogreat.png");
	pSprite->setPosition(VCENTER);
	pSprite->setScale(2.0f);
	pSprite->setColor(ccc3(255, 255, 255));
	this->addChild(pSprite);

	this->setCascadeOpacityEnabled(true);
	this->setOpacity(0);
	this->runAction(CCSequence::create(
		CCFadeIn::create(0.2f),
		CCCallFunc::create(this, callfunc_selector(ReloadResCoverLayer::callBackForReloadRes)),
		CCFadeOut::create(0.2f),
		CCRemoveSelf::create(),
		nullptr));

	this->setVisible(true);

	return true;
}

void ReloadResCoverLayer::onEnter()
{
	BaseLayer::onEnter();
	CCLOG("ReloadResCoverLayer::onEnter()");
}

void ReloadResCoverLayer::onExit()
{
	BaseLayer::onExit();
	CCLOG("ReloadResCoverLayer::onExit()");
}

void ReloadResCoverLayer::CoverWhenReloadRes()
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(INT_MAX);
	ReloadResCoverLayer* pReloadRes = ReloadResCoverLayer::create();
	CCDirector::sharedDirector()->getRunningScene()->addChild(pReloadRes, INT_MAX, INT_MAX);
}

void ReloadResCoverLayer::callBackForReloadRes()
{
	CCTextureCache::reloadAllTextures();
}