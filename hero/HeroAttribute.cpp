#include "HeroAttribute.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"

#include "HeroControl.h"
#include "mainCity/StrengthenItem.h"
#include "tools/ShowTexttip.h"
#include "scene/AnimationManager.h"
#include "HeroBreak.h"
#include "Global.h"
#include "common/CommonFunction.h"
#include "common/CHeroBodyShowData.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "common/CCRollLabelAction.h"

CHeroAttribute::CHeroAttribute():m_itemIndex(0),m_foodCount(0),m_bIsToLeft(false),m_pHeroTemporary(nullptr),
	m_show_info_scroll(nullptr), m_pInfo1(nullptr), m_pInfo2(nullptr)
{

}


bool CHeroAttribute::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("heroattribute");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		//CColorView *blackView = CColorView::create(ccc4(0,0,0,200));
		//blackView->setContentSize(CCSizeMake(1138,640));
		//blackView->setPosition(VCENTER);
		//this->addChild(blackView);

		m_ui = LoadComponent("HeroSystem.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		m_heroBodyShowdData = new CHeroBodyShowData;
		m_heroBodyShowdData->reloadFile();

		setCoverScreen(true);

		//展示区图片
		m_show_info_scroll = (CScrollView*)m_ui->findWidgetById("scroll_info");
		m_show_info_scroll->setDirection(eScrollViewDirectionVertical);
		m_show_info_scroll->setBounceable(true);

		m_pInfo1 = (CLabel*)m_ui->findWidgetById("info_1");
		CC_SAFE_RETAIN(m_pInfo1);
		m_pInfo1->removeFromParentAndCleanup(false);
		m_show_info_scroll->getContainer()->addChild(m_pInfo1);
		m_pInfo1->setAnchorPoint(ccp(0, 1));
		CC_SAFE_RELEASE(m_pInfo1);

		m_pInfo2 = (CLabel*)m_ui->findWidgetById("info_2");
		CC_SAFE_RETAIN(m_pInfo2);
		m_pInfo2->removeFromParentAndCleanup(false);
		m_show_info_scroll->getContainer()->addChild(m_pInfo2);
		m_pInfo2->setAnchorPoint(ccp(0, 1));
		CC_SAFE_RELEASE(m_pInfo2);

		updateShowInfoScroll();

		m_show_info_scroll->setVisible(false);

		CCNode* pMaskInfo = (CCNode*)m_ui->findWidgetById("mask_info");
		pMaskInfo->setVisible(false);

		return true;
	}
	return false;

}

void CHeroAttribute::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
// 	if (m_delegate)
// 	{
// 		m_delegate->onResetLayer();
// 	}
}

void CHeroAttribute::onExpbtn(CCObject* pSender)
{
	CRadioButton* checkBtn = (CRadioButton*)(m_ui->getChildByTag(4)); 
	checkBtn->setChecked(false);
	m_delegate->onHeroInfo(m_selectHero, 5);
}

void CHeroAttribute::showUpLevelAnimation()
{
	if (m_ui->getChildByTag(1000))
	{
// 		m_ui->removeChildByTag(1000);
		CCAnimation *anim = AnimationManager::sharedAction()->getAnimation("shenji");
		CCAction *action = CCAnimate::create(anim);
		action->setTag(1);
		CCNode *shenjiSpr = m_ui->getChildByTag(1000);
		shenjiSpr->runAction(action);
	}
	else if (!m_ui->getChildByTag(1000))
	{
		CCAnimation *anim = AnimationManager::sharedAction()->getAnimation("shenji");
		CCSprite *shenjiSpr = CCSprite::create("skill/shenji.png");
		shenjiSpr->setPosition(((CCNode*)(m_ui->findWidgetById("ying")))->getPosition());
		shenjiSpr->setPositionY(shenjiSpr->getPositionY()+50);
		shenjiSpr->setTag(1000);
		CCAction *action = CCAnimate::create(anim);
		action->setTag(1);
		shenjiSpr->runAction(action);
		m_ui->addChild(shenjiSpr,0,1000);
	}
}

void CHeroAttribute::onBreakBtn(CCObject* pSender)
{
	m_delegate->onAddBreak(100);
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();
	if (data->getRoleFood()<100 && data->getRoleFood()>0)
	{
		m_delegate->onAddBreak(data->getRoleFood());
		data->setRoleFood(0);
	}
	else if (data->getRoleFood()>100)
	{
		m_delegate->onAddBreak(100);
		data->setRoleFood(data->getRoleFood()-100);
	}
	CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

// 	CHeroBreak *protmo = CHeroBreak::create();
// 	LayerManager::instance()->push(protmo);
}

void CHeroAttribute::onLeftHero(CCObject* pSender)
{
	m_delegate->onBeforeHero();
	m_bIsToLeft = false;
}

void CHeroAttribute::onRightHero(CCObject* pSender)
{
	m_delegate->onAfterHero();
	m_bIsToLeft = true;
}

