#include "SmeltArmor.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "StrengthenItem.h"

#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "common/CheckMoney.h"
#include "Resources.h"
#include "tools/CCShake.h"
#include "common/CGameSound.h"
#include "common/CommonFunction.h"
#include "common/CShowToBuyResource.h"
#include "tools/UICloneMgr.h"
#include "Strengthen.h"
#include "common/CCRollLabelAction.h"


CSmeltArmor::CSmeltArmor():m_noEquip(true),m_yesEquip(true)
	,m_coin(0),m_diamond(0),m_pFire1(nullptr),m_pFire2(nullptr),m_iStrengthType(0),m_pCellInfo(nullptr)
	,m_iCount(0),m_pSpineHero(nullptr)
{
	for(unsigned int i=0; i<4; i++)
	{
		m_pBaseInfo[i] = nullptr;
	}
}

using namespace BattleSpace;
CSmeltArmor::~CSmeltArmor()
{
	CC_SAFE_RELEASE(m_pCellInfo);
}


bool CSmeltArmor::init()
{
	if (BaseLayer::init())
	{
		m_ui = LoadComponent("RinseItem.xaml");  //  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setIsShowBlack(false);
		
		m_attr = (CLayout*)m_ui->findWidgetById("attr");

		//获取cell
		m_pCellInfo = (CLayout*)m_ui->findWidgetById("cell_info");
		CC_SAFE_RETAIN(m_pCellInfo);
		m_pCellInfo->removeFromParentAndCleanup(false);

		CCPoint pBasePos = ccp(1281, 160);
		//初始化基础属性四条
		initItemInfo(pBasePos, m_pBaseInfo, 4);

		//添加动画
		SkeletonAnimation *pSkeletonAnimation = SkeletonAnimation::createWithFile("strengthen/5002.json", "strengthen/5002.atlas", 1);
		pSkeletonAnimation->setPosition(ccp(887, -2));
		pSkeletonAnimation->setAnimation(0, "stand", true);
		m_ui->addChild(pSkeletonAnimation, 3);
		m_pSpineHero = pSkeletonAnimation;

		return true;
	}
	return false;

}



void CSmeltArmor::onEnter()
{
	BaseLayer::onEnter();

	//四个选择框
	for (unsigned int i=0; i<4; ++i)
	{
		CCheckBox *checkBtn = (CCheckBox*)(m_pBaseInfo[i]->findWidgetById("check"));
		checkBtn->setOnCheckListener(this,ccw_check_selector(CSmeltArmor::onCheckAttr));
	}

	//选择武器
	CButton *select = (CButton*)(m_ui->findWidgetById("select"));
	select->setOnClickListener(this, ccw_click_selector(CSmeltArmor::onSelectItem));

	//洗练
	CButton *refine = (CButton*)(m_ui->findWidgetById("refine"));
	refine->setOnClickListener(this, ccw_click_selector(CSmeltArmor::onRefineItem));
	
	//初始化火
	initFire();

	//旋转圈
	CImageView *circleFire = (CImageView*)(m_ui->findWidgetById("fire_circle"));
	circleFire->setScale(1.95f);
	circleFire->runAction(CCRepeatForever::create(CCRotateBy::create(1.0f, 60)));
	circleFire->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(1.2f, 2.1f), CCScaleTo::create(1.2f, 1.95f))));


	GetTcpNet->registerMsgHandler(SmeltArmorMsg,this,CMsgHandler_selector(CSmeltArmor::proceesMessage));
	GetTcpNet->registerMsgHandler(SmeltDataRequestMsg,this,CMsgHandler_selector(CSmeltArmor::proceesMessage));

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(CSmeltArmor::roleUpdate));
	
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_ITEM_DATA, this, GameMsghandler_selector(CSmeltArmor::updateStar));
}

void CSmeltArmor::onExit()
{
	BaseLayer::onExit();

	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
	
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_ITEM_DATA,this);

	GetTcpNet->unRegisterAllMsgHandler(this);

	NOTIFICATION->removeAllObservers(this);
}

//显示或隐藏选中框
void CSmeltArmor::setYellowBgState(CCheckBox* pCheck, bool isCheck)
{
	CCNode* pMask = ((CLayout*)pCheck->getParent())->findWidgetById("black");
	if (pCheck->isVisible())
	{
		pMask->setVisible(isCheck);
	}
	else
	{
		pMask->setVisible(false);
	}
}

