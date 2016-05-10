#include "CCompaseLayer.h"
#include "DefendWuJiang.h"
#include "CityData.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "mainCity/QuickProduct.h"
#include "tools/UICloneMgr.h"
#include "MainCityControl.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "Global.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "scene/AnimationManager.h"

#include "common/color.h"
#include "bag/BagInfoTip.h"
#include "common/CGameSound.h"
#include "common/ShaderDataHelper.h"
#include "scene/layer/PropGetInfo.h"
#include "StrengthenItem.h"
#include "common/CommonFunction.h"
#include "common/CShowToBuyResource.h"
#include "tools/CCShake.h"
#include "common/CheckMoney.h"
#include "guide/GuideManager.h"

bool SortCComposeData(const CComposeData& data1, const CComposeData& data2)
{
	return data1.isCanGet>data2.isCanGet;
}


CCompaseLayer::CCompaseLayer():m_selectHero(0),m_selectItem(nullptr),m_selectIndex(0),m_armorId(0),m_switchType(0)
{
	m_tableOffset = CCPointZero; m_pSearchItem.iconId = NULL;
}


CCompaseLayer::~CCompaseLayer()
{
	CC_SAFE_RELEASE(m_cell);
}


bool CCompaseLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("CCompaseLayerMaskLayer");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		
		m_ui = LoadComponent("compose.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CCompaseLayer::onEnter()
{ 
	BaseLayer::onEnter();

	m_switchType =1;
	
	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(m_ui->convertToNodeSpace(ccp(VLEFT+50, VTOP-50)));
	pClose->setId("close");
	pClose->setOnClickListener(this,ccw_click_selector(CCompaseLayer::onClose));
	m_ui->addChild(pClose, 999);

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_poseLayer = (CLayout*)(m_ui->findWidgetById("pose"));	
	m_poseLayer->setVisible(false);


	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CCompaseLayer::tableviewDataSource));
	m_tableView->reloadData();

	m_selectCellImg = (CCSprite*)(m_ui->findWidgetById("selectImg"));
	m_selectCellImg->retain();

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	for (int i = 0; i < 2; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CCompaseLayer::onSwitchBtn));
	}

	CButton *compose = (CButton *)(m_poseLayer->findWidgetById("strengthen"));
	compose->setOnClickListener(this,ccw_click_selector(CCompaseLayer::onComposeButton));

	//m_ui->setScale(0.0f);
	//CCScaleTo *big = CCScaleTo::create(0.2f,1.0f);
	//m_ui->runAction(big);

	//效果
	CCSprite* pSprite = (CCSprite *)m_ui->findWidgetById("run_circle");
	//pSprite->setScale(0.55f);
	pSprite->runAction(CCRepeatForever::create(CCRotateBy::create(1.0f, 60)));

	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("BoneAnimation/8021.ExportJson");

	GetTcpNet->registerMsgHandler(ComposeDataMsg,this,CMsgHandler_selector(CCompaseLayer::processNetMsg));
	GetTcpNet->registerMsgHandler(ComposeItemMsg,this,CMsgHandler_selector(CCompaseLayer::processNetMsg));

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(CCompaseLayer::roleUpdate));
}

void CCompaseLayer::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;
	int selIndex = 2*btn->getTag();
	if (bChecked)
	{

		PlayEffectSound(SFX_Button);

		m_switchType = btn->getTag();
		m_tableView->setCountOfCell(m_composeMap[m_switchType].size());
		m_tableView->reloadData();

		if (m_composeMap[m_switchType].size()>0)
		{
			m_poseLayer->setVisible(true);
			updateCompose(&m_composeMap[m_switchType].at(0));
			m_selectItem =&m_composeMap[m_switchType].at(0).item;
			m_selectIndex = 0;
		}
		else
		{
			CPlayerControl::getInstance().sendComposeItem(m_switchType);
 			m_poseLayer->setVisible(false);
		}

		CLayout *attr = (CLayout*)(m_poseLayer->findWidgetById("attr"));
		attr->setVisible(m_switchType==1);

		CImageView *bg_1 = (CImageView*)(m_ui->findWidgetById("bg_1"));
		bg_1->setVisible(m_switchType==1);
		CImageView *bg_2 = (CImageView*)(m_ui->findWidgetById("bg_2"));
		bg_2->setVisible(m_switchType==2);
	}
}