void CHeroAttribute::onEnter()
{
	BaseLayer::onEnter();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(m_ui->convertToNodeSpace(ccp(VLEFT+50, VTOP-50)));
	pClose->setId("close");
	pClose->setOnClickListener(this,ccw_click_selector(CHeroAttribute::onClose));
	m_ui->addChild(pClose, 999);

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(99);
	for (int i = 0; i < 4; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
 		radioBtn->setOnCheckListener(this,ccw_check_selector(CHeroAttribute::onCheckBtn));
	}

// 	CRadioButton* advanceBtn =(CRadioButton*)(m_ui->getChildByTag(4));
// 	advanceBtn->setOnCheckListener(this,ccw_check_selector(CHeroAttribute::onCheckAdvanceBtn));

// 	CButton* expbtn = (CButton*)(m_ui->findWidgetById("expbtn"));
//  	expbtn->setOnClickListener(this,ccw_click_selector(CHeroAttribute::onExpbtn));
// 	expbtn->setOnCheckListener(this,ccw_check_selector(CHeroAttribute::onRaiseExpBtn));
	
// 	CButton* upbtn = (CButton*)(m_ui->findWidgetById("upbtn"));
// 	upbtn->setOnClickListener(this,ccw_click_selector(CHeroAttribute::onBreakBtn)); 
// 	upbtn->setOnLongClickListener(this,ccw_longclick_selector(CHeroAttribute::onlongClickBreak));
// 	this->setOnTouchEndedAfterLongClickListener(this, ccw_afterlongclick_selector(CHeroAttribute::onlongClickEnd));

	CButton* leftbtn = (CButton*)(m_ui->findWidgetById("left"));
	leftbtn->setOnClickListener(this,ccw_click_selector(CHeroAttribute::onLeftHero));

	CCFadeTo *fade1 = CCFadeTo::create(0.8f,100);
	CCFadeTo *fade2 = CCFadeTo::create(0.8f,255);
	leftbtn->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(fade1,fade2)));

	CButton* rightbtn = (CButton*)(m_ui->findWidgetById("right"));
	rightbtn->setOnClickListener(this,ccw_click_selector(CHeroAttribute::onRightHero));
	fade1 = CCFadeTo::create(0.8f,100);
	fade2 = CCFadeTo::create(0.8f,255);
	rightbtn->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(fade1,fade2)));


	//book
	CCheckBox* pBook = (CCheckBox*)(m_ui->findWidgetById("book"));
	pBook->setOnCheckListener(this,ccw_check_selector(CHeroAttribute::showHeroBook));

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO,this,GameMsghandler_selector(CHeroAttribute::updateHeroInfo));
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROEXP,this,GameMsghandler_selector(CHeroAttribute::updateHeroExp));

	GetTcpNet->registerMsgHandler(ChangeArmor,this, CMsgHandler_selector(CHeroAttribute::ProcessMsg));

	NOTIFICATION->addObserver(this, callfuncO_selector(CHeroAttribute::compaseSuccess), COMPOSE_SUCCESS, nullptr);

	runAction(CCCallFunc::create(this, callfunc_selector(CHeroAttribute::callbackForHideBackLayer)));

	this->setVisible(true);
	NOTIFICATION->postNotification(HIDE_TOP_LAYER);
}

void CHeroAttribute::onlongClickEnd(CCObject* pSender, CCTouch* pTouch, float fDuration)
{
	this->unschedule(schedule_selector(CHeroAttribute::onAddFood));
	/*ShowConfirmTextTip(U8(CCString::createWithFormat("确认花费%d粮食突破品质吗？",m_foodCount)->getCString()),this,ccw_click_selector(CHeroAttribute::isUseFood));*/
	m_delegate->onAddBreak(m_foodCount);
}

void CHeroAttribute::isUseFood(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==1)
	{
		m_delegate->onAddBreak(m_foodCount);
	    UserData *data = DataCenter::sharedData()->getUser()->getUserData(); 
		data->setRoleFood(data->getRoleFood()-m_foodCount);
		CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
	}
	else
	{
		CProgressBar *promoteBar = (CProgressBar*)(m_ui->findWidgetById("promote"));
		promoteBar->setValue(m_selectHero->starExp - m_foodCount);
	}
	btn->getParent()->getParent()->removeFromParent();
}

bool CHeroAttribute::onlongClickBreak(CCObject* pSender, CCTouch* pTouch)
{
	m_foodCount = 0;
	this->schedule(schedule_selector(CHeroAttribute::onAddFood),0.1f);
	return true;
}

void CHeroAttribute::onAddFood(float dt)
{
	m_foodCount += 50;
    UserData *data = DataCenter::sharedData()->getUser()->getUserData();

	if (m_foodCount <= data->getRoleFood()&&m_selectHero->starExp + m_foodCount< m_selectHero->nextStarExp)
	{
		CProgressBar *promoteBar = (CProgressBar*)(m_ui->findWidgetById("promote"));
		promoteBar->setValue(m_selectHero->starExp + m_foodCount);

		CLabel *starexpVal = (CLabel*)(m_ui->findWidgetById("starexpVal"));
		starexpVal->setString(CCString::createWithFormat("%d/%d",m_selectHero->starExp+ m_foodCount,m_selectHero->nextStarExp)->getCString());

		UserData *data = DataCenter::sharedData()->getUser()->getUserData(); 
		data->setRoleFood(data->getRoleFood()-m_foodCount);
		CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
	}
	else if (m_selectHero->starExp + m_foodCount>= m_selectHero->nextStarExp&&m_foodCount <= data->getRoleFood())
	{
		m_delegate->onAddBreak(m_foodCount);
		m_foodCount = 0;
		CProgressBar *promoteBar = (CProgressBar*)(m_ui->findWidgetById("promote"));
		promoteBar->setValue(0);
	}
	else
	{
		m_foodCount = data->getRoleFood();
		this->unschedule(schedule_selector(CHeroAttribute::onAddFood));
	}
}

