#include "LevelUpItem.h"
#include "CityData.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "MainCityControl.h"
#include "model/DataCenter.h"

#include "army/SelectEquipment.h"
#include "GMessage.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"

#include "StrengthenItem.h"
#include "scene/CPopTip.h"
#include "bag/BagInfoTip.h"

#include "common/color.h"
#include "common/CheckMoney.h"
#include "Battle/AnimationManager.h"
#include "Resources.h"
#include "tools/CCShake.h"
#include "common/CGameSound.h"
#include "scene/layer/PropGetInfo.h"
#include "common/CommonFunction.h"
#include "common/CShowToBuyResource.h"
#include "tools/UICloneMgr.h"
#include "Strengthen.h"
#include "common/CCRollLabelAction.h"
#include "StarUpItem.h"

CLevelUpItem::CLevelUpItem():m_strenLeft(nullptr),m_itemAttr(nullptr),m_smeltLay(nullptr)
	,m_isHpAdding(false),m_isAtkAdding(false),m_isDefAdding(false),m_isOneKey(false)
	,m_pCellInfo(nullptr)
{
	for(unsigned int i=0; i<4; i++)
	{
		m_pBaseInfo[i] = nullptr;
	}
}

using namespace BattleSpace;
CLevelUpItem::~CLevelUpItem()
{
	CC_SAFE_RELEASE(m_pCellInfo);
}

bool CLevelUpItem::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("CLevelUpItemMask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("LevelUpItem.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		m_strenLeft = (CLayout*)(m_ui->findWidgetById("left"));
		m_itemAttr = (CLayout*)(m_ui->findWidgetById("attr"));
		this->setVisible(true);

		//获取cell
		m_pCellInfo = (CLayout*)m_ui->findWidgetById("cell_info");
		CC_SAFE_RETAIN(m_pCellInfo);
		m_pCellInfo->removeFromParentAndCleanup(false);

		CCPoint pBasePos = ccp(1281, 115);
		//初始化基础属性四条
		initItemInfo(pBasePos, m_pBaseInfo, 4);

		return true;
	}
	return false;
}


void CLevelUpItem::onEnter()
{ 
	BaseLayer::onEnter();
	PlayEffectSound(SFX_427);

	//普通强化
	CButton *strength = dynamic_cast<CButton*>(m_ui->findWidgetById("strengthen"));
	strength->setOnClickListener(this,ccw_click_selector(CLevelUpItem::onStrengthen));

	//勾玉强化
	CButton *strength1 = dynamic_cast<CButton*>(m_ui->findWidgetById("strengthen1"));
	strength1->setOnClickListener(this,ccw_click_selector(CLevelUpItem::onOneKeyLevelUp));

	//选择装备
	CButton *btn = dynamic_cast<CButton*>(m_ui->findWidgetById("bgBtn"));
	btn->setOnClickListener(this,ccw_click_selector(CLevelUpItem::onSelectItem));

	//火焰效果
	CImageView *circleFire = (CImageView*)(m_ui->findWidgetById("fire_circle"));
	circleFire->setScale(1.65f);
	circleFire->runAction(CCRepeatForever::create(CCRotateBy::create(1.0f, 60)));

	//获取参考位置
	CCNode* pNode = circleFire;
	CCPoint pPos = ccp(pNode->getPositionX()+15, pNode->getPositionY()+75);
	CCAnimation* pAnimation = AnimationManager::sharedAction()->getAnimation("9010");
	pAnimation->setDelayPerUnit(0.15f);
	CCSprite* m_pFire1 = CCSprite::create("skill/9010.png");
	m_pFire1->setPosition(pPos);
	m_pFire1->runAction(CCRepeatForever::create(CCAnimate::create(pAnimation)));
	m_pFire1->setScale(1.6f);
	pNode->getParent()->addChild(m_pFire1, pNode->getZOrder());


	//强化装备回调
	GetTcpNet->registerMsgHandler(StrengthenMsg, this, CMsgHandler_selector(CLevelUpItem::strengthenResponse));
	//选择装备回调
	GetTcpNet->registerMsgHandler(SelectStrengthMsg, this, CMsgHandler_selector(CLevelUpItem::strengthenResponse));

	NOTIFICATION->addObserver(this, callfuncO_selector(CLevelUpItem::sameEquip), SAME_EQUIP_STARUP_LEVELUP, nullptr);

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(CLevelUpItem::roleUpdate));
}


