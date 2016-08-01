#include "SignLayer.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"

#include "PopItem.h"
#include "scene/CPopTip.h"
#include "common/ShaderDataHelper.h"
#include "common/CheckMoney.h"
#include "common/CommonFunction.h"
#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"
#include "common/CShowToBuyResource.h"
#include "tools/CCShake.h"
#include "Battle/AnimationManager.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "model/CWholeBodyShowData.h"

#include <spine/spine-cocos2dx.h>


using namespace spine;
using namespace BattleSpace;

CSignLayer::CSignLayer():
	m_ui(0),
	m_gridView(nullptr)
	,m_cell(nullptr)
	,m_resignGold(0)
	,m_signLay(nullptr)
	,m_bIsFirst(false)
	,m_pMonsterInfo(nullptr)
	,m_pItemInfo(nullptr)
	,m_show_info_scroll(nullptr)
	,m_pInfo1(nullptr)
	,m_pInfo2(nullptr)
	,m_bIsReSign(false)
	,m_pLineEffect(nullptr)
	,m_pYellowTip(nullptr)
	,m_pYellowTipText(nullptr)
	,m_pClip(nullptr)
{

}


CSignLayer::~CSignLayer()
{
	CC_SAFE_RELEASE(m_cell);
}


bool CSignLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("CSignLayermask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("Sign.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setOpacity(180);

		//背景框
		CImageView* pImageRect = (CImageView*)m_ui->findWidgetById("board_sign");

		//添加一个裁切层
		CCClippingNode* pClip = CCClippingNode::create();
		CImageView* pImage = UICloneMgr::cloneImageView(pImageRect);
		pImage->setAnchorPoint(ccp(0.5f, 0.5f));
		pImage->setPosition(ccp(pImage->getContentSize().width/2, pImage->getContentSize().height/2));
		pClip->setStencil(pImage);
		pClip->setContentSize(pImageRect->getContentSize());
		pClip->setAnchorPoint(pImageRect->getAnchorPoint());
		pClip->setPosition(pImageRect->getPosition());
		pImageRect->getParent()->addChild(pClip, pImageRect->getZOrder()+1);
		m_pClip = pClip;

		//展示区图片
		m_show_info_scroll = (CScrollView*)m_ui->findWidgetById("scroll_info");
		m_show_info_scroll->setDirection(eScrollViewDirectionVertical);
		m_show_info_scroll->setBounceable(true);

		m_pInfo1 = (CLabel*)m_ui->findWidgetById("info_1");
		CC_SAFE_RETAIN(m_pInfo1);
		m_pInfo1->removeFromParentAndCleanup(false);
		m_show_info_scroll->getContainer()->addChild(m_pInfo1);
		m_pInfo1->setAnchorPoint(ccp(0, 1));
		CC_SAFE_RELEASE(m_pInfo1);

		m_pInfo2 = (CLabel*)m_ui->findWidgetById("info_2");
		CC_SAFE_RETAIN(m_pInfo2);
		m_pInfo2->removeFromParentAndCleanup(false);
		m_show_info_scroll->getContainer()->addChild(m_pInfo2);
		m_pInfo2->setAnchorPoint(ccp(0, 1));
		CC_SAFE_RELEASE(m_pInfo2);

		updateShowInfoScroll();

		m_show_info_scroll->setVisible(false);

		CCNode* pMaskInfo = (CCNode*)m_ui->findWidgetById("mask_info");
		pMaskInfo->setVisible(false);

		m_pLineEffect = new CLineLightEffect;
		m_pLineEffect->bindUI(m_ui);
		m_pLineEffect->bindRectEffect(pImageRect);
		this->addChild(m_pLineEffect, 999);


		m_pYellowTip = CCSprite::create("sign/lightbox5.png");
		m_pYellowTip->setAnchorPoint(ccp(0.5f, 0.0f));
		m_pYellowTip->setScale(0.8f);
		m_pYellowTip->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0, -10)), CCMoveBy::create(0.3f, ccp(0, 10)))));
		m_ui->addChild(m_pYellowTip, 50);
		m_pYellowTipText = UICloneMgr::cloneLable((CLabel*)m_ui->findWidgetById("day1"));
		m_pYellowTipText->setAnchorPoint(ccp(0.5f, 0.5f));
		m_pYellowTipText->setPosition(ccp(m_pYellowTip->getContentSize().width/2, m_pYellowTip->getContentSize().height/2+12));
		m_pYellowTipText->setScale(1.2f);
		m_pYellowTip->addChild(m_pYellowTipText);
		m_pYellowTip->setVisible(false);

		return true;
	}
	return false;

}

void CSignLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	if(m_pLineEffect->isTouchLock())
	{
		return;
	}

	m_pLineEffect->hideEffect();
}