void CHeroAttribute::updateHeroInfo(const TMessage& tMsg)
{
	CHero *hero = (CHero*)tMsg.lParam;

	updateRaiseAttr(hero);

	if (tMsg.nMsg==1)
	{
		showBaseInfo(hero);

		//showBookAnimate(m_ui, m_bIsToLeft);
	}

/*	if (hero->armor1.id != m_selectHero->armor1.id || hero->armor2.id != m_selectHero->armor2.id)*/
	{
		showArmor(hero);
	}
	*m_selectHero = *hero;
}

void CHeroAttribute::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROEXP, this);
	BaseLayer::onExit();
	if (m_delegate)
		m_delegate->onResetLayer();
	sort(m_VecMode.begin(),m_VecMode.end());
	m_VecMode.erase(unique(m_VecMode.begin(),m_VecMode.end()),m_VecMode.end());	
	vector<int>::iterator iter = m_VecMode.begin();
	for (;iter!=m_VecMode.end();iter++)
	{
		CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(*iter));
		if (animationData)
		{
			char str[60]={0};
			sprintf(str,"BoneAnimation/%d.ExportJson",*iter);
			CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(str);
		}
	}
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	//显示被遮盖的层，渲染
	LayerManager::instance()->showLayerUnBeCovered(this);
	NOTIFICATION->removeAllObservers(this);
	NOTIFICATION->postNotification(SHOW_TOP_LAYER);
}

void CHeroAttribute::onCheckBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;
	int selIndex = 2*btn->getTag();
	if (bChecked)
	{
// 		CRadioButton* advanceBtn =(CRadioButton*)(m_ui->getChildByTag(4));
// 		advanceBtn->setChecked(false);
// 
// 		CRadioButton* expbtn = (CRadioButton*)(m_ui->findWidgetById("expbtn"));
// 		expbtn->setChecked(false);
// 		
// 		CRadioButton *checkBtn = (CRadioButton*)pSender;
//  		if (m_delegate)
// 		{
//  			m_delegate->onHeroInfo(m_selectHero,checkBtn->getTag());
//  		}
// 		if (btn->getTag()==2)
// 		{
// 			m_delegate->onHeroInfo(m_selectHero,3);
// 		}
// 		else
// 		{
// 			m_delegate->onHeroInfo(m_selectHero,1);
// 		}

		PlayEffectSound(SFX_Button);

		if(btn->getTag()==1)
		{
			bgHideOrShow(true);
		}
		else
		{
			bgHideOrShow(false);
		}
		m_delegate->onHeroInfo(m_selectHero,btn->getTag());

	}
}


void CHeroAttribute::onCheckAdvanceBtn(CCObject *pSender, bool bChecked)
{
	if (bChecked)
	{
		CRadioButton *checkBtn = (CRadioButton*)pSender;
		if (m_delegate)
		{
			m_delegate->onHeroInfo(m_selectHero,checkBtn->getTag());
		}
// 		CRadioButton *radioBtn = (CRadioButton*)(m_ui->getChildByTag(10)->getChildByTag(1));
// 		radioBtn->setChecked(true);
// 		onCheckBtn(radioBtn,true);
	}
}

void CHeroAttribute::onRaiseExpBtn(CCObject *pSender, bool bChecked)
{
	if (bChecked)
	{
		CRadioButton* checkBtn = (CRadioButton*)(m_ui->getChildByTag(4)); 
		checkBtn->setChecked(false);
		m_delegate->onHeroInfo(m_selectHero, 5);
	}
}

void CHeroAttribute::onSelectHero(CCObject *pSender)
{
	
}

CHeroAttribute::~CHeroAttribute()
{
	CC_SAFE_DELETE(m_heroBodyShowdData);
	CC_SAFE_DELETE(m_pHeroTemporary);
}

void CHeroAttribute::updateHeroAttr(CHero* hero)
{
	m_selectHero = hero;

	showBaseInfo(hero);
	updateRaiseAttr(hero);
	showArmor(hero);

	CCNode *redPoint = (CCNode*)(m_ui->findWidgetById("redPoint"));
	redPoint->setVisible(hero->evol);
}

void CHeroAttribute::onSelectItem(CCObject *pSender)
{
	PlayEffectSound(SFX_Button);
	m_itemIndex = ((CCNode*)pSender)->getTag();

	CStrengthenItem *selectItem = CStrengthenItem::create();
	selectItem->setVisible(false);
	selectItem->setSelForHero(m_selectHero->thumb);
	selectItem->setSelectType(2);
	selectItem->setEquipType(m_itemIndex);
	LayerManager::instance()->push(selectItem);
	selectItem->setOnSelectItemListener(this,ccw_select_Item(CHeroAttribute::onSelectArmor));	
	
	CPlayerControl::getInstance().sendRoleBag(1,false,m_selectHero->id,m_itemIndex);
}

