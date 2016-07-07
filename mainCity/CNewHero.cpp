#include "CNewHero.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "model/CWholeBodyShowData.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "Battle/AnimationManager.h"
#include "SDK/FaceBookSDK.h"
#include "RecruitResult.h"
#include "guide/GuideManager.h"
CNewHero::CNewHero():m_ui(nullptr),m_pEffect(nullptr),m_bEvolve(false),m_bShowing(false),m_isFinish(false),m_isShare(false),m_pTarget(nullptr),m_pNewHeroEffectCallback(nullptr)
{

}
using namespace BattleSpace;
bool CNewHero::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("NewHeroShowMask");
		pMaskLayer->setContentSize(winSize);
		this->addChild(pMaskLayer);

		//内容
		m_ui = LoadComponent("NewHeroShow.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		//加特效
		CCAnimation *batAnim = AnimationManager::sharedAction()->getAnimation("7035");
		batAnim->setDelayPerUnit(0.05f);
		m_pEffect = createAnimationSprite("skill/9017.png", ccp(1138/2, 605), batAnim, true);
		m_ui->addChild(m_pEffect, 99);
		m_pEffect->setVisible(false);

		this->setVisible(false);

		setTouchPriority(-18);

		return true;
	}

	return false;
}


void CNewHero::onEnter()
{
	BaseLayer::onEnter();

	//背景处理
	CCSprite* pBg = (CCSprite*)m_ui->findWidgetById("bg");
	pBg->setAnchorPoint(ccp(0.5f, 0.5f));
	//pBg->setPosition(pBg->getPosition()+ccp(pBg->getContentSize().width/2, pBg->getContentSize().height/2));

	//圆圈处理
	CCSprite* pCircle = (CCSprite*)m_ui->findWidgetById("light");
	pCircle->setAnchorPoint(ccp(0.5f, 0.5f));
	//pCircle->setPosition(pCircle->getPosition()+ccp(pCircle->getContentSize().width/2, pCircle->getContentSize().height/2));

	CButton* fbBtn = (CButton*)m_ui->findWidgetById("fbBtn");
	fbBtn->setOnClickListener(this,ccw_click_selector(CNewHero::shareFb));
	fbBtn->setVisible(false);
}

void CNewHero::shareFb(CCObject* pObj )
{
	if (m_bEvolve)
	{
		const ShareData *data = DataCenter::sharedData()->getShareData()->getCfg(m_pHero.quality+4);
		const HeroInfoData *heroInfo = DataCenter::sharedData()->getHeroInfo()->getCfg(m_pHero.thumb);
		CCString *strDesc = CCString::createWithFormat(data->desc.c_str(),heroInfo->heroName.c_str());
		CCString *strUrl = CCString::createWithFormat(data->url.c_str(),m_pHero.thumb);
		FaceBookSDK::sharedInstance()->onShareToFb(strDesc->getCString(),strUrl->getCString());
	}
	else
	{
		const ShareData *data = DataCenter::sharedData()->getShareData()->getCfg(m_pHero.quality);
		const HeroInfoData *heroInfo = DataCenter::sharedData()->getHeroInfo()->getCfg(m_pHero.thumb);
		CCString *strDesc = CCString::createWithFormat(data->desc.c_str(),heroInfo->heroName.c_str());
		CCString *strUrl = CCString::createWithFormat(data->url.c_str(),m_pHero.thumb);
		FaceBookSDK::sharedInstance()->onShareToFb(strDesc->getCString(),strUrl->getCString());
	}
}

void CNewHero::showNewHeroEffect( HeroLotteryData* pHero )
{
	m_pHero = *pHero;
	m_isFinish = false;
	if(m_bShowing)
	{
		//停掉上一次的动画,手动执行回调
		this->stopAllActions();
		showEffectCallBack();
	}
	
	m_bShowing = true;
	setVisible(true);
	NOTIFICATION->postNotification(HIDE_TOP_LAYER);
	

	//播放声音
	PlayEffectSound(SFX_432);

	float fTime = 0.3f;

	//背景拉宽+透明渐变255-116
	CCSprite* pBg = (CCSprite*)m_ui->findWidgetById("bg");
	pBg->setScale(1.0f);
	pBg->setOpacity(255);
	pBg->stopAllActions();
	pBg->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(fTime, 1.35f, 1), CCFadeTo::create(0, 116)));

	//圆圈放大+透明渐变0-255
	CCSprite* pCircle = (CCSprite*)m_ui->findWidgetById("light");
	pCircle->setScale(0.0f);
	pCircle->setOpacity(0);
	pCircle->stopAllActions();
	pCircle->runAction(CCSpawn::createWithTwoActions(CCScaleTo::create(fTime, 1.4f), CCFadeIn::create(fTime)));
	pCircle->runAction(CCRepeat::create(CCRotateBy::create(1.0,360), 1000));

	//隐藏其他
	hideOrShow(false);

	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fTime), CCCallFunc::create(this, callfunc_selector(CNewHero::callBackForEffectShow))));
}