void CLevelUpItem::onExit()
{
	BaseLayer::onExit();

	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
	NOTIFICATION->removeAllObservers(this);
}

void CLevelUpItem::updateHero(const TMessage& tMsg)
{

}

void CLevelUpItem::onStrengthen(CCObject* pSender)
{
	if (isMaxLevel())
	{
		ShowPopTextTip(GETLANGSTR(237));
		return;
	}
	//检查货币
	if(!CheckCoin(m_strengthItem.coin))
	{
		//ShowPopTextTip(GETLANGSTR(205));
		CShowToBuyResource* pShow = CShowToBuyResource::create();
		pShow->showToBuyResourceByType(ShowBuyResourceCoin);
		return;
	}

	m_isOneKey =false;
	CButton *btn = dynamic_cast<CButton*>(pSender);
	CPlayerControl::getInstance().sendStrengthen(m_strengthItem.armor.id);
}

void CLevelUpItem::onOneKeyLevelUp(CCObject* pSender)
{
	if (isMaxLevel())
	{
		ShowPopTextTip(GETLANGSTR(237));
		return;
	}
	//检查货币
	if(!CheckCoin(m_strengthItem.coin))
	{
		//ShowPopTextTip(GETLANGSTR(205));
		CShowToBuyResource* pShow = CShowToBuyResource::create();
		pShow->showToBuyResourceByType(ShowBuyResourceCoin);
		return;
	}
	m_isOneKey = true;
	ShowConfirmTextTip(GETLANGSTR(1195), this,ccw_click_selector(CLevelUpItem::onConfirm));	
}

void CLevelUpItem::onConfirm(CCObject *pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==0)
	{
		CPlayerControl::getInstance().sendStrengthen(m_strengthItem.armor.id, true);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}


void CLevelUpItem::strengthenResponse(int type, google::protobuf::Message *msg)
{
	switch (type)
	{
		//装备数据
	case SelectStrengthMsg:
		{
			PlayEffectSound(SFX_427);

			StrengthenResponse *res = (StrengthenResponse*)msg;

			if (res->has_armor())
			{
				m_strengthItem.read(*res);
				showItem();
				NOTIFICATION->postNotification(SAME_EQUIP_STARUP_LEVELUP, this);
			}
			//最大等级
			else
			{
				ShowPopTextTip(GETLANGSTR(237));
				return;
			}

		}break;
		//强化
	case StrengthenMsg:
		{
			StrengthenResponse *res = (StrengthenResponse*)msg;
			////1 强化成功，2 金币不足，3 达到最大等级，4 数据错误
			switch (res->result())
			{
			case 1:
				{
					{
						UserData *user = DataCenter::sharedData()->getUser()->getUserData();
						if (m_isOneKey)
						{
							user->setRoleGold(user->getRoleGold()-m_strengthItem.gold);
						}
						else
						{
							user->setCoin(user->getCoin()-m_strengthItem.coin);
						}

						CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO, 0, nullptr, nullptr);

						if (res->has_armor())
						{
							m_strengthItem.read(*res);
						}
						//最大等级
						else
						{
							m_strengthItem.armor.itemLevel = m_strengthItem.maxLevel;
						}

						CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_ITEM_DATA, 1, nullptr, &m_strengthItem.armor);

						//显示效果
						float fTime = showHammerEffect(m_isOneKey?5:2);
						//更新数据
						this->runAction(CCSequence::create(CCDelayTime::create(fTime),CCCallFunc::create(this, callfunc_selector(CLevelUpItem::showItem)),nullptr));
					}
				}
				break;
			case 2:
				{
					//ShowPopTextTip(GETLANGSTR(203));
					CShowToBuyResource* pShow = CShowToBuyResource::create();
					pShow->showToBuyResourceByType(ShowBuyResourceCoin);
				}
				break;
			case 3:
				{
					ShowPopTextTip(GETLANGSTR(237));		
				}
				break;
			case 4:
				{
					ShowPopTextTip(GETLANGSTR(170));		
				}
				break;

			default:
				break;
			}

		}break;
	default:
		break;
	}
}