void CHeroAttribute::onSelectArmor(CItem* pItem)
{
	if (pItem)
	{
		CPlayerControl::getInstance().sendChangeArmor(m_selectHero->id, m_itemIndex, pItem->id);
	}
	else
	{
		CPlayerControl::getInstance().sendChangeArmor(m_selectHero->id,m_itemIndex,0);
	}	
}

void CHeroAttribute::setDelegate(CHeroDelegate *delegate)
{
	m_delegate = delegate;
}

CHeroDelegate* CHeroAttribute::getDelegate()
{
	return m_delegate;
}

void CHeroAttribute::updateLeftRightBtn(bool isLeft, bool isVis)
{
	if (isLeft)
	{
		CButton* leftbtn = (CButton*)(m_ui->findWidgetById("left"));
		leftbtn->setVisible(isVis);
	}
	else
	{
		CButton* rightbtn = (CButton*)(m_ui->findWidgetById("right"));
		rightbtn->setVisible(isVis);
	}
}

void CHeroAttribute::updateRaiseAttr(CHero* hero)
{
	CLabel *level = (CLabel *)(m_ui->findWidgetById("level"));
	level->setString(CCString::createWithFormat("%d/%d",hero->level,hero->maxLevel)->getCString());

	CLabel *explab = (CLabel*)(m_ui->findWidgetById("expVal"));
	explab->setString(CCString::createWithFormat("%d/%d",hero->itemNum1,hero->itemNum2)->getCString());

	CImageViewScale9 *mask = (CImageViewScale9 *)(m_ui->findWidgetById("bar_bg"));
	CCSize size = mask->boundingBox().size;
	mask->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/mask_%d.png",hero->quality)->getCString()));
	mask->setContentSize(size);

	CImageViewScale9* process = (CImageViewScale9 *)(m_ui->findWidgetById("lv_process"));
	float cpe = (float)hero->level/hero->maxLevel;
	if (cpe>1)
		cpe = 1;
	process->setScaleX(cpe);
	CImageViewScale9* process1 = (CImageViewScale9 *)(m_ui->findWidgetById("exp_process"));
	float pe = (float)hero->itemNum1/hero->itemNum2;
	if (pe>1)
		pe = 1;
	process1->setScaleX(pe);

	{
		//上面的等级也更新
		CLabel* pLevel = (CLabel*)(m_ui->findWidgetById("level_Copy"));
		pLevel->setString(CCString::createWithFormat("%d/%d", hero->level, hero->maxLevel)->getCString());	

		//战力
		CLabelAtlas *fight = (CLabelAtlas*)(m_ui->findWidgetById("level_fight"));
		fight->setAnchorPoint(ccp(0, 0.5f));
		int iFrom = atoi(fight->getString());
		fight->setString(ToString(hero->combat));
		fight->runAction(CCRollLabelAction::create(0.5f, iFrom, (hero->combat), fight));
		fight->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.2f,1.2f),CCScaleTo::create(0.2f,1.0f)));
	}
}

void CHeroAttribute::ProcessMsg(int type, google::protobuf::Message *msg)
{
	ChangeArmorResponse *res = (ChangeArmorResponse*)msg;

	if (res->result()==1)
	{
		if (res->has_hero())
		{
			CC_SAFE_DELETE(m_pHeroTemporary);
			m_pHeroTemporary = new CHero;
			m_pHeroTemporary->readData(res->hero());
			CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HEROINFO,0,nullptr, m_pHeroTemporary);
			((CHeroControl*)m_delegate)->updateHeroValue(m_pHeroTemporary);

			//可能是更换装备,更新另一个人的数据
			CHero *hero2 = new CHero;
			hero2->readData(res->hero2());
			if(hero2->heroid>0)
			{
				((CHeroControl*)m_delegate)->updateHeroValue(hero2);
			}
			delete hero2;
		}
	}
	else if (res->result()==2)
	{
		//背包空间不足
		ShowPopTextTip(GETLANGSTR(178));
	}
	else if (res->result()==3)
	{
		ShowPopTextTip(GETLANGSTR(170));
	}
}