void CNewHero::callBackForEffectShow()
{
	//显示其他
	hideOrShow(true);

	//符文
	CCNode *fu1 = (CCNode*)(m_ui->findWidgetById("fu1"));
	fu1->setScale(0.65f);
	fu1->runAction(CCRepeat::create(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.8f,ccp(0,120)),
		CCFadeTo::create(0.8f,30)),CCCallFuncN::create(this,callfuncN_selector(CNewHero::recoverPosition))),3));

	CCNode *fu2 = (CCNode*)(m_ui->findWidgetById("fu2"));
	fu2->setScale(0.65f);
	fu2->runAction(CCRepeat::create(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.8f,ccp(0,120)),
		CCFadeTo::create(0.8f,30)),CCCallFuncN::create(this,callfuncN_selector(CNewHero::recoverPosition))),3));

	CCNode *fu3 = (CCNode*)(m_ui->findWidgetById("fu3"));
	fu3->setScale(0.65f);
	fu3->runAction(CCRepeat::create(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.8f,ccp(0,120)),
		CCFadeTo::create(0.8f,30)),CCCallFuncN::create(this,callfuncN_selector(CNewHero::recoverPosition))),3));

	//增加人物贴图
	const CWholeBodyShowInfo& pInfo = CWholeBodyShowData::getInstance()->getDataById(m_pHero.thumb);

	//换骨骼
	//CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(CCString::createWithFormat("BoneAnimation/%d.ExportJson", pHero->thumb)->getCString());
	CCSprite* pNodePos = (CCSprite*)m_ui->findWidgetById("body_pos");
	pNodePos->setVisible(true);
	pNodePos->setOpacity(0);
	pNodePos->removeChildByTag(1000);

	//CCArmature* pArmatue = CCArmature::create(CCString::createWithFormat("%d", pHero->thumb)->getCString());
	//pArmatue->setPosition(pInfo.pos);
	//pArmatue->setAnchorPoint(ccp(0.5f, 0.5f));
	//pNodePos->addChild(pArmatue, 1000, 1000);
	//pArmatue->getAnimation()->playByIndex(0);

	CCPoint pPos = pNodePos->getPosition();
	CCSprite* pBody = CCSprite::create(CCString::createWithFormat("selectRole/%d.png", m_pHero.thumb)->getCString());
	if (!pBody)
	{	
		CCLOG("ERROR CNewHero::showNewHeroEffect");
		pBody = CCSprite::create("selectRole/331.png");
	}
	pBody->setScale(pInfo.fScale);
	pBody->setFlipX(pInfo.bFlipX);
	pBody->setPosition(pInfo.pos);
	pBody->setRotation(pInfo.iRoate);
	pNodePos->addChild(pBody, 1000, 1000);


	const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(m_pHero.thumb);

	//名字
	CLabel *name = (CLabel *)(m_ui->findWidgetById("name"));
	if(data)
	{
		name->setString(data->heroName.c_str());
	}
	name->setVisible(true);

	//名字底
	CCSprite *di =(CCSprite *)(m_ui->findWidgetById("di"));
	di->setVisible(true);

	//品质名称
	CCSprite *level1 = (CCSprite *)(m_ui->findWidgetById("level1"));
	level1->setAnchorPoint(ccp(0.5,0.5));
	level1->setVisible(true);
	CCTexture2D* pTexture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("recruit/star_%d.png",m_pHero.iColor)->getCString());
	if(pTexture)
	{
		level1->setTexture(pTexture);
		level1->setTextureRect(CCRect(0,0,49*m_pHero.iColor,45));
	}
	
	CCPoint pos = level1->getPosition();
	level1->setPosition(ccpAdd(pos,ccp(0,-180)));
	CCMoveTo *move = CCMoveTo::create(0.1f,pos);
	level1->runAction(CCEaseBackIn::create(move));

	if(m_bEvolve)
	{
		showEvolveEffect();
		m_bEvolve = false;
	}

	// 	CCSprite *level2 = (CCSprite *)(m_ui->findWidgetById("level2"));
	// 	level2->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("recruit/8004_%d.png",10001+2*(pHero->quality-1))->getCString()));
	// 	pos = level2->getPosition();
	// 	level2->setPosition(ccpAdd(pos,ccp(200,-180)));	
	// 	level2->setVisible(true);
	// 	move = CCMoveTo::create(0.1f,pos);
	// 	level2->runAction(CCEaseBackIn::create(move));

	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.5f), CCCallFunc::create(this, callfunc_selector(CNewHero::effectFinish))));
}


