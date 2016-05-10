#include "BuyResource.h"
#include "model/DataCenter.h"
#include "netcontrol/CPlayerControl.h"
#include "tools/UICloneMgr.h"
#include "vip/VipLayer.h"
#include "ExchangeTip.h"
#include "GMessage.h" 
#include "tools/ShowTexttip.h"
#include "common/CShowToBuyResource.h"
#include "common/CheckMoney.h"
#include "common/CGameSound.h"
#include "Resources.h"

BuyResource::BuyResource()
	:m_ui(nullptr),m_tableView(nullptr),m_vipConvertLay(nullptr)
	,m_goldToCoinLay(nullptr),m_norConvertLay(nullptr),m_exchangeType(0),m_animIndex(0)
	,m_isPlay(false),m_buyNum(0)
{}

BuyResource::~BuyResource()
{}

bool BuyResource::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("BuyResourmask");
		lay->setContentSize(CCSizeMake(1138,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("AddResource.xaml");  //  
		m_ui->setPosition(VCENTER);

		//退出
// 		CButton* pClose = CButton::create("common/back.png", "common/back.png");
// 		pClose->getSelectedImage()->setScale(1.1f);
// 		pClose->setPosition(VLEFT+50, VTOP-50);
// 		pClose->setOnClickListener(this, ccw_click_selector(BuyResource::onClick));
// 		this->addChild(pClose, 999);

		this->addChild(m_ui);

		return true;
	}
	return false;
}

void BuyResource::setExchangeType(int type)
{
	m_exchangeType = type;
}

void BuyResource::initByType(int but_type)
{
}

void BuyResource::onClick(CCObject*ob)
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void BuyResource::onEnter()
{
	BaseLayer::onEnter();
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_tableView = (CTableView*)m_ui->findWidgetById("scroll");
	m_tableView->setDirection(eScrollViewDirectionVertical);	
	m_tableView->setCountOfCell(0);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setAnchorPoint(ccp(0,0));
	m_tableView->setAutoRelocate(true);
	m_tableView->setDeaccelerateable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(BuyResource::tableviewDataSource));
	m_tableView->reloadData();

	m_norConvertLay = (CLayout *)(m_ui->findWidgetById("nor_buy_btm"));
	m_vipConvertLay = (CLayout *)(m_ui->findWidgetById("Vip_convert"));
	m_goldToCoinLay = (CLayout *)(m_ui->findWidgetById("convert_panel"));
	m_goldToCoinLay->setVisible(false);

	GetTcpNet->registerMsgHandler(GetConvertInfo,this,CMsgHandler_selector(BuyResource::ProcessMsg));
	GetTcpNet->registerMsgHandler(ConvertResult,this,CMsgHandler_selector(BuyResource::ProcessMsg));

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(BuyResource::roleUpdate));
}

void BuyResource::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
	//解绑场景事件监听
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
}

void BuyResource::exchangeToCoin()
{
	m_goldToCoinLay->setVisible(true);
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	if (user->getVip()>0)
	{
		m_vipConvertLay->setVisible(true);
		m_norConvertLay->setVisible(false);

		CButton *buy_one = (CButton *)m_ui->findWidgetById("Vip_buy_one");
		buy_one->setOnClickListener(this,ccw_click_selector(BuyResource::exchangeOne));
		CButton *buy_more = (CButton *)m_ui->findWidgetById("Vip_buy_more");
		buy_more->setOnClickListener(this,ccw_click_selector(BuyResource::exchangeMore));
	}
	else
	{
		m_norConvertLay->setVisible(true);
		CButton *buy_one = (CButton *)m_ui->findWidgetById("exchange");
		buy_one->setOnClickListener(this,ccw_click_selector(BuyResource::exchangeOne));
	}
	updateExchange();
}

