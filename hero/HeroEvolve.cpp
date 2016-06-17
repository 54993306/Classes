#include "HeroEvolve.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "Battle/ActionNameDefine.h"
#include "tools/UICloneMgr.h"
#include "netcontrol/CPlayerControl.h"
#include "HeroControl.h"
#include "tools/ShowTexttip.h"
#include "GMessage.h"
#include "tollgate/TollgatePreview.h"
#include "tollgate/SelectArmy.h"
#include "common/CommonFunction.h"
#include "mainCity/CCompaseLayer.h"
#include "Battle/AnimationManager.h"
#include "Global.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/UITools.h"
#include "hero/HeroCall.h"
#include "mainCity/CNewHero.h"
#include "common/CGameSound.h"
#include "Resources.h"
using namespace BattleSpace;
CHeroEvolve::CHeroEvolve()
	:m_iType(1),m_itemNeedNum(0), m_bAskNextQualityHero(false),m_pNewHero(nullptr),m_pHeroArmature(nullptr)
{

}

bool CHeroEvolve::init()
{
	if (BaseLayer::init())
	{
		this->setIsShowBlack(false);
		this->setVisible(true);
		
		m_ui = LoadComponent("HeroEvolve.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}
	return false;
}

void CHeroEvolve::onEvolveBtn(CCObject* pSender)
{
	if(checkHeroEvolveOpen(m_hero))
	{
		((CHeroAttribute*)(this->getParent()))->getDelegate()->onEvolveHero();
		CCLOG("ERROR  CHeroEvolve::onEvolveBtn");
	}
	//showEvoleEffect();
}

void CHeroEvolve::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CHeroEvolve::onEnter()
{
	BaseLayer::onEnter();

	CButton *evolve =(CButton*)(m_ui->findWidgetById("evolve"));
	if (evolve)
	{
		evolve->setOnClickListener(this,ccw_click_selector(CHeroEvolve::onEvolveBtn));
	}

	CButton * add =(CButton*)(m_ui->findWidgetById("add"));
	add->setOnClickListener(this,ccw_click_selector(CHeroEvolve::heroCall));

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CHeroEvolve::onClose));
	this->addChild(pClose, 999);

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO, this, GameMsghandler_selector(CHeroEvolve::updateHeroInfo));	

	NOTIFICATION->addObserver(this, callfuncO_selector(CHeroEvolve::compaseSuccess), COMPOSE_SUCCESS, nullptr);

}

void CHeroEvolve::updateHeroInfo(const TMessage& tMsg)
{
	CHero *hero = (CHero*)tMsg.lParam;
	if (tMsg.nMsg == 1)
	{
		showHeroHead(hero);
	}
	else
	{
		showHeroQuality(hero);
	}

	updateAttr(hero);
}

void CHeroEvolve::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
	NOTIFICATION->removeAllObservers(this);

	CCArmatureDataManager::sharedArmatureDataManager()->removeAnimationData("recruit/jinjie.ExportJson");
}


void CHeroEvolve::showHeroHead(CHero *hero)
{
	m_hero = hero;
	CImageView *itembg1 = (CImageView *)(m_ui->findWidgetById("itembg1"));
	CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero->thumb)->getCString());
	if (!head)
	{
		head = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CHeroEvolve::showHeroHead Lost Image = %d",hero->thumb);
	}
	itembg1->removeChildByTag(1);
	head->setScale(90.0f/100.0f);
	head->setTag(1);
	head->setPosition(ccp(itembg1->getContentSize().width/2, itembg1->getContentSize().height/2));
	itembg1->addChild(head);
	showHeroQuality(hero);

	CLabel* val = (CLabel *)(m_ui->findWidgetById("expVal"));
	val->setString(CCString::createWithFormat("%d/%d",hero->itemNum1,hero->itemNum2)->getCString());

	CProgressBar* process = (CProgressBar *)(m_ui->findWidgetById("exp_process"));
	process->setMaxValue(hero->itemNum2);
	process->setValue(hero->itemNum1);
}