void CLevelUpItem::onSelectItem(CCObject* pSender)
{
	m_selectOPType = 1;
	CStrengthenItem *selectItem = CStrengthenItem::create();
	selectItem->setVisible(false);
	selectItem->setSelectType(1);
	selectItem->setAskType(CStrengthenItemTypeItemLevelUp);
	selectItem->setOnSelectItemListener(this,ccw_select_Item(CLevelUpItem::selectITemCallBack));
	LayerManager::instance()->push(selectItem);
	CPlayerControl::getInstance().sendRoleBag(11, true, 0);
}


void CLevelUpItem::selectITemCallBack( CItem* pItem )
{
	if(!pItem)
	{
		return;
	}

	//最大等级
	if(pItem->armor.strenLv == pItem->itemLevel)
	{
		ShowPopTextTip(GETLANGSTR(237));
		return;
	}

	if(pItem)
	{
		CPlayerControl::getInstance().sendStrengthenArmor(pItem->id,0);
	}
}

void CLevelUpItem::showItem()
{
	CLabel *tip = dynamic_cast<CLabel*>(m_ui->findWidgetById("tip"));
	tip->setVisible(false);
	CCNode *hero = dynamic_cast<CCNode*>(m_ui->findWidgetById("hero"));
	hero->setVisible(false);

	CLayout *attribute = (CLayout*)(m_ui->findWidgetById("attr"));
	attribute->setVisible(true);

	CCSprite *why = (CCSprite*)(m_ui->findWidgetById("why"));
	why->setVisible(false);

	updateItemProperty();

	updateBaseAttr();

	updateStrengthenItem();
}

void CLevelUpItem::updateItemProperty()
{
	CItem* item = &(m_strengthItem.armor);

	//item数据
	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item->itemId);
	if(itemData)
	{
		//设置名称
		CLabel* pName = dynamic_cast<CLabel*>(m_ui->findWidgetById("name"));
		CCString *str = CCString::create("XXX");
		str = CCString::createWithFormat("%s", itemData->itemName.c_str());
		pName->setString(str->getCString());
	}

	//设置可升级到多少级
	CLabel* pMaxLevel = dynamic_cast<CLabel*>(m_ui->findWidgetById("max_level"));
	pMaxLevel->setString(CCString::createWithFormat(GETLANGSTR(1193), m_strengthItem.maxLevel)->getCString());
}


void CLevelUpItem::updateBaseAttr()
{	
	CItem* item = &(m_strengthItem.armor);

	//名字对应的langID
	int iNameId[8] = {42, 44, 43, 49, 45, 46, 48, 47};

	//基础属性
	int iBaseValue[8] = 
	{
		item->armor.baseHp, item->armor.baseDef, 
		item->armor.baseAtk, item->armor.baseHit,
		item->armor.baseDex, item->armor.baseCrit, 
		item->armor.baseRenew, item->armor.baseDodge
	};
	//目标属性
	int iAimValue[8] = 
	{
		item->armor.nextHp, item->armor.nextDef, 
		item->armor.nextAtk, item->armor.nextHit,
		item->armor.nextDex, item->armor.nextCrit, 
		item->armor.nextRenew, item->armor.nextDodge
	};

	//更新数据
	updateInfoToUIAndReturnCount(m_pBaseInfo, iNameId, iBaseValue, iAimValue, 8, nullptr);


	//设置货币需求
	CLabel* pCoin = dynamic_cast<CLabel*>(m_ui->findWidgetById("coin_num"));
	pCoin->setString(ToString(m_strengthItem.coin));
	
	checkMoney();

	if(isMaxLevel())
	{
		pCoin->setString("????");
	}

	//设置当前等级和目标等级
	CLabel* pCurLevel = dynamic_cast<CLabel*>(m_ui->findWidgetById("cur_level"));
	pCurLevel->setString(CCString::createWithFormat("+%d", m_strengthItem.armor.itemLevel)->getCString());
	CLabel* pAimLevel = dynamic_cast<CLabel*>(m_ui->findWidgetById("aim_level"));
	pAimLevel->setString(CCString::createWithFormat("+%d", m_strengthItem.armor.itemLevel+1)->getCString());
	if(isMaxLevel())
	{
		pAimLevel->setString("????");
	}
}