void CSmeltArmor::onCheckAttr(CCObject *pSender, bool bChecked)
{
	int checknum =0;
	int iShowCount = 0;

	for (unsigned int i=0; i<4; ++i)
	{
		CCheckBox* pCheckBtn = (CCheckBox*)(m_pBaseInfo[i]->findWidgetById("check"));
		if (pCheckBtn->isChecked())
		{
			checknum++;
		}
		if(m_pBaseInfo[i]->isVisible())
		{
			iShowCount++;
		}
		setYellowBgState(pCheckBtn, pCheckBtn->isChecked());
	}

	//如果选中了四个，则屏蔽掉一个，提示不能全选
	if(checknum>=iShowCount && pSender!=nullptr)
	{
		CCheckBox* pCheck = (CCheckBox*)pSender;
		pCheck->setEnabled(false);
		pCheck->setChecked(false);
		pCheck->setEnabled(true);
		setYellowBgState(pCheck, false);
		ShowPopTextTip(GETLANGSTR(1182));
		return;
	}

	if (checknum==0)
	{
		CCSprite *coin = (CCSprite *)(m_ui->findWidgetById("coin"));
		coin->setVisible(true);

		m_coin = 0; 
		switch (m_armor.quality)
		{
		case 1:
			m_coin =10000;
			break;
		case 2:
			m_coin =20000;
			break;
		case 3:
			m_coin =40000;
			break;
		case 4:
			m_coin =80000;
			break;
		case 5:
			m_coin =160000;
			break;
		default:
			break;
		}
		m_diamond = 0;

		CCSprite *diamond = (CCSprite *)(m_ui->findWidgetById("diamond"));
		diamond->setVisible(false);

		CLabel *money = (CLabel *)(m_ui->findWidgetById("money"));
		money->setString(ToString(m_coin));
		//检查货币数量
		if(!CheckCoin(atoi(money->getString())))
		{
			money->setColor(RGB_RED);
		}
		else
		{
			money->setColor(RGB_WHITE);
		}

		showFireWithType(0);
	}
	else
	{
		CCSprite *coin = (CCSprite *)(m_ui->findWidgetById("coin"));
		coin->setVisible(false);

		CCSprite *diamond = (CCSprite *)(m_ui->findWidgetById("diamond"));
		diamond->setVisible(true);

		CLabel *money = (CLabel *)(m_ui->findWidgetById("money"));
		money->setString(ToString(100*checknum));
		//检查货币数量
		if(!CheckGold(atoi(money->getString())))
		{
			money->setColor(RGB_RED);
		}
		else
		{
			money->setColor(RGB_WHITE);
		}

		m_coin = 0;
		m_diamond = 100*checknum;

		showFireWithType(1);
	}
}


void CSmeltArmor::onSelectItem(CCObject* pSender)
{
	CStrengthenItem *selectItem = CStrengthenItem::create();
	selectItem->setSelectType(1);
	selectItem->setVisible(false);
	selectItem->setAskType(CStrengthenItemTypeItemReset);
	selectItem->setOnSelectItemListener(this,ccw_select_Item(CSmeltArmor::onSelectArmor));
	LayerManager::instance()->push(selectItem);
	CPlayerControl::getInstance().sendRoleBag(13, true, 0);//11升级12升星13洗练
}

void CSmeltArmor::onSelectArmor(CItem* item)
{
	if (!item)
	{
		return;
	}
	
	if (item->quality==1)
	{
		ShowPopTextTip(GETLANGSTR(210));
		return;
	}

	m_armor = *item;

	//请求数据
	 CPlayerControl::getInstance().askSmeltData(item->id);
}