void CHeroEvolve::showHeroQuality(CHero * hero)
{
	CImageView *mask1 = (CImageView *)(m_ui->findWidgetById("mask1"));
	mask1->setTexture(setItemQualityTexture(hero->iColor));

	CLabel* val = (CLabel *)(m_ui->findWidgetById("expVal"));
	val->setString(CCString::createWithFormat("%d/%d",hero->itemNum1,hero->itemNum2)->getCString());

	CProgressBar* process = (CProgressBar *)(m_ui->findWidgetById("exp_process"));
	process->setMaxValue(hero->itemNum2);
	process->setValue(hero->itemNum1);

}

void CHeroEvolve::showItem(CItem * item)
{
	CImageView *itembg1 = (CImageView *)(m_ui->findWidgetById("itembg1"));
	CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", item->iconId)->getCString());
	if (!head)
	{
		head = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CHeroEvolve::showHeroHead Lost Image = %d",item->iconId);
	}
	itembg1->removeChildByTag(1);
	head->setScale(90.0f/100.0f);
	head->setTag(1);
	head->setPosition(ccp(itembg1->getContentSize().width/2, itembg1->getContentSize().height/2));
	itembg1->addChild(head);

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item->itemId);
	if (itemData)
	{
		CLabel* name = (CLabel *)(m_ui->findWidgetById("name"));
		name->setString(itemData->itemName.c_str());
	}
	
	CImageView *mask1 = (CImageView *)(m_ui->findWidgetById("mask1"));
	mask1->setTexture(setItemQualityTexture(item->quality));

	CLabel* val = (CLabel *)(m_ui->findWidgetById("expVal"));
	val->setString(CCString::createWithFormat("%d/%d",item->itemNum,m_itemNeedNum)->getCString());

	CProgressBar* process = (CProgressBar *)(m_ui->findWidgetById("exp_process"));
	process->setMaxValue(m_itemNeedNum);
	process->setValue(item->itemNum);
}

bool CHeroEvolve::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch, pEvent);
	
	if(m_iType!=2)return res;

	CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");
	if( !res && !bgSpr->boundingBox().containsPoint(pTouchPos))
	{
		res = true;
		onClose(nullptr);
	}
	return res;
}


void CHeroEvolve::setNeedNum(int num)
{
	m_itemNeedNum = num;
}

bool CHeroEvolve::checkHeroEvolveOpen(CHero * hero)
{
	//检查英雄进阶条件
	///*等级是否足够*/，碎片是否足够
	bool isLevelOk = /*hero->level>=hero->maxLevel*/true;
	bool isShardsOk = hero->itemNum1>=hero->itemNum2;
	if(!isLevelOk && !isShardsOk)
	{
		ShowPopTextTip(GETLANGSTR(1019));
		return false;
	}
	else if(!isLevelOk)
	{
		ShowPopTextTip(GETLANGSTR(1018));
		return false;
	}
	else if(!isShardsOk)
	{
		ShowPopTextTip(GETLANGSTR(1020));
		return false;
	}
	else
	{
		return true;
	}
}

void CHeroEvolve::levelSkillUp( CHero* hero,const Hero* pHero )
{
	//int atk = pHero->atk() - hero->atk;
	//int hp = pHero->hp() - hero->hp;
	//int def = pHero->def() - hero->def;
	//const char *str = nullptr;
	//if (atk>0)
	//{
	//	str = CCString::createWithFormat(GETLANGSTR(278), atk)->getCString();
	//	CLabel *lab = CLabel::create(str,DEFAULT_FONT,20);
	//	lab->setPosition(ccp(VCENTER.x+380,VCENTER.y-150));
	//	lab->setColor(RGB_GREEN);
	//	runAnimation(0.5f,lab);
	//}
	//if (hp>0)
	//{
	//	str = CCString::createWithFormat(GETLANGSTR(279),hp)->getCString();
	//	CLabel *lab = CLabel::create(str,DEFAULT_FONT,20);
	//	lab->setPosition(ccp(VCENTER.x+380,VCENTER.y-150));
	//	lab->setColor(RGB_GREEN);
	//	runAnimation(0.8f,lab);
	//}
	//if (def>0)
	//{
	//	str = CCString::createWithFormat(GETLANGSTR(280),def)->getCString();
	//	CLabel *lab = CLabel::create(str,DEFAULT_FONT,20);
	//	lab->setPosition(ccp(VCENTER.x+380,VCENTER.y-150));
	//	lab->setColor(RGB_GREEN);
	//	runAnimation(1.1f,lab);
	//}

	//CCSprite *skillspr = CCSprite::create("common/word_07.png");
	//skillspr->setPosition(ccp(VCENTER.x+380,VCENTER.y-150));
	//runAnimation(1.4f,skillspr);

	showEvoleEffect();
}

