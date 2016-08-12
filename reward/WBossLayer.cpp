#include "WBossLayer.h"
#include "model/DataDefine.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "tools/UICloneMgr.h"
#include "common/CommonFunction.h"
#include "tools/ShowTexttip.h"
#include "hero/SkillDesc.h"
#include "common/CheckMoney.h"
#include "common/CShowToBuyResource.h"
#include "Battle/BattleLayer/StoryLayer.h"
#include "reward/SkillInfo.h"
#include "tollgate/SelectArmy.h"
#include "netcontrol/CPlayerControl.h"
#include "model/DataCenter.h"
#include "Battle/WarManager.h"
#include "reward/WorldBoss.h"
#include "Battle/EffectData.h"
#include "Battle/ActionNameDefine.h"
#include "Global.h"
#include "Battle/BattleCenter.h"
using namespace BattleSpace;
bool WBossSortWarPrize(const Prize& data1, const Prize& data2)
{
	return data1.type()<data2.type();
}

CWBossLayer::CWBossLayer():
m_ui(nullptr),m_iBossCome(0),m_bFightedBoss(false), 
m_iNextChallengeSecond(0), m_iBossSecond(0), m_iResetNeedGold(0), m_iInspireAdd(0), 
m_pLayerShow(nullptr),m_iInspireCost(0),m_iBuffId(0),m_pSkillInfo(nullptr),m_pArmature(nullptr)
,m_pStoryData(nullptr)
{

}

CWBossLayer::~CWBossLayer()
{
	CC_SAFE_RELEASE(m_pStoryData);
}

bool CWBossLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//内容
		m_ui = LoadComponent("wBoss.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}

	return false;
}

void CWBossLayer::onEnter()
{
	BaseLayer::onEnter();

	//几个按钮
	CButton* pBtnReset = (CButton*)m_ui->findWidgetById("reset");
	pBtnReset->setOnClickListener(this, ccw_click_selector(CWBossLayer::reset));
	pBtnReset->getSelectedImage()->setScale(1.1f);

	CButton* pBtnBattle = (CButton*)m_ui->findWidgetById("fight");
	pBtnBattle->setOnClickListener(this, ccw_click_selector(CWBossLayer::onBattle));
	pBtnBattle->getSelectedImage()->setScale(1.1f);

	CButton* pBufferBuy = (CButton*)m_ui->findWidgetById("inspire_btn");
	pBufferBuy->setOnClickListener(this, ccw_click_selector(CWBossLayer::inspire));

	CImageView* pRank = (CImageView*)m_ui->findWidgetById("rank_bg");
	pRank->setTouchEnabled(true);
	pRank->setOnClickListener(this, ccw_click_selector(CWBossLayer::showRank));

	//击杀获得BUFF
	CImageView* pRewardBox1 = (CImageView*)m_ui->findWidgetById("reward");
	pRewardBox1->setTouchEnabled(true);
	pRewardBox1->setOnPressListener(this, ccw_press_selector(CWBossLayer::pressRewardBox));

	CImageView* pRewardBox2 = (CImageView*)m_ui->findWidgetById("reward_2");
	pRewardBox2->setTouchEnabled(true);
	pRewardBox2->setOnPressListener(this, ccw_press_selector(CWBossLayer::pressRewardBox));

	//绑定活动列表回调
	GetTcpNet->registerMsgHandler(BossDataMsg, this, CMsgHandler_selector(CWBossLayer::processNetMsg));
	GetTcpNet->registerMsgHandler(BuyInspireMsg, this, CMsgHandler_selector(CWBossLayer::processNetMsg));
	GetTcpNet->registerMsgHandler(ResetBattleTimeMsg, this, CMsgHandler_selector(CWBossLayer::processNetMsg));

	NOTIFICATION->addObserver(this, callfuncO_selector(CWBossLayer::timeInBackground), TIME_IN_BACKGROUND, nullptr);
}

void CWBossLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
	//解绑场景事件监听
	CSceneManager::sharedSceneManager()->removeMsgObserver("NULL", this);

	if(dynamic_cast<CCArmature*>(m_pArmature))
	{
		char str[60]={0};
		sprintf(str,"BoneAnimation/%d.ExportJson", m_pBoss.thumb);
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(str);
	}

	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	NOTIFICATION->removeAllObservers(this);
}