void CSignLayer::onEnter()
{
	BaseLayer::onEnter();

	//签到grid的cell
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
    m_ui->removeChild(m_cell);

	m_gridView = (CGridView*)m_ui->findWidgetById("scroll");
	m_gridView->setDirection(eScrollViewDirectionVertical);	
	m_gridView->setCountOfCell(0);
	m_gridView->setSizeOfCell(m_cell->getContentSize());
	m_gridView->setAnchorPoint(ccp(0,0));
	m_gridView->setColumns(5);
	m_gridView->setAutoRelocate(true);
	m_gridView->setDeaccelerateable(false);
	m_gridView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CSignLayer::gridviewDataSource));
	m_gridView->reloadData();


	m_signLay = (CLayout*)(m_ui->findWidgetById("Sign"));


	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CSignLayer::onClose));
	this->addChild(pClose, 999);

	//book
	CCheckBox* pBook = (CCheckBox*)(m_ui->findWidgetById("book"));
	pBook->setOnCheckListener(this,ccw_check_selector(CSignLayer::showHeroBook));

	//按钮补签
	CButton* pResign = (CButton*)m_ui->findWidgetById("buqian_btn");
	pResign->setOnClickListener(this, ccw_click_selector(CSignLayer::onBtnResignDay));

	//按钮签到
	CButton* pSign = (CButton*)m_ui->findWidgetById("qiandao_btn");
	pSign->setOnClickListener(this, ccw_click_selector(CSignLayer::onBtnSignDay));

	GetTcpNet->registerMsgHandler(SignDataMsg,this, CMsgHandler_selector(CSignLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(SignReqMsg,this, CMsgHandler_selector(CSignLayer::ProcessMsg));
}


CCObject* CSignLayer::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;
	if (!pCell)
	{
		pCell = new CGridViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
	}
	else
	{
		pCell->removeAllChildren();
	}
	
	addGridCell(uIdx,pCell);

	return pCell;
}


void CSignLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	BaseLayer::onExit();
	//NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	NOTIFICATION->postNotification("CheckShowActivity");
	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);

	CPopItem *popItem = dynamic_cast<CPopItem*>(LayerManager::instance()->getLayer("CPopItem"));
	if (popItem)
	{
		popItem->bindTargetCallBack(nullptr,nullptr);
	}

	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("skill/9065.plist");
	CCTextureCache::sharedTextureCache()->removeTextureForKey("skill/9065.png");
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("skill/9066.plist");
	CCTextureCache::sharedTextureCache()->removeTextureForKey("skill/9066.png");
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("skill/9067.plist");
	CCTextureCache::sharedTextureCache()->removeTextureForKey("skill/9067.png");
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("skill/9068.plist");
	CCTextureCache::sharedTextureCache()->removeTextureForKey("skill/9068.png");
}


void CSignLayer::addGridCell(unsigned int uIdx, CGridViewCell* pCell)
{
// 	CHero *hero = m_currHeroList->at(uIdx);
	CSign &sign = m_signData.signList.at(uIdx);
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	pCell->setScale(0.97f);

	for (int i = 1; i <=4; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
	}


	CImageView *imagView = (CImageView*)pCell->getChildByTag(1);	
	imagView->setTouchEnabled(true);
	imagView->setEnabled(true);
	imagView->setUserData(&m_signData.signList.at(uIdx));
	imagView->setOnPressListener(this, ccw_press_selector(CSignLayer::onPress));
	


	CCSprite *item = nullptr;
	if (sign.prize.thumb>=0)
	{
		CImageView * pMask = (CImageView*)pCell->getChildByTag(2);
		SmartSetRectPrizeColor(pMask, &sign.prize);
		item = CCSprite::create(GetImageName(sign.prize.type, sign.prize.color, sign.prize.thumb).c_str());
		if(!item)
		{
			item = CCSprite::create("headImg/101.png");
			CCLOG("CSignLayer::addGridCell  error load image %d", sign.prize.thumb);
		}
		item->setPosition(ccp(pMask->getContentSize().width/2, pMask->getContentSize().height/2));
		pMask->addChild(item, -1, 2);
		item->setScale(0.9f);
	

		//加数字
		CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
		haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
		haveNum->setPosition(ccp(85, 5));
		pMask->addChild(haveNum, 99, 99);

		haveNum->setString(ToString(sign.prize.num));
		if(sign.prize.num<=1)
		{
			haveNum->setVisible(false);
		}

		//添加星星
		if(sign.prize.quality > 0)
		{
			CLayout* pStarLayout = SmartGetStarLayout(&sign.prize);
			pMask->addChild(pStarLayout, 211, 211);
		}
	}

	//vip icon
	CCSprite *vip = (CCSprite*)pCell->getChildByTag(3);
	if (sign.vip>0)
	{ 
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()
											->addImage(CCString::createWithFormat("public/vip_%d.png",sign.vip)->getCString());
		vip->setVisible(true);
		vip->setTexture(texture);
	}
	else
	{
		vip->setVisible(false);
	}

	switch (sign.status)
	{
		//没签
	case 2:
		{
			//对勾
			pCell->getChildByTag(4)->setVisible(false);
			//考虑是否加个框标示为可签
			if(uIdx == m_signData.sign && 
				(m_signData.bCanSign || (m_signData.bCanResign && m_signData.resign>0) ) )
			{
				CImageView * pMask = (CImageView*)pCell->getChildByTag(2);
				CCSprite* pRect = CCSprite::create("common/box_light.png");
				pRect->setPosition(ccp(pMask->getPositionX()+pMask->getContentSize().width/2, pMask->getPositionY()+pMask->getContentSize().height/2));
				pCell->addChild(pRect, 100);
				pRect->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.3f, 1.03f), CCScaleTo::create(0.3f, 1.0f))));
			}
		}
		break;
		//已签
	case 1:
		{
			//对勾
			pCell->getChildByTag(4)->setVisible(true);
			//变黑
			pCell->getChildByTag(1)->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			pCell->getChildByTag(2)->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			pCell->getChildByTag(2)->getChildByTag(2)->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			pCell->getChildByTag(3)->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			//星星
			CLayout * pStarLay = (CLayout*)pCell->getChildByTag(2)->getChildByTag(211);
			if(pStarLay!=nullptr)
			{
				CCArray* pChildren = pStarLay->getChildren();
				for(int i=0; i<pChildren->count(); i++)
				{
					CCSprite* pStar = dynamic_cast<CCSprite*>(pChildren->objectAtIndex(i));
					if(pStar)
					{
						pStar->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
					}
				}
			}
			////数字
			//CCLabelAtlas * pLabel = (CCLabelAtlas*)pCell->getChildByTag(2)->getChildByTag(211);
			//if(pLabel && pLabel->isVisible())
			//{
			//	pLabel->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			//}
		}
		break;
	default:
		break;
	}

}