void CHeroAttribute::showArmor(CHero* hero)
{
	for(unsigned int i=0; i<4; i++)
	{
		CItem* pItem;
		if(i==0) pItem = &hero->armor1;
		if(i==1) pItem = &hero->armor2;
		if(i==2) pItem = &hero->armor3;
		if(i==3) pItem = &hero->armor4;

		//100.锁住 200.解锁了没装备 300.解锁了有装备 400.库里有可用装备
		int iParam = pItem->itparam;

		//添加
		CCSprite* pAdd = (CCSprite*)(m_ui->findWidgetById(CCString::createWithFormat("add%d", i+1)->getCString()));

		//锁
		CImageView* pLock = (CImageView*)(m_ui->findWidgetById(CCString::createWithFormat("_lock%d", i+1)->getCString()));

		//边框
		CImageView* pMask = (CImageView*)(m_ui->findWidgetById(CCString::createWithFormat("mask%d", i+1)->getCString()));
		pMask->removeChildByTag(1);
		pMask->removeChildByTag(2);

		//未装备字样
		pAdd->removeAllChildren();
		CCSprite* pUnEquip = CCSprite::create("common/word_unequipped.png");
		pUnEquip->setPosition(ccp(pAdd->getContentSize().width/2, pUnEquip->getContentSize().height/2));
		pAdd->addChild(pUnEquip);

		//开放
		if(iParam != 100)
		{
			pLock->setVisible(false);
			pLock->setTouchEnabled(false);

			pMask->setVisible(true);
			pMask->setTouchEnabled(true);
			pMask->setOnClickListener(this, ccw_click_selector(CHeroAttribute::onSelectItem));
			pMask->setTag(i+1);

			//是否装备
			if(iParam == 300)
			{
				CCSprite *item = CCSprite::create(GetImageName(1, pItem->quality, pItem->iconId).c_str());
				if (!item)
				{
					item = CCSprite::create("prop/32003.png");
					CCLOG("[ ERROR ] CHeroAttribute::showArmor Lost Image = %d", pItem->iconId);
				}
				item->setPosition(ccp(pMask->getContentSize().width/2, pMask->getContentSize().height/2));
				item->setTag(1);
				pMask->addChild(item, -1);

				pMask->setTexture(setItemQualityTexture(pItem->quality));
				pAdd->setVisible(false);

				//添加星星
				pMask->removeChildByTag(616);
				CLayout* pStarLayout = getStarLayout(pItem->iStar);
				pMask->addChild(pStarLayout, 616, 616);
			}
			//没有装备
			else if(iParam == 200)
			{
				pAdd->setVisible(true);
				pMask->setTexture(setItemQualityTexture(1));//默认白色
			}
			else if (iParam==400)
			{
				pUnEquip->setVisible(false);
				pAdd->setVisible(true);
				pMask->setTexture(setItemQualityTexture(1));//默认白色

				CCSprite *item = CCSprite::create("common/icon_add.png");
				item->setPosition(ccp(pMask->getContentSize().width/2, pMask->getContentSize().height/2));
				item->setTag(2);
				pMask->addChild(item);
			}

		}
		//没开放
		else
		{
			pAdd->setVisible(false);

			pMask->setTouchEnabled(false);
			pMask->setTexture(setItemQualityTexture(1));

			pLock->setVisible(true);
			pLock->setTouchEnabled(true);
			pLock->setOnClickListener(this, ccw_click_selector(CHeroAttribute::onTipsForUnlockArmor));
			pLock->setTag(i);
		}
	}
}

void CHeroAttribute::showBaseInfo(CHero* hero)
{
	CCSprite *role = (CCSprite*)(m_ui->findWidgetById("ying"));
	//使用模型代替贴图
	//CCSprite *sp = CCSprite::create(CCString::createWithFormat("selectRole/%d.png",hero->thumb)->getCString());
	int m_ModeID = hero->thumb;
	char ExportJson_str[60] = {0};//"BoneAnimation/101.ExportJson"
	sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",m_ModeID);
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(ExportJson_str);
	CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(m_ModeID));
	if (!animationData)													//模型容错性处理
	{
		CCLOG("[ *ERROR ]  CHeroAttribute::showBaseInfo Model=%d IS NULL",m_ModeID); 
		m_ModeID = 516;
		sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",m_ModeID);
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(ExportJson_str);
	}	
	CCArmature* Armature = CCArmature::create(ToString(m_ModeID));
	if (Armature)
	{	
		m_VecMode.push_back(m_ModeID);
		Armature->getAnimation()->play(Stand_Action,0.01f);
		role->getParent()->removeChildByTag(100);
		float zoom = hero->zoom*0.01f;
		if (!zoom)
			zoom = 300.0/Armature->getContentSize().height;			//容错性处理
		Armature->setScale(zoom+0.3f);
		Armature->setTag(100);
		CCPoint pos = ccpAdd(role->getPosition(),ccp(0, 40-role->getContentSize().height)/2);
		Armature->setPosition(pos);
		role->getParent()->addChild(Armature, role->getZOrder());
		Armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(CHeroAttribute::movementCallBack));
		m_pArmature = Armature;
	}	

	const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(hero->thumb);

	CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
	if(data)
	{
		name->setString(data->heroName.c_str());
	}
	

	CLabel *level = (CLabel*)(m_ui->findWidgetById("level_Copy"));
	level->setString(CCString::createWithFormat("%d/%d", hero->level, hero->maxLevel)->getCString());	

	//cost消耗
	CCSprite* pCost = (CCSprite*)(m_ui->findWidgetById("cost"));
	//添加火和数字
	if(pCost->getChildByTag(1)==nullptr)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/9051.plist");//将plist文件加载进入缓存
		AnimationManager::sharedAction()->ParseAnimation("9051");
		CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("9051");
		culAnim->setDelayPerUnit(0.1f);
		CCAnimate* pAnimate = CCAnimate::create(culAnim);
		CCSprite* pFire  = CCSprite::create();
		pFire->setPosition(ccp(pCost->getContentSize().width/2, pCost->getContentSize().height/2+5));
		pCost->addChild(pFire, 1, 1);
		pFire->runAction(CCRepeatForever::create(pAnimate));
	}
	pCost->removeChildByTag(2);
	CCLabelAtlas* pCostAtlas = CCLabelAtlas::create(ToString(hero->useCost), "label/number2.png", 26, 32, 46);
	pCostAtlas->setAnchorPoint(ccp(0.5f, 0.5f));
	pCostAtlas->setScale(0.9f);
	pCostAtlas->setPosition(ccp(pCost->getContentSize().width/2, pCost->getContentSize().height/2));
	pCost->addChild(pCostAtlas, 2, 2);

	//黑色背景图加星星
	updateStarRank(hero);

	//战力
	CLabelAtlas *fight = (CLabelAtlas*)(m_ui->findWidgetById("level_fight"));
	fight->setAnchorPoint(ccp(0, 0.5f));
	int iFrom = atoi(fight->getString());
	fight->setString(ToString(hero->combat));
	fight->runAction(CCRollLabelAction::create(0.5f, iFrom, (hero->combat), fight));
