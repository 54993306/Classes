#include "SkillInfo.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "tools/UICloneMgr.h"
#include "common/CommonFunction.h"

CSkillInfo::CSkillInfo()
{

}

bool CSkillInfo::init()
{
	if (BaseLayer::init())
	{
		m_ui = LoadComponent("SkillInfo.xaml"); 
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

CSkillInfo::~CSkillInfo()
{

}

void CSkillInfo::setInfo(int iSkillId)
{
	const SkillCfg *cfg = DataCenter::sharedData()->getSkill()->getCfg(iSkillId);

	CLabel* pName = (CLabel*)(m_ui->findWidgetById("name"));
	CLabel* pDesc = (CLabel*)(m_ui->findWidgetById("desc"));
	CLabel* pAra = (CLabel*)(m_ui->findWidgetById("para"));
	if (cfg)
	{
		if(cfg->name.size()>0)
		{
			pName->setString(cfg->name.c_str());
		}

		if(cfg->desc.size()>0)
		{
			pDesc->setString(cfg->desc.c_str());
			pDesc->setFontSize(m_iBaseDescFontSize);
		}

		if(cfg->desc.size()>72)
		{
			pDesc->setFontSize(m_iBaseDescFontSize-3);
		}

		CButton *headbg = (CButton *)m_ui->findWidgetById("head");
		CImageView *head = CImageView::create(CCString::createWithFormat("skillIcon/%d.png", cfg->icon)->getCString());
		if (!head)
		{
			head = CImageView::create("skillIcon/100000.png");
			CCLOG("[ ERROR ] CSkillInfo::setInfo Lost Image");
		}

		headbg->removeAllChildren();
		head->setScale(0.9f);
		head->setPosition(ccp(headbg->getContentSize().width/2, headbg->getContentSize().height/2));
		headbg->addChild(head);

	}
}


void CSkillInfo::setPara( const char* str )
{
	CLabel* pAra = (CLabel*)(m_ui->findWidgetById("para"));
	pAra->setString(str);
}


void CSkillInfo::show()
{
	setVisible(true);
}

void CSkillInfo::hide()
{
	setVisible(false);
}

void CSkillInfo::bindPos( CCNode* pAimNode )
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