void CSignLayer::onResignDay(CCObject *pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==PopTipConfirm)
	{
		//货币不足
		if(!CheckGold(m_resignGold))
		{
			//ShowPopTextTip(GETLANGSTR(203));
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
			return;
		}
		else
		{
			m_bIsReSign = true;
			CPlayerControl::getInstance().sendSignReq(m_bIsReSign);
		}
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

void CSignLayer::onBtnResignDay( CCObject* pSender )
{
	CCString *str = CCString::createWithFormat(GETLANGSTR(9), m_resignGold);
	ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(CSignLayer::onResignDay));
}

void CSignLayer::onBtnSignDay( CCObject *pSender )
{
	m_bIsReSign = false;
	CPlayerControl::getInstance().sendSignReq(m_bIsReSign);
}

void CSignLayer::ProcessMsg(int type, google::protobuf::Message *msg)
 {
	if(!isVisible())
	{
		this->setVisible(true);
		//NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
		m_pLineEffect->showEffect();
	}

	if (type == SignDataMsg)
	{
		SignData *res = (SignData*)msg;
		m_signData.read(*res);

	    m_gridView->setCountOfCell(m_signData.signList.size());
		ReloadGridViewWithSameOffset(m_gridView);

		m_resignGold = m_signData.resign_gold;

		//更新阶段签到情况
		updateTotoalSign();
		//加黄标提示
		showTipsOnNextTotalSign();

		updateSignDays();

		showCheckDayToCenter();

		initHeroData();
	}
	else if (type== SignReqMsg)
	{
		SignResponse *res = (SignResponse *)msg;
		CSignRes signRes;
		signRes.read(*res);

		switch (res->result())
		{
		case 1:
			{
				if (m_bIsReSign)//补签要扣勾玉
				{
					UserData *data = DataCenter::sharedData()->getUser()->getUserData();
					data->setRoleGold(data->getRoleGold()-m_resignGold);
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
				}

				if (res->has_resigngold())
				{
					m_resignGold = res->resigngold();
				}

				//下一个未签的改为已签
				int iIndex = autoNextSignStatusChange();

				m_signData.resign-= m_bIsReSign;
				m_signData.sign+=1 ;

				//普通签到
				if(!m_bIsReSign)
				{
					m_signData.bCanSign = false;
				}
		
				ReloadGridViewWithSameOffset(m_gridView);

				updateSignDays();

				showCheckDayToCenter();

				if(iIndex!=-1)
				{
					CSign& sign = m_signData.signList[iIndex];
					if(DataCenter::sharedData()->getUser()->getUserData()->getVip()>=sign.vip && sign.vip>0)
					{
						sign.prize.num *= 2;
					}
					CPopItem *popItem = CPopItem::create();
					popItem->signPrize(&sign.prize);
					popItem->setDescVisiable(true);
					LayerManager::instance()->push(popItem);

					//界面关闭后给回一个回调，用于处理阶段签的宝箱
					int iIndexTotal = checkTotalBox();
					if(iIndexTotal!=-1)
					{
						popItem->bindTargetCallBack(this, popitem_selector(CSignLayer::callBackForTotalSignEffect));
					}
				}

				PlayEffectSound(SFX_407);
			}
			break;
		case 2:
			{
				//ShowPopTextTip(GETLANGSTR(240));
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceAction);
			}
			break;
		case 3:
			ShowPopTextTip(GETLANGSTR(170));
			break;
		default:
			break;
		}
	}
}