CCObject* CCompaseLayer::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell *pCell = (CTableViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CTableViewCell;
		pCell->autorelease();

		addTableCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx, pCell);
	}
	return pCell;
}

void CCompaseLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CCompaseLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	BaseLayer::onExit();
	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("BoneAnimation/8021.ExportJson");
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO, this);
}

void CCompaseLayer::onComposeButton(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	CComposeData *data = (CComposeData*)btn->getUserData();
	m_tableOffset = m_tableView->getContentOffset();

	//素材不足
	if(!data->isCanGet)
	{
		ShowPopTextTip(GETLANGSTR(251));
		return;
	}

	//金币不足
	if(!CheckCoin(data->item.buyPrice))
	{
		CShowToBuyResource* pShow = CShowToBuyResource::create();
		pShow->showToBuyResourceByType(ShowBuyResourceCoin);
		return;
	}

	if (m_selectItem)
	{
		CMainCityControl::getInstance()->sendComposeItem(m_selectItem->itemId, m_armorId);
	}
}

void CCompaseLayer::showComposeItem()
{
	m_tableView->setCountOfCell(m_composeMap[m_switchType].size());
	m_tableView->reloadData();

	if (m_selectIndex>=0&&m_selectIndex<m_composeList.size())
	{
		CComposeData &data = m_composeList[m_selectIndex];
		if (m_selectIndex!=0)
		{
			m_tableView->setContentOffset(m_tableOffset);
		}
		updateCompose(&m_composeList[m_selectIndex]);
	}

}

void CCompaseLayer::onSelectItem(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CButton *btn = (CButton*)pSender;
	m_selectCellImg->removeFromParent();

	btn->addChild(m_selectCellImg);
	m_selectCellImg->setVisible(true);
	m_selectCellImg->setPosition(ccp(0,0));

	CComposeData &data =m_composeMap[m_switchType].at(btn->getTag()-100);
	updateCompose(&data);

	m_selectItem = (CItem*)btn->getUserData();
	m_selectIndex = btn->getTag()-100;
}