void CSmeltArmor::showArmorAttr()
{
	CItem* item = &(m_armor);

	//1 攻击addAtk，2 防御addDef，3 血量addHp，4 敏捷addDex，5 命中addHit，6 暴击addCrit，7 回复addRenew，8 闪避addDodge

	//名字对应的langID
	int iNameId[8] = {43, 44, 42, 45, 49, 46, 48, 47};

	//基础属性
	int iBaseValue[8] = 
	{
		item->armor.addAtk, item->armor.addDef, 
		item->armor.addHp, item->armor.addDex,
		item->armor.addHit, item->armor.addCrit, 
		item->armor.addRenew, item->armor.addDodge
	};
	//目标属性
	const char* sRange[8] = 
	{
		item->armor.atkRange.c_str(), item->armor.defRange.c_str(), item->armor.hpRange.c_str(), item->armor.IntRange.c_str(),
		item->armor.hitRange.c_str(), item->armor.critRange.c_str(), item->armor.renewRange.c_str(), item->armor.dodgeRange.c_str()
	};

	//更新数据
	m_iCount = updateInfoToUIAndReturnCount(m_pBaseInfo, iNameId, iBaseValue, sRange, 8, nullptr);

	if(m_iCount == 0)
	{
		ShowPopTextTip(GETLANGSTR(1128));
		m_attr->setVisible(false);
		return;
	}

}

void CSmeltArmor::onRefineItem(CCObject* pSender)
{
	//检查货币是否足够
	CLabel *money = (CLabel *)(m_ui->findWidgetById("money"));
	int iCost = atoi(money->getString());
	if(m_iStrengthType==0)
	{
		if(!CheckCoin(iCost))
		{
			//ShowPopTextTip(GETLANGSTR(205));
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceCoin);
			return;
		}
	}
	else
	{
		if(!CheckGold(iCost))
		{
			//ShowPopTextTip(GETLANGSTR(203));
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
			return;
		}
	}

	vector<int> checkVec;
	if(m_iStrengthType != 0)
	{
		for (unsigned int i=0; i<4; ++i)
		{
			CCheckBox* pCheckBtn = (CCheckBox*)(m_pBaseInfo[i]->findWidgetById("check"));
			if (pCheckBtn->isChecked())
			{
				checkVec.push_back(pCheckBtn->getTag());
			}
		}
	}

	if (m_armor.id>0)
	{
	   CPlayerControl::getInstance().sendSmeltArmor(m_armor.id, checkVec);
	}
}


void CSmeltArmor::proceesMessage(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	switch (type)
	{
		//洗练数据
	case SmeltDataRequestMsg:
		{
			Armor *res = (Armor*)msg;
			if(res)
			{
				m_armor.armor.atkRange = res->atkrange();
				m_armor.armor.hpRange = res->hprange();
				m_armor.armor.defRange = res->defrange();
				m_armor.armor.IntRange = res->dexrange();
				m_armor.armor.hitRange = res->hitrange();
				m_armor.armor.critRange = res->critrange();
				m_armor.armor.renewRange = res->renewrange();
				m_armor.armor.dodgeRange = res->dodgerange();

				showSmeltDataToUI();
			}
		}break;
		//洗练
	case SmeltArmorMsg:
		{
			SmeltResponse *res = (SmeltResponse*)msg;
			//1 成功，2 金币不足，3 元宝不足，4 数据错误
			if (res->reslut()==1)
			{
				//1 攻击addAtk，2 防御addDef，3 血量addHp，4 敏捷addDex，5 命中addHit，6 暴击addCrit，7 回复addRenew，8 闪避addDodge)
				m_armor.armor.addAtk = res->armor().armor().addatk();
				m_armor.armor.addDef = res->armor().armor().adddef();
				m_armor.armor.addHp = res->armor().armor().addhp();
				m_armor.armor.addDex = res->armor().armor().adddex();
				m_armor.armor.addHit = res->armor().armor().addhit();
				m_armor.armor.addCrit = res->armor().armor().addcrit();
				m_armor.armor.addRenew = res->armor().armor().addrenew();
				m_armor.armor.addDodge = res->armor().armor().adddodge();

				//更新金钱
				UserData *data = DataCenter::sharedData()->getUser()->getUserData();
				data->setCoin(data->getCoin() - m_coin);
				data->setRoleGold(data->getRoleGold() - m_diamond);
				CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

				CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_ITEM_DATA, 2, nullptr, &m_armor);

				//播放效果
				float fTime = showStrengthEffect();
				this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fTime), CCCallFunc::create(this, callfunc_selector(CSmeltArmor::showArmorAttr))));

			}
			else if (res->reslut()==2)
			{
				//ShowPopTextTip(GETLANGSTR(205));
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceCoin);
			}
			else if (res->reslut()==3)
			{
				//ShowPopTextTip(GETLANGSTR(203));
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceGold);
			}
			else if (res->reslut()==4)
			{
				ShowPopTextTip(GETLANGSTR(170));
			}
		}break;
	default:
		break;
	}

	
	
}