void CSignLayer::updateTotoalSign()
{
	//更新
	//已领取的，宝箱变灰，加对勾
	//未领取的，宝箱变亮，
	//将要领取的，加黄色标题突出

	for (int i = 0; i < m_signData.totalList.size()&&i<4; i++)
	{
		CSign &sign = m_signData.totalList.at(i);

		//宝箱
		CImageView *mask = (CImageView*)m_ui->findWidgetById(CCString::createWithFormat("mask%d",i+1)->getCString());
		mask->setVisible(true);
		//mask->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("sign/box_%d.png", i+1)->getCString()));
		mask->setUserData(&sign);
		mask->setTouchEnabled(true);
		mask->setOnPressListener(this, ccw_press_selector(CSignLayer::onPress));

		//天数
		CLabel *day = (CLabel*)(m_ui->findWidgetById(CCString::createWithFormat("day%d",i+1)->getCString()));
		day->setString(CCString::createWithFormat(GETLANGSTR(209),sign.day)->getCString());

		//获得字样
		CImageView *getspr = (CImageView*)(m_ui->findWidgetById(CCString::createWithFormat("get%d",i+1)->getCString()));

		//已领取
		if (sign.status==1)
		{
			//宝箱变灰
			mask->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			//加对勾
			getspr->setVisible(true);
		}
		//未领取
		else if(sign.status==2)
		{
			//宝箱变灰
			mask->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
			//加对勾
			getspr->setVisible(false);
		}
	}
}


void CSignLayer::showTipsOnNextTotalSign()
{
	m_pYellowTip->setVisible(false);
	//遍历阶段签到
	for (int i = 0; i < m_signData.totalList.size()&&i<4; i++)
	{
		CSign &sign = m_signData.totalList.at(i);
		//未领取
		if(sign.status==2)
		{
			if(sign.day>m_signData.sign)
			{
				//当前底加高亮
				CCSprite* pBg = (CCSprite*)m_ui->findWidgetById(CCString::createWithFormat("bg%d",i+1)->getCString());
				//pBg->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
				pBg->setTexture(CCTextureCache::sharedTextureCache()->addImage("sign/arrival_02.png"));
				//pBg->setScale(0.85f);
				//拿到当前宝箱
				CImageView *mask = (CImageView*)m_ui->findWidgetById(CCString::createWithFormat("mask%d",i+1)->getCString());
				//隐藏自己的日期
				CLabel *day = (CLabel*)(m_ui->findWidgetById(CCString::createWithFormat("day%d",i+1)->getCString()));
				day->setVisible(false);
				//加黄标提示
				m_pYellowTip->setVisible(true);
				m_pYellowTip->setPosition(ccp(mask->getPositionX()+mask->getContentSize().width/2, mask->getPositionY()+mask->getContentSize().height-60));
				m_pYellowTipText->setString(day->getString());
				break;
			}
		}
		else
		{
			//当前底加高亮
			CCSprite* pBg = (CCSprite*)m_ui->findWidgetById(CCString::createWithFormat("bg%d",i+1)->getCString());
			//pBg->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
			pBg->setTexture(CCTextureCache::sharedTextureCache()->addImage("sign/arrival_01.png"));
			//pBg->setScale(1.0f);

			//天数
			CLabel *day = (CLabel*)(m_ui->findWidgetById(CCString::createWithFormat("day%d",i+1)->getCString()));
			day->setVisible(true);
		}
	}
}