void CLevelUpItem::updateStrengthenItem()
{
	CItem* item = &(m_strengthItem.armor);

	//装备图标
	CCSprite *prop = CCSprite::create(CCString::createWithFormat("prop/%d.png",m_strengthItem.armor.iconId)->getCString());
	if (!prop)
	{
		prop = CImageView::create("prop/32003.png");
	}
	CButton *btn = dynamic_cast<CButton*>(m_ui->findWidgetById("bgBtn"));
	btn->removeChildByTag(1);
	prop->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
	prop->setTag(1);
	btn->addChild(prop, -1);

	//设置品质
	((CCSprite*)btn->getNormalImage())->setTexture(setItemQualityTexture(m_strengthItem.armor.quality));

	//设置等级
	CLabel* pLevel = dynamic_cast<CLabel*>(m_ui->findWidgetById("level"));
	CCNode* pLevelBg = dynamic_cast<CCNode*>(m_ui->findWidgetById("level_mask"));
	if(item->itemLevel>0)
	{
		pLevel->setVisible(true);
		pLevelBg->setVisible(true);

		pLevel->setString(CCString::createWithFormat("+%d", item->itemLevel)->getCString());
	}
	else
	{
		pLevel->setVisible(false);
		pLevelBg->setVisible(false);
	}

	//添加星星层
	btn->removeChildByTag(415);
	CLayout* pStarLayout = getStarLayout(m_strengthItem.armor.iStar);
	btn->addChild(pStarLayout, 415, 415);
}

void CLevelUpItem::setIsStrengthen(bool isEnable)
{
	CButton *strengBtn = (CButton*)(m_ui->findWidgetById("strengthen"));
	strengBtn->setEnabled(isEnable);
}

void CLevelUpItem::onItemTip(CCObject *pSender)
{ 
	CCNode* pNode = (CCNode*)pSender;
	CItem* item =  (CItem*)(pNode->getUserData());
	PropGetInfo* pLayer = PropGetInfo::create();
	LayerManager::instance()->push(pLayer);
	CPlayerControl::getInstance().sendItemInfo(item->itemId);
}

void CLevelUpItem::onUserStone(CCObject *pSender)
{
	CButton *btn = (CButton*)pSender;
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}


void CLevelUpItem::setAddHpString(CCNode* node)
{
	CLabel *tips = dynamic_cast<CLabel*>(m_ui->findWidgetById("tips"));
	CLabel *level = dynamic_cast<CLabel*>(m_ui->findWidgetById("level"));
	
	if (!m_isHpAdding)
	{
		((CLabel*)node)->setString(CCString::createWithFormat("+%d",m_strengthItem.armor.armor.nextHp)->getCString());	
		((CLabel*)node)->setColor(RGB_GREEN);

		tips->setString(GETLANGSTR(219));
		tips->setColor(RGB_GREEN);
		if (m_strengthItem.armor.itemLevel<20)
		{
			CCString *str = CCString::createWithFormat("+%d",m_strengthItem.armor.itemLevel+1);
			level->setString(str->getCString());
			level->setColor(RGB_GREEN);
		}
	}
	else
	{
		((CLabel*)node)->setString(CCString::createWithFormat("+%d",m_strengthItem.armor.armor.addHp)->getCString());
		((CLabel*)node)->setColor(RGB_ARMORYELLOW_COLOR);
		tips->setString(GETLANGSTR(218));
		tips->setColor(RGB_ARMORYELLOW_COLOR);
		
		CCString *str = CCString::createWithFormat("+%d",m_strengthItem.armor.itemLevel);
		level->setString(str->getCString());
		level->setColor(RGB_ARMORYELLOW_COLOR);
	}
	m_isHpAdding = !m_isHpAdding;
}