void CSmeltArmor::initSpandAndCheckBox()
{
	CCSprite *coin = (CCSprite *)(m_ui->findWidgetById("coin"));
	coin->setVisible(true);

	//m_coin = m_armor.useLevel *360;
	m_diamond = 0;

	m_coin = 0; 
	switch (m_armor.quality)
	{
	case 1:
		m_coin =10000;
		break;
	case 2:
		m_coin =20000;
		break;
	case 3:
		m_coin =40000;
		break;
	case 4:
		m_coin =80000;
		break;
	case 5:
		m_coin =160000;
		break;
	default:
		break;
	}

	CCSprite *diamond = (CCSprite *)(m_ui->findWidgetById("diamond"));
	diamond->setVisible(false);

	CLabel *money = (CLabel *)(m_ui->findWidgetById("money"));
	money->setString(ToString(m_coin));

	for (unsigned int i=0; i< 4; ++i)
	{
		CCheckBox* checkBtn = (CCheckBox*)(m_pBaseInfo[i]->findWidgetById("check"));
		checkBtn->setChecked(false);
	}
}

void CSmeltArmor::showMoveAction( const char* name )
{
	CCNode* pNode = ((CCNode*)m_ui->findWidgetById(name));
	if(pNode->getActionByTag(999) == nullptr)
	{
		CCRepeatForever* pAction = CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0, 15)), CCMoveBy::create(0.7f, ccp(0, -15))));
		pAction->setTag(999);
		pNode->runAction(pAction);
	}
}

void CSmeltArmor::initFire()
{
	//获取参考位置
	CCNode* pNode = (CCNode*)m_ui->findWidgetById("fire_circle");
	CCPoint pPos = ccp(pNode->getPositionX()+15, pNode->getPositionY()+75);

	if(m_pFire1 == nullptr)
	{
		CCAnimation* pAnimation = AnimationManager::sharedAction()->getAnimation("9010");
		pAnimation->setDelayPerUnit(0.15f);
		m_pFire1 = CCSprite::create("skill/9010.png");
		m_pFire1->setPosition(pPos);
		m_pFire1->runAction(CCRepeatForever::create(CCAnimate::create(pAnimation)));
		m_pFire1->setVisible(false);
		m_pFire1->setScale(1.6f);
		pNode->getParent()->addChild(m_pFire1, pNode->getZOrder());
	}
	if(m_pFire2 == nullptr)
	{
		CCAnimation* pAnimation = AnimationManager::sharedAction()->getAnimation("9011");
		pAnimation->setDelayPerUnit(0.15f);
		m_pFire2 = CCSprite::create("skill/9011.png");
		m_pFire2->setPosition(pPos);
		m_pFire2->runAction(CCRepeatForever::create(CCAnimate::create(pAnimation)));
		m_pFire2->setVisible(false);
		m_pFire2->setScale(1.5f);
		pNode->getParent()->addChild(m_pFire2, pNode->getZOrder());
	}
}

void CSmeltArmor::showFireWithType( int iType )
{
	m_iStrengthType = iType;

	if(iType == 0)
	{
		m_pFire1->setVisible(true);
		m_pFire2->setVisible(false);
	}
	else if(iType == 1)
	{
		m_pFire1->setVisible(false);
		m_pFire2->setVisible(true);
	}
}