// 	fight->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.15f,1.2f),CCScaleTo::create(0.15f,1.0f)));

	//品质
	if(data)
	{
		CCSprite *type = (CCSprite*)(m_ui->findWidgetById("type"));
		type->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png", data->iType1, data->iType2)->getCString()));
	}
	

	////类型(神将僵尸道士)
	//if(data)
	//{
	//	if(data->heroName.size() > 0)
	//		name->setString(data->heroName.c_str());
	//	CCSprite *DAO = (CCSprite*)(m_ui->findWidgetById("dao"));
	//	DAO->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("hero/%d.png", data->iType1)->getCString()));
	//}
	

	//立绘
	CImageView *pBgRect = (CImageView*)(m_ui->findWidgetById("rect_bg"));
	pBgRect->removeChildByTag(100);

	//裁切区域
	CCClippingNode* pClip = CCClippingNode::create();
	pClip->setPosition(CCPointZero);
	pClip->setTag(100);
	CImageView* pImage = UICloneMgr::cloneImageView(pBgRect);
	pImage->setAnchorPoint(CCPointZero);
	pImage->setPosition(CCPointZero);
	pClip->setStencil(pImage);
	pBgRect->addChild(pClip, 99);

	const CHeroBodyShowInfo& bodyData = m_heroBodyShowdData->getDataById(hero->thumb);
	CCSprite *spr = CCSprite::create(CCString::createWithFormat("selectRole/%d.png",hero->thumb)->getCString());
	if (spr)
	{	
		CCTextureCache::sharedTextureCache()->removeTextureForKey(CCString::createWithFormat("selectRole/%d.png",hero->thumb)->getCString());
		spr->setPosition(pBgRect->convertToNodeSpace(m_ui->convertToWorldSpace(bodyData.pos)));
		spr->setScale(bodyData.fScale);
		spr->setScaleX(spr->getScaleX()/pBgRect->getScaleX());
		spr->setFlipX(bodyData.bFlipX);
		spr->setRotation(bodyData.iRoate);
		pClip->addChild(spr);
	}

	updateBgColor(hero->roletype);

	//更新信息
	if(data)
	{
		m_pInfo1->setString(CCString::createWithFormat("%s(%s)", data->heroName.c_str(), GETLANGSTR(hero->roletype+1189))->getCString());
		if(hero->roletype==1) m_pInfo1->setColor(ccc3(220, 82, 31));
		if(hero->roletype==2) m_pInfo1->setColor(ccc3(91, 162, 247));
		if(hero->roletype==3) m_pInfo1->setColor(ccc3(255, 234, 19));

		std::string sInfo = data->heroStory;
		for(unsigned int i=0; i<sInfo.size(); i++)
		{
			if(sInfo[i] == '|')
			{
				sInfo[i] = '\n';
			}
		}
		m_pInfo2->setString(sInfo.c_str());
		updateShowInfoScroll();
	}
}

void CHeroAttribute::updateHeroExp(const TMessage& tMsg)
{
//	CProgressBar *expBar = (CProgressBar*)(m_ui->findWidgetById("exp"));
// 	CLabel *explab = (CLabel*)(m_ui->findWidgetById("expVal"));
// 	//add exp
// 	if (tMsg.nMsg == 1)
// 	{
//      expBar->setValue(m_selectHero->exp + tMsg.intParam);
// 		explab->setString(CCString::createWithFormat("%d/%d",m_selectHero->exp+tMsg.intParam,m_selectHero->nextExp)->getCString());
// 	}

}

void CHeroAttribute::hideHeroOperation()
{
	CRadioButton* advanceBtn =(CRadioButton*)(m_ui->getChildByTag(4));
	advanceBtn->setVisible(false);

	CRadioButton* expbtn = (CRadioButton*)(m_ui->findWidgetById("expbtn"));
	expbtn->setVisible(false);
	CButton* upbtn = (CButton*)(m_ui->findWidgetById("upbtn"));
	upbtn->setVisible(false);
	CButton* leftbtn = (CButton*)(m_ui->findWidgetById("left"));
	leftbtn->setVisible(false);
	CButton* rightbtn = (CButton*)(m_ui->findWidgetById("right"));
	rightbtn->setVisible(false);

	CImageView *img1 = (CImageView*)(m_ui->findWidgetById("itembg1"));
	img1->setOnClickListener(this, ccw_click_selector(CHeroAttribute::onItemProperity));

	CImageView *img2 = (CImageView*)(m_ui->findWidgetById("itembg2"));
	img2->setOnClickListener(this, ccw_click_selector(CHeroAttribute::onItemProperity));

	CRadioButton* promo_font = (CRadioButton*)(m_ui->findWidgetById("promo_font"));
	promo_font->setVisible(false);
	CRadioButton* uplev_font = (CRadioButton*)(m_ui->findWidgetById("uplev_font"));
	uplev_font->setVisible(false);
}