void CSignLayer::updateSignDays()
{
	//更新已签天数
	CLabel * pSignNum = (CLabel*)m_ui->findWidgetById("qiandao_num");
	pSignNum->setString(ToString(m_signData.sign));
	if(!m_signData.bCanSign)
	{
		//更新已签天数
		CButton * pSignBtn = (CButton*)m_ui->findWidgetById("qiandao_btn");
		pSignBtn->setEnabled(false);
		pSignBtn->getDisabledImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
	}


	//未补签天数
	CLabel* pRemainResignNum = (CLabel*)m_ui->findWidgetById("buqian_num");
	pRemainResignNum->setString(ToString(m_signData.resign));
	if(!m_signData.bCanResign || m_signData.resign==0)
	{
		//更新已签天数
		//CButton * pReSignBtn = (CButton*)m_ui->findWidgetById("buqian_btn");
		//pReSignBtn->setEnabled(false);
		//pReSignBtn->getDisabledImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));

		pRemainResignNum->setVisible(false);
		CLabel* pRemainResignText = (CLabel*)m_ui->findWidgetById("buqian_text");
		pRemainResignText->setVisible(false);
		CLabel* pRemainResignBg = (CLabel*)m_ui->findWidgetById("buqian_bg");
		pRemainResignBg->setVisible(false);
		CLabel* pRemainResignBtn = (CLabel*)m_ui->findWidgetById("buqian_btn");
		pRemainResignBtn->setVisible(false);
		CLabel* pRemainResignBtnText = (CLabel*)m_ui->findWidgetById("buqian_btn_text");
		pRemainResignBtnText->setVisible(false);
	}

	////totalSign
	//for (int i = 0; i < m_signData.totalList.size()&&i<4; i++)
	//{
	//	CSign &sign = m_signData.totalList.at(i);
	//	if (m_signData.sign >= sign.day)
	//	{
	//		CCSprite *getspr = (CCSprite*)(m_ui->findWidgetById(CCString::createWithFormat("get%d",i+1)->getCString()));
	//		getspr->setVisible(true);
	//	}
	//}
}

CCSprite* CSignLayer::getPrize(const CSign &sign)
{
	CCSprite *item = NULL;
	//1,daoju，2 英雄，3 铜钱，4 元宝，5 粮食)
	if (sign.prize.type ==1)
	{ 
		item = CCSprite::create(CCString::createWithFormat("prop/%d.png",sign.prize.thumb)->getCString());
		if (!item)
		{
			item = CCSprite::create(CCString::createWithFormat("headImg/%d.png",sign.prize.thumb)->getCString());
			//CCLOG("[ ERROR ] CSignLayer::getPrize Lost Image = %d",sign.prize.thumb);
		}
	}
	else if (sign.prize.type ==2)
	{
		item = CCSprite::create(CCString::createWithFormat("headImg/%d.png", sign.prize.thumb)->getCString());
		if (!item)
		{
			item = CCSprite::create("headImg/101.png");
			CCLOG("[ ERROR ] CSignLayer::getPrize Lost Image = %d",sign.prize.thumb);
		}
	} 
	else if (sign.prize.type ==3)
	{
		item = CCSprite::create("common/icon_coin.png");
	}
	else if (sign.prize.type ==4)
	{
		item = CCSprite::create("common/gold.png");
	}
	else if (sign.prize.type ==5)
	{
		item = CCSprite::create("common/tili.png");
	}
	else if (sign.prize.type ==0)
	{
		item = CCSprite::create("common/icon_11.png");
	}
	return item;
}