void CLevelUpItem::setAddAtkString(CCNode* node)
{
	if (!m_isAtkAdding)
	{
		((CLabel*)node)->setString(CCString::createWithFormat("+%d",m_strengthItem.armor.armor.nextAtk)->getCString());
		((CLabel*)node)->setColor(RGB_GREEN);
	}
	else
	{
		((CLabel*)node)->setString(CCString::createWithFormat("+%d",m_strengthItem.armor.armor.addAtk)->getCString());
		((CLabel*)node)->setColor(RGB_ARMORYELLOW_COLOR);
	}
	m_isAtkAdding = !m_isAtkAdding;
}

void CLevelUpItem::setAddDefString(CCNode* node)
{
	if (!m_isDefAdding)
	{
		((CLabel*)node)->setString(CCString::createWithFormat("+%d",m_strengthItem.armor.armor.nextDef)->getCString());	
		((CLabel*)node)->setColor(RGB_GREEN);

	}
	else
	{
		((CLabel*)node)->setString(CCString::createWithFormat("+%d",m_strengthItem.armor.armor.addDef)->getCString());	
		((CLabel*)node)->setColor(RGB_ARMORYELLOW_COLOR);
	}
	m_isDefAdding = !m_isDefAdding;
}

void CLevelUpItem::showSmeltLayer()
{
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	radioGroup->getItemByTag(2)->setChecked(true);
	radioGroup->getItemByTag(1)->setChecked(false);
}


float CLevelUpItem::showHammerEffect(int iTimesHit)
{
	//屏蔽层
	MaskLayer* pLayer1 = MaskLayer::create("CLevelUpItem::effect");
	pLayer1->setOpacity(0);
	LayerManager::instance()->push(pLayer1);
	MaskLayer* lay = MaskLayer::create("HammerEffect");
	lay->setContentSize(CCSizeMake(1138,640));
	lay->setAnchorPoint(ccp(0.5f, 0.5f));
	lay->setPosition(VCENTER);
	lay->setOpacity(0);
	LayerManager::instance()->push(lay);
	
	//获取位置参考
	CCNode* pPosNode = (CCNode*)m_ui->findWidgetById("why");

	//增加锤子
	CCSprite* pHammer = CCSprite::create("strengthen/tool.png");
	pHammer->setPosition(ccp(170+pPosNode->getPositionX(), -60+pPosNode->getPositionY()));
	pHammer->setAnchorPoint(ccp(1.0f, 0.0f));
	pHammer->setScale(1.4f);
	pHammer->setRotation(70);
	lay->addChild(pHammer, 2);
	pHammer->setOpacity(0);
	pHammer->runAction(CCSequence::create(
		CCFadeIn::create(0.3f),
		
		CCEaseBackInOut::create(CCRotateBy::create(0.2f, -110)),
		CCCallFuncN::create(this, callfuncN_selector(CLevelUpItem::showHammerSoundCallBack)),
		CCDelayTime::create(0.1f),

		CCRepeat::create(CCSequence::create(CCRotateBy::create(0.3f, 110),
		CCEaseBackInOut::create(CCRotateBy::create(0.2f, -110)),
		CCCallFuncN::create(this, callfuncN_selector(CLevelUpItem::showHammerSoundCallBack)),CCDelayTime::create(0.1f),nullptr), iTimesHit-1),
		CCFadeOut::create(0.2f),
		nullptr));

	float fTime = 0.8f+iTimesHit*0.6f;


	CCSprite* pText = CCSprite::create("strengthen/word_32.png");
	pText->setPosition(ccp(pPosNode->getPositionX(), pPosNode->getPositionY()-100));
	lay->addChild(pText, 3);
	pText->setVisible(false);
	pText->setScale(0.7f);
	pText->runAction(CCSequence::create(
		CCDelayTime::create(fTime-0.4f),
		CCShow::create(),
		CCFadeIn::create(0.2f),
		CCSpawn::createWithTwoActions(CCMoveBy::create(0.5f, ccp(0, 100)), CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.0f), CCFadeOut::create(0.3f))),
		nullptr));

	lay->runAction(CCSequence::create(CCDelayTime::create(fTime), CCCallFuncN::create(this, callfuncN_selector(CLevelUpItem::showHammerEffectCallBack)),nullptr));
	return fTime+0.5f;
}

