#include "ItemInfo.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "tools/UICloneMgr.h"
#include "common/CommonFunction.h"

CItemInfo::CItemInfo()
{

}

bool CItemInfo::init()
{
	if (BaseLayer::init())
	{
		m_ui = LoadComponent("ItemInfo.xaml"); 
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

CItemInfo::~CItemInfo()
{

}

void CItemInfo::setInfo(CItem *info)
{
	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(info->itemId);

	CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
	if (itemData)
	{
		if(itemData->itemName.size()>0)
		name->setString(itemData->itemName.c_str());
	}

	CLabel *desc = (CLabel*)(m_ui->findWidgetById("desc"));
	if (itemData)
	{
		if(itemData->itemDesc.size()>0)
		{
			desc->setString(itemData->itemDesc.c_str());
			desc->setFontSize(m_iBaseDescFontSize);
		}

		if(itemData->itemDesc.size()>72)
		{
			desc->setFontSize(m_iBaseDescFontSize-3);
		}
	}

	CLabel *num = (CLabel*)(m_ui->findWidgetById("num"));
	num->setString(CCString::createWithFormat("%d", info->itemNum)->getCString());

	CButton *headbg = (CButton *)m_ui->findWidgetById("head");
	CCSprite *mask = (CCSprite *)m_ui->findWidgetById("mask");
	mask->setTexture(setItemQualityTexture(info->quality));

	CImageView *head = CImageView::create(GetImageName(1, info->quality, info->iconId).c_str());
	if (!head)
	{
		head = CImageView::create("headImg/101.png");
		CCLOG("[ ERROR ] CItemInfo::setMonster Lost Image");
	}

	headbg->removeAllChildren();
	head->setScale(0.9f);
	head->setPosition(ccp(headbg->getContentSize().width/2, headbg->getContentSize().height/2));
	headbg->addChild(head);
	//NodeFillParent(head);
}

void CItemInfo::setInfo( CPrize *pPrize )
{
	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(pPrize->id);

	CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
	if (itemData)
	{
		if(itemData->itemName.size()>0)
			name->setString(itemData->itemName.c_str());
	}

	CLabel *desc = (CLabel*)(m_ui->findWidgetById("desc"));
	if (itemData)
	{
		if(itemData->itemDesc.size()>0)
		{
			desc->setString(itemData->itemDesc.c_str());
			desc->setFontSize(m_iBaseDescFontSize);
		}
			
		if(itemData->itemDesc.size()>72)
		{
			desc->setFontSize(m_iBaseDescFontSize-3);
		}
	}

	CLabel *num = (CLabel*)(m_ui->findWidgetById("num"));
	num->setString(CCString::createWithFormat("%d", pPrize->num)->getCString());

	CButton *headbg = (CButton *)m_ui->findWidgetById("head");
	CCSprite *mask = (CCSprite *)m_ui->findWidgetById("mask");
	mask->setTexture(setItemQualityTexture(pPrize->color));

	CImageView *head = CImageView::create(GetImageName(1, pPrize->color, pPrize->thumb).c_str());
	if (!head)
	{
		head = CImageView::create("headImg/101.png");
		CCLOG("[ ERROR ] CItemInfo::setMonster Lost Image");
	}

	headbg->removeAllChildren();
	head->setScale(0.9f);
	head->setPosition(ccp(headbg->getContentSize().width/2, headbg->getContentSize().height/2));
	headbg->addChild(head);
	//NodeFillParent(head);
}



void CItemInfo::changeForSign( CItem *info )
{
	CLabel *signNum = (CLabel*)(m_ui->findWidgetById("sign_num"));
	signNum->setVisible(true);
	signNum->setString(CCString::createWithFormat("%d",info->itemNum)->getCString());
	CLabel *signText = (CLabel*)(m_ui->findWidgetById("sign_text"));
	signText->setVisible(true);

	CLabel *num = (CLabel*)(m_ui->findWidgetById("num"));
	num->setVisible(false);
	CLabel *numText = (CLabel*)(m_ui->findWidgetById("num_icon"));
	numText->setVisible(false);
}

void CItemInfo::changeForSign( CPrize *pPrize )
{
	CLabel *signNum = (CLabel*)(m_ui->findWidgetById("sign_num"));
	signNum->setVisible(true);
	signNum->setString(CCString::createWithFormat("%d", pPrize->num)->getCString());
	CLabel *signText = (CLabel*)(m_ui->findWidgetById("sign_text"));
	signText->setVisible(true);

	CLabel *num = (CLabel*)(m_ui->findWidgetById("num"));
	num->setVisible(false);
	CLabel *numText = (CLabel*)(m_ui->findWidgetById("num_icon"));
	numText->setVisible(false);
}


void CItemInfo::show()
{
	setVisible(true);
}

void CItemInfo::hide()
{
	setVisible(false);
}

void CItemInfo::bindPos( CCNode* pAimNode )
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