void CHeroAttribute::onItemProperity(CCObject *pSender)
{

}

void CHeroAttribute::showSelectRadioImg(int selIndex)
{

}

void CHeroAttribute::levelSkillUp(CHero* hero,const Hero* pHero)
{
	int atk = pHero->atk() - hero->atk;
	int hp = pHero->hp() - hero->hp;
	int def = pHero->def() - hero->def;
	const char *str = nullptr;
	if (atk>0)
	{
		PlayEffectSound(SFX_431);

		str = CCString::createWithFormat(GETLANGSTR(278), atk)->getCString();
		CLabel *lab = CLabel::create(str,DEFAULT_FONT,20);
		lab->setPosition(ccp(VCENTER.x+380,VCENTER.y-150));
		lab->setColor(RGB_GREEN);
		runAnimation(0.5f,lab);
	}
	if (hp>0)
	{
		str = CCString::createWithFormat(GETLANGSTR(279),hp)->getCString();
		CLabel *lab = CLabel::create(str,DEFAULT_FONT,20);
		lab->setPosition(ccp(VCENTER.x+380,VCENTER.y-150));
		lab->setColor(RGB_GREEN);
		runAnimation(0.8f,lab);
	}
	if (def>0)
	{
		str = CCString::createWithFormat(GETLANGSTR(280),def)->getCString();
		CLabel *lab = CLabel::create(str,DEFAULT_FONT,20);
		lab->setPosition(ccp(VCENTER.x+380,VCENTER.y-150));
		lab->setColor(RGB_GREEN);
		runAnimation(1.1f,lab);
	}

	bool isUp = pHero->skill1().level()>hero->skill1.level||pHero->skill2().level()>hero->skill2.level||
		pHero->skill3().level()>hero->skill3.level||pHero->skill4().level()>hero->skill4.level||pHero->skill5().level()>hero->skill5.level;
	if (isUp)
	{
		CCSprite *skillspr = CCSprite::create("common/word_06.png");
		skillspr->setPosition(ccp(VCENTER.x+380,VCENTER.y-150));
		runAnimation(1.5f,skillspr);
	}
}

void CHeroAttribute::runAnimation(float dt, CCNode * node)
{
	this->addChild(node);
	node->setVisible(false);
	CCFiniteTimeAction* fly = CCSpawn::create(
		CCMoveBy::create(1.3f, ccp(0, 130)),
		CCSequence::create(CCScaleTo::create(0.15f,1.1f),CCDelayTime::create(0.5f),CCScaleTo::create(0.15f,1.0f),CCFadeTo::create(0.3f, 150), NULL),
		NULL);
	CCFiniteTimeAction* action = CCSequence::create(CCDelayTime::create(dt),CCShow::create(),fly,CCRemoveSelf::create(),
		NULL);
	node->runAction(action);
}

void CHeroAttribute::callbackForHideBackLayer()
{
	//隐藏被遮盖的层，不渲染
	LayerManager::instance()->hideLayerBeCovered(this);

	//showBookAnimate(m_ui);
}

void CHeroAttribute::movementCallBack( CCArmature* pAramture, MovementEventType type, const char* sData )
{
	switch (type)
	{
	case cocos2d::extension::START:
		break;
	case cocos2d::extension::LOOP_COMPLETE:
	case cocos2d::extension::COMPLETE:
		{
			//动作1
			float fRand = CCRANDOM_0_1();
			if(fRand>0.7)
			{
				if(m_pArmature->getAnimation()->getAnimationData()->getMovement(Stand_Action))
				{
					m_pArmature->getAnimation()->play(Stand_Action, 0.01f);
					return;
				}
			}
			else if(fRand>0.5)
			{
				if(m_pArmature->getAnimation()->getAnimationData()->getMovement(Attack_Action))
				{
					m_pArmature->getAnimation()->play(Attack_Action, 0.01f);
					return;
				}
			}
			else if(fRand>0.3f)
			{
				if(m_pArmature->getAnimation()->getAnimationData()->getMovement(SpAttack_Action))
				{
					m_pArmature->getAnimation()->play(SpAttack_Action, 0.01f);
					return;
				}
			}
			else if(fRand>0.1f)
			{
				if(m_pArmature->getAnimation()->getAnimationData()->getMovement(Win_Action))
				{
					m_pArmature->getAnimation()->play(Win_Action, 0.01f);
					return;
				}
			}

			if(m_pArmature->getAnimation()->getAnimationData()->getMovement(Walk_Action))
			{
				m_pArmature->getAnimation()->play(Walk_Action, 0.01f);
			}
		}
		break;
	default:
		break;
	}
}

void CHeroAttribute::compaseSuccess( CCObject* pObj )
{
	m_selectHero->itemNum1++;
	CLabel *explab = (CLabel*)(m_ui->findWidgetById("expVal"));
	explab->setString(CCString::createWithFormat("%d/%d",m_selectHero->itemNum1,m_selectHero->itemNum2)->getCString());
	CImageViewScale9* process1 = (CImageViewScale9 *)(m_ui->findWidgetById("exp_process"));
	float pe = (float)m_selectHero->itemNum1/m_selectHero->itemNum2;
	if (pe>1)
		pe = 1;
	process1->setScaleX(pe);
}

