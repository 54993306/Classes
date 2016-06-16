#include "SkillDesc.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"
#include "common/CGameSound.h"
#include "Resources.h"

bool CSkillDesc::init()
{
	if (BaseLayer::init())
	{
		this->setIsShowBlack(false);
		LayerManager::instance()->push(MaskLayer::create("desc"));
		m_ui = LoadComponent("skillDesc.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		setVisible(true);

		return true;
	}
	return false;
}

void CSkillDesc::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSkillDesc::showSkill(CHero* hero, int type)
{
	m_hero = hero;
	m_type = type;
}

void CSkillDesc::onEnter()
{
	BaseLayer::onEnter();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CSkillDesc::onClose));
	this->addChild(pClose, 999);

}


void CSkillDesc::updateSkillInfo(CSkill *skill, bool isPassive, int type)
{		
	CLabel *name1 = (CLabel*)m_ui->findWidgetById("name1");
	CLabel *level1 = (CLabel*)m_ui->findWidgetById("level1");
	CProgressBar *process1 = (CProgressBar*)m_ui->findWidgetById("lv_process1");
	CCSprite *mask1= (CCSprite*)m_ui->findWidgetById("mask1");
	
	level1->setString(CCString::createWithFormat("%d/%d",skill->level,skill->maxLevel)->getCString());
	process1->setMaxValue(skill->maxLevel);
	process1->setValue(skill->level);

	const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(skill->skillId);
	CLabel *desc= (CLabel *)m_ui->findWidgetById("desc");
	desc->setString(cfg1->desc.c_str());
	CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
	if(!spr)
	{
		CCLOG("CSkillDesc::updateSkillInfo");
		spr  = CImageView::create("skillIcon/100000.png");
	}
	spr->setPosition(ccp(mask1->getContentSize().width/2,mask1->getContentSize().height/2));
	spr->setScale(0.9f);
	mask1->addChild(spr);

	name1->setString(cfg1->name.c_str());
	CLabel *skillType = (CLabel*)(m_ui->findWidgetById("type"));
	if (type==0)
	{
		skillType->setString(GETLANGSTR(273));
	}
	else
	{
		skillType->setString(GETLANGSTR(38));
	}

	CLabel *passvible = (CLabel*)(m_ui->findWidgetById("passvible"));
	if (!isPassive)
	{
		passvible->setString(GETLANGSTR(274));
	}
}

void CSkillDesc::updateHeroInfo(const TMessage& tMsg)
{
	CHero *hero = (CHero*)tMsg.lParam;
	showSkill(hero,m_type);
}

void CSkillDesc::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
}

bool CSkillDesc::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");
	if( !res && !bgSpr->boundingBox().containsPoint(pTouch->getLocation()))
	{
		res = true;
		onClose(nullptr);
	}
	return res;
}