void BuyResource::exchangeToFood()
{
	m_goldToCoinLay->setVisible(false);

	CLayout *foodLay = (CLayout *)(m_ui->findWidgetById("buy_foodNumMaxPanel"));
	CLayout *buyLay = (CLayout *)(m_ui->findWidgetById("buy_food_show"));
	CLayout *vipLay = (CLayout *)(m_ui->findWidgetById("buy_food_Vipmax")); //buy_food_max
	CLayout *maxBuyLay = (CLayout *)(m_ui->findWidgetById("buy_food_max"));

	foodLay->setVisible(true);
	
	//已经达到体力上限
	if(m_exchangeInfo.exchange==0)
	{
		////弹框告知
		//if(!dynamic_cast<CPopTip*>(LayerManager::instance()->getLayer("BuyResourceText")))
		//{
		//	LayerManager::instance()->pop();
		//	LayerManager::instance()->pop();
		//}
		
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
		CPopTip* BuyResourceText = ShowPopTextTip(GETLANGSTR(1113));
		BuyResourceText->setName("BuyResourceText");

		return;
	}

	if (m_exchangeInfo.exTime>0)
	{
		buyLay->setVisible(true);
		CLabel *gold = (CLabel *)m_ui->findWidgetById("buy_food_needNum");
		CLabel *food = (CLabel *)m_ui->findWidgetById("buy_food_getNum");
		CLabel *time = (CLabel *)m_ui->findWidgetById("can_buy_foodNum");
		gold->setString(ToString(m_exchangeInfo.gold));
		//检查货币数量
		if(!CheckCoin(atoi(gold->getString())))
		{
			gold->setColor(RGB_RED);
		}
		else
		{
			gold->setColor(RGB_WHITE);
		}

		food->setString(ToString(m_exchangeInfo.exchange));
		time->setString(CCString::createWithFormat("%d/%d",m_exchangeInfo.exTime,m_exchangeInfo.maxTime)->getCString());

		CButton *exchange = (CButton*)(buyLay->findWidgetById("exchange_food"));
		exchange->setOnClickListener(this,ccw_click_selector(BuyResource::exchangeOne));
	}
	else 
	{
		CLabel *time = (CLabel*)m_ui->findWidgetById("buf_food_Vip_alreadNum");
		time->setString(ToString(m_exchangeInfo.maxTime-m_exchangeInfo.exTime));
		buyLay->setVisible(false);
		vipLay->setVisible(true);
		m_norConvertLay->setVisible(false);
		CLayout *vip = (CLayout *)m_ui->findWidgetById("checkvip");
		vip->setVisible(true);

		CButton *check_vip = (CButton *)vip->findWidgetById("check_vip");
		check_vip->setOnClickListener(this,ccw_click_selector(BuyResource::checkVip));
	}
}

void BuyResource::checkVip(CCObject* pObject)
{
	CVipLayer *vipLayer= CVipLayer::create();
	if(LayerManager::instance()->push(vipLayer))
	{
		vipLayer->setOptionType(1);
		GetTcpNet->sendDataType(VipInfoMsg);
		GetTcpNet->sendDataType(VipShopMsg);
	}
}

void BuyResource::exchangeOne(CCObject* pObject)
{
	//runResAnimate();
	CPlayerControl::getInstance().sendExchange(m_exchangeType,1);
}

void BuyResource::onContinueExchange(CCObject* pObject)
{
	CButton* btn = (CButton*)pObject;
	if (strcmp(btn->getId(),"ok")==0)
	{
		CPlayerControl::getInstance().sendExchange(m_exchangeType,2);
	}
	LayerManager::instance()->pop();
	LayerManager::instance()->pop(); 
}

void BuyResource::exchangeMore(CCObject* pObject)
{
// 	
	CExchangeTip *tip = CExchangeTip::create();
	LayerManager::instance()->push(tip);
	int time = m_exchangeInfo.exTime>5?5:m_exchangeInfo.exTime;
	tip->setData(m_exchangeInfo.gold,m_exchangeInfo.exchange,time);
	tip->setBtnListener(this,ccw_click_selector(BuyResource::onContinueExchange));
}

CCObject* BuyResource::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell* pCell = (CTableViewCell*)pConvertCell;
	if (!pCell)
	{
		pCell = new CTableViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		addTableCell(uIdx,pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx,pCell);
	}
	return pCell;
}

