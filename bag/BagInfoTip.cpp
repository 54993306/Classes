#include "BagInfoTip.h"
#include "common/commonString.h"
#include "common/CommonFunction.h"
#include "model/DataCenter.h"

bool CBagInfoTip::init()
{
	if (BaseLayer::init())
	{
	    MaskLayer* lay = MaskLayer::create("BagInfoTipMaskLayer");
		LayerManager::instance()->push(lay);
		m_ui = LoadComponent("bagUI/bagtip.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
// 		this->removeAllItem(2);
		m_basePoint = m_ui->getChildByTag(6)->getPosition();
		m_offsetHeight = 40.0f;
// 		this->removeAllItem(1);
// 		this->removeAllItem(2);
// 		this->removeAllItem(3);
// 		this->addPriceTip();
// 		this->updateTip();

		CCNode *pNode = m_ui->getChildByTag(10);
		CLabel* lab1 = (CLabel*)(pNode->getChildByTag(1));
		CCNode* line = pNode->getChildByTag(20);
		CCNode* lb = pNode->getChildByTag(11);
		CLabel* lab2 = (CLabel*)(pNode->getChildByTag(3));

		m_labelInter = lab1->getPositionY() - lab2->getPositionY();
		m_lineInter = line->getPositionY() - lb->getPositionY();

		return true;
	}
	return false;
}

bool CBagInfoTip::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	if( !res )
	{
		if( this->boundingBox().containsPoint(pTouch->getLocation()))
		{
			res = true;
		}
// 		this->removeFromParent();
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
	}
	return true;
}

void CBagInfoTip::removeAllItem(int type)
{
	int tag = type*10;
	m_ui->removeChildByTag(tag);
}

void CBagInfoTip::updateTip(int index)
{
	int height = 4*m_labelInter + 15 + m_offsetHeight;
	CCNode *bg = m_ui->getChildByTag(1);

	CCSize size = bg->getContentSize();

	bg->setAnchorPoint(ccp(0.5,1));
	bg->setPositionY(bg->getPositionY()+bg->getContentSize().height/2);
// 	bg->setScaleY((bg->getContentSize().height-(height - m_offsetHeight))/bg->getContentSize().height);
	bg->setScaleY(height/bg->getContentSize().height);
	
	float suboffset = (size.height - bg->boundingBox().size.height)/2;

	CCArray *child = m_ui->getChildren();
	int take = (index-1)%6;
	int offsetX = 0;
	switch (take)
	{
	case 1:
		offsetX = 100;
		break;
	case 2:
		offsetX = 200;
		break;
	case 3:
		offsetX = -180;
		break;
	case 4:
		offsetX = -80;
		break;
	case 5:
		offsetX = 50;
		break;
	default:
		break;
	}
	for (int i = 0; i < m_ui->getChildrenCount(); i++)
	{
		CCNode *node = (CCNode *)(child->objectAtIndex(i));
		node->setPositionY(node->getPositionY() - suboffset);
		node->setPositionX(node->getPositionX() + offsetX);
	}

	CCFiniteTimeAction *scaleS = CCScaleTo::create(0.0,0);
	CCFiniteTimeAction *scaleL = CCScaleTo::create(0.25,1);
	m_ui->runAction(CCSequence::createWithTwoActions(scaleS,scaleL));
}

//基本信息
void CBagInfoTip::addBasicInfo(const CItem *item) 
{
	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item->itemId);

	CLabel* namelab = (CLabel*)(m_ui->getChildByTag(3));
	if(itemData)
	{
		namelab->setString(itemData->itemName.c_str());
	}
	

	CCSprite *itemImg = (CCSprite*)(m_ui->findWidgetById("itemImg"));
	CImageView *prop = CImageView::create(CCString::createWithFormat("prop/%d.png",item->iconId)->getCString());
	if (!prop)
	{
		prop = CImageView::create("prop/32003.png");
		CCLOG("[ ERROR ] CBagInfoTip::addBasicInfo Lost Image = %d",item->iconId);
	}
	prop->setPosition(ccp(itemImg->getContentSize().width/2,itemImg->getContentSize().height/2));
	itemImg->addChild(prop);

	CLabel *type = (CLabel*)(m_ui->findWidgetById("type"));
	type->setString(U8(getArmorTypeName(item->armor.armorType).c_str()));

	if (item->itemType!=2)
	{
		type->setVisible(false);
		CLabel *place = (CLabel*)(m_ui->findWidgetById("place"));
		place->setVisible(false);
	}

	CCSprite *mask = (CCSprite*)(m_ui->findWidgetById("mask"));
	mask->setTexture(setItemQualityTexture(item->quality));
}