float CSmeltArmor::showStrengthEffect()
{
	//屏蔽层
	MaskLayer* pLayer1 = MaskLayer::create("CSmeltArmor::effect");
	pLayer1->setOpacity(0);
	LayerManager::instance()->push(pLayer1);
	MaskLayer* lay = MaskLayer::create("HammerEffect");
	lay->setContentSize(CCSizeMake(1138,640));
	lay->setAnchorPoint(ccp(0.5f, 0.5f));
	lay->setPosition(VCENTER);
	lay->setOpacity(0);
	LayerManager::instance()->push(lay);

	//效果
	std::string effName = "9008";
	if(m_iStrengthType == 1)
	{
		effName = "9009";
	}

	//增加大号火焰
	CCAnimation *effect = AnimationManager::sharedAction()->getAnimation(effName.c_str());
	effect->setDelayPerUnit(0.05f);
	CCAnimate* pAnimate = CCAnimate::create(effect);

	CCSprite *fireBig = CCSprite::create(CCString::createWithFormat("skill/%s.png", effName.c_str())->getCString());
	fireBig->setPosition(ccp(m_pFire1->getPositionX(), m_pFire1->getPositionY()+10));
	fireBig->runAction(CCSequence::create(CCRepeat::create(CCAnimate::create(effect), 2),CCHide::create(), CCCallFuncN::create(this, callfuncN_selector(CSmeltArmor::showSmoke)), nullptr));
	fireBig->setScale(1.6f);
	m_pFire1->getParent()->addChild(fireBig, m_pFire1->getZOrder());

	showStrengthSoundCallBack();

	//强化成功
	CCSprite* pText = CCSprite::create("strengthen/word_33.png");
	pText->setPosition(ccp(fireBig->getPositionX(), fireBig->getPositionY()-100));
	lay->addChild(pText, 3);
	pText->setVisible(false);
	pText->setScale(0.7f);
	pText->runAction(CCSequence::create(
		CCDelayTime::create(0.5f),
		CCShow::create(),
		CCFadeIn::create(0.2f),
		CCSpawn::createWithTwoActions(CCMoveBy::create(0.5f, ccp(0, 100)), CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.0f), CCFadeOut::create(0.3f))),
		nullptr));

	lay->runAction(CCSequence::create(CCDelayTime::create(1.4f),CCCallFuncN::create(this, callfuncN_selector(CSmeltArmor::showStrengthCallBack)),nullptr));

	return 1.4f+0.5f;//加上播TV光效时间
}

void CSmeltArmor::showStrengthCallBack(CCNode* pSender)
{
	//强化成功
	PlayEffectSound(SFX_407);

	CCNode* pNode = (CCNode*)m_ui->findWidgetById("alpha_panel");
	float fTime = ShowTVLight(pNode, pSender);
	CCNodeRGBA* pWhitePanel = (CCNodeRGBA*)m_ui->findWidgetById("white_panel");
	pWhitePanel->setOpacity(0);
	pWhitePanel->runAction(CCSequence::create(CCDelayTime::create(fTime), CCFadeTo::create(0.15f, 180), CCFadeOut::create(0.03f), nullptr));

	pSender->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fTime), CCCallFunc::create(this, callfunc_selector(CSmeltArmor::showTVLightCallBack))));
}

void CSmeltArmor::showStrengthSoundCallBack()
{
	PlayEffectSound(SFX_428);
}

void CSmeltArmor::showSmoke(CCNode* pSender)
{
	CCAnimation *effect = AnimationManager::sharedAction()->getAnimation("9012");
	effect->setDelayPerUnit(0.1f);
	CCAnimate* pAnimate = CCAnimate::create(effect);
	CCSprite *smoke = CCSprite::create("skill/9012.png");
	smoke->setPosition(ccp(pSender->getPositionX(), pSender->getPositionY()-20));
	smoke->runAction(CCSequence::create(CCAnimate::create(effect),CCHide::create(), nullptr));
	smoke->runAction(CCMoveBy::create(0.5f, ccp(0, 50)));
	pSender->getParent()->addChild(smoke, 1);
	pSender->removeFromParentAndCleanup(true);
}

void CSmeltArmor::roleUpdate( const TMessage& tMsg )
{
	checkMoney();
}

void CSmeltArmor::initItemInfo( CCPoint posStart, CLayout* pLayout[], int iCount )
{
	for(int i=0; i<iCount; i++)
	{
		CLayout* pCopyLayout = UICloneMgr::cloneLayout(m_pCellInfo);
		pCopyLayout->setPosition(posStart+ccp(0, -i*50));
		m_attr->addChild(pCopyLayout, 911);
		pLayout[i] = pCopyLayout;
		pLayout[i]->setVisible(false);
	}
}

void CSmeltArmor::hideNoneValueCell( int iCount, CLayout* pLayout[] )
{
	for(unsigned int i=iCount; i<4; i++)
	{
		pLayout[i]->setVisible(false);
	}
}