void CLevelUpItem::showHammerEffectCallBack(CCNode* pSender)
{
	//强化成功
	PlayEffectSound(SFX_407);

	CCNode* pNode = (CCNode*)m_ui->findWidgetById("alpha_panel");
	float fTime = ShowTVLight(pNode, pSender);
	CCNodeRGBA* pWhitePanel = (CCNodeRGBA*)m_ui->findWidgetById("white_panel");
	pWhitePanel->setOpacity(0);
	pWhitePanel->runAction(CCSequence::create(CCDelayTime::create(fTime), CCFadeTo::create(0.15f, 180), CCFadeOut::create(0.03f), nullptr));

	pSender->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fTime), CCCallFunc::create(this, callfunc_selector(CLevelUpItem::showTVLightCallBack))));
}

void CLevelUpItem::showHammerSoundCallBack(CCNode* pSender)
{
	////强化成功
	//PlayEffectSound(SFX_432);

	CCNode* lay = pSender->getParent();
	//获取位置参考
	CCNode* pPosNode = (CCNode*)m_ui->findWidgetById("why");
	//增加光效
	CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("9007");
	culAnim->setDelayPerUnit(0.05f);
	CCAnimate* pAnimate = CCAnimate::create(culAnim);
	CCSprite *culSpr = CCSprite::create("skill/9007.png");
	culSpr->setPosition(pPosNode->getPosition());
	lay->addChild(culSpr, 1);
	culSpr->setVisible(false);
	culSpr->setScale(1.3f);
	culSpr->runAction(CCSequence::create(
		CCShow::create(),
		pAnimate,
		CCRemoveSelf::create(),
		nullptr));

	m_ui->setScale(1.05f);
	m_ui->runAction(CCSequence::createWithTwoActions(CCShake::create(0.4f,13.0f), CCCallFunc::create(this, callfunc_selector(CLevelUpItem::showHammerShakeCallBack))));
	PlayEffectSound(SFX_427);
}

void CLevelUpItem::showHammerShakeCallBack()
{
	m_ui->setScale(1.0f);
}

void CLevelUpItem::roleUpdate( const TMessage& tMsg )
{
	checkMoney();
}


void CLevelUpItem::initItemInfo( CCPoint posStart, CLayout* pLayout[], int iCount )
{
	for(int i=0; i<iCount; i++)
	{
		CLayout* pCopyLayout = UICloneMgr::cloneLayout(m_pCellInfo);
		pCopyLayout->setPosition(posStart+ccp(0, -i*40));
		m_itemAttr->addChild(pCopyLayout, 911);
		pLayout[i] = pCopyLayout;
		pLayout[i]->setVisible(false);
	}
}

void CLevelUpItem::hideNoneValueCell( int iCount, CLayout* pLayout[] )
{
	for(unsigned int i=iCount; i<4; i++)
	{
		pLayout[i]->setVisible(false);
	}
}