void CSignLayer::onPress( CCObject* pSender, CTouchPressState iState )
{
	CImageView* pImage = (CImageView*)pSender;
	CSign* sign = (CSign*)pImage->getUserData();
	int iType = sign->prize.type;

	if(m_pMonsterInfo)
	{
		m_pMonsterInfo->hide();
	}
	if(m_pItemInfo)
	{
		m_pItemInfo->hide();
	}
	
	//1 道具，2 英雄，3 铜钱，4 元宝，5 粮食
	switch (iType)
	{
	case 3:
	case 4:
	case 5:
	case 1:
		{
			if(m_pItemInfo==nullptr)
			{
				m_pItemInfo = CItemInfo::create();
				this->addChild(m_pItemInfo);
			}

			switch (iState)
			{
			case CTouchPressOn:
				{
					PlayEffectSound(SFX_429);

					CSign* sign = (CSign*)pImage->getUserData();
					m_pItemInfo->setInfo(&sign->prize);
					m_pItemInfo->bindPos(pImage);
					m_pItemInfo->changeForSign(&sign->prize);
					m_pItemInfo->show();
					CCLOG("show");
				}
				break;
			case CTouchPressOff:
				{
					m_pItemInfo->hide();
					CCLOG("hide");
				}
				break;
			default:
				break;
			}

		}break;
	case 2:
		{
			if(m_pMonsterInfo==nullptr)
			{
				m_pMonsterInfo = CMonsterInfo::create();
				this->addChild(m_pMonsterInfo);
			}

			switch (iState)
			{
			case CTouchPressOn:
				{
					PlayEffectSound(SFX_429);

					CSign* sign = (CSign*)pImage->getUserData();
					CHero pHero;
					pHero.heroid = sign->prize.id;
					pHero.thumb = sign->prize.thumb;
					pHero.quality = sign->prize.quality;
					pHero.iColor = sign->prize.color;
					pHero.level = 1;
					m_pMonsterInfo->setHero(&pHero);
					m_pMonsterInfo->bindPos(pImage);
					m_pMonsterInfo->changeForSign(&pHero);
					m_pMonsterInfo->show();
					CCLOG("show");
				}
				break;
			case CTouchPressOff:
				{
					m_pMonsterInfo->hide();
					CCLOG("hide");
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


bool CSignLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	if(m_pLineEffect->isTouchLock())
	{
		return true;
	}

	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);

	CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("board_sign");
	if( !res && !bgSpr->boundingBox().containsPoint(pTouchPos))
	{
		res = true;
		onClose(nullptr);
	}
	return res;
}

void CSignLayer::showHeroBook( CCObject* pSender, bool bChecked )
{
	CCLOG("CSignLayer::showHeroBook");

	PlayEffectSound(SFX_429);

	m_show_info_scroll->setVisible(bChecked);

	CCNode* pMaskInfo = (CCNode*)m_ui->findWidgetById("mask_info");
	pMaskInfo->setVisible(bChecked);
}

void CSignLayer::updateShowInfoScroll()
{
	CCSize contentSize = m_show_info_scroll->getContentSize();

	int iBaseHeight = contentSize.height;

	//计算文字占高度
	int iHeight1 = m_pInfo1->getContentSize().height;
	int iHeight2 = m_pInfo2->getContentSize().height;

	int iHeightGap = 50;

	contentSize.height = iHeight1 + iHeight2 + iHeightGap;

	contentSize.height  = contentSize.height < iBaseHeight?iBaseHeight:contentSize.height;

	m_pInfo1->setPosition(ccp(10, contentSize.height-65));
	m_pInfo2->setPosition(m_pInfo1->getPosition()-ccp(0, 40));

	m_show_info_scroll->setContainerSize(contentSize);
	m_show_info_scroll->setContentOffsetToTop();
}

void CSignLayer::showCheckDayToCenter()
{
	//当前签到是哪一天
	int iCurrentSignDayIndex = getCurrentSignIndex();
	if(iCurrentSignDayIndex == -1)
	{
		return;
	}

	m_gridView->setContentOffsetToTop();

	int offsetX = iCurrentSignDayIndex/5;
	m_gridView->setContentOffset(ccpAdd(m_gridView->getContentOffset(),ccp(0, m_gridView->getSizeOfCell().height*offsetX)));

}

void CSignLayer::initHeroData()
{
	int iSize = m_signData.signList.size();
	for(unsigned int i=0; i<iSize; i++)
	{
		CSign &sign = m_signData.signList.at(i);
		if(sign.prize.type==2)
		{
			const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(sign.prize.thumb);

			//详细信息
			if(data)
			{
				m_pInfo1->setString(CCString::createWithFormat("%s(%s)", data->heroName.c_str(), GETLANGSTR(data->iType1+1189))->getCString());
				if(data->iType1==1) m_pInfo1->setColor(ccc3(220, 82, 31));
				if(data->iType1==2) m_pInfo1->setColor(ccc3(91, 162, 247));
				if(data->iType1==3) m_pInfo1->setColor(ccc3(255, 234, 19));

				std::string sInfo = data->heroStory;
				for(unsigned int i=0; i<sInfo.size(); i++)
				{
					if(sInfo[i] == '|')
					{
						sInfo[i] = '\n';
					}
				}
				m_pInfo2->setString(sInfo.c_str());
				updateShowInfoScroll();
			}


			//动画
			//参考zorder
			CCNode* pNode = (CCNode*)m_ui->findWidgetById("board_sign");
			//增加人物贴图
			CCString* pStr = CCString::createWithFormat("sign/%d.png", sign.prize.thumb);
			CCSprite* pHero = CCSprite::create(pStr->getCString());
			if(!pHero)
			{
				CCLOG("ERROR-Missing picture %s", pStr->getCString());
				pHero = CCSprite::create("sign/arrival_ch_01.png");
			}
			pHero->setPosition(CCPointZero);
			pHero->setAnchorPoint(CCPointZero);
			m_pClip->addChild(pHero);
			
			//添加粒子
			CCString* pStrLz = CCString::createWithFormat("lz/sign_lz_%d.plist", data->iType1);
			std::string fullPathLz = CCFileUtils::sharedFileUtils()->fullPathForFilename(pStrLz->getCString());
			if(!CCFileUtils::sharedFileUtils()->isFileExist(fullPathLz))
			{
				//创建过程不会返回null，直接报错，所以在前面判断文件存在
				CCLOG("ERROR-missing plist file%s", pStrLz->getCString());
				pStrLz = CCString::create("lz/sign_lz_3.plist");
			}
			CCParticleSystemQuad* pLz = CCParticleSystemQuad::create(pStrLz->getCString());
			pLz->setPosition(ccp(pHero->getPositionX()+pHero->getContentSize().width/2-30, pHero->getPositionY()-20));
			m_pClip->addChild(pLz, 2);

			pHero->setPositionX(pHero->getPositionX()-600);
			pHero->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCMoveBy::create(0.2f, ccp(600, 0))));


			//const CWholeBodyShowInfo& pInfo = CWholeBodyShowData::getInstance()->getDataById(sign.prize.thumb);
			//CCSprite* pNodePos = (CCSprite*)m_ui->findWidgetById("monster_pos");
			//pNodePos->setVisible(true);
			//pNodePos->setOpacity(0);
			//CCPoint pPos = pNodePos->getPosition();

			//char c_json[60]   = {0};
			//char c_altlas[60] = {0};
			//sprintf(c_json,"storyImage/Spine/888%d.json",sign.prize.thumb);
			//sprintf(c_altlas,"storyImage/Spine/888%d.atlas",sign.prize.thumb);
			//std::string sFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(c_json);
			//if(!CCFileUtils::sharedFileUtils()->isFileExist(sFullPath))
			//{
			//	CCLOG("ERROR -CSignLayer::initHeroData-file not foud-%s", c_json);
			//	sprintf(c_json,"storyImage/Spine/888756.json");
			//	sprintf(c_altlas,"storyImage/Spine/888756.atlas");
			//}
			//SkeletonAnimation*  Animation = SkeletonAnimation::createWithFile(c_json,c_altlas,1);
			//Animation->setAnimation(0,"stand",true);
			//pNodePos->addChild(Animation);

			//Animation->setScale(pInfo.fScale*0.65f);

			break;
		}
	}
}

int CSignLayer::getCurrentSignIndex()
{
	int iSize = m_signData.signList.size();
	for(unsigned int i=0; i<iSize; i++)
	{
		CSign &sign = m_signData.signList.at(i);
		if(sign.status == 2)
		{
			return i;
		}
	}
	return -1;
}

int CSignLayer::autoNextSignStatusChange()
{
	int iSize = m_signData.signList.size();
	for(unsigned int i=0; i<iSize; i++)
	{
		CSign &sign = m_signData.signList.at(i);
		if(sign.status == 2)
		{
			sign.status = 1;
			return i;
		}
	}
	return -1;
}

void CSignLayer::showFirstLoginEffect()
{
	//粒子飞过去
}

int CSignLayer::checkTotalBox()
{
	//遍历阶段签到
	for (int i = 0; i < m_signData.totalList.size()&&i<4; i++)
	{
		CSign &sign = m_signData.totalList.at(i);
		//未领取
		if(sign.status==2)
		{
			if(sign.day<=m_signData.sign)
			{
				return i;
			}
		}
	}
	return -1;
}


void CSignLayer::callBackForTotalSignEffect()
{
	this->runAction(CCCallFunc::create(this, callfunc_selector(CSignLayer::callBackForTotalSignEffectOneFrameLater)));
}


void CSignLayer::callBackForTotalSignEffectOneFrameLater()
{
	//隐藏黄标
	m_pYellowTip->setVisible(false);

	//触摸屏蔽
	MaskLayer* pLayer1 = MaskLayer::create("CSignLayer::callBackForTotalSignEffect1");
	pLayer1->setOpacity(0);
	LayerManager::instance()->push(pLayer1);
	MaskLayer* lay = MaskLayer::create("CSignLayer::callBackForTotalSignEffect2");
	lay->setContentSize(CCSizeMake(1138,640));
	lay->setAnchorPoint(ccp(0.5f, 0.5f));
	lay->setPosition(VCENTER);
	lay->setOpacity(0);
	LayerManager::instance()->push(lay);

	//开宝箱
	int iIndexTotal = checkTotalBox();
	if(iIndexTotal==-1)
	{
		//触摸释放
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
		return;
	}

	int iTag = 9065+iIndexTotal;
	CCString* pStr = CCString::createWithFormat("%d", iTag);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(CCString::createWithFormat("skill/%s.plist", pStr->getCString())->getCString());
	AnimationManager::sharedAction()->ParseAnimation(pStr->getCString());
	CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation(pStr->getCString());
	culAnim->setDelayPerUnit(0.15f);
	CCAnimate* pAnimate = CCAnimate::create(culAnim);
	CImageView *mask = (CImageView*)m_ui->findWidgetById(CCString::createWithFormat("mask%d",iIndexTotal+1)->getCString());	
	mask->setVisible(false);
	CImageView* pSprite = UICloneMgr::cloneImageView(mask);
	pSprite->setVisible(true);
	pSprite->setId("mask_copy");
	pSprite->runAction(CCSequence::create(
		pAnimate, 
		CCCallFunc::create(this, callfunc_selector(CSignLayer::callBackForTotalSignEffectEnd)),
		nullptr
		));
	mask->getParent()->addChild(pSprite);
}


void CSignLayer::callBackForTotalSignEffectEnd()
{
	//触摸释放
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();

	//奖励窗关闭回调，刷总签到显示
	CSign& sign = m_signData.totalList[checkTotalBox()];
	sign.status = 1;
	if(DataCenter::sharedData()->getUser()->getUserData()->getVip()>=sign.vip && sign.vip>0)
	{
		sign.prize.num *= 2;
	}
	CPopItem *popItem = CPopItem::create();
	popItem->signPrize(&sign.prize);
	popItem->setDescVisiable(true);
	LayerManager::instance()->push(popItem);

	popItem->bindTargetCallBack(this, popitem_selector(CSignLayer::callBackForTotalSignUiUpdate));

}


void CSignLayer::callBackForTotalSignUiUpdate()
{
	CImageView* pSprite = (CImageView*)m_ui->findWidgetById("mask_copy");
	pSprite->removeFromParentAndCleanup(true);

	//更新阶段签到情况
	updateTotoalSign();
	//加黄标提示
	showTipsOnNextTotalSign();
}


void CLineLightEffect::showEffect()
{
	m_bTouchLock = true;

	CCSprite* pLine = CCSprite::create("sign/line_10.png");
	pLine->setPosition(VCENTER);
	this->addChild(pLine, 999);

	pLine->setScale(0.0f);
	pLine->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCScaleTo::create(0.2f, 1.0f), CCFadeIn::create(0.1f)),
		CCDelayTime::create(0.1f),
		CCHide::create(),
		CCCallFunc::create(this, callfunc_selector(CLineLightEffect::showEffectCallBack)),
		CCRemoveSelf::create()
		,nullptr));

	m_ui->setVisible(false);
	m_ui->setScaleY(0);
}

