#include "MonsterInfo.h"
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
#include "common/CommonFunction.h"


CMonsterInfo::CMonsterInfo()
{

}

bool CMonsterInfo::init()
{
	if (BaseLayer::init())
	{
		m_ui = LoadComponent("MonsterInfo.xaml"); 
		m_ui->setAnchorPoint(ccp(0.0f, 0.0f));
		m_ui->setPosition(CCPointZero);
		this->addChild(m_ui);

		setScale(0.8f);

		setOpacity(0);

		setVisible(true);

		CLabel *desc = (CLabel*)(m_ui->findWidgetById("desc"));
		m_iBaseDescFontSize = desc->getFontSize();

		return true;
	}
	return false;

}

CMonsterInfo::~CMonsterInfo()
{

}

void CMonsterInfo::setMonster(CMonster *monster)
{
	const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(monster->thumb);
	CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));

	CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
	level->setString(CCString::createWithFormat("%d",monster->level)->getCString());

	CLabel *desc = (CLabel*)(m_ui->findWidgetById("desc"));
	if (data)
	{
		if(data->heroName.size()>0)
		{
			name->setString(data->heroName.c_str());
		}
		
		if(data->heroDesc.size()>0)
		{
			desc->setString(data->heroDesc.c_str());
			desc->setFontSize(m_iBaseDescFontSize);
		}

		if(data->heroDesc.size()>72)
		{
			desc->setFontSize(m_iBaseDescFontSize-3);
		}
	}

	CButton *headbg = (CButton *)m_ui->findWidgetById("head");
	CCSprite *mask = (CCSprite *)m_ui->findWidgetById("mask");
	mask->setTexture(setItemQualityTexture(monster->quality));

	CCSprite *head = CCSprite::create(getImageName(monster).c_str());
	if (!head)
	{
		head = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CMonsterInfo::setMonster Lost Image = %d",monster->thumb);
	}
	headbg->removeAllChildren();
	head->setScale(0.9f);
	head->setPosition(ccp(headbg->getContentSize().width/2, headbg->getContentSize().height/2));
	headbg->addChild(head);
	//NodeFillParent(head);
}


void CMonsterInfo::setHero( CHero* pHero )
{
	const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(pHero->thumb);
	CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
	CLabel *desc = (CLabel*)(m_ui->findWidgetById("desc"));
	if (data)
	{
		if(data->heroName.size()>0)
		{
			name->setString(data->heroName.c_str());
		}

		if(data->heroDesc.size()>0)
		{
			desc->setString(data->heroDesc.c_str());
			desc->setFontSize(m_iBaseDescFontSize);
		}

		if(data->heroDesc.size()>72)
		{
			desc->setFontSize(m_iBaseDescFontSize-3);
		}
	}


	CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
	level->setString(CCString::createWithFormat("%d",pHero->level)->getCString());

	CButton *headbg = (CButton *)m_ui->findWidgetById("head");
	CCSprite *mask = (CCSprite *)m_ui->findWidgetById("mask");
	mask->setTexture(setItemQualityTexture(pHero->iColor));

	CCSprite *head = CCSprite::create(getImageName(pHero).c_str());
	if (!head)
	{
		head = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CMonsterInfo::setMonster Lost Image = %d",pHero->thumb);
	}
	headbg->removeAllChildren();
	head->setScale(0.9f);
	head->setPosition(ccp(headbg->getContentSize().width/2, headbg->getContentSize().height/2));
	headbg->addChild(head);
	//NodeFillParent(head);
}

void CMonsterInfo::changeForSign( CHero* pHero )
{
	CLabel *signNum = (CLabel*)(m_ui->findWidgetById("sign_num"));
	signNum->setVisible(true);
	signNum->setString(CCString::createWithFormat("%d",pHero->level)->getCString());
	CLabel *signText = (CLabel*)(m_ui->findWidgetById("sign_text"));
	signText->setVisible(true);

	CLabel *num = (CLabel*)(m_ui->findWidgetById("level"));
	num->setVisible(false);
	CLabel *numText = (CLabel*)(m_ui->findWidgetById("level_icon"));
	numText->setVisible(false);
}


void CMonsterInfo::show()
{
	setVisible(true);
}

void CMonsterInfo::hide()
{
	setVisible(false);
}

void CMonsterInfo::bindPos( CCNode* pAimNode )
{
	//base size
	CCSize size = ((CCNode*)(m_ui->findWidgetById("bg")))->getContentSize();
	size = CCSizeMake(size.width*getScaleX(), size.height*getScaleY());

	CCPoint pPos;
	CCPoint pWorldpos = pAimNode->getParent()->convertToWorldSpace(pAimNode->getPosition());

	CCPoint pCenterPos = pWorldpos+ccp(pAimNode->getContentSize().width*(0.5f-pAimNode->getAnchorPoint().x), pAimNode->getContentSize().height*(0.5f-pAimNode->getAnchorPoint().y));

	CCSize worldSize = CCDirector::sharedDirector()->getWinSize();
	if(pCenterPos.x<worldSize.width/2)
	{
		//尝试右上方显示
		pPos = ccp(
			pWorldpos.x+pAimNode->getScaleX()*pAimNode->getContentSize().width*(1-pAimNode->getAnchorPoint().x), 
			pWorldpos.y+pAimNode->getScaleY()*pAimNode->getContentSize().height*(1-pAimNode->getAnchorPoint().y));
		this->setPosition(pPos);
		if(checkRectInScreen(CCRectMake(pPos.x, pPos.y, size.width, size.height), 50.0f))
		{
			return;
		}

		//尝试右下方显示
		pPos = ccp(
			pWorldpos.x+pAimNode->getScaleX()*pAimNode->getContentSize().width*(1-pAimNode->getAnchorPoint().x), 
			pWorldpos.y-pAimNode->getScaleY()*pAimNode->getContentSize().height*pAimNode->getAnchorPoint().y-size.height/2);
		this->setPosition(pPos);
		if(checkRectInScreen(CCRectMake(pPos.x, pPos.y, size.width, size.height), 50.0f))
		{
			return;
		}
	}
	else
	{
		//尝试左上方显示
		pPos = ccp(
			pWorldpos.x-pAimNode->getScaleX()*pAimNode->getContentSize().width*pAimNode->getAnchorPoint().x-size.width, 
			pWorldpos.y+pAimNode->getScaleY()*pAimNode->getContentSize().height*(1-pAimNode->getAnchorPoint().y));
		this->setPosition(pPos);
		if(checkRectInScreen(CCRectMake(pPos.x, pPos.y, size.width, size.height), 50.0f))
		{
			return;
		}

		//尝试左下方显示
		pPos = ccp(
			pWorldpos.x-pAimNode->getScaleX()*pAimNode->getContentSize().width*pAimNode->getAnchorPoint().x-size.width, 
			pWorldpos.y-pAimNode->getScaleY()*pAimNode->getContentSize().height*pAimNode->getAnchorPoint().y-size.height/2);
		this->setPosition(pPos);
		if(checkRectInScreen(CCRectMake(pPos.x, pPos.y, size.width, size.height), 50.0f))
		{
			return;
		}
	}

}