void CHeroEvolve::showUpLevelAnimation()
{
	//if (m_ui->getChildByTag(1000))
	//{
	//	// 		m_ui->removeChildByTag(1000);
	//	CCAnimation *anim = AnimationManager::sharedAction()->getAnimation("shenji");
	//	CCAction *action = CCAnimate::create(anim);
	//	action->setTag(1);
	//	CCNode *shenjiSpr = m_ui->getChildByTag(1000);
	//	shenjiSpr->runAction(action);
	//}
	//else if (!m_ui->getChildByTag(1000))
	//{
	//	CCAnimation *anim = AnimationManager::sharedAction()->getAnimation("shenji");
	//	CCSprite *shenjiSpr = CCSprite::create("skill/shenji.png");
	//	shenjiSpr->setPosition(((CCNode*)(m_ui->findWidgetById("ying")))->getPosition());
	//	shenjiSpr->setPositionY(shenjiSpr->getPositionY()+50);
	//	shenjiSpr->setTag(1000);
	//	CCAction *action = CCAnimate::create(anim);
	//	action->setTag(1);
	//	shenjiSpr->runAction(action);
	//	m_ui->addChild(shenjiSpr,0,1000);
	//}
}

void CHeroEvolve::runAnimation( float dt, CCNode * node )
{
	//this->addChild(node);
	//node->setVisible(false);
	//CCFiniteTimeAction* fly = CCSpawn::create(
	//	CCMoveBy::create(1.3f, ccp(0, 130)),
	//	CCSequence::create(CCScaleTo::create(0.15f,1.1f),CCDelayTime::create(0.5f),CCScaleTo::create(0.15f,1.0f),CCFadeTo::create(0.3f, 150), NULL),
	//	NULL);
	//CCFiniteTimeAction* action = CCSequence::create(CCDelayTime::create(dt),CCShow::create(),fly,CCRemoveSelf::create(),
	//	NULL);
	//node->runAction(action);
}

void CHeroEvolve::compaseSuccess( CCObject* pObj )
{
	m_hero->itemNum1++;
	CLabel* val = (CLabel *)(m_ui->findWidgetById("expVal"));
	val->setString(CCString::createWithFormat("%d/%d",m_hero->itemNum1,m_hero->itemNum2)->getCString());
	CProgressBar* process = (CProgressBar *)(m_ui->findWidgetById("exp_process"));
	process->setMaxValue(m_hero->itemNum2);
	process->setValue(m_hero->itemNum1);
}