void CLineLightEffect::showEffectCallBack()
{
	m_ui->setVisible(true);
	m_ui->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.1f, 1.0f), CCCallFunc::create(this, callfunc_selector(CLineLightEffect::showRectEffect))));
	m_bTouchLock = false;
}

void CLineLightEffect::hideEffect()
{
	m_bTouchLock = true;

	m_ui->setVisible(true);
	m_ui->runAction(CCSequence::create(
		CCScaleTo::create(0.1f, 1.0f, 0.0f), 
		CCDelayTime::create(0.1f), 
		CCCallFunc::create(this, callfunc_selector(CLineLightEffect::hideEffectCallBack)),
		nullptr));
}

void CLineLightEffect::hideEffectCallBack()
{
	CCSprite* pLine = CCSprite::create("sign/line_10.png");
	pLine->setPosition(VCENTER);
	this->addChild(pLine, 999);

	pLine->setScale(1.0f);
	pLine->runAction(CCSequence::create(CCScaleTo::create(0.2f, 0.0f),
		CCCallFunc::create(this, callfunc_selector(CLineLightEffect::pop)),
		CCRemoveSelf::create()
		,nullptr));
}

void CLineLightEffect::pop()
{
	m_bTouchLock = false;

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CLineLightEffect::showRectEffect()
{
	if(m_pRectEffectNode!=nullptr)
	{
		//先准备两个条
		CCSprite* pUpBar1 = CCSprite::create("sign/line_10.png");
		CCSprite* pDownBar1 = CCSprite::create("sign/line_10.png");

		CCSize sizeNode = m_pRectEffectNode->getContentSize();
		//位置1
		int iMaxY = m_pRectEffectNode->getPositionY()+(1.0f-m_pRectEffectNode->getAnchorPoint().y)*sizeNode.height*m_pRectEffectNode->getScaleY();
		int iMinY = m_pRectEffectNode->getPositionY()-m_pRectEffectNode->getAnchorPoint().y*sizeNode.height*m_pRectEffectNode->getScaleY();

		//宽度
		int iWidth = pUpBar1->getContentSize().width;

		float fTime = 2.4f;
		pUpBar1->setFlipY(true);
		pUpBar1->setAnchorPoint(ccp(0, 0.5f));
		pUpBar1->setPosition(ccp(DESIGN_WIDTH, iMaxY));
		m_ui->addChild(pUpBar1, 30);
		pUpBar1->setScaleY(0.5f);
		pUpBar1->runAction(CCRepeatForever::create(
			CCSequence::create(
			CCMoveBy::create(fTime, ccp(-iWidth*2, 0)), 
			CCMoveBy::create(0, ccp(iWidth*2, 0)), 
			CCDelayTime::create(0.2f),
			nullptr)));

		pDownBar1->setAnchorPoint(ccp(1, 0.5f));
		pDownBar1->setPosition(ccp(0, iMinY));
		pDownBar1->setScaleY(0.5f);
		m_ui->addChild(pDownBar1, 30);
		pDownBar1->runAction(CCRepeatForever::create(
			CCSequence::create(
			CCMoveBy::create(fTime, ccp(iWidth*2, 0)), 
			CCMoveBy::create(0, ccp(-iWidth*2, 0)), 
			CCDelayTime::create(0.2f),
			nullptr)));

	}
}