void CWBossLayer::processNetMsg( int type, google::protobuf::Message *msg )
{
	
	if(!isVisible())
	{
		this->setVisible(true);
		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
		LayerManager::instance()->getLayer("CWorldBoss")->setVisible(true);
	}
	
	if(!msg) return;

	switch (type)
	{
	case BossDataMsg:
		{
			BossData* pData = (BossData*)msg;

			switch (pData->result())//(1 正常，2 等级未达到，3 没有世界BOSS数据，4 其它错误)
			{
			case 1:
				{
					updateData(pData);
					schedule(schedule_selector(CWBossLayer::updateOneSecond), 1.0f);
					updateOneSecond(0);//先刷新一次

					////出动画
					//if(m_bBossCome)
					//{
					//	CWorldBoss* pWorldBoss =  dynamic_cast<CWorldBoss*>(LayerManager::instance()->getLayer("CWorldBoss"));
					//	if(pWorldBoss)
					//	{
					//		pWorldBoss->showWorldBossEffect(m_pBoss.thumb);
					//	}
					//}
				}break;
			case 2:
				{

				}break;
			case 3:
				{

				}break;
			case 4:
				{
					ShowPopTextTip(GETLANGSTR(170));
				}break;
			default:
				break;
			}
		}
		break;
	case BuyInspireMsg:
		{
			BuyInspire* pData = (BuyInspire*)msg;
			switch (pData->result()) //(1 成功，2 金钱不足，3 数据错误)
			{
			case 1:
				{
					UserData *data = DataCenter::sharedData()->getUser()->getUserData();
					data->setRoleGold(data->getRoleGold()-m_iInspireCost);
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO, 0, nullptr,nullptr);

					m_iInspireCost = pData->inspireprice();
					m_iInspireAdd = pData->inspire();
					updateInspireLabel(true);
				}break;
			case 2:
				{
					CShowToBuyResource* pShow = CShowToBuyResource::create();
					pShow->showToBuyResourceByType(ShowBuyResourceGold);

				}break;
			case 3:
				{
					ShowPopTextTip(GETLANGSTR(170));
				}break;
			default:
				break;
			}
		}
		break;
	case ResetBattleTimeMsg:
		{
			ResetBattleTime* pData = (ResetBattleTime*)msg;
			if(pData->result() == 1)
			{
				UserData *data = DataCenter::sharedData()->getUser()->getUserData();
				data->setRoleGold(data->getRoleGold()-m_iResetNeedGold);
				CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO, 0, nullptr,nullptr);

				m_iNextChallengeSecond = 0;
				updateOneSecond(0);
			}
		}
		break;
	default:
		break;
	}
}


void CWBossLayer::updateOneSecond( float dt )
{
	if(isVisible())
	{
		m_iBossSecond--;

		//BOSS来了
		if(isBossCome())
		{
			updateBossWillLeaveLabel();
			
			if(m_iBossSecond<=0)
			{
				//BOSS离开，重新刷界面
				unschedule(schedule_selector(CWBossLayer::updateOneSecond));
				AskForWorldBoss();
			}

			//重置挑战刷新
			m_iNextChallengeSecond--;
			if(m_iNextChallengeSecond<0) m_iNextChallengeSecond = 0;
			updateResetFightLabel();

		}
		//BOSS没来
		else
		{
			updateBossWillComeLabel();

			if(m_iBossSecond<=0)
			{
				//BOSS到来，重新刷界面
				unschedule(schedule_selector(CWBossLayer::updateOneSecond));
				AskForWorldBoss();
			}
		}
	}
}