void CHeroEvolve::updateAttr( CHero * hero )
{
	m_hero = hero;

	//按钮那边的变化，满级以后不再显示按钮
	CButton* evolve = (CButton*)(m_ui->findWidgetById("evolve"));
	CLabel* pEvolveLabel = (CLabel*)(m_ui->findWidgetById("evolve_label"));
	CLabel* pEvolveTips = (CLabel*)(m_ui->findWidgetById("wrong_tip"));

	if(m_hero->quality == 5)
	{
		evolve->setVisible(false);
		pEvolveLabel->setVisible(false);
		pEvolveTips->setVisible(true);
	}
	else
	{
		evolve->setVisible(true);
		pEvolveLabel->setVisible(true);
		pEvolveTips->setVisible(false);
	}

	//当前属性更新
	CLabel* pLevel = (CLabel*)(m_ui->findWidgetById("level"));
	pLevel->setString(CCString::createWithFormat("%d/%d", m_hero->level, m_hero->maxLevel)->getCString());
	if(m_hero->level < m_hero->maxLevel && m_hero->quality != 5)
	{
		pLevel->stopAllActions();
		pLevel->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeTo::create(0.2f, 120), CCFadeTo::create(0.2f, 255))));
	}
	else
	{
		pLevel->stopAllActions();
		pLevel->setOpacity(255);
	}

	CLabel* pHp = (CLabel*)(m_ui->findWidgetById("hp"));
	pHp->setString(ToString(m_hero->hp));
	CLabel* pAtk = (CLabel*)(m_ui->findWidgetById("attack"));
	pAtk->setString(ToString(m_hero->atk));
	CLabel* pDefense = (CLabel*)(m_ui->findWidgetById("defend"));
	pDefense->setString(ToString(m_hero->def));

}

void CHeroEvolve::updateNextQualityAttr( CHero* hero )
{
	//更新下一阶段的数据
	CLabel* pLevel = (CLabel*)(m_ui->findWidgetById("addlevel"));
	pLevel->setString(CCString::createWithFormat("%d/%d", 1, hero->maxLevel)->getCString());
	CLabel* pHp = (CLabel*)(m_ui->findWidgetById("addhp"));
	pHp->setString(ToString(hero->hp));
	CLabel* pAtk = (CLabel*)(m_ui->findWidgetById("addatk"));
	pAtk->setString(ToString(hero->atk));
	CLabel* pDefense = (CLabel*)(m_ui->findWidgetById("adddef"));
	pDefense->setString(ToString(hero->def));
	if(m_hero->quality == 5)
	{
		pLevel->setString("??/??");
		pHp->setString("???");
		pAtk->setString("???");
		pDefense->setString("???");
	}
}

void CHeroEvolve::askForData()
{
	if(m_hero->quality==5)
	{
		//上限了，
		updateNextQualityAttr(m_hero);
	}
	else
	{
		//请求更新数据
		if (this->isVisible())
		{	
			runAction(CCSequence::create(CCDelayTime::create(0.8f), CCCallFunc::create(this, callfunc_selector(CHeroEvolve::askForDataCallBack)),nullptr));
		}
	}
}

void CHeroEvolve::askForDataCallBack()
{
	setAskNextQualityHero(true);
	CPlayerControl::getInstance().sendGetHeroInfo(m_hero->id, 0, 0, m_hero->quality+1);
}

void CHeroEvolve::heroCall( CCObject* pSender )
{
	CHeroCall *evolveLayer = CHeroCall::create();
	evolveLayer->setNeedNum(m_hero->itemNum2);
	LayerManager::instance()->push(evolveLayer);
	CPlayerControl::getInstance().sendItemInfo(m_hero->itemId);
}

void CHeroEvolve::showEvoleEffect()
{
	//加载资源
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("recruit/jinjie.ExportJson");

	//屏蔽层
	MaskLayer* lay = MaskLayer::create("showEvoleEffect");
	lay->setContentSize(CCSizeMake(1138,640));
	lay->setAnchorPoint(ccp(0.5f, 0.5f));
	lay->setPosition(VCENTER);
	lay->setOpacity(0);
	lay->setTouchPriority(-13);
	this->addChild(lay, 9999, 9999);

	//效果
	CCArmature* pEffect = CCArmature::create("jinjie");
	pEffect->setPosition(ccp(lay->getContentSize().width/2 - 18, lay->getContentSize().height/2));
	lay->addChild(pEffect);
	pEffect->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(CHeroEvolve::frameEventCallBack));
	pEffect->getAnimation()->playWithIndex(0, 0.01f);
	CCBone* pBone = pEffect->getBone("fog2");


	//人物
	CCArmature* Armature = CCArmature::create(ToString(m_hero->thumb));
	Armature->getAnimation()->play(Stand_Action,0.01f);
	float zoom = m_hero->zoom*0.01f;
	if (!zoom)
		zoom = 300.0/Armature->getContentSize().height;			//容错性处理
	Armature->setScale(zoom+0.3f);
	CCPoint pos = ccp(0, -235);
	Armature->setPosition(pos);
	pEffect->addChild(Armature, pBone->getZOrder());
	m_pHeroArmature = Armature;

}