void BuyResource::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);		
	CExRes *res = &m_exchangeRes.resultList.at(uIdx);

	for (int i = 1; i <= 9; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		switch (i)
		{
		case 3:
			{
				CLabel *num = (CLabel*)child;
				num->setString(ToString(res->gold));
			}
			break;
		case 6:
			{
				CLabel *num = (CLabel*)child;
				num->setString(ToString(res->exchange));
			}
			break;
		case 7:
			{
				child->setVisible(res->crit>0);
			}
			break;
		case 8:
			{
				if (res->crit>0)
				{			
					CLabelAtlas *num = (CLabelAtlas*)child;
					num->setString(ToString(res->crit));
				}
				else
				{
				    child->setVisible(false);
				}
			}
			break;
		case 9:
			{
				child->setVisible(res->crit>0);
			}
			break;
		default:
			break;
		}
	}
	if (m_isPlay)
	{
		if ((m_buyNum==1&&uIdx==m_exchangeRes.resultList.size()-1)||m_buyNum==5)
		{
			pCell->setVisible(false);
			pCell->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2+0.4*uIdx),CCShow::create()));
		}
	}
}

void BuyResource::resetPosition(CCNode *pNode)
{
	pNode->setPositionY(pNode->getPositionY()-m_cell->getContentSize().height);
}