void CNewHero::recoverPosition( CCNode *pNode )
{
	pNode->setPositionY(pNode->getPositionY()-120);
	((CCSprite*)pNode)->setOpacity(255);
}

void CNewHero::showEffectCallBack()
{
	m_bShowing = false;
	setVisible(false);
	NOTIFICATION->postNotification(SHOW_TOP_LAYER);

	checkCallBackBind();
}

void CNewHero::showEvolveEffect()
{
	//品质名称
	int iLowQuality = m_pHero.quality-1;
	CCSprite *level1 = (CCSprite *)(m_ui->findWidgetById("level1"));
	level1->stopAllActions();

	level1->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("recruit/star_%d.png", iLowQuality)->getCString()));
	level1->setTextureRect(CCRect(0, 0, 49*iLowQuality, 45));


	//添加额外星星
	CCSprite* pExtraStar = CCSprite::create("recruit/star_1.png");
	pExtraStar->setPosition(ccp(level1->getContentSize().width+pExtraStar->getContentSize().width/2, level1->getContentSize().height/2));
	level1->addChild(pExtraStar);
	level1->setPositionX(level1->getPositionX()-pExtraStar->getContentSize().width/2);

	CCMoveTo *move = CCMoveTo::create(0.1f, level1->getPosition()+ccp(0, 180));
	level1->runAction(CCEaseBackIn::create(move));

	//额外星星动作
	pExtraStar->setVisible(false);
	pExtraStar->runAction(CCSequence::create(CCDelayTime::create(0.9f), CCShow::create(), CCScaleTo::create(0, 5.0f), CCEaseBackOut::create(CCScaleTo::create(0.35f, 1.0f)), nullptr));

	PlayEffectSound(SFX_425);
}

bool CNewHero::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	bool res = isVisible();
	CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	return res;
}

void CNewHero::hideOrShow( bool bShow )
{
	//其他先隐藏
	CCObject* pObj;
	CCArray* pChildren = m_ui->getChildren();
	CCARRAY_FOREACH(pChildren, pObj)
	{
		CWidget* pWidget = dynamic_cast<CWidget*>(pObj);
		if(pWidget)
		{
			CCNode* pNode = dynamic_cast<CCNode*>(pObj);
			if(pNode)
			{
				if(strcmp(pWidget->getId(), "bg")==0 || strcmp(pWidget->getId(), "light")==0)
				{
					pNode->setVisible(true);
				}
				else
				{
					pNode->setVisible(bShow);
				}
			}
		}
	}

	//特效
	m_pEffect->setVisible(bShow);
}

void CNewHero::onExit()
{
	BaseLayer::onExit();

	if(m_bShowing)
	{
		//停掉上一次的动画,手动执行回调
		this->stopAllActions();
		showEffectCallBack();
	}
}


void CNewHero::effectFinish()
{
	m_isFinish = true;
	if (CGuideManager::getInstance()->getIsRunGuide())
	{
		showEffectCallBack();
	}
}

void CNewHero::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
	CWidgetWindow::ccTouchEnded(pTouch, pEvent);
	CButton* fbBtn = (CButton*)m_ui->findWidgetById("fbBtn");
	bool isSel = fbBtn->boundingBox().containsPoint(pTouch->getLocation());
	if (m_isFinish&&!isSel)
	{
		showEffectCallBack();
		RecruitResult *recruitRes = (RecruitResult*)LayerManager::instance()->getLayer("RecruitResult");
		if (recruitRes)
		{
// 			recruitRes->continueMoveLight(1.0f);
			recruitRes->setHeroViewVis(true);
			recruitRes->heroInfoAnim();
		}
	}
}

void CNewHero::setIsShare(bool isShare)
{
	m_isShare = isShare;
	CButton* fbBtn = (CButton*)m_ui->findWidgetById("fbBtn");
    fbBtn->setVisible(isShare);
	if (CGuideManager::getInstance()->getIsRunGuide())
	{
		fbBtn->setVisible(false);
	}
}

void CNewHero::checkCallBackBind()
{
	if(m_pTarget!=nullptr && m_pNewHeroEffectCallback!=nullptr)
	{
		(m_pTarget->*m_pNewHeroEffectCallback)();
		m_pTarget = nullptr;
		m_pNewHeroEffectCallback = nullptr;
	}
}

void CNewHero::bindCallback( CCObject* pTarget, newHeroEffectCallBack pFunCall )
{
	m_pTarget = pTarget;
	m_pNewHeroEffectCallback = pFunCall;
}