void CHeroEvolve::showEvoleEffectCallBack()
{
	removeChildByTag(9999);
}

void CHeroEvolve::frameEventCallBack( CCBone* pBone, const char* sEvent, int iTag1, int iTag2 )
{
	if(strcmp(sEvent, "in") == 0)
	{
		MaskLayer* lay = (MaskLayer*)getChildByTag(9999);

		m_pHeroArmature->runAction(CCFadeOut::create(0.15f));

		//人物变鬼火，非进去
		int animId = 8033;	
		if (m_hero->quality < 10)
		{
			animId = (m_hero->iColor-1)*10 +8003;
		}

		CCAnimation *soulAnim = AnimationManager::sharedAction()->getAnimation(ToString(animId));
		CCSprite *soulMove = createAnimationSprite(CCString::createWithFormat("skill/%d.png", animId)->getCString(), ccp(1138/2, 250), soulAnim, true);
		soulMove->setScale(1.0f);
		soulMove->setOpacity(0);
		soulMove->runAction(CCSequence::create(CCFadeIn::create(0.15f), 
			CCEaseBackInOut::create(CCSpawn::createWithTwoActions(CCMoveBy::create(0.15f, ccp(0, 230)), CCScaleTo::create(0.15f, 0.0f))),  CCRemoveSelf::create(), nullptr));
		lay->addChild(soulMove);

	}
	else if(strcmp(sEvent, "out") == 0)
	{
		MaskLayer* lay = (MaskLayer*)getChildByTag(9999);

		////人物变鬼火，非进去
		//int animId = 8033;	
		//if (m_hero->quality < 10)
		//{
		//	animId = (m_hero->quality-1)*10 +8003;
		//}

		//CCAnimation *soulAnim = AnimationManager::sharedAction()->getAnimation(ToString(animId));
		//CCSprite *soulMove = createAnimationSprite(CCString::createWithFormat("skill/%d.png", animId)->getCString(), ccp(1138/2, 450), soulAnim, true);
		//soulMove->setScale(0.0f);
		//soulMove->runAction(CCSequence::create(
		//	CCEaseBackInOut::create(CCSpawn::createWithTwoActions(CCMoveBy::create(0.15f, ccp(0, -230)), CCScaleTo::create(0.15f, 1.0f))), CCRemoveSelf::create(), 
		//	CCCallFunc::create(this, callfunc_selector(CHeroEvolve::outCallBack)), nullptr));
		//lay->addChild(soulMove, 222);
		outCallBack();
		//this->runAction(CCSequence::create(
		//	CCDelayTime::create(0.2f),
		//	CCCallFunc::create(this, callfunc_selector(CHeroEvolve::outCallBack)), nullptr));
	}
}

void CHeroEvolve::outCallBack()
{
	MaskLayer* lay = (MaskLayer*)getChildByTag(9999);

	//人物出场
	m_pNewHero = CNewHero::create();
	m_pNewHero->setVisible(false);
	lay->addChild(m_pNewHero, 200);
	m_pNewHero->setVisible(false);

	HeroLotteryData data;
	data.heroType = m_hero->roletype;
	data.thumb = m_hero->thumb;
	data.quality = m_hero->quality;
	m_pNewHero->setEvolve(true);
	m_pNewHero->showNewHeroEffect(&data);
	m_pNewHero->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.5f), CCCallFunc::create(this, callfunc_selector(CHeroEvolve::showEvoleEffectCallBack))));
}