void BuyResource::ProcessMsg(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	switch (type)
	{
	case GetConvertInfo:
		{
			ExchangeInfoRes *res = (ExchangeInfoRes*)msg;
			m_exchangeInfo.read(*res);

			if (m_exchangeType==BUY_COIN)
			{
				exchangeToCoin();
			}
			else if (m_exchangeType==BUY_FOOD)
			{
				exchangeToFood();
			}
		}
		break;
	case ConvertResult:
		{
			ExchangeRes *res = (ExchangeRes*)msg;		
			m_buyNum = res->result_size();
			PlayEffectSound(SFX_Ensure);
			if (m_exchangeRes.resultList.size()>=5||m_buyNum==5)
			{
				m_exchangeRes.resultList.clear();
			}
			m_exchangeRes.read(*res);
			//兑换结果(1 成功，2 兑换次数不足，3 元宝不足，4 数据错误)
			switch (res->status())
			{
			case 1:
				{
					PlayEffectSound(SFX_415);

					int gold=0, coin=0;
					for (int i = 0; i < res->result_size(); i++)
					{
						gold+= res->result(i).gold();
						coin+= res->result(i).exchange();
					}
					
					UserData *user = DataCenter::sharedData()->getUser()->getUserData();
					user->setRoleGold(user->getRoleGold()-gold);
					if (m_exchangeType!=1)
					{
						user->setRoleAction(user->getRoleAction()+m_exchangeInfo.exchange);
					}

					m_exchangeInfo.exTime = m_exchangeRes.exTime;
					m_exchangeInfo.gold = m_exchangeRes.gold;
					m_exchangeInfo.exchange = m_exchangeRes.exchange;
					if (m_exchangeType==1)
					{
						updateExchange();
						m_isPlay = true;
						CLayout *resLay = (CLayout *)(m_ui->findWidgetById("result_panel"));
						resLay->setVisible(true);
						CCPoint pos = m_tableView->getMaxOffset();
						m_tableView->setCountOfCell(m_exchangeRes.resultList.size());
						m_tableView->reloadData();

						m_isPlay = false;
						if (m_exchangeRes.resultList.size()>4)
						{
							int time = m_exchangeRes.resultList.size()-5;
							this->schedule(schedule_selector(BuyResource::updateTableOffset),0.3f,time,1.0f);
						}
						m_animIndex = 0;
						this->unschedule(schedule_selector(BuyResource::showCoinAnim));
						this->schedule(schedule_selector(BuyResource::showCoinAnim),0.5f,m_buyNum-1,0.05f);
						//runResAnimate();
						user->setCoin(user->getCoin()+coin);
					}
					else
					{                                                                                                                                  
						exchangeToFood();
					}
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
				}
				break;
			case 2:
				{
					ShowPopTextTip(GETLANGSTR(285));
				}
				break;
			case 3:
				{
					//ShowPopTextTip(GETLANGSTR(286));
					CShowToBuyResource* pShow = CShowToBuyResource::create();
					pShow->showToBuyResourceByType(ShowBuyResourceGold);
				}
				break;
			case 4:
				{
					ShowPopTextTip(GETLANGSTR(287));
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}

void BuyResource::updateTableOffset(float dt)
{
	CCPoint offset = m_tableView->getContentOffset()+ ccp(0,m_cell->getContentSize().height);
	m_tableView->setContentOffsetInDuration(offset,0.1f);
}

void BuyResource::showCoinAnim(float dt)
{
	int idx = m_exchangeRes.resultList.size()-m_buyNum;
	this->runResAnimate(idx);
	m_buyNum--;
}

void BuyResource::flyDone(CCNode *pNode)
{
	pNode->removeFromParent();
}

void BuyResource::updateExchange()
{
	CLabel *getNum = (CLabel*)m_goldToCoinLay->findWidgetById("canget_num");
	getNum->setString(CCString::createWithFormat("%d/%d",m_exchangeInfo.exTime,m_exchangeInfo.maxTime)->getCString());

	CLayout *conv = (CLayout *)m_goldToCoinLay->findWidgetById("conv");
	if (m_exchangeInfo.exTime>0)
	{
		conv->setVisible(true);	
		CLayout *tips = (CLayout *)m_ui->findWidgetById("convert_over_tips");
		tips->setVisible(false);

		CLabel *needNum = (CLabel*)conv->findWidgetById("show_need_num");
		needNum->setString(ToString(m_exchangeInfo.gold));
		//检查货币数量
		if(!CheckGold(atoi(needNum->getString())))
		{
			needNum->setColor(RGB_RED);
		}
		else
		{
			needNum->setColor(RGB_WHITE);
		}

		CLabel *exchangNum = (CLabel*)conv->findWidgetById("show_get_num");
		exchangNum->setString(ToString(m_exchangeInfo.exchange));
	}
	else
	{
		conv->setVisible(false);
		CLayout *tips = (CLayout *)m_ui->findWidgetById("convert_over_tips");
		tips->setVisible(true);
		m_norConvertLay->setVisible(false);
		m_vipConvertLay->setVisible(false);
		CLayout *vip = (CLayout *)m_ui->findWidgetById("checkvip");
		vip->setVisible(true);

		CButton *check_vip = (CButton *)vip->findWidgetById("check_vip");
		check_vip->setOnClickListener(this,ccw_click_selector(BuyResource::checkVip));
	}
}

void BuyResource::runResAnimate(int index)
{
	CExRes *res =&m_exchangeRes.resultList.at(index);

	CLayout *anim = (CLayout*)(m_ui->findWidgetById("anim"));
	CLayout *cloneAnim = UICloneMgr::cloneLayout(anim);

	CLabelAtlas *coinNum = (CLabelAtlas*)cloneAnim->findWidgetById("coin_num");
	if (!coinNum)
	{
		CLabelAtlas *label = (CLabelAtlas *)(anim->findWidgetById("coin_num"));
		coinNum = CLabelAtlas::create(label->getString(), label->getMapFile(), label->getItemWidth(), label->getItemHeight(),label->getStartCharMap());
		coinNum->setPosition(label->getPosition());
		coinNum->setAnchorPoint(label->getAnchorPoint());
		cloneAnim->addChild(coinNum);
		
		CImageView *img = (CImageView*)anim->getChildByTag(1);
		CImageView *cloneimg = CImageView::create();
		cloneimg->setTexture(img->getTexture());
		cloneimg->setPosition(img->getPosition());
		cloneAnim->addChild(cloneimg);
	}
	coinNum->setString(ToString(res->exchange));

	CCFiniteTimeAction* fly = CCSpawn::create(
		CCMoveBy::create(1.35f, ccp(0,90)),
		CCSequence::create(CCScaleTo::create(0.2f,1.1f),CCDelayTime::create(0.3f),CCScaleTo::create(0.2f,1.0f),CCFadeTo::create(0.3f, 150), NULL),
		NULL);
	CCFiniteTimeAction* action = CCSequence::create(CCDelayTime::create(0.3f),CCShow::create(),fly,
		CCCallFuncN::create(this, callfuncN_selector(BuyResource::flyDone)),
		NULL);
	this->addChild(cloneAnim);
	cloneAnim->runAction(action);

	if (m_animIndex>=m_exchangeRes.resultList.size())
	{
		return;
	}
	//CExRes *res = &m_exchangeRes.resultList.at(m_animIndex);
	if (res->crit>0)
	{
		CLayout *anim = (CLayout*)(m_ui->findWidgetById("critAnim"));
		CLayout *cloneAnim = UICloneMgr::cloneLayout(anim);
		cloneAnim->retain();
		cloneAnim->setVisible(true);
		CLabelAtlas *critnum = (CLabelAtlas*)cloneAnim->findWidgetById("critnum");
		if (!critnum)
		{
			CLabelAtlas *label = (CLabelAtlas *)(anim->findWidgetById("critnum"));
			critnum = CLabelAtlas::create(label->getString(), label->getMapFile(), label->getItemWidth(), label->getItemHeight(),label->getStartCharMap());
			critnum->setPosition(label->getPosition());
			critnum->setAnchorPoint(label->getAnchorPoint());
			critnum->addChild(coinNum);

			CImageView *img = (CImageView*)anim->findWidgetById("baoji");
			CImageView *cloneimg = CImageView::create();
			cloneimg->setTexture(img->getTexture());
			cloneimg->setPosition(img->getPosition());
			cloneimg->setId("baoji");
			cloneAnim->addChild(cloneimg);
		}
		critnum->setString(ToString(res->crit));

		CCSprite *baoji = (CCSprite*)cloneAnim->findWidgetById("baoji");
		baoji->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("public/baoji_%d.png",res->crit)->getCString()));
		cloneAnim->setScale(1.0+0.1*res->crit);
		float scale = 1.0+0.1*res->crit;
		CCFiniteTimeAction* fly = CCSpawn::create(
			CCMoveBy::create(0.8f, ccp(0,80)),
			CCSequence::create(CCScaleTo::create(0.2f,1.1f*scale),CCDelayTime::create(0.2f),CCScaleTo::create(0.2f,1.0*scale),CCFadeTo::create(0.2f, 150), NULL),
			NULL);
		CCFiniteTimeAction* action = CCSequence::create(fly,
			CCCallFuncN::create(this, callfuncN_selector(BuyResource::flyDone)),
			NULL);
		this->addChild(cloneAnim);
		cloneAnim->release();
		cloneAnim->runAction(action);
	}
	m_animIndex++;

}

bool BuyResource::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	if( !res )
	{
		CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

		CImageView *bgView1 = (CImageView*)(m_ui->findWidgetById("bg1"));
		CLayout * layer = (CLayout*)(m_ui->findWidgetById("result_panel"));
		CImageView *bgView2 = (CImageView*)(m_ui->findWidgetById("bg2"));

		if( bgView1->boundingBox().containsPoint(pTouchPos)  ||  (layer->isVisible()  &&  bgView2->boundingBox().containsPoint(pTouchPos)))
		{
			res = true;
		}
		else
		{
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
	}
	return true;
}

void BuyResource::roleUpdate( const TMessage& tMsg )
{
	CLayout *conv = (CLayout *)m_goldToCoinLay->findWidgetById("conv");
	CLabel *needNum = (CLabel*)conv->findWidgetById("show_need_num");
	//检查货币数量
	if(!CheckGold(atoi(needNum->getString())))
	{
		needNum->setColor(RGB_RED);
	}
	else
	{
		needNum->setColor(RGB_WHITE);
	}


	CLabel *gold = (CLabel *)m_ui->findWidgetById("buy_food_needNum");
	//检查货币数量
	if(!CheckCoin(atoi(gold->getString())))
	{
		gold->setColor(RGB_RED);
	}
	else
	{
		gold->setColor(RGB_WHITE);
	}


}