//附加能力
void CBagInfoTip::addAdditionalTip(int index,CExtAttr& att)
{
	CCNode *pNode = m_ui->getChildByTag(20);
	CLabel* lab1 = (CLabel*)(pNode->getChildByTag(3*index+1));
	CLabel* lab2 = (CLabel*)(pNode->getChildByTag(3*index+2));
	CLabel* lab3 = (CLabel*)(pNode->getChildByTag(3*index+3));

	lab1->setPositionY(m_basePoint.y-m_offsetHeight);
	lab2->setPositionY(m_basePoint.y-m_offsetHeight);
	lab3->setPositionY(m_basePoint.y-m_offsetHeight);

	lab1->setString(ToString(att.attrType));
	lab2->setString(ToString(att.attrVal));
	lab3->setString(ToString(att.attrMax));

	((CCSprite*)(pNode->getChildByTag(15+index)))->setPositionY(m_basePoint.y-m_offsetHeight);

	m_offsetHeight += m_labelInter;
}

//唯一能力
void CBagInfoTip::addOnlyAbilityTip()
{
	CCNode *abNode = m_ui->getChildByTag(30);
	CCNode *tiao = abNode->getChildByTag(20);
	tiao->setPositionY(m_basePoint.y - m_offsetHeight);
	m_offsetHeight += m_lineInter;
	CCNode *lab1 = abNode->getChildByTag(1);
	lab1->setPositionY(m_basePoint.y - m_offsetHeight);
	m_offsetHeight += m_labelInter*2 - 10;
	CCNode *lab2 = abNode->getChildByTag(2);
	lab2->setPositionY(m_basePoint.y - m_offsetHeight);
	m_offsetHeight += m_labelInter + 10 ;
}

//售价
void CBagInfoTip::addPriceTip(int money)
{
	CLayout *priceLay = (CLayout*)m_ui->getChildByTag(40);
	CCNode *node = (CCNode*)(priceLay->getChildByTag(20));
	node->setPositionY(m_basePoint.y - m_offsetHeight); 
	m_offsetHeight += m_labelInter+10;
	for (int i=0; i<5; ++i)
	{
		CCNode *node = priceLay->getChildByTag(i+1);
		node->setPositionY(m_basePoint.y - m_offsetHeight);
	}
	CLabel* lab1 = (CLabel*)(priceLay->getChildByTag(4));
	lab1->setString(ToString(money));
// 	m_offsetHeight += 30;
}

void CBagInfoTip::addEquipmentTip(const CItem *item)
{
	CArmor armor = item->armor;
	CLayout *lay = (CLayout*)(m_ui->getChildByTag(10));
	//攻击力，防御力
	((CLabel*)(lay->getChildByTag(2)))->setString(ToString(armor.baseAtk));
	((CLabel*)(lay->getChildByTag(4)))->setString(ToString(armor.baseDef));

	//删除血量
	lay->removeChildByTag(5);
	lay->removeChildByTag(6);
	lay->removeChildByTag(10);

	/*m_offsetHeight += 15;*//*lay->getChildByTag(20)->boundingBox().size.height*5;*/
	m_offsetHeight += m_labelInter*3 + m_lineInter;

	if (armor.attrList.size()==0)
	{
		CCNode *pNode = m_ui->getChildByTag(20);
		pNode->removeChildByTag(20);
		pNode->removeChildByTag(13);
		pNode->removeChildByTag(14);
	}	
	else
	{
		lay = (CLayout*)(m_ui->getChildByTag(20));
		lay->getChildByTag(20)->setPositionY(m_basePoint.y - m_offsetHeight);
		m_offsetHeight += m_lineInter;
		lay->getChildByTag(13)->setPositionY(m_basePoint.y - m_offsetHeight);
		lay->getChildByTag(14)->setPositionY(m_basePoint.y - m_offsetHeight);
		m_offsetHeight += m_labelInter;
	}
	
	int i = 0;
	for (; i < armor.attrList.size(); i++)
	{
		CExtAttr atr = armor.attrList[i];
		this->addAdditionalTip(i,atr);
	}

	CCNode *pNode = m_ui->getChildByTag(20);
	
	while (i<4)
	{
		pNode->removeChildByTag(3*i+1);
		pNode->removeChildByTag(3*i+2);
		pNode->removeChildByTag(3*i+3);
		pNode->removeChildByTag(15+i);
		++i;
	}
}

void CBagInfoTip::setItemData(int index, const CItem* item)
{
	addBasicInfo(item);
	if (item->itemType == 1)
	{
		addEquipmentTip(item);
// 		m_ui->removeChildByTag(10);
// 		m_ui->removeChildByTag(20);
// 		m_ui->removeChildByTag(30);
	}
	else if (item->itemType == 2)
	{
		addEquipmentTip(item);
	}
	else
	{
		m_ui->removeChildByTag(10);
		m_ui->removeChildByTag(20);
// 		m_ui->removeChildByTag(30);
	}
	m_ui->removeChildByTag(30);
// 	addOnlyAbilityTip();
 	addPriceTip(item->sellPrice);
	updateTip(index);
}
