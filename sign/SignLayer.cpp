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

#include "common/CGameSound.h"
#include "Resources.h"
#include "model/CWholeBodyShowData.h"

#include <spine/spine-cocos2dx.h>
using namespace spine;

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
	,m_bTouchLock(false)
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


		return true;
	}
	return false;

}

void CSignLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	if(m_bTouchLock)
	{
		return;
	}

	hideEffect();
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
	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
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
		pMask->setTexture(setItemQualityTexture(sign.prize.color));
		item = CCSprite::create(GetImageName(sign.prize.type, sign.prize.color, sign.prize.thumb).c_str());
		if(!item)
		{
			item = CCSprite::create("headImg/101.png");
			CCLOG("CSignLayer::addGridCell  error load image %d", sign.prize.thumb);
		}
		item->setPosition(ccp(pMask->getContentSize().width/2, pMask->getContentSize().height/2));
		item->setZOrder(1);
		pMask->addChild(item, -1);
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
			CLayout* pStarLayout = getStarLayout(sign.prize.quality);
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
	case 2:
		{
			//对勾
			pCell->getChildByTag(4)->setVisible(false);
		}
		break;
	case 1:
		{
			//对勾
			pCell->getChildByTag(4)->setVisible(true);
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
		showEffect();
	}

	if (type == SignDataMsg)
	{
		SignData *res = (SignData*)msg;
		m_signData.read(*res);

	    m_gridView->setCountOfCell(m_signData.signList.size());
		ReloadGridViewWithSameOffset(m_gridView);

		m_resignGold = m_signData.resign_gold;

		updateTotoalSign();

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
					CPopItem *popItem = CPopItem::create();
					popItem->signPrize(&sign.prize);
					popItem->setDescVisiable(true);
					LayerManager::instance()->push(popItem);
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

	for (int i = 0; i < m_signData.totalList.size()&&i<4; i++)
	{
		CSign &sign = m_signData.totalList.at(i);
		CCSprite *itemspr = (CCSprite*)(m_ui->findWidgetById(CCString::createWithFormat("item%d",i+1)->getCString()));

		CImageView *item = (CImageView*)getPrize(sign);
		if (item)
		{
			item->setPosition(ccp(itemspr->getContentSize().width/2,itemspr->getContentSize().height/2));
			itemspr->addChild(item);
		}
		

		CImageView *mask = (CImageView*)m_ui->findWidgetById(CCString::createWithFormat("mask%d",i+1)->getCString());	
		mask->setTexture(setItemQualityTexture(sign.prize.color));
		mask->setUserData(&sign);
		mask->setTouchEnabled(true);

		//加数字
		CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
		haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
		haveNum->setPosition(ccp(85, 5));
		mask->addChild(haveNum, 99, 99);
		haveNum->setString(ToString(sign.prize.num));
		if(sign.prize.num<=1)
		{
			haveNum->setVisible(false);
		}

		//获得字样
		CImageView *getspr = (CImageView*)(m_ui->findWidgetById(CCString::createWithFormat("get%d",i+1)->getCString()));

		//已领取
		if (sign.status==1)
		{
			getspr->setVisible(true);
		}
		////未领取
		//else if(sign.status==2)
		//{
		//	mask->setOnPressListener(this, ccw_press_selector(CSignLayer::onPress));
		//}
		mask->setOnPressListener(this, ccw_press_selector(CSignLayer::onPress));

		CLabel *day = (CLabel*)(m_ui->findWidgetById(CCString::createWithFormat("day%d",i+1)->getCString()));
		day->setString(CCString::createWithFormat(GETLANGSTR(209),sign.day)->getCString());
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
		CButton * pReSignBtn = (CButton*)m_ui->findWidgetById("buqian_btn");
		pReSignBtn->setEnabled(false);
		pReSignBtn->getDisabledImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));

		//pRemainResignNum->setVisible(false);
		//CLabel* pRemainResignText = (CLabel*)m_ui->findWidgetById("buqian_text");
		//pRemainResignText->setVisible(false);
		//CLabel* pRemainResignBg = (CLabel*)m_ui->findWidgetById("buqian_bg");
		//pRemainResignBg->setVisible(false);
		//CLabel* pRemainResignBtn = (CLabel*)m_ui->findWidgetById("buqian_btn");
		//pRemainResignBtn->setVisible(false);
		//CLabel* pRemainResignBtnText = (CLabel*)m_ui->findWidgetById("buqian_btn_text");
		//pRemainResignBtnText->setVisible(false);
	}

	//totalSign
	for (int i = 0; i < m_signData.totalList.size()&&i<4; i++)
	{
		CSign &sign = m_signData.totalList.at(i);
		if (m_signData.sign >= sign.day)
		{
			CCSprite *getspr = (CCSprite*)(m_ui->findWidgetById(CCString::createWithFormat("get%d",i+1)->getCString()));
			getspr->setVisible(true);
		}
	}
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
		item = CCSprite::create("mainCity/tili.png");
	}
	else if (sign.prize.type ==0)
	{
		item = CCSprite::create("mainCity/icon_11.png");
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
	if(m_bTouchLock)
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

	int iHeightGap = 150;

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

	int offsetX = (iCurrentSignDayIndex-6)/5;
	if (offsetX>0)
	{	
		m_gridView->setContentOffset(ccpAdd(m_gridView->getContentOffset(),ccp(0, 95*offsetX)));
	}
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

			if(data)
			{
				//姓名
				CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
				name->setString(data->heroName.c_str());

				//类型
				CCSprite *type = (CCSprite*)(m_ui->findWidgetById("type"));
				type->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png",data->iType1, data->iType2)->getCString()));
			}
			

			//星级
			CImageView* pImageView = (CImageView*)(m_ui->findWidgetById("bg_black"));
			CLayout* pStarLay = getStarLayout(sign.prize.quality, 1.2f);
			pImageView->removeAllChildren();
			pStarLay->setPosition(pStarLay->getPosition()+ccp(53, -20));
			pImageView->addChild(pStarLay);

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
			//增加人物贴图
			const CWholeBodyShowInfo& pInfo = CWholeBodyShowData::getInstance()->getDataById(sign.prize.thumb);
			CCSprite* pNodePos = (CCSprite*)m_ui->findWidgetById("monster_pos");
			pNodePos->setVisible(true);
			pNodePos->setOpacity(0);
			CCPoint pPos = pNodePos->getPosition();

			char c_json[60]   = {0};
			char c_altlas[60] = {0};
			sprintf(c_json,"storyImage/Spine/888%d.json",sign.prize.thumb);
			sprintf(c_altlas,"storyImage/Spine/888%d.atlas",sign.prize.thumb);
			std::string sFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(c_json);
			if(!CCFileUtils::sharedFileUtils()->isFileExist(sFullPath))
			{
				CCLOG("ERROR -CSignLayer::initHeroData-file not foud-%s", c_json);
				sprintf(c_json,"storyImage/Spine/888756.json");
				sprintf(c_altlas,"storyImage/Spine/888756.atlas");
			}
			SkeletonAnimation*  Animation = SkeletonAnimation::createWithFile(c_json,c_altlas,1);
			Animation->setAnimation(0,"stand",true);
			pNodePos->addChild(Animation);

			Animation->setScale(pInfo.fScale*0.65f);

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

void CSignLayer::showEffect()
{
	m_bTouchLock = true;

	CCSprite* pLine = CCSprite::create("sign/line_10.png");
	pLine->setPosition(VCENTER);
	this->addChild(pLine, 999);

	pLine->setScale(0.0f);
	pLine->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCScaleTo::create(0.2f, 1.0f), CCFadeIn::create(0.1f)),
		CCDelayTime::create(0.1f),
		CCHide::create(),
		CCCallFunc::create(this, callfunc_selector(CSignLayer::showEffectCallBack)),
		CCRemoveSelf::create()
		,nullptr));
	
	m_ui->setVisible(false);
	m_ui->setScaleY(0);
}

void CSignLayer::showEffectCallBack()
{
	m_ui->setVisible(true);
	m_ui->runAction(CCScaleTo::create(0.1f, 1.0f));
	m_bTouchLock = false;
}

void CSignLayer::hideEffect()
{
	m_bTouchLock = true;

	m_ui->setVisible(true);
	m_ui->runAction(CCSequence::create(
		CCScaleTo::create(0.1f, 1.0f, 0.0f), 
		CCDelayTime::create(0.1f), 
		CCCallFunc::create(this, callfunc_selector(CSignLayer::hideEffectCallBack)),
		nullptr));
}

void CSignLayer::hideEffectCallBack()
{
	CCSprite* pLine = CCSprite::create("sign/line_10.png");
	pLine->setPosition(VCENTER);
	this->addChild(pLine, 999);

	pLine->setScale(1.0f);
	pLine->runAction(CCSequence::create(CCScaleTo::create(0.2f, 0.0f),
		CCCallFunc::create(this, callfunc_selector(CSignLayer::popSelf)),
		CCRemoveSelf::create()
		,nullptr));
}

void CSignLayer::popSelf()
{
	m_bTouchLock = false;

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSignLayer::showFirstLoginEffect()
{
	//粒子飞过去
}