void CWBossLayer::updateData(BossData* pData)
{
	//BOSS是否来临，BOSS
	m_iBossCome = pData->open();

	//是否挑战过BOSS
	if (pData->cdata().rank())
	{
		m_bFightedBoss = true;
	}
		
	//几点出现/几点消失
	m_iBossSecond = pData->bosstime()+2;

	//世界BOSS数据
	m_pBoss.readData(pData->boss());

	m_iInspireCost = pData->cdata().inspireprice();

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	//Layer1
	CLayout* pLayer1 = (CLayout*)m_ui->findWidgetById("layer_rect_1");

	//Layer2
	CLayout* pLayer2 = (CLayout*)m_ui->findWidgetById("layer_rect_2");

	m_pLayerShow = isBossCome()?pLayer1:pLayer2;

	//世界BOSS来临//战斗状态(1 世界BOSS进行中，2 未开启，3 已结束)
	if(isBossCome())
	{
		//更新世界BOSS数据
		updateDataBossCome(pData);

		CButton* pBtnReset = (CButton*)m_ui->findWidgetById("reset");
		CButton* pBtnBattle = (CButton*)m_ui->findWidgetById("fight");
		CLabel* pLabel = (CLabel*)m_ui->findWidgetById("reset_time");
		pLabel->setVisible(true);
		pBtnReset->setVisible(true);
		pBtnBattle->setVisible(true);
		pLayer1->setVisible(true);
		pLayer2->setVisible(false);
	}
	//没有来临
	else
	{
		updateDataBossWait(pData);

		CButton* pBtnReset = (CButton*)m_ui->findWidgetById("reset");
		CButton* pBtnBattle = (CButton*)m_ui->findWidgetById("fight");
		CLabel* pLabel = (CLabel*)m_ui->findWidgetById("reset_time");
		pLabel->setVisible(false);
		pBtnReset->setVisible(false);
		pBtnBattle->setVisible(false);
		pLayer1->setVisible(false);
		pLayer2->setVisible(true);
	}
}