void CCompaseLayer::processNetMsg(int type, google::protobuf::Message *msg)
{
	bool isFirst = false;
	if (!this->isVisible())
	{
		PlayEffectSound(SFX_432);
		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
		this->setVisible(true);
		isFirst = true;
	}

	switch (type)
	{
	case ComposeDataMsg:
		{
			 ComposeResponse *res = (ComposeResponse*)msg;
			 m_composeList.clear();

			 vector<CComposeData> composeArmorVec;
			 
			 for (int i = 0; i < res->composelist_size(); i++)
			 {
				 CComposeData data;
				 data.read(res->composelist(i)); 
				 m_composeList.push_back(data);
			 }

			  std::sort(m_composeList.begin(), m_composeList.end(), SortCComposeData);

			 for (int i = 0; i < res->composelist_size(); i++)
			 { 
				 composeArmorVec.push_back(m_composeList.at(i));
			 }

			 m_composeMap[m_switchType] = composeArmorVec;
			 if (m_composeList.size()>0)
			 {
				 showComposeItem();
				 m_poseLayer->setVisible(true);
			 }

			 findSearchItem();
		}
		
		break;
	case ComposeItemMsg:
		{
			BuyItemResponse *res = (BuyItemResponse*)msg;
			if (res->result()==1)
			{
 			//	ShowTexttip(U8("调和成功"),RGB_RED,0,VCENTER,2.0f,2.0f,0.0f,10.0f);		
			//	AnimationManager::sharedAction()->getEffectAnimation()
				CCAnimation *anim = AnimationManager::sharedAction()->getAnimation("compose");
				CCSprite *comSpr = CCSprite::create("skill/compose.png");
				CCNode* pMask = (CCNode*)m_poseLayer->findWidgetById("mask");
				comSpr->setPosition(ccp(pMask->getPositionX()+pMask->getContentSize().width*0.5f, pMask->getPositionY()+pMask->getContentSize().height*0.5f));
				m_poseLayer->addChild(comSpr);
				comSpr->setVisible(false);
				CCCallFuncN *call = CCCallFuncN::create(this, callfuncN_selector(CCompaseLayer::removeAnimation));
				comSpr->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCShow::create(), CCAnimate::create(anim), call, nullptr));
				

				NOTIFICATION->postNotification(COMPOSE_SUCCESS);
				
				MaskLayer* pLayer1 = MaskLayer::create("ComposeItemMsg1");
				pLayer1->setOpacity(0);
				MaskLayer* pLayer2 = MaskLayer::create("ComposeItemMsg2");
				pLayer2->setOpacity(0);
				LayerManager::instance()->push(pLayer1);
				LayerManager::instance()->push(pLayer2);
				pLayer2->runAction(CCSequence::create(CCDelayTime::create(1.5f), CCCallFunc::create(this, callfunc_selector(CCompaseLayer::callBackForReleaseTouchAfterCompase)),nullptr));

				//强化成功
				PlayEffectSound(SFX_424);
				CCSprite* pText = CCSprite::create("compose/word_34.png");
				pText->setPosition(ccp(comSpr->getPositionX(), comSpr->getPositionY()-100));
				m_ui->addChild(pText, 3);
				pText->setVisible(false);
				pText->setScale(0.7f);
				pText->runAction(CCSequence::create(
					CCDelayTime::create(0.5f),
					CCShow::create(),
					CCFadeIn::create(0.2f),
					CCSpawn::createWithTwoActions(CCMoveBy::create(0.5f, ccp(0, 100)), CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.0f), CCFadeOut::create(0.3f))),
					CCRemoveSelf::create(),
					CCCallFunc::create(this, callfunc_selector(CCompaseLayer::callBackForRequestNewDataAfterCompase)),
					nullptr));

				//飞素材进去
				CLayout* itemLayout = (CLayout*)m_ui->findWidgetById("item_list");
				for (int i=1; i <=4; i++)
				{
					CLayout* lay = (CLayout*)itemLayout->findWidgetById(CCString::createWithFormat("item%d", i)->getCString());
					CImageView *masks = (CImageView*)(m_ui->findWidgetById(CCString::createWithFormat("mask%d",i)->getCString()));
					if (lay->isVisible())
					{
						CImageView* icon = UICloneMgr::cloneImageView((CImageView*)masks->getChildByTag(1));
						icon->setPosition(masks->getPosition()+ccp(masks->getContentSize().width/2, masks->getContentSize().height/2));
						lay->addChild(icon, 99);
						icon->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCMoveTo::create(0.3f, comSpr->getPosition()), CCFadeTo::create(0.3f, 105)), 
							CCRemoveSelf::create(),
							nullptr));
					}
				}
			}
			//2钱不够，3道具已售完，4数据错误，5材料不足，6背包空间不足
			else
			{
				switch (res->result())
				{
				case 2:				
					{
						//ShowPopTextTip(GETLANGSTR(205));
						CShowToBuyResource* pShow = CShowToBuyResource::create();
						pShow->showToBuyResourceByType(ShowBuyResourceCoin);
					}
					break;
				case 3:
					ShowPopTextTip(GETLANGSTR(206));		
					break;
				case 4:
					ShowPopTextTip(GETLANGSTR(170));		
					break;
				case 5:
					ShowPopTextTip(GETLANGSTR(234));
					break;
				case 6:
					ShowPopTextTip(GETLANGSTR(178));		
					break;
				default:
					break;
				}
			}

			if (res->has_role())
			{
				UserData *user = DataCenter::sharedData()->getUser()->getUserData();
				user->read(res->role());
				CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
			}
		}
		break;

	default:
		break;
	}

	if(isFirst)
	{
		showEffect();
	}
}