int CSmeltArmor::updateInfoToUIAndReturnCount( CLayout* pLayout[], int iNameId[], int iValue[], const char* sRange[], int iMax, const char* sTitle )
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
			pAimValue->setString(sRange[i]);
			//存ID
			CCheckBox* pCheck = (CCheckBox*)pLayout[iMaxLineCount]->findWidgetById("check");
			pCheck->setTag(i+1);

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

void CSmeltArmor::showSmeltDataToUI()
{
	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(m_armor.itemId);

	//贴图
	CCSprite *prop = getItemSprite(&m_armor);
	if (!prop)
	{
		prop = CImageView::create("prop/32003.png");
	}

	//选择按钮
	CButton *btn = dynamic_cast<CButton*>(m_ui->findWidgetById("select"));
	btn->removeChildByTag(1);
	prop->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
	prop->setTag(1);
	btn->addChild(prop, -1);
	((CCSprite*)btn->getNormalImage())->setTexture(setItemQualityTexture(m_armor.quality));

	//添加星星层
	btn->removeChildByTag(415);
	CLayout* pStarLayout = getStarLayout(m_armor.iStar);
	btn->addChild(pStarLayout, 415, 415);

	//更新名字
	CLabel *name = dynamic_cast<CLabel*>(m_ui->findWidgetById("name"));
	CCString *str = CCString::create("XXX");
	if(itemData)
	{
		str = CCString::createWithFormat("%s",itemData->itemName.c_str());
	}
	name->setString(str->getCString());
	name->setVisible(true);

	//加号
	CCSprite *why = dynamic_cast<CCSprite*>(m_ui->findWidgetById("why"));
	why->setVisible(false);

	//设置等级
	CLabel* pLevel = dynamic_cast<CLabel*>(m_ui->findWidgetById("level"));
	CCNode* pLevelBg = dynamic_cast<CCNode*>(m_ui->findWidgetById("level_mask"));
	if(m_armor.itemLevel>0)
	{
		pLevel->setVisible(true);
		pLevelBg->setVisible(true);

		pLevel->setString(CCString::createWithFormat("+%d", m_armor.itemLevel)->getCString());
	}
	else
	{
		pLevel->setVisible(false);
		pLevelBg->setVisible(false);
	}


	m_attr->setVisible(true);

	//显示背景框等等
	CCNode* pNode4 = (CCNode*)m_ui->findWidgetById("tip");
	pNode4->setVisible(false);

	m_pSpineHero->setVisible(false);


	initSpandAndCheckBox();

	showArmorAttr();

	//更新钱
	onCheckAttr(nullptr, false);

	//设置动作
	showMoveAction("box_bg");
	showMoveAction("select");
	showMoveAction("why");
	showMoveAction("level");
	showMoveAction("level_mask");
}

void CSmeltArmor::checkMoney()
{
	CCSprite *coin = (CCSprite *)(m_ui->findWidgetById("coin"));
	CCSprite *diamond = (CCSprite *)(m_ui->findWidgetById("diamond"));

	CLabel *money = (CLabel *)(m_ui->findWidgetById("money"));
	if(coin->isVisible())
	{
		//检查货币数量
		if(!CheckCoin(atoi(money->getString())))
		{
			money->setColor(RGB_RED);
		}
		else
		{
			money->setColor(RGB_WHITE);
		}
	}

	if(diamond->isVisible())
	{
		//检查货币数量
		if(!CheckGold(atoi(money->getString())))
		{
			money->setColor(RGB_RED);
		}
		else
		{
			money->setColor(RGB_WHITE);
		}
	}
}

void CSmeltArmor::showTVLightCallBack()
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSmeltArmor::updateStar( const TMessage& tMsg )
{
	//加号
	CItem* pData = (CItem*)tMsg.lParam;
	if(pData)
	{
		//自己的数据不更新
		if(&(m_armor) == pData)
		{
			return;
		}
		CCSprite *why = dynamic_cast<CCSprite*>(m_ui->findWidgetById("why"));
		if(why->isVisible())
		{
			return;
		}
		CButton *btn = dynamic_cast<CButton*>(m_ui->findWidgetById("select"));
		btn->removeChildByTag(415);
		CLayout* pStarLayout = getStarLayout(pData->iStar);
		btn->addChild(pStarLayout, 415, 415);
	}
}