void CWBossLayer::updateDataBossCome( BossData* pData )
{
	//几点出现
	m_iBossSecond = pData->bosstime()+2;

	//如果BOSS已经死掉，离开变结算
	if(m_iBossCome == 3)
	{
		CLabel* pTitle = (CLabel*)(m_ui->findWidgetById("title_tip"));
		pTitle->setString(GETLANGSTR(1160));
	}

	//Fight
	CCLayer* pFight = (CCLayer*)m_ui->findWidgetById("fight");
	
	//挑战过世界BOSS，显示排名信息
	CImageView* pT1 = (CImageView*)m_ui->findWidgetById("text_1");
	CImageView* pT2 = (CImageView*)m_ui->findWidgetById("text_2");
	//Rank
	CCLayer* pRank= (CCLayer*)m_ui->findWidgetById("layer_rank");
	if(m_bFightedBoss)
	{
		pT1->setVisible(false);
		pT2->setVisible(false);
		pRank->setVisible(true);
	}
	else
	{
		pT1->setVisible(true);
		pT2->setVisible(true);
		pRank->setVisible(false);
	}

	//更新名字
	CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
	const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(m_pBoss.thumb);

	if(data)
	{
		name->setString(data->heroName.c_str());
	}

	//类型
	if(data && m_pBoss.roletype>0)
	{
		CCSprite* pType = (CCSprite*)(m_ui->findWidgetById("type"));
		CCString* pStr = CCString::createWithFormat("common/type_%d_%d.png", m_pBoss.roletype, data->iType2);
		pType->setTexture(CCTextureCache::sharedTextureCache()->addImage(pStr->getCString()));
	}

	//描述
	CLabel * pDescribe = (CLabel*)(m_ui->findWidgetById("describe"));
	if(data)
	{
		pDescribe->setString(data->heroDesc.c_str());
	}

	//贴图
	updateBossTexture();

	//技能3个
	for(unsigned int i=0; i<3; i++)
	{
		CSkill* pSkill;
		if(i==0) pSkill = &m_pBoss.skill1;
		if(i==1) pSkill = &m_pBoss.skill2;
		if(i==2) pSkill = &m_pBoss.skill3;

		CImageView* pBg = (CImageView*)m_ui->findWidgetById(CCString::createWithFormat("itembg%d", i+1)->getCString());

		CImageView* pRect = (CImageView*)m_ui->findWidgetById(CCString::createWithFormat("mask%d", i+1)->getCString());
		pRect->setTouchEnabled(true);
		pRect->setOnPressListener(this, ccw_press_selector(CWBossLayer::pressSkill));	
		pRect->setUserData(pSkill);
		pRect->setTag(i);
		pRect->setUserObject(CCBool::create(true));

		const SkillCfg * cfg = DataCenter::sharedData()->getSkill()->getCfg(pSkill->skillId);

		//贴图
		CCSprite* pIcon = CImageView::create(CCString::createWithFormat("skillIcon/%d.png", cfg->icon)->getCString());
		if(!pIcon)
		{
			CCLOG("ERROR  CWBossLayer::updateData");
			pIcon  = CImageView::create("skillIcon/100000.png");
		}	
		pBg->addChild(pIcon);
		NodeFillParent(pIcon);
	}

	//狂暴状态
	CImageView* pBg = (CImageView*)m_ui->findWidgetById("itembg4");
	CImageView* pRect = (CImageView*)m_ui->findWidgetById("mask4");
	pRect->setTag(3);
	pRect->setTouchEnabled(true);
	pRect->setOnPressListener(this,ccw_press_selector(CWBossLayer::pressSkill));	
	pRect->setUserObject(CCBool::create(true));
	//贴图
	CCSprite* pIcon = CImageView::create("skillIcon/10000.png");
	pBg->addChild(pIcon);
	NodeFillParent(pIcon);
	//等级
	CLabel* pAngryLevel = (CLabel*)m_pLayerShow->findWidgetById("angry_level");
	pAngryLevel->setString(ToString(m_pBoss.level));


	//挑战， 重置按钮， 挑战间隔13:10
	m_iResetNeedGold = pData->resetgold();
	m_iNextChallengeSecond = pData->cdata().againtime();
	updateResetFightLabel();

	//如果挑战过，排名，累计伤害，获得奖励
	if(m_bFightedBoss)
	{
		//排名
		int iRank = pData->cdata().rank();
		CCSprite* pRankIcon = (CCSprite*)(m_pLayerShow->findWidgetById("rank_icon"));
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

		//累计伤害
		int iHurt = pData->cdata().hurt();
		CCSprite* pDamageBg= (CCSprite*)(m_pLayerShow->findWidgetById("text"));
		pDamageBg->setVisible(false);
		CLabelAtlas* pDamage = CLabelAtlas::create("", "worldBoss/no_03.png", 32, 49, 48);
		pDamage->setString(CCString::createWithFormat("%d", iHurt)->getCString());
		pDamage->setPosition(pDamageBg->getPosition()+ccp(0, 26));
		pDamage->setAnchorPoint(ccp(0.5f, 0.5f));
		pDamageBg->getParent()->addChild(pDamage, pDamageBg->getZOrder()+1);

		//奖励
		vector<Prize> vecWarPrize;
		for(unsigned int b=0; b<pData->cdata().prizes().size();b++)
		{
			vecWarPrize.push_back(pData->cdata().prizes().Get(b));
		}
		std::sort(vecWarPrize.begin(), vecWarPrize.end(), WBossSortWarPrize);
		for(unsigned int j=0; j<vecWarPrize.size(); j++)
		{
			if(j==2)break;

			const Prize& prize = vecWarPrize[j];

			CLabel* pPrize = (CLabel*)m_pLayerShow->findWidgetById(CCString::createWithFormat("icon_text_%d", j+1)->getCString());
			pPrize->setString(CCString::createWithFormat("%d", prize.num())->getCString());

			CCSprite* pIcon = (CCSprite *)m_pLayerShow->findWidgetById(CCString::createWithFormat("icon_%d", j+1)->getCString());
			pIcon->setOpacity(0);
			CImageView *head = CImageView::create(GetImageName(prize.type(), prize.color(), prize.thumb()).c_str());
			if (!head)
			{
				head = CImageView::create("prop/0.png");
				CCLOG("[ ERROR ] CWRuleLayer::initRank Lost Image");
			}
			head->setPosition(ccp(pIcon->getContentSize().width/2, pIcon->getContentSize().height/2));
			pIcon->addChild(head);
		}
	}

	//Buff购买
	m_iInspireAdd = pData->cdata().inspire();
	updateInspireLabel();
}