void CCompaseLayer::removeAnimation(CCNode* node)
{
	node->removeFromParent();
}

void CCompaseLayer::updateCompose(CComposeData *data)
{
	CLabel *atk = (CLabel*)(m_poseLayer->findWidgetById("atk"));
	atk->setString(ToString(data->item.armor.baseAtk));

	CLabel *def = (CLabel*)(m_poseLayer->findWidgetById("def"));
	def->setString(CCString::createWithFormat("%d",data->item.armor.baseDef)->getCString());

	CLabel *hp = (CLabel*)(m_poseLayer->findWidgetById("hp"));
	hp->setString(ToString(data->item.armor.baseHp));

	CButton *btn = (CButton *)(m_poseLayer->findWidgetById("strengthen"));
	//btn->setDisabledImageColor();
	//btn->setEnabled(data->isCanGet);
	btn->setTag(data->item.iconId);
	btn->setUserData(data);

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(data->item.itemId);
	CLabel *skill = (CLabel*)(m_poseLayer->findWidgetById("skill"));
//	skill->setVisible(m_switchType==2);
	if (m_switchType==2)
	{
		if (itemData)
		{
			skill->setString(itemData->itemDesc.c_str());
			skill->setPositionY(180);
			skill->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
		}
		else
		{
			skill->setString("");
		}
	}
	else
	{
		if (data->item.armor.armor_skill>0&&itemData)
		{
			skill->setString(itemData->skillDesc.c_str());
			skill->setPositionY(150);
			skill->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentCenter);
		}
		else
		{
			skill->setString("");
		}
	}
	
	
	//CCSprite *grayImg =(CCSprite*)(m_poseLayer->findWidgetById("grayImg"));
	//grayImg->setVisible(!data->isCanGet);

	int i = 1;

	CCSprite *itemBg = (CCSprite*)(m_poseLayer->findWidgetById("itembg1"));
	itemBg->removeChildByTag(999);

	CCSprite *mask = (CCSprite*)(m_poseLayer->findWidgetById("mask"));
	mask->setTexture(setItemQualityTexture(data->item.quality));

	m_poseLayer->removeChildByTag(999);
	CImageView *prop = getItemSprite(&data->item);
	if (!prop)
	{
		prop = CImageView::create("prop/32003.png");
		CCLOG("[ ERROR ] CCompaseLayer::updateCompose Lost Image = %d",data->item.iconId);
	}
	if (prop)
	{
		prop->setPosition(ccpAdd(mask->getPosition(),ccp(mask->getContentSize().width/2,mask->getContentSize().height/2)));
		prop->setTouchEnabled(true);
		prop->setEnabled(true);
// 		prop->setOnLongClickListener(this,ccw_longclick_selector(CCompaseLayer::longClickItemListener));
		prop->setUserData(&data->item);
		
		CCRepeatForever* pAction1 = CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0, 15)), CCMoveBy::create(0.7f, ccp(0, -15))));
		pAction1->setTag(121);
		CCRepeatForever* pAction2 = CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0, 15)), CCMoveBy::create(0.7f, ccp(0, -15))));
		pAction2->setTag(121);
		CCRepeatForever* pAction3 = CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0, 15)), CCMoveBy::create(0.7f, ccp(0, -15))));
		pAction3->setTag(121);

		if(!itemBg->getActionByTag(121))itemBg->runAction(pAction1);
		if(!mask->getActionByTag(121))mask->runAction(pAction2);
		
		//装备
		if(m_switchType==1)
		{
			itemBg->setVisible(false);
			mask->setVisible(false);
			prop->setScale(1.3f);
			m_poseLayer->addChild(prop,0,999);

			if(!prop->getActionByTag(121))prop->runAction(pAction3);
		}
		//材料
		else
		{
			itemBg->setVisible(true);
			mask->setVisible(true);
			prop->setScale(1.0f);
			prop->stopAllActions();
			itemBg->addChild(prop,0,999);
			NodeFillParent(prop);
		}
	} 
	 
	CLabel *coin = (CLabel*)(m_poseLayer->findWidgetById("coin"));
	coin->setString(ToString(data->item.buyPrice));
	//检查货币数量
	if(!CheckCoin(atoi(coin->getString())))
	{
		coin->setColor(RGB_RED);
	}
	else
	{
		coin->setColor(RGB_WHITE);
	}


	CLabel *ned = (CLabel*)m_ui->findWidgetById("need5");
	ned->setVisible(false);

	CLayout* itemLayout = (CLayout*)m_ui->findWidgetById("item_list");

	for (i=1; i <=4; i++)
	{
		CLayout* lay = (CLayout*)itemLayout->findWidgetById(CCString::createWithFormat("item%d", i)->getCString());
		CImageView *masks = (CImageView*)(m_ui->findWidgetById(CCString::createWithFormat("mask%d",i)->getCString()));
		CLabel *needOld = (CLabel*)(m_ui->findWidgetById(CCString::createWithFormat("need%d",i)->getCString()));
		needOld->setVisible(false);
		CCLabelAtlas* need = (CCLabelAtlas*)masks->getChildByTag(11);
		if(need==nullptr)
		{
			need = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
			need->setAnchorPoint(ccp(0.5f, 0.0f));
			need->setPosition(ccp(45, 5));
			masks->addChild(need, 11, 11);
		}
		

		if (i<=data->itemList.size())
		{
			lay->setVisible(true);
			CItem &it = data->itemList[i-1];
			
			//数量更新
			need->setString(CCString::createWithFormat("%d:%d",it.itemNum,it.itparam)->getCString());
			if (it.itemNum<it.itparam)
			{
				need->setColor(ccRED);
			}
			else
			{
				need->setColor(RGB_WHITE);
			}

			//武器特殊处理
			if(it.itemType==2)
			{
				if (it.itemNum>=it.itparam && it.itemNum>0)
				{
					masks->setTouchEnabled(true);
					masks->setUserData(&data->itemList.at(i-1));
					masks->setOnClickListener(this,ccw_click_selector(CCompaseLayer::onTouchProp));
					ned->setVisible(false);
				}
				else if(it.itemNum==0)
				{
					ned->setVisible(true);
					//need->setString("0:1");
					masks->setOnClickListener(nullptr,nullptr);
				}
			}
// 			CCSprite *img = (CCSprite*)(needLayout->findWidgetById(CCString::createWithFormat("mask%d.png",i+1)->getCString()));
// 			img->removeChildByTag(1);

			masks->setTexture(setItemQualityTexture(it.quality));
			masks->removeChildByTag(1);
			masks->setUserData(&it);
			masks->setTouchEnabled(true);
			if (it.itemType!=2)
			{		
				masks->setOnClickListener(this, ccw_click_selector(CCompaseLayer::itemGetInfo));
			}
			CImageView *prop = getItemSprite(&it);
			if (!prop)
			{
				prop = CImageView::create("prop/32003.png");
				CCLOG("[ ERROR ] CCompaseLayer::updateCompose Lost Image = %d",it.iconId);
			}
			if (prop)
			{
				prop->setPosition(ccp(masks->getContentSize().width/2,masks->getContentSize().height/2));
				masks->addChild(prop, -1, 1);
			}
		}
		else
		{
			lay->setVisible(false);
		}
	}
}

bool CCompaseLayer::longClickItemListener(CCObject* pSender, CCTouch* pTouch)
{
	CImageView *prop = (CImageView*)pSender;
	CBagInfoTip* tip =CBagInfoTip::create();
	LayerManager::instance()->push(tip);
	tip->setItemData(2,(CItem*)prop->getUserData());
	return true;
}

void CCompaseLayer::onTouchProp(CCObject* pSender)
{
	CImageView *spr = (CImageView*)pSender;
	CItem *item = (CItem*)spr->getUserData();
	CStrengthenItem *selectItem = CStrengthenItem::create();
	selectItem->setVisible(false);
	selectItem->setOnSelectItemListener(this,ccw_select_Item(CCompaseLayer::onSelectItemArmor));
	LayerManager::instance()->push(selectItem);
	CPlayerControl::getInstance().sendArmorList(item->itemId);
}

void CCompaseLayer::onSelectItemArmor(CItem* pItem)
{
	if (pItem)
	{
		m_armorId = pItem->id;
	}
	else
	{
		m_armorId = 0;
	}
}

void CCompaseLayer::showSelectRadioImg(int selIndex)
{
	//for (int i=1; i<=4; ++i)
	//{
	//	CCNode *spr = nullptr;
	//	if (i%2==0)
	//	{
	//		spr= (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("switch%d",i)->getCString());
	//		spr->setVisible(true);
	//	}
	//	else
	//	{
	//		spr = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("switch%d",i)->getCString());
	//		spr->setVisible(false);
	//	}
	//	if (selIndex ==i)
	//	{
	//		spr->setVisible(false);
	//	}
	//	else if (selIndex==i+1)
	//	{
	//		spr->setVisible(true);
	//	}
	//}
}

void CCompaseLayer::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CComposeData &data = m_composeMap[m_switchType].at(uIdx);/*m_composeList[uIdx];*/
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(data.item.itemId);

	for (int i=1;i<=9;++i)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		pCell->setTag(uIdx+111);
		if (i==1)
		{
			child->setTag(uIdx+100);
			child->setUserData(&m_composeMap[m_switchType].at(uIdx).item);
			((CButton*)child)->setOnClickListener(this,ccw_click_selector(CCompaseLayer::onSelectItem));

			if(m_selectIndex==uIdx)
			{
				onSelectItem(child);
			}

		}
		else if(i==3)
		{
			CLabel *name = (CLabel*)child;
			if(itemData)
			{
				name->setString(itemData->itemName.c_str());
			}
		}
		else if (i==2)
		{
			((CButton*)child)->setEnabled(false);
			CCSprite *prop = CCSprite::create(GetImageName(1, data.item.quality, data.item.iconId).c_str());
			if (prop)
			{
				prop->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
				child->addChild(prop,0,1);
			}
			if(!prop)
			{
				prop = CImageView::create("prop/32003.png");
				prop->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
				child->addChild(prop,0,1);
			}
		}
		else if (i==4&&data.isCanGet)
		{
			child->setVisible(false);
		}
		else if (i==5)
		{
			((CCSprite*)child)->setTexture(setItemQualityTexture(data.item.quality));
		}
		else if (i==6)
		{
			((CCSprite*)child)->setVisible(!data.isCanGet);
		}
		else if (i==7)
		{
			CLabel *name = (CLabel*)child;
			if (data.item.armor.armor_skill>0&&itemData)
			{
				//if(itemData->skillName.size()>0)
				name->setString(itemData->skillName.c_str());
			}
			else
			{
				name->setString("");
			}
		}
		else if (i==8)
		{
			if (itemData)
			{
				if(itemData->skillName.size()<=0||data.item.armor.armor_skill<=0)
				{
					child->setVisible(false);
				}
			}
		}
		else if (i==9)
		{
			if (itemData)
			{
				if(itemData->skillName.size()<=0||data.item.armor.armor_skill<=0)
				{
					child->setVisible(false);
				}
			}
		}
	}
}

void CCompaseLayer::itemGetInfo( CCObject* pSender )
{
	CCNode* pNode = (CCNode*)pSender;
	CItem* item =  (CItem*)(pNode->getUserData());
	PropGetInfo* pLayer = PropGetInfo::create();
	LayerManager::instance()->push(pLayer);
	CPlayerControl::getInstance().sendItemInfo(item->itemId);
}

void CCompaseLayer::showEffect()
{
	if(!CGuideManager::getInstance()->getIsRunGuide())
	{
		//鸟进来，item展开，
		CCArmature* pArmature = CCArmature::create("8021");
		pArmature->setPosition(ccp(-250, 50));
		pArmature->setScale(0.5f);
		pArmature->getAnimation()->playWithIndex(0);
		m_ui->addChild(pArmature, 3);
		pArmature->runAction(CCSequence::create(CCMoveBy::create(0.2f, ccp(400, 0)), CCDelayTime::create(1.1f), CCFadeOut::create(0.3f), nullptr));
	}

	CLayout* buttonLay = (CLayout*)m_ui->findWidgetById("button_lay");
	buttonLay->setPositionY(buttonLay->getPositionY()-300);
	buttonLay->runAction(CCMoveBy::create(0.2f, ccp(0, 300)));
	

	//数据条飞入，震屏3次
	for(unsigned int i=0; i<3; i++)
	{
		CLayout* lay = (CLayout*)m_ui->findWidgetById(CCString::createWithFormat("lay%d", i+1)->getCString());
		lay->setVisible(false);
		lay->setPositionX(lay->getPositionX()+300);
		lay->runAction(CCSequence::create(
			CCDelayTime::create(i*0.15f+0.2f),
			CCShow::create(),
			CCMoveBy::create(0.1f, ccp(-300, 0)),
			CCCallFunc::create(this, callfunc_selector(CCompaseLayer::shake))
			,nullptr));
	}
	//鸟离开，黑底渐隐
	CCSprite* pBlack = (CCSprite*)m_ui->findWidgetById("black_bar");
	pBlack->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.3f), CCFadeOut::create(0.3f)));
}

void CCompaseLayer::shake()
{
	m_ui->setScale(1.02f);
	m_ui->runAction(CCSequence::createWithTwoActions(
		CCShake::create(0.1f,8.0f), 
		CCScaleTo::create(0.0f, 1.0f)		
		));
}

void CCompaseLayer::touchRadioButton( int iTag )
{
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	CRadioButton *radioBtn1= (CRadioButton*)(radioGroup->getChildByTag(1));
	CRadioButton *radioBtn2= (CRadioButton*)(radioGroup->getChildByTag(2));
	if(iTag==2)
	{
		onSwitchBtn(radioBtn2, true);
		radioBtn1->setChecked(false);
		radioBtn2->setChecked(true);
	}
	else
	{
		onSwitchBtn(radioBtn1, true);
		radioBtn2->setChecked(false);
		radioBtn1->setChecked(true);
	}
	
}

void CCompaseLayer::findSearchItem()
{
	if(m_pSearchItem.iconId == NULL)
	{
		return;
	}

	for(unsigned int i=0; i<m_tableView->getCountOfCell(); i++)
	{
		CComposeData &data = m_composeMap[m_switchType].at(i);
		if(data.item.iconId == m_pSearchItem.iconId)
		{
			m_pSearchItem.iconId = NULL;
			m_selectIndex = i;
			int iGap = m_tableView->getCountOfCell()-i-1;
			iGap -= 3;
			iGap = iGap<0?0:iGap;
			float fTime = 0.5f;
			m_tableView->setContentOffsetInDuration(ccp(0, -iGap*m_tableView->getSizeOfCell().height), fTime);
			this->runAction(CCSequence::create(CCDelayTime::create(fTime), CCCallFunc::create(this, callfunc_selector(CCompaseLayer::callBackTouchSearchItem)), nullptr));
			this->setTouchEnabled(false);
			break;
		}
	}
}

void CCompaseLayer::callBackTouchSearchItem()
{
	CButton* pButton = (CButton*)m_tableView->getContainer()->getChildByTag(m_selectIndex+111)->getChildByTag(m_selectIndex+100);
	onSelectItem(pButton);
	this->setTouchEnabled(true);
}

void CCompaseLayer::roleUpdate( const TMessage& tMsg )
{
	CLabel *coin = (CLabel*)(m_poseLayer->findWidgetById("coin"));
	//检查货币数量
	if(!CheckCoin(atoi(coin->getString())))
	{
		coin->setColor(RGB_RED);
	}
	else
	{
		coin->setColor(RGB_WHITE);
	}
}

void CCompaseLayer::callBackForReleaseTouchAfterCompase()
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CCompaseLayer::callBackForRequestNewDataAfterCompase()
{
	CPlayerControl::getInstance().sendComposeItem(m_switchType);
}