int CLevelUpItem::updateInfoToUIAndReturnCount( CLayout* pLayout[], int iNameId[], int iValue[], int iValueAim[], int iMax, const char* sTitle )
{
	int iMaxLineCount = 0;

	for(unsigned int i=0; i<iMax; i++)
	{
		//有值
		if(iValue[i] > 0)
		{
			pLayout[iMaxLineCount]->setVisible(true);
			//设置名称
			CLabel* pInfoType = (CLabel*)pLayout[iMaxLineCount]->findWidgetById("info_type");
			pInfoType->setString(GETLANGSTR(iNameId[i]));
			//设置值大小
			CLabel* pValue = (CLabel*)pLayout[iMaxLineCount]->findWidgetById("value");
			if(atoi(pValue->getString())!=iValue[i])
			{
				pValue->setString(ToString(iValue[i]));
				pValue->runAction(CCRollLabelAction::create(0.3f, 0, iValue[i], pValue));
			}
			//目标值大小
			CLabel* pAimValue = (CLabel*)pLayout[iMaxLineCount]->findWidgetById("aim_value");
			pAimValue->setString(ToString(iValueAim[i]));
			if(isMaxLevel())
			{
				pAimValue->setString("????");
			}

			iMaxLineCount++;
		}

		//找到四个了，退出循环
		if(iMaxLineCount>=4)
		{
			break;
		}
	}
	//隐藏掉没有数据填充的cell
	hideNoneValueCell(iMaxLineCount, pLayout);

	//如果一个都没有，隐藏标题
	if(sTitle != nullptr)
	{
		CCNode* pNode = (CCNode*)m_ui->findWidgetById(sTitle);
		if(pNode)
		{
			pNode->setVisible(iMaxLineCount != 0);
		}
	}
	return iMaxLineCount;
}

void CLevelUpItem::checkMoney()
{
	CLabel* pCoin = dynamic_cast<CLabel*>(m_ui->findWidgetById("coin_num"));
	//检查货币数量
	if(!CheckCoin(atoi(pCoin->getString())))
	{
		pCoin->setColor(RGB_RED);
	}
	else
	{
		pCoin->setColor(RGB_WHITE);
	}
}

bool CLevelUpItem::isMaxLevel()
{
	return m_strengthItem.maxLevel==m_strengthItem.armor.itemLevel;
}

void CLevelUpItem::showTVLightCallBack()
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CLevelUpItem::sameEquip( CCObject* pObj )
{
	if(m_strengthItem.armor.itemId == 0)
	{
		return;
	}
	CStarUpItem* pData = dynamic_cast<CStarUpItem*>(pObj);
	if(pData)
	{
		int iId = pData->getItemId();
		if(m_strengthItem.armor.id == iId)
		{
			//清空当前数据
			clearEquip();
			m_strengthItem.armor.itemId = 0;
		}
	}
}

void CLevelUpItem::clearEquip()
{
	CLabel *tip = dynamic_cast<CLabel*>(m_ui->findWidgetById("tip"));
	tip->setVisible(true);
	CCNode *hero = dynamic_cast<CCNode*>(m_ui->findWidgetById("hero"));
	hero->setVisible(true);

	CLayout *attribute = (CLayout*)(m_ui->findWidgetById("attr"));
	attribute->setVisible(false);

	CCSprite *why = (CCSprite*)(m_ui->findWidgetById("why"));
	why->setVisible(true);

	//装备图标
	CButton *btn = dynamic_cast<CButton*>(m_ui->findWidgetById("bgBtn"));
	btn->removeChildByTag(1);

	//等级
	CLabel* pLevel = dynamic_cast<CLabel*>(m_ui->findWidgetById("level"));
	CCNode* pLevelBg = dynamic_cast<CCNode*>(m_ui->findWidgetById("level_mask"));
	pLevel->setVisible(false);
	pLevelBg->setVisible(false);

	//星星层
	btn->removeChildByTag(415);
}

int CLevelUpItem::getItemId()
{
	return m_strengthItem.armor.id;
}