void CWBossLayer::updateDataBossWait( BossData* pData )
{
	//变黑
	CImageView* pT1 = (CImageView*)m_ui->findWidgetById("shadow");
	if(!pT1)
	{
		CCLOG("[ ERROR ]--CWBossLayer::updateDataBossWait--no child named 'shadow' ");
		return;
	}

	pT1->setColor(ccc3(0, 0, 0));
	//pT1->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeTo::create(0.3f, 120), CCFadeTo::create(0.3f, 255))));
	pT1->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(0.25f, 1.05f), CCScaleTo::create(0.25f, 1.0f), nullptr)));

	pT1->setTouchEnabled(true);
	pT1->setOnClickListener(this, ccw_click_selector(CWBossLayer::showStory));

	//换贴图
	CCString* pTextureStr = CCString::createWithFormat("selectRole/%d.png", m_pBoss.thumb);
	std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pTextureStr->getCString());
	if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
	{
		pT1->setTexture(CCTextureCache::sharedTextureCache()->addImage(pTextureStr->getCString()));
	}

	CCNode* pParent = pT1->getParent();
	pT1->retain();
	pT1->removeFromParentAndCleanup(false);
	CCNode* pNewPt1 = packageWithClippingNode(pT1);
	pParent->addChild(pNewPt1,  -1);
	pT1->release();
}

std::string CWBossLayer::secondToString1( int iSecond )
{
	int iDay = iSecond/86400;
	int iHour = (iSecond%86400)/3600;
	int iMinute = (iSecond%3600)/60;
	int iSec = iSecond%60;
	return CCString::createWithFormat("%d%s%d%s%d%s%d%s", iDay, GETLANGSTR(1130), iHour, GETLANGSTR(1131), iMinute, GETLANGSTR(1132), iSec, GETLANGSTR(1133))->m_sString;
}


std::string CWBossLayer::secondToString2( int iSecond )
{
	int iHour = iSecond/3600;
	int iMinute = (iSecond%3600)/60;
	int iSec = iSecond%60;
	return CCString::createWithFormat("%s%d:%s%d:%s%d", iHour>9?"":"0", iHour, iMinute>9?"":"0", iMinute, iSec>9?"":"0", iSec)->m_sString;
}

std::string CWBossLayer::secondToString3( int iSecond )
{
	int iMinute = iSecond/60;
	int iSec = iSecond%60;
	return CCString::createWithFormat("%s%d:%s%d", iMinute>9?"":"0", iMinute, iSec>9?"":"0", iSec)->m_sString;
}


void CWBossLayer::updateBossWillLeaveLabel()
{
	CLabel* pLabel = (CLabel*)m_pLayerShow->findWidgetById("time_remain");
	std::string dayTime = secondToString2(m_iBossSecond);
	pLabel->setString(CCString::createWithFormat("%s", dayTime.c_str())->getCString());
}

void CWBossLayer::updateBossWillComeLabel()
{
	CLabel* pLabel = (CLabel*)m_pLayerShow->findWidgetById("title_tip1");
	std::string dayTime = secondToString1(m_iBossSecond);
	const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(m_pBoss.thumb);
	std::string sName = "";
	if(data)
	{
		sName = data->heroName;
	}
	pLabel->setString(CCString::createWithFormat(GETLANGSTR(1134), sName.c_str(), dayTime.c_str())->getCString());
}


void CWBossLayer::updateResetFightLabel()
{
	CButton* pBtnReset = (CButton*)m_ui->findWidgetById("reset");
	CButton* pBtnBattle = (CButton*)m_ui->findWidgetById("fight");
	CLabel* pLabel = (CLabel*)m_ui->findWidgetById("reset_time");
	if(m_iNextChallengeSecond<=0)
	{
		pLabel->setVisible(false);
		pBtnReset->setVisible(false);
		pBtnBattle->setVisible(true);
	}
	else
	{
		pLabel->setVisible(true);
		pBtnReset->setVisible(true);
		pBtnBattle->setVisible(false);

		std::string dayTime = secondToString3(m_iNextChallengeSecond);
		pLabel->setString(dayTime.c_str());
	}
}


void CWBossLayer::updateInspireLabel(bool bWithAction)
{
	CLabel* pPrize = (CLabel*)m_pLayerShow->findWidgetById("inspire_text");
	if(m_iInspireAdd == 0)
	{
		pPrize->setString(GETLANGSTR(1161));
	}
	else
	{
		pPrize->setVisible(true);
		pPrize->setString(CCString::createWithFormat(GETLANGSTR(1135), m_iInspireAdd)->getCString());
		if(bWithAction)
		{
			pPrize->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.3f), CCScaleTo::create(0.1f, 1.0f)));
		}
	}
}


void CWBossLayer::onClickSkill( CCObject* pSender )
{
	PlayEffectSound(SFX_Button);

	CCNode *node = (CCNode*)pSender;
	CSkill *skill = (CSkill*)node->getUserData();

	bool isPasssive = ((CCBool*)node->getUserObject())->getValue();	
	int idx = ((CTableViewCell*)node->getParent())->getIdx();

	CSkillDesc *skillDesc= CSkillDesc::create();
	LayerManager::instance()->push(skillDesc);
	skillDesc->updateSkillInfo(skill,isPasssive, idx);
}

void CWBossLayer::reset( CCObject* pSender )
{
	PlayEffectSound(SFX_Button);

	ShowConfirmTextTip(CCString::createWithFormat(GETLANGSTR(1137), m_iResetNeedGold)->getCString(),this, ccw_click_selector(CWBossLayer::resetConfirm));
}

void CWBossLayer::onBattle( CCObject* pSender )
{
	PlayEffectSound(SFX_Button);

	if(m_iBossCome==3)
	{
		ShowPopTextTip(GETLANGSTR(1158));
		return;
	}

	BattleManage->setWorldBossRank(m_pBoss.level);

	CSelectArmy *selArmy = CSelectArmy::create();
	selArmy->setWordBoss(m_pBoss.mId);
	LayerManager::instance()->push(selArmy);
	CPlayerControl::getInstance().sendUnion(0, 0, true);
}

void CWBossLayer::inspire( CCObject* pSender )
{
	PlayEffectSound(SFX_Button);

	ShowConfirmTextTip(CCString::createWithFormat(GETLANGSTR(1136), m_iInspireCost)->getCString(),this, ccw_click_selector(CWBossLayer::inspireConfirm));
}

void CWBossLayer::showRank( CCObject* pSender )
{
	PlayEffectSound(SFX_Button);
}

void CWBossLayer::inspireConfirm( CCObject* pSender )
{
	CButton* pBtn = (CButton*)pSender;

	if(pBtn->getTag()==PopTipConfirm)
	{
		//检查货币
		if(!CheckGold(m_iInspireCost))
		{
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
			return;
		}

		AskForBuyInspire();
	}

	((CPopTip*)(pBtn->getParent()->getParent()))->onClose(nullptr);
}

void CWBossLayer::resetConfirm( CCObject* pSender )
{
	CButton* pBtn = (CButton*)pSender;

	if(pBtn->getTag()==PopTipConfirm)
	{
		//检查货币
		if(!CheckGold(m_iResetNeedGold))
		{
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
			return;
		}

		AskForRestChallengeTime();
	}

	((CPopTip*)(pBtn->getParent()->getParent()))->onClose(nullptr);
}


void CWBossLayer::showStory( CCObject* pSender )
{
	StoryLayer* pLayer = dynamic_cast<StoryLayer*>(CCDirector::sharedDirector()->getRunningScene()->getChildByTag(627));
	if(pLayer==nullptr)
	{
		pLayer = StoryLayer::create();
		pLayer->setVisible(false);
		pLayer->setTouchEnabled(false);
		this->getParent()->getParent()->addChild(pLayer, 627, 627);
	}

	if(m_pStoryData==nullptr)
	{
		m_pStoryData = new BattleSpace::StoryData;
		CC_SAFE_RETAIN(m_pStoryData);
		std::string sFile = CCString::createWithFormat("csv/story/boss_%d.json", m_pBoss.thumb)->m_sString;
		std::string sFullPathName = CCFileUtils::sharedFileUtils()->fullPathForFilename(sFile.c_str());
		if(!CCFileUtils::sharedFileUtils()->isFileExist(sFullPathName.c_str()))
		{
			sFile = "csv/story/boss_1101.json";
		}
		m_pStoryData->LoadFile(sFile.c_str(), (int)StoryType::eBeginStory);
	}

	pLayer->CreateStoryWithStoryData(CCInteger::create((int)StoryType::eBeginStory), m_pStoryData);

	pLayer->hideJumpBtn();
}

void CWBossLayer::SpineComplete( int trackIndex,int loopCount )
{
	if(!m_pSkillInfo||m_pSkillInfo->isVisible())
		return;
	SkeletonAnimation* skeletonNode = dynamic_cast<SkeletonAnimation*>(m_pArmature);
	if (skeletonNode)
		skeletonNode->setAnimation(0,Stand_Action,true);
}

void CWBossLayer::updateBossTexture()
{
	CCSprite *role = (CCSprite*)(m_ui->findWidgetById("ying"));
	//使用模型代替贴图
	int m_ModeID = m_pBoss.thumb;
	
	if (BattleManage->isSpine(m_ModeID))
	{ 
		char json[60] = {0};
		char altlas[60] = {0};
		sprintf(json,"Spine/%d.json",m_ModeID);
		sprintf(altlas,"Spine/%d.atlas",m_ModeID);
		std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(altlas);
		if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		{
			SkeletonAnimation * skeletonNode = SkeletonAnimation::createWithFile(json, altlas, 1);
			skeletonNode->setAnimation(0,Stand_Action,true);
			role->getParent()->removeChildByTag(100);
			skeletonNode->setTag(100);
			CCPoint pos = ccpAdd(role->getPosition(),ccp(0, 40-role->getContentSize().height)/2);
			skeletonNode->setPosition(pos);
			role->getParent()->addChild(packageWithClippingNode(skeletonNode));
			skeletonNode->completeListener = std::bind(&CWBossLayer::SpineComplete,this,std::placeholders::_1,std::placeholders::_2);
			m_pArmature = skeletonNode;
			skeletonNode->setVisible(false);
			this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f), CCCallFunc::create(this, callfunc_selector(CWBossLayer::callBackForSetArmatureScale))));

		}else{
			CCLOG("[ *ERROR ] CWBossLayer::updateBossTexture");
		}
	}else{
		char ExportJson_str[60] = {0};
		sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",m_ModeID);
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(ExportJson_str);
		CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(m_ModeID));
		if (!animationData)													//模型容错性处理
		{
			CCLOG("[ *ERROR ]  CWBossLayer::updateBossTexture Model=%d IS NULL", m_ModeID); 
			m_ModeID = 516;
			sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",m_ModeID);
			CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(ExportJson_str);
		}
		CCArmature* Armature = CCArmature::create(ToString(m_ModeID));
		if (Armature)
		{	
			Armature->getAnimation()->play(Stand_Action, 0.01f);
			role->getParent()->removeChildByTag(100);
			float zoom = m_pBoss.zoom*0.01f;
			if (!zoom)
			{
				zoom = 300.0/Armature->getContentSize().height;			//容错性处理
			}
			Armature->setScale(zoom+0.3f);
			Armature->setTag(100);
			CCPoint pos = ccpAdd(role->getPosition(),ccp(0, 40-role->getContentSize().height)/2);
			Armature->setPosition(pos);
			role->getParent()->addChild(packageWithClippingNode(Armature), role->getZOrder());
			Armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(CWBossLayer::movementCallBack));
			m_pArmature = Armature;

			callBackForSetArmatureScale();
		}	
	}
}

void CWBossLayer::pressSkill( CCObject* pSender, CTouchPressState iState )
{
	CButton* pBtn = (CButton*)pSender;
	CSkill* pSkill = (CSkill*)pBtn->getUserData();

	//如果弹窗没够构建
	if(m_pSkillInfo==nullptr)
	{
		m_pSkillInfo = CSkillInfo::create();
		this->addChild(m_pSkillInfo);
	}

	switch (iState)
	{
	case cocos2d::cocoswidget::CTouchPressOn:
		{
			PlayEffectSound(SFX_429);

			if(pBtn->getTag()==3)
			{
				pSkill = new CSkill;
				pSkill->skillId = 1111;
				pSkill->skillType = 2;
			}
			m_pSkillInfo->setInfo(pSkill->skillId);
			m_pSkillInfo->setPara(pSkill->skillType==1?GETLANGSTR(1090):GETLANGSTR(274));
			m_pSkillInfo->bindPos(pBtn);
			m_pSkillInfo->show();
			CCLOG("show");
			const char* action[4] = {Attack_Action, SpAttack_Action, Skill_Action, Stand_Action};

			CCArmature* pArmature = dynamic_cast<CCArmature*>(m_pArmature);
			if(pArmature)
				pArmature->getAnimation()->play(action[pBtn->getTag()], 0.01f);

			SkeletonAnimation * skeletonNode = dynamic_cast<SkeletonAnimation*>(m_pArmature);
			if (skeletonNode)
			{
				spTrackEntry*TrackEntry = skeletonNode->getCurrent();
				if (strcmp(TrackEntry->animation->name,action[pBtn->getTag()]))
					skeletonNode->setAnimation(0,action[pBtn->getTag()],true);
			}
		}
		break;
	case cocos2d::cocoswidget::CTouchPressOff:
		{
			m_pSkillInfo->hide();
			CCLOG("hide");
			//CCArmature* pArmature = dynamic_cast<CCArmature*>(m_pArmature);
			//if(pArmature)
			//{
			//	pArmature->getAnimation()->play(Stand_Action, 0.01f);
			//}
		}
		break;
	default:
		break;
	}
}