void CHeroAttribute::bgHideOrShow(bool bShow)
{
	CCSprite* pBgRectCopy = (CCSprite*)(m_ui->findWidgetById("rect_bg_copy"));
	CCSprite* pBgRect = (CCSprite*)(m_ui->findWidgetById("rect_bg"));
	CCSprite* pFgRect1 = (CCSprite*)(m_ui->findWidgetById("rect_fg_1"));
	CCSprite* pDao = (CCSprite*)(m_ui->findWidgetById("dao"));
	CCNode* pRange = (CCNode*)(m_ui->findWidgetById("book"));
	CCSprite* pCost = (CCSprite*)(m_ui->findWidgetById("cost"));
	CCNode* pMaskInfo = (CCNode*)m_ui->findWidgetById("mask_info");
	CCheckBox* pBook = (CCheckBox*)(m_ui->findWidgetById("book"));

	if(pBook->isChecked())
	{
		m_show_info_scroll->setVisible(bShow);
		pMaskInfo->setVisible(bShow);
	}
	
	pBgRectCopy->setVisible(!bShow);
	pBgRect->setVisible(bShow);
	pFgRect1->setVisible(bShow);
	pDao->setVisible(bShow);
	pRange->setVisible(bShow);
	pCost->setVisible(bShow);


	if(bShow)
	{

		const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(m_selectHero->thumb);

		//品质
		if(data)
		{
			CCSprite *type = (CCSprite*)(m_ui->findWidgetById("type"));
			type->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png",m_selectHero->roletype, data->iType2)->getCString()));
		}

		//立绘
		CImageView *pBgRect = (CImageView*)(m_ui->findWidgetById("rect_bg"));
		pBgRect->removeChildByTag(100);

		//裁切区域
		CCClippingNode* pClip = CCClippingNode::create();
		pClip->setPosition(CCPointZero);
		pClip->setTag(100);
		CImageView* pImage = UICloneMgr::cloneImageView(pBgRect);
		pImage->setAnchorPoint(CCPointZero);
		pImage->setPosition(CCPointZero);
		pClip->setStencil(pImage);
		pBgRect->addChild(pClip, 99);

		const CHeroBodyShowInfo& bodyData = m_heroBodyShowdData->getDataById(m_selectHero->thumb);
		CCSprite *spr = CCSprite::create(CCString::createWithFormat("selectRole/%d.png",m_selectHero->thumb)->getCString());
		if (spr)
		{	
			CCTextureCache::sharedTextureCache()->removeTextureForKey(CCString::createWithFormat("selectRole/%d.png",m_selectHero->thumb)->getCString());
			spr->setPosition(pBgRect->convertToNodeSpace(m_ui->convertToWorldSpace(bodyData.pos)));
			spr->setScale(bodyData.fScale);
			spr->setScaleX(spr->getScaleX()/pBgRect->getScaleX());
			spr->setFlipX(bodyData.bFlipX);
			spr->setRotation(bodyData.iRoate);
			pClip->addChild(spr);
		}
	}
}

void CHeroAttribute::updateBgColor( int iType )
{
	CCSprite* pBgRect = (CCSprite*)(m_ui->findWidgetById("rect_bg"));
	pBgRect->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("hero/pattern_%d.png", iType)->getCString()));
}

void CHeroAttribute::onTipsForUnlockArmor( CCObject *pSender )
{
	CCNode* pNode = (CCNode*)pSender;
	int iTag = pNode->getTag();
	ShowPopTextTip(CCString::createWithFormat(GETLANGSTR(226), HERO_ARMOR_LOCK_LEVEL[iTag])->getCString());
	CCLOG("CHeroAttribute::onTipsForUnlockArmor");
}

void CHeroAttribute::updateStarRank( CHero* hero )
{
	CImageView* pImageView = (CImageView*)(m_ui->findWidgetById("bg_black"));
	CLayout* pStarLay = getStarLayoutWithBlackBase(hero->quality, 1.0f);
	pImageView->removeAllChildren();
	pStarLay->setPosition(pStarLay->getPosition()+ccp(55, -5));
	pImageView->addChild(pStarLay);
}

void CHeroAttribute::showHeroBook( CCObject* pSender, bool bChecked )
{
	PlayEffectSound(SFX_429);

	CCLOG("CHeroAttribute::showHeroBook");

	m_show_info_scroll->setVisible(bChecked);

	CCNode* pMaskInfo = (CCNode*)m_ui->findWidgetById("mask_info");
	pMaskInfo->setVisible(bChecked);
}

void CHeroAttribute::updateShowInfoScroll()
{
	CCSize contentSize = m_show_info_scroll->getContentSize();
	
	int iBaseHeight = contentSize.height;

	//计算文字占高度
	int iHeight1 = m_pInfo1->getContentSize().height;
	int iHeight2 = m_pInfo2->getContentSize().height;

	int iHeightGap = 150;

	contentSize.height = iHeight1 + iHeight2 + iHeightGap;

	contentSize.height  = contentSize.height < iBaseHeight?iBaseHeight:contentSize.height;

	m_pInfo1->setPosition(ccp(10, contentSize.height-65));
	m_pInfo2->setPosition(m_pInfo1->getPosition()-ccp(0, 40));

	m_show_info_scroll->setContainerSize(contentSize);
	m_show_info_scroll->setContentOffsetToTop();
}