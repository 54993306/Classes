#include "RewardHardLayer.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"
#include "RewardLayer.h"
#include "tollgate/TollgatePreview.h"
#include "tollgate/Chapter.h"
#include "model/WarManager.h"
#include "tools/ShowTexttip.h"
#include "common/CheckMoney.h"
#include "common/CShowToBuyResource.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "common/ShaderDataHelper.h"

bool CRewardHardLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("RewardHardMask");
		lay->setContentSize(CCSizeMake(1138,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("rewardSelect.xaml");  //  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);
		return true;
	}
	return false;

}

CRewardHardLayer::CRewardHardLayer():m_iChapterId(0)
{

}

void CRewardHardLayer::onEnter()
{
	BaseLayer::onEnter();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CRewardHardLayer::onClose));
	this->addChild(pClose, 999);

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(CRewardHardLayer::roleUpdate));
}

void CRewardHardLayer::setStageList(vector<CBountyStage> pList)
{
	m_stageList = pList;
	CLabel *action1 = (CLabel*)m_ui->findWidgetById("action1");
	CLabel *action2 = (CLabel*)m_ui->findWidgetById("action2");
	CLabel *action3 = (CLabel*)m_ui->findWidgetById("action3");

	action1->setString(ToString(pList.at(0).action));
	action2->setString(ToString(pList.at(1).action));
	action3->setString(ToString(pList.at(2).action));
	roleUpdate(TMessage());


	CImageView *low = (CImageView*)m_ui->findWidgetById("low");
	low->setTouchEnabled(true);
	low->setUserData(&m_stageList.at(0));
	low->setOnClickListener(this,ccw_click_selector(CRewardHardLayer::onStage));
	if (m_stageList.at(0).isOpen)
	{
		CCSprite* pLowEffect = CCSprite::create("reward/a2_on.png");
		pLowEffect->setPosition(ccp(low->getContentSize().width*0.5f, low->getContentSize().height*0.5f));
		low->addChild(pLowEffect);
		pLowEffect->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeIn::create(0.35f), CCFadeOut::create(0.25f))));
	}


	CImageView *middle = (CImageView*)m_ui->findWidgetById("middle");
	middle->setTouchEnabled(true);
	middle->setUserData(&m_stageList.at(1));
	middle->setOnClickListener(this,ccw_click_selector(CRewardHardLayer::onStage));
	//添加效果
	if (m_stageList.at(1).isOpen)
	{
		CCSprite* pMiddleEffect = CCSprite::create("reward/a3_on.png");
		pMiddleEffect->setPosition(ccp(middle->getContentSize().width*0.5f, middle->getContentSize().height*0.5f));
		middle->addChild(pMiddleEffect);
		pMiddleEffect->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeIn::create(0.35f), CCFadeOut::create(0.25f))));
	}
	
	CImageView *high = (CImageView*)m_ui->findWidgetById("high");
	high->setTouchEnabled(true);
	high->setUserData(&m_stageList.at(2));
	high->setOnClickListener(this,ccw_click_selector(CRewardHardLayer::onStage));
	//添加效果
	if (m_stageList.at(2).isOpen)
	{
		CCSprite* pHightEffect = CCSprite::create("reward/a4_on.png");
		pHightEffect->setPosition(ccp(middle->getContentSize().width*0.5f, middle->getContentSize().height*0.5f));
		high->addChild(pHightEffect);
		pHightEffect->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeIn::create(0.35f), CCFadeOut::create(0.25f))));
	}
	CLabel *level1 = (CLabel*)m_ui->findWidgetById("level1");
	CLabel *level2 = (CLabel*)m_ui->findWidgetById("level2");
	CLabel *level3 = (CLabel*)m_ui->findWidgetById("level3");

	for(int i=0; i<3&&i<m_stageList.size();i++)
	{
		CLabel *level = (CLabel*)m_ui->findWidgetById(CCString::createWithFormat("level%d",i+1)->getCString());
		level->setString(ToString(m_stageList.at(i).openLevel));
		if (!m_stageList.at(i).isOpen)
		{
			CLayout *stageLay = (CLayout*)m_ui->findWidgetById(CCString::createWithFormat("stage%d",i+1)->getCString());
			for (int i = 0; i <stageLay->getChildrenCount(); i++)
			{
				CCNode *child =(CCNode*)stageLay->getChildren()->objectAtIndex(i);
				child->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			}
		}
	}
}

void CRewardHardLayer::onStage(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	CBountyStage *bounty = (CBountyStage*)btn->getUserData();

	PlayEffectSound(SFX_Button);

	//先检查体力上限
	if(!CheckActionPowerLimit(bounty->action))
	{
		CShowToBuyResource* pShow = CShowToBuyResource::create();
		pShow->showToBuyResourceByType(ShowBuyResourceActionLimit);
		return;
	}
	else
	{
		//再检查当前体力是否足够
		if(!CheckActionPower(bounty->action))
		{
			//ShowPopTextTip(GETLANGSTR(240));
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceAction);
			return;
		}
	}
	

	if (bounty&&bounty->isOpen)
	{
		CStage stage;
		stage.id = bounty->id;
		DataCenter::sharedData()->getWar()->setTollgate(stage); 
		CChapter chapter;
		chapter.id = getChapterId();
		DataCenter::sharedData()->getWar()->setChapter(chapter); 
		DataCenter::sharedData()->getWar()->setLastStageId(-1); 
		CPlayerControl::getInstance().sendStageInfo(bounty->id);
		CTollgatePreview *preview = CTollgatePreview::create();
		LayerManager::instance()->push(preview);
		preview->setStage(bounty->id, "");
		preview->setNormal(false);
	}
	else
	{
		ShowPopTextTip(GETLANGSTR(183));
	}
}

void CRewardHardLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CRewardHardLayer::onExit()
{
	BaseLayer::onExit();

	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
}

void CRewardHardLayer::roleUpdate( const TMessage& tMsg )
{
	CLabel *action1 = (CLabel*)m_ui->findWidgetById("action1");
	CLabel *action2 = (CLabel*)m_ui->findWidgetById("action2");
	CLabel *action3 = (CLabel*)m_ui->findWidgetById("action3");

	//检查货币数量
	if(!CheckActionPower(atoi(action1->getString())))
	{
		action1->setColor(RGB_RED);
	}
	else
	{
		action1->setColor(RGB_WHITE);
	}

	//检查货币数量
	if(!CheckActionPower(atoi(action2->getString())))
	{
		action2->setColor(RGB_RED);
	}
	else
	{
		action2->setColor(RGB_WHITE);
	}

	//检查货币数量
	if(!CheckActionPower(atoi(action3->getString())))
	{
		action3->setColor(RGB_RED);
	}
	else
	{
		action3->setColor(RGB_WHITE);
	}
}