void CWBossLayer::pressRewardBox( CCObject* pSender, CTouchPressState iState )
{
	CButton* pBtn = (CButton*)pSender;

	//如果弹窗没够构建
	if(m_pSkillInfo==nullptr)
	{
		m_pSkillInfo = CSkillInfo::create();
		this->addChild(m_pSkillInfo);
	}

	switch (iState)
	{
	case cocos2d::cocoswidget::CTouchPressOn:
		{
			PlayEffectSound(SFX_429);

			m_pSkillInfo->setInfo(m_iBuffId);
			m_pSkillInfo->bindPos(pBtn);
			m_pSkillInfo->show();
			CCLOG("show");
		}
		break;
	case cocos2d::cocoswidget::CTouchPressOff:
		{
			m_pSkillInfo->hide();
			CCLOG("hide");
		}
		break;
	default:
		break;
	}
}

int CWBossLayer::getBossId()
{
	return m_pBoss.thumb;
}

void CWBossLayer::movementCallBack( CCArmature* pAramture, MovementEventType type, const char* sData )
{
	switch (type)
	{
	case cocos2d::extension::START:
		break;
	case cocos2d::extension::LOOP_COMPLETE:
	case cocos2d::extension::COMPLETE:
		{
			if(!m_pSkillInfo) return;

			//如果弹框中，重复播放当前动作
			if(m_pSkillInfo->isVisible())
			{
				CCArmature* pArmature = dynamic_cast<CCArmature*>(m_pArmature);
				if(pArmature)
				{
					pArmature->getAnimation()->play(sData, 0.01f);
				}
				return;
			}

			//默认播放站立动作
			CCArmature* pArmature = dynamic_cast<CCArmature*>(m_pArmature);
			if(pArmature)
			{
				if(pArmature->getAnimation()->getAnimationData()->getMovement(Stand_Action))
				{
					pArmature->getAnimation()->play(Stand_Action, 0.01f);
					return;
				}
			}

			
		}
		break;
	default:
		break;
	}
}

bool CWBossLayer::isBossCome()
{
	return m_iBossCome != 2;
}

CCNode* CWBossLayer::packageWithClippingNode( CCNode* pNode )
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	//选定被裁切内容
	CCNode* pLayerBeClip = pNode;
	//if(!pLayerBeClip) return nullptr;
	//裁切
	CCClippingNode* pClip = CCClippingNode::create();
	pClip->setContentSize(winSize);
	CImageView* pBg = (CImageView*)m_ui->findWidgetById("board_blue1");
	CCSprite* pStenCil = UICloneMgr::cloneImageView(pBg);
	pClip->setStencil(pStenCil);
	pClip->addChild(pLayerBeClip);
	return pClip;
}

void CWBossLayer::callBackForSetArmatureScale()
{
	m_pArmature->setVisible(true);

	SkeletonAnimation* pSkeleton = dynamic_cast<SkeletonAnimation*>(m_pArmature);
	if(pSkeleton)
	{
		pSkeleton->setScale(550/pSkeleton->boundingBox().size.height);
		return;
	}

	CCArmature* pArmature = dynamic_cast<CCArmature*>(m_pArmature);
	if(pArmature)
	{
		pArmature->setScale(550/pArmature->boundingBox().size.height);
		return;
	}
}

void CWBossLayer::timeInBackground( CCObject* pObj )
{
	CCInteger* pValue = dynamic_cast<CCInteger*>(pObj);
	int iValue = pValue->getValue()/1000;
	m_iBossSecond -= iValue;
	m_iNextChallengeSecond -= iValue;
}


