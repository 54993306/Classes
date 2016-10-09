#include "PairCardsActivity.h"
#include "common/CShowToBuyResource.h"
#include "common/CheckMoney.h"
#include "common/CGameSound.h"
#include "netcontrol/CPlayerControl.h"
#include "tools/UICloneMgr.h"
#include "tools/ShowTexttip.h"
#include "GMessage.h"
#include "sign/PopItem.h"
#include "platform/CCPlatformMacros.h"

PairCardsActivityLayer::PairCardsActivityLayer():
	m_itemInfo(nullptr),
	m_monsterInfo(nullptr),

	m_isFree(false),
	m_needGold_GameTimes(0),
	m_needGold_otherPairCardTimes(0),
	m_gameTimes_free(0),
	m_gameTimes_Buy(0),

	m_cardsIndex(0),
	m_gameNum(0),
	m_cardsNum(0),
	m_gameState(unStartGame),
	m_openCardsNum(0),
	m_currCard(0),

	isShowPrizePopItem(false),
	m_record_onPressTimes(0),

	m_pairTime(0),
	m_minPosY(0),
	m_maxPosY(0),
	m_currNoticeLab(0),
	m_isUpdateNotice(false),
	m_isInit(true)
{}
PairCardsActivityLayer::~PairCardsActivityLayer()
{
	//m_ImageList.~vector();
	//m_cardsPos.~vector();

	vector<CImageView* >::iterator _Image = m_ImageList.begin();
	for (;_Image != m_ImageList.end();)
		_Image = m_ImageList.erase(_Image);
	m_ImageList.clear();

	vector<CCPoint >::iterator _point = m_cardsPos.begin();
	for (;_point != m_cardsPos.end();)
		_point = m_cardsPos.erase(_point);
	m_cardsPos.clear();

	vector<CPrize >::iterator _prize = m_prizeItemList.begin();
	for (;_prize != m_prizeItemList.end();)
		_prize = m_prizeItemList.erase(_prize);
	m_prizeItemList.clear();

	_prize = m_showItemList.begin();
	for (;_prize != m_showItemList.end();)
		_prize = m_showItemList.erase(_prize);
	m_showItemList.clear();

	_prize = m_getPrizeList.begin();
	for (;_prize != m_getPrizeList.end();)
		_prize = m_getPrizeList.erase(_prize);
	m_getPrizeList.clear();

	vector<CNotice >::iterator _notice = m_noticeList.begin();
	for (;_notice != m_noticeList.end();)
		_notice = m_noticeList.erase(_notice);
	m_noticeList.clear();

	vector<CLabel* >::iterator _Label = m_noticeLab.begin();
	for (;_Label != m_noticeLab.end();)
		_Label = m_noticeLab.erase(_Label);
	m_noticeLab.clear();
}

bool PairCardsActivityLayer::init()
{
	if (BaseLayer::init())
	{
		CCLOG("init");
		MaskLayer* pMaskLayer = MaskLayer::create("PairCardsActivityLayerMask");
		pMaskLayer->setContentSize(CCDirector::sharedDirector()->getWinSize());
		pMaskLayer->setVisible(false);
		LayerManager::instance()->push(pMaskLayer);

		m_ui = LoadComponent("PairActivity.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(false);

		m_cardsImage = (CImageView*)m_ui->findWidgetById("ItemBg");
		m_cardsImage->retain();
		m_cardsImage->removeFromParent();

		m_cardsNameLab = (CLabel* )m_ui->findWidgetById("Name");
		m_cardsNameLab->retain();
		m_cardsNameLab->removeFromParent();

		m_cardsBlueFrame = (CImageView* )m_ui->findWidgetById("BlueFrame");
		m_cardsBlueFrame->retain();
		m_cardsBlueFrame->removeFromParent();

		m_noticeLabel= (CLabel* )m_ui->findWidgetById("noticeLabel");
		m_noticeLabel->retain();
		m_noticeLabel->removeFromParent();

		return true;
	}
	return false;
}
void PairCardsActivityLayer::onEnter()
{
	CCLOG("onEnter");
	BaseLayer::onEnter();

	m_startGameBtn = (CButton* )m_ui->findWidgetById("StartBtn");
	m_startGameBtn->getSelectedImage()->setScale(1.1f);
	m_startGameBtn->setOnClickListener(this,ccw_click_selector(PairCardsActivityLayer::onStartGameBtnClick));

	m_closeBtn = (CButton*)m_ui->findWidgetById("CloseBtn");
	m_closeBtn->getSelectedImage()->setScale(1.1f);
	m_closeBtn->setPosition(VLEFT+50 ,VTOP-50);
	m_closeBtn->setOnClickListener(this,ccw_click_selector(PairCardsActivityLayer::onClose));

	m_texture_on = CCTextureCache::sharedTextureCache()->addImage("pairCards/card_front.png");
	m_texture_off = CCTextureCache::sharedTextureCache()->addImage("pairCards/card_back.png");

	m_cardsPosImage = (CImageView* )m_ui->findWidgetById("CardPos");
	m_cardsPosImage->setVisible(false);

	m_currFreeTimesLab = (CLabel* )m_ui->findWidgetById("GameTimes");
	m_currFreeTimesLab->setVisible(false);

	m_redPoint = (CImageView* )m_ui->findWidgetById("redPoint");
	m_redPoint->setVisible(false);

	GetTcpNet->registerMsgHandler(CardListMsg,this,CMsgHandler_selector(PairCardsActivityLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(CardResMsg,this,CMsgHandler_selector(PairCardsActivityLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(CardStartMsg,this,CMsgHandler_selector(PairCardsActivityLayer::ProcessMsg));
}
void PairCardsActivityLayer::onExit()
{
	BaseLayer::onExit();

	GetTcpNet->unRegisterAllMsgHandler(this);
}
void PairCardsActivityLayer::onClose(CCObject* obj)
{
	switch (m_gameState)
	{
	case gameOver:
	case unStartGame:
		{
			this->setVisible(false);
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
		break;
	case StartGame:
		{
			//弹框确定退出本次游戏
			CCString* str = CCString::createWithFormat(GETLANGSTR(2067));
			ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(PairCardsActivityLayer::callFuncClose));
		}
		break;
	default:
		break;
	}
}
void PairCardsActivityLayer::callFuncClose(CCObject* pSender)
{
	CButton* btn = (CButton* )pSender;
	if (btn->getTag() == PopTipConfirm)
	{
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
		isShowPrizePopItem = true;
		showPrizePopItem();
		return;
	}
	((CPopTip* )btn->getParent()->getParent())->onClose(nullptr);
}

void PairCardsActivityLayer::ProcessMsg(int type, google::protobuf::Message* msg)
{
	CCLOG("ProcessMsg");
	if (!isVisible())
	{
		this->setVisible(true);
	}

	switch (type)
	{
	case CardListMsg:
		{
			m_prizeItemList.clear();
			m_noticeList.clear();

			CardListRes* res = (CardListRes* )msg;

			for (int i = 0;i<res->card_list_size();i++)
			{
				CPrize _prize;
				_prize.read(res->card_list(i));
				CCLOG("%d_%d_%d",_prize.type,_prize.id,_prize.num);
				m_prizeItemList.push_back(_prize);
			}
			for (int i =0; i< res->notices_size();i++)
			{
				CNotice _notice;
				_notice.read(res->notices(i));
				m_noticeList.push_back(_notice);
			}
			CCLOG("%d",m_noticeList.size());
			m_isFree = res->is_free();
			m_needGold_GameTimes = res->price_1();
			m_needGold_otherPairCardTimes = res->price_2();
			m_gameTimes_free = res->draw_time();
			m_gameTimes_Buy = res->ex_time();

			m_gameNum = res->draw_time();
			m_cardsNum = res->card_list_size();
			//弹框显示规则
			//CCString* str = CCString::createWithFormat(GETLANGSTR(2070),m_needGold_otherPairCardTimes);
			//ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(PairCardsActivityLayer::gameRuleExplain));

			if (m_gameState == gameOver)
			{
				m_gameState = unStartGame;
			}

			initCards();
			initNotice();
		}
		break;
	case CardResMsg:
		{
			GetCardRes* res = (GetCardRes*) msg;
			CCLOG("%d",res->result());
			switch (res->result())
			{
			case 1:
				{
					CPrize _prize;
					_prize.read(res->card());
					m_showItemList.push_back(_prize);
					updateOneCards();
					startAction();
				}
				break;
			case 2:
				{
					//抽卡次数用完
				}
				break;
			case 3:
				{
					CShowToBuyResource* show = CShowToBuyResource::create();
					show->showToBuyResourceByType(ShowBuyResourceGold);
				}
				break;
			case 4:
				{
					ShowPopTextTip(GETLANGSTR(170));
				}
				break;
			default:
				CCLOG("error");
				break;
			}
		}
		break;
	case CardStartMsg:
		{
			DrawCardStart* res = (DrawCardStart* )msg;
			switch (res->result())
			{
			case 1:
				{
					if (!m_isFree)
					{
						UserData* data = DataCenter::sharedData()->getUser()->getUserData();
						data->setRoleGold(data->getRoleGold() - m_needGold_GameTimes);
						CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
					}
					startGame();
				}
				break;
			case 2:
				{
					CShowToBuyResource* show = CShowToBuyResource::create();
					show->showToBuyResourceByType(ShowBuyResourceGold);
				}
				break;
			case 3:
				{
					ShowPopTextTip(GETLANGSTR(170));
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
void PairCardsActivityLayer::gameRuleExplain(CCObject* pSender)
{
	CButton* btn = (CButton* )pSender;
	((CPopTip* )btn->getParent()->getParent())->onClose(nullptr);
}
void PairCardsActivityLayer::initCards()
{
	CCLOG("initCards");
	//初始化卡牌信息
	m_pairTime = 0.1f;
	
	CCString* str = CCString::createWithFormat("%d",m_gameNum);
	m_currFreeTimesLab->setString(str->getCString());
	m_currFreeTimesLab->setVisible(true);
	//设置按钮为开始游戏按钮
	CImageView* image = (CImageView* )m_ui->findWidgetById("StartGoldImage");
	image->setVisible(true);
	CLabel* start = (CLabel* )m_ui->findWidgetById("StartLab");
	start->setVisible(true);
	CLabel* again = (CLabel* )m_ui->findWidgetById("AgainLab");
	again->setVisible(false);

	CLabel* _free_lab = (CLabel* )m_ui->findWidgetById("StartNeed_0");
	_free_lab->setVisible(false);
	CLabel* _needGold_lab = (CLabel* )m_ui->findWidgetById("StartNeed");
	_needGold_lab->setVisible(false);
	CCLOG("m_isFree_%d",m_isFree);
	if (!m_isFree)
	{ 
		CCString* str = CCString::createWithFormat("%d",m_needGold_GameTimes);
		_needGold_lab->setString(str->getCString());
		_needGold_lab->setVisible(true);
		_free_lab->setVisible(false);
	}else
	{
		_free_lab->setVisible(true);
		_needGold_lab->setVisible(false);
		m_redPoint->setVisible(true);
	}

	if (m_isInit)
	{
		m_isInit = false;
		initCardsData();
	}else
	{
		switch (m_gameState)
		{
		case unStartGame:
			{
				updateCardsData();
			}
			break;
		case StartGame:
			{
				updateCardsData();
				openAllCards();
			}
			break;
		default:
			break;
		}
	}
}
void PairCardsActivityLayer::initCardsData()
{
	//扩充vector列表
	//vector就是动态数组，当vector内存不足时会重新申请一段内存，并进行拷贝
	//clear不会清除内存预留内存-即最大容量
	for (int i = 0;i < m_gameTimes_free + m_gameTimes_Buy ;i++)
	{
		CPrize _prize = m_prizeItemList.at(0);
		m_showItemList.push_back(_prize);
	}
	m_showItemList.clear();

	m_ImageList.clear();
	for (int i = 0; i < m_cardsNum; i++)
	{
		CPrize& prize  = m_prizeItemList.at(i);
		//拷贝卡牌Image
		CImageView* cloneImage = UICloneMgr::cloneImageView(m_cardsImage);
		cloneImage->removeAllChildren();
		cloneImage->setVisible(true);
		cloneImage->setAnchorPoint(ccp(0.5f,0));
		cloneImage->setEnabled(true);
		cloneImage->setTouchEnabled(true);
		cloneImage->setUserData(&prize);
		cloneImage->setOnPressListener(this,ccw_press_selector(PairCardsActivityLayer::onPress));
		m_ui->addChild(cloneImage);

		float cardSize_x = cloneImage->getContentSize().width;
		float cardSize_y = cloneImage->getContentSize().height;
		if (i < 6)
		{
			cloneImage->setPosition(ccp(i* (cardSize_x + 5) + 50 + cardSize_x/2,VTOP - 90 - cardSize_y));
		}
		else if (i>= 6 &&i< 12)
		{
			cloneImage->setPosition(ccp((i -6)* (cardSize_x + 5)+ 50 + cardSize_x/2,VTOP -90 - 10 - 2* cardSize_y));
		}
		else if(i>= 12&&i< 18)
		{
			cloneImage->setPosition(ccp((i -12)* (cardSize_x + 5)+ 50 + cardSize_x/2,VTOP -90 - 20 - 3* cardSize_y));
		}
		//拷贝卡牌蓝光
		CImageView* cloneBlueFrame = UICloneMgr::cloneImageView(m_cardsBlueFrame);
		cloneBlueFrame->setPosition(ccp(cardSize_x/2,cardSize_y/2));
		cloneBlueFrame->setVisible(false);
		cloneImage->addChild(cloneBlueFrame,9,9);

		//拷贝卡牌Name
		CLabel* cloneNameLab = UICloneMgr::cloneLable(m_cardsNameLab);
		const ItemData* itemData = DataCenter::sharedData()->getItemDesc()->getCfg(prize.id);
		if (itemData)
		{
			if (itemData->itemName.size()>0)
			{
				cloneNameLab->setString(itemData->itemName.c_str());
			}
		}
		cloneNameLab->setAnchorPoint(ccp(0.5,0));
		cloneNameLab->setVisible(true);
		cloneImage->addChild(cloneNameLab,10,10);
		cloneNameLab->setPosition(ccp(cardSize_x/2,3));

		if (prize.thumb >= 0)
		{
			CCSprite* spItem = CCSprite::create(GetImageName(prize.type,prize.color,prize.thumb).c_str());
			if (!spItem)
				spItem = CCSprite::create("headImg/101.png");
			spItem->setPosition(ccp(cardSize_x/2,cardSize_y/2 + 10));
			cloneImage->addChild(spItem,10,11);
			switch (prize.type)
			{
			case 1:
			case 2:
				{
					spItem->setScale(0.8f);
				}
				break;
			case 3:
			case 4:
			case 5:
			case 6:
				break;
			default:
				break;
			}

			if (prize.num > 1)
			{
				CCLabelAtlas* itemNum = CCLabelAtlas::create("","label/no_02.png",9,15,46);
				itemNum->setAnchorPoint(ccp(1,0));
				itemNum->setScale(1.2f);
				itemNum->setPosition(ccp(cardSize_x -10,cardSize_y/4));
				cloneImage->addChild(itemNum,10,12);
				itemNum->setString(ToString(prize.num));
			}
		}
		m_ImageList.push_back(cloneImage);
	}
}
void PairCardsActivityLayer::updateCardsData()
{
	for (int i = 0; i < m_cardsNum; i++)
	{
		CPrize& prize  = m_prizeItemList.at(i);
		CImageView* image = (CImageView* )m_ImageList.at(i);
		image->setUserData(&prize);
		float cardSize_x = image->getContentSize().width;
		float cardSize_y = image->getContentSize().height;
		const ItemData* itemData = DataCenter::sharedData()->getItemDesc()->getCfg(prize.id);

		if (itemData)
		{
			if (itemData->itemName.size()>0)
			{
				CLabel* _lab = (CLabel* )image->getChildByTag(10);
				_lab->setString(itemData->itemName.c_str());
				_lab->setVisible(false);
			}
		}

		if (prize.thumb >= 0)
		{
			image->removeChildByTag(11);
			CCSprite* spItem = CCSprite::create(GetImageName(prize.type,prize.color,prize.thumb).c_str());
			if (!spItem)
				spItem = CCSprite::create("headImg/101.png");
			spItem->setPosition(ccp(cardSize_x/2,cardSize_y/2 + 10));
			spItem->setVisible(false);
			image->addChild(spItem,10,11);
			switch (prize.type)
			{
			case 1:
			case 2:
				{
					spItem->setScale(0.8f);
				}
				break;
			case 3:
			case 4:
			case 5:
			case 6:
				break;
			default:
				break;
			}

			if (prize.num > 1)
			{
				CCLabelAtlas* _numLab = (CCLabelAtlas* )image->getChildByTag(12);
				if (!_numLab)
				{
					CCLabelAtlas* itemNum = CCLabelAtlas::create("","label/no_02.png",9,15,46);
					itemNum->setAnchorPoint(ccp(1,0));
					itemNum->setScale(1.2f);
					itemNum->setPosition(ccp(cardSize_x -10,cardSize_y/4));
					image->addChild(itemNum,10,12);
					itemNum->setString(ToString(prize.num));
					itemNum->setVisible(false);
				}else
				{
					_numLab->setString(ToString(prize.num));
					_numLab->setVisible(false);
				}
			}else
			{
				CCLabelAtlas* _numLab = (CCLabelAtlas* )image->getChildByTag(12);
				if(_numLab)
				{
					image->removeChildByTag(12);
				}
			}
		}
	}
	for (int i = 0; i < m_cardsNum; i++)
	{
		CImageView* image = (CImageView* )m_ImageList.at(i);
		if (image->getTexture() == m_texture_on)
		{
			setImageAllChildVisibleTo(image,true);
		}
	}
}

void PairCardsActivityLayer::onStartGameBtnClick(CCObject* obj)
{
	//点击开始游戏按钮
	CCLOG("m_gameState_%d",m_gameState);

	switch (m_gameState)
	{
	case unStartGame:
		{
			if (!m_isFree)
			{
				if (!CheckGold(m_needGold_GameTimes))
				{
					CShowToBuyResource* show = CShowToBuyResource::create();
					show->showToBuyResourceByType(ShowBuyResourceGold);
					return;
				}
			}
			DrawCardStart* req = new DrawCardStart;
			GetTcpNet->sendDataType(CardStartMsg);
			delete req;
		}
		break;
	case StartGame:
		{
			//弹框确定放弃本次游戏
			CCString* str = CCString::createWithFormat(GETLANGSTR(2067));
			ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(PairCardsActivityLayer::isGiveUpThisGame));
		}
		break;
	case gameOver:
		{
			//重新拉取数据
			CardListReq* req = new CardListReq;
			req->set_result(false);
			GetTcpNet->sendData(req,CardListMsg);

			for (int i = 0;i<m_ImageList.size();i++)
			{
				m_ImageList.at(i)->stopAllActions();
				m_ImageList.at(i)->setScale(1.0f);
				CImageView* _blueFrame = (CImageView*)m_ImageList.at(i)->getChildByTag(9);
				_blueFrame->setVisible(false);
			}
			m_showItemList.clear();
			m_getPrizeList.clear();
			for (int i =0;i<50;i++)
			{
				m_index_click[i] = 0;
			}
		}
		break;
	default:
		break;
	}
}
void PairCardsActivityLayer::startGame()
{
	m_openCardsNum = 0;
	m_currCard = 0;
	m_getPrizeList.clear();
	m_record_onPressTimes = 0;
	isShowPrizePopItem = false;

	for (int i =0;i<m_ImageList.size();i++)
	{
		m_ImageList.at(i)->stopAllActions();
		m_ImageList.at(i)->getChildByTag(9)->setVisible(false);
	}
	m_currFreeTimesLab->setString(CCString::createWithFormat("%d",m_gameNum)->getCString());
	//发牌第一阶段-旋转
	m_startGameBtn->setEnabled(false);
	m_closeBtn->setEnabled(false);
	setCardsListTouchEnableTo(false);
	for (int i  = 0;i<m_ImageList.size() ;i++)
	{
		CImageView* cardsImage = (CImageView* )m_ImageList.at(i);

		cardsImage->runAction(CCSequence::create(
			CCDelayTime::create(m_pairTime* i),
			CCScaleTo::create(m_pairTime,0,1),
			CCCallFuncN::create(this,callfuncN_selector(PairCardsActivityLayer::callFuncStartGameAction)),
			nullptr));
	}
}
void PairCardsActivityLayer::isGiveUpThisGame(CCObject* pSender)
{
	CButton* btn = (CButton* )pSender;
	if (btn->getTag() == PopTipConfirm)
	{
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
		isShowPrizePopItem = true;
		showPrizePopItem();
		return;
	}
	((CPopTip* )btn->getParent()->getParent())->onClose(nullptr);
}
void PairCardsActivityLayer::callFuncStartGameAction(CCNode* node)
{
	//发牌第二阶段-旋转移动+第三阶段-回到起始位置
	m_currCard++;
	CImageView* cardsImage = (CImageView* )node;
	CImageView* target = (CImageView*)m_ui->findWidgetById("CardPos");
	CCPoint posTarget = target->getPosition();
	CCPoint posSelf = cardsImage->getPosition();
	m_cardsPos.push_back(posSelf);

	cardsImage->setTexture(m_texture_off);
	setImageAllChildVisibleTo(cardsImage,false);
	cardsImage->runAction(CCSequence::create(
		CCScaleTo::create(m_pairTime,1,1),
		CCMoveTo::create(m_pairTime,posTarget),
		CCCallFunc::create(this,callfunc_selector(PairCardsActivityLayer::callFuncShowPos)),
		nullptr));

	if (m_currCard == m_cardsNum)
	{
		m_currCard = 0;
		for (int i =0; i<m_cardsNum ;i++)
		{
			CImageView* image = (CImageView*)m_ImageList.at(i);
			image->runAction(CCSequence::create(
				CCDelayTime::create(0.5+ i * m_pairTime),
				CCMoveTo::create(m_pairTime,m_cardsPos.at(i)),
				CCCallFunc::create(this,callfunc_selector(PairCardsActivityLayer::callFuncStartGameOverAction)),
				nullptr));
		}
	}
}
void PairCardsActivityLayer::callFuncShowPos()
{
	if (m_currCard == 2)
	{
		m_cardsPosImage->setVisible(true);
		m_cardsPosImage->setOpacity(255);
	}
}
void PairCardsActivityLayer::callFuncStartGameOverAction()
{
	//发牌动画结束

	m_currCard++;
	if (m_currCard == m_cardsNum -1)
	{
		m_cardsPosImage->setVisible(false);
	}

	if (m_currCard == m_cardsNum)
	{
		m_gameState = StartGame;
		m_startGameBtn->setEnabled(true);
		m_closeBtn->setEnabled(true);
		setCardsListTouchEnableTo(true);
	}
}

void PairCardsActivityLayer::onPress(CCObject* obj,CTouchPressState state)
{
	//点击显示物品信息---翻牌
	CImageView* cardsImage = (CImageView* )obj;
	CPrize* prize = (CPrize* )cardsImage->getUserData();
	int iType = prize->type;

	if (state == CTouchPressOn && m_gameState == StartGame
		&& cardsImage->getTexture() == m_texture_off && m_record_onPressTimes <m_gameNum)
	{
		PlayEffectSound(SFX_429);
		for (int i = 0;i<m_ImageList.size(); i++)
		{
			float posX = cardsImage->getPositionX();
			float posY = cardsImage->getPositionY();
			if (posX == m_cardsPos.at(i).x && posY == m_cardsPos.at(i).y)
			{
				m_cardsIndex = i+1;
				
				CCLOG("index _ %d", m_cardsIndex);
				break;
			}
		}
		//发送请求
		GetCardReq* req = new GetCardReq;
		req->set_card_pos(m_cardsIndex);
		GetTcpNet->sendData(req,CardResMsg);
		delete req;
	}

	if (cardsImage->getTexture() == m_texture_on)
	{
		switch (iType)
		{
		case 3:
		case 4:
		case 5:
		case 6:
		case 1:
			{
				if (m_itemInfo == nullptr)
				{
					m_itemInfo = CItemInfo::create();
					this->addChild(m_itemInfo);
				}
				switch (state)
				{
				case CTouchPressOn:
					{
						CCLOG("itemInfo");
						PlayEffectSound(SFX_429);
						CPrize* prize = (CPrize* )cardsImage->getUserData();
						m_itemInfo->setInfo(prize);
						m_itemInfo->bindPos(cardsImage);
						m_itemInfo->changeForSign(prize);
						m_itemInfo->show();
					}
					break;
				case CTouchPressOff:
					{
						m_itemInfo->hide();
					}
					break;
				default:
					break;
				}
			}
			break;
		case 2:
			{
				if (m_monsterInfo == nullptr)
				{
					m_monsterInfo = CMonsterInfo::create();
					this->addChild(m_monsterInfo);
				}
				switch (state)
				{
				case CTouchPressOn:
					{
						//显示英雄信息弹框
						CCLOG("monsterInfo");
						PlayEffectSound(SFX_429);
						CHero hero;
						hero.heroid = prize->id;
						hero.thumb = prize->thumb;
						hero.quality = prize->quality;
						hero.iColor = prize->color;
						hero.level = 1;
						m_monsterInfo->setHero(&hero);
						m_monsterInfo->bindPos(cardsImage);
						m_monsterInfo->changeForSign(&hero);
						m_monsterInfo->show();
					}
					break;
				case CTouchPressOff:
					{
						m_monsterInfo->hide();
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			CCLOG("error Type is %d",iType);
			for (int i=0;i<m_showItemList.size();i++)
			{
				CCLOG("i = %d_\n_showItemList_%d_%d_%d",i,m_showItemList.at(i).type,m_showItemList.at(i).id,m_showItemList.at(i).num);
			}
			break;
		}
	}
}
void PairCardsActivityLayer::updateOneCards()
{
	for (int i = 0; i < m_cardsNum; i++)
	{
		if (m_cardsIndex -1 == i)
		{
			CCLOG("updateOneCards_%d",m_cardsIndex);
			CPrize& prize = m_showItemList.at(m_showItemList.size() -1);

			CImageView* image = (CImageView* )m_ImageList.at(i);
			image->setUserData(&prize);
			float cardSize_x = image->getContentSize().width;
			float cardSize_y = image->getContentSize().height;
			const ItemData* _itemData = DataCenter::sharedData()->getItemDesc()->getCfg(prize.id);
			if (_itemData)
			{
				if (_itemData->itemName.size()>0)
				{
					CLabel* _lab = (CLabel* )image->getChildByTag(10);
					_lab->setString(_itemData->itemName.c_str());
					_lab->setVisible(false);
				}
			}
			if (prize.thumb >= 0)
			{
				image->removeChildByTag(11);
				CCSprite* spItem = CCSprite::create(GetImageName(prize.type,prize.color,prize.thumb).c_str());
				if (!spItem)
					spItem = CCSprite::create("headImg/101.png");
				spItem->setPosition(ccp(cardSize_x/2,cardSize_y/2 + 10));
				spItem->setVisible(false);
				image->addChild(spItem,10,11);
				switch (prize.type)
				{
				case 1:
				case 2:
					{
						spItem->setScale(0.8f);
					}
					break;
				case 3:
				case 4:
				case 5:
				case 6:
					break;
				default:
					break;
				}

				if (prize.num > 1)
				{
					CCLabelAtlas* _numLab = (CCLabelAtlas* )image->getChildByTag(12);
					if (!_numLab)
					{
						CCLabelAtlas* itemNum = CCLabelAtlas::create("","label/no_02.png",9,15,46);
						itemNum->setAnchorPoint(ccp(1,0));
						itemNum->setScale(1.2f);
						itemNum->setPosition(ccp(cardSize_x -10,cardSize_y/4));
						image->addChild(itemNum,10,12);
						itemNum->setString(ToString(prize.num));
						itemNum->setVisible(false);
					}else
					{
						_numLab->setString(ToString(prize.num));
						_numLab->setVisible(false);
					}
				}else
				{
					CCLabelAtlas* _numLab = (CCLabelAtlas* )image->getChildByTag(12);
					if(_numLab)
					{
						image->removeChildByTag(12);
					}
				}
			}
			break;
		}
	}
}
void PairCardsActivityLayer::startAction()
{
	CCLOG("startAction");
	m_record_onPressTimes++;
	m_currFreeTimesLab->setString(CCString::createWithFormat("%d",m_gameNum - m_record_onPressTimes)->getCString());
	setCardsListTouchEnableTo(false);

	for (int i = 0;i<m_cardsNum;i++)
	{
		if (i == m_cardsIndex-1)
		{
			CImageView* image = (CImageView* )m_ImageList.at(i);

			image->runAction(CCSequence::create(CCScaleTo::create(m_pairTime,0,1),
				CCCallFuncN::create(this,callfuncN_selector(PairCardsActivityLayer::callFuncNOpenCardsAction)),
				nullptr));
		}
	}
}
void PairCardsActivityLayer::callFuncNOpenCardsAction(CCNode* node)
{
	//翻牌第二阶段
	CImageView* cardsImage = (CImageView* )node;
	cardsImage->setTexture(m_texture_on);
	setImageAllChildVisibleTo(cardsImage,true);

	if (m_openCardsNum <= 2)
	{
		cardsImage->runAction(CCSequence::create(
			CCScaleTo::create(m_pairTime,1,1),
			//CCDelayTime::create(0.5f),
			CCCallFuncN::create(this,callfuncN_selector(PairCardsActivityLayer::callFuncNOpenCardsOverAction)),
			nullptr));
	}else
	{
		cardsImage->runAction(CCSequence::create(
			CCScaleTo::create(m_pairTime,1,1),
			CCDelayTime::create(1.0f),
			CCCallFuncN::create(this,callfuncN_selector(PairCardsActivityLayer::callFuncNOpenCardsOverAction)),
			nullptr));
	}
	cardsImage->getChildByTag(9)->setVisible(true);
	setCardsListTouchEnableTo(true);
}
void PairCardsActivityLayer::callFuncNOpenCardsOverAction(CCNode* node)
{
	//翻牌动画结束阶段
	CImageView* cardsImage = (CImageView* )node;
	CPrize* prize = (CPrize* )cardsImage->getUserData();
	judegeHaveSameCardsAndAction(prize);
	
	m_index_click[m_openCardsNum] = m_cardsIndex;
	m_openCardsNum++;
	
	if (m_openCardsNum == m_gameNum &&m_gameTimes_Buy > 0)
	{
		//弹框显示是否花费勾玉购买一次
		CCString* str = CCString::createWithFormat(GETLANGSTR(2066),m_needGold_otherPairCardTimes);
		ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(PairCardsActivityLayer::isBuyOpenCardsNum));
	}
	if (isShowPrizePopItem || m_gameTimes_Buy == 0)
	{
		showPrizePopItem();
	}
}
void PairCardsActivityLayer::isBuyOpenCardsNum(CCObject* pSender)
{
	//花费勾玉购买次数弹框
	CButton* btn = (CButton* )pSender;
	if (btn->getTag() == PopTipConfirm)
	{
		if (!CheckGold(m_needGold_otherPairCardTimes))
		{
			CShowToBuyResource* show = CShowToBuyResource::create();
			show->showToBuyResourceByType(ShowBuyResourceGold);
			return;
		}else
		{
			m_gameNum++;
			m_gameTimes_Buy--;
			m_currFreeTimesLab->setString(CCString::createWithFormat("%d",m_gameNum - m_record_onPressTimes)->getCString());
			isShowPrizePopItem = false;
			
			UserData* data = DataCenter::sharedData()->getUser()->getUserData();
			data->setRoleGold(data->getRoleGold() - m_needGold_otherPairCardTimes);
			CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

			((CPopTip* )btn->getParent()->getParent())->onClose(nullptr);
		}
	}
	else if(btn->getTag() == PopTipCancel)
	{
		isShowPrizePopItem = true;
		((CPopTip* )btn->getParent()->getParent())->onClose(nullptr);
		showPrizePopItem();
	}
}

void PairCardsActivityLayer::showPrizePopItem()
{
	m_startGameBtn->setEnabled(false);
	m_closeBtn->setEnabled(false);

	if (isShowPrizePopItem || m_gameTimes_Buy == 0)
	{
		//弹框显示奖励
		if (m_getPrizeList.size() != 0)
		{
			CGetPrizeRes prizes;
			for (int i =0;i<m_getPrizeList.size();i++)
			{
				prizes.prizeList.push_back(m_getPrizeList.at(i));
			}
			CPopItem* popItem = CPopItem::create();
			LayerManager::instance()->push(popItem);
			popItem->popPrizeRes(&prizes);

			updateUserData();
		}
		//发送请求
		CardListReq* req = new CardListReq;
		req->set_result(true);
		GetTcpNet->sendData(req,CardListMsg);

	}
}
void PairCardsActivityLayer::openAllCards()
{
	//打开所有cards
	m_gameState = gameOver;
	for (int i = 0; i < m_ImageList.size(); i++)
	{
		CImageView* cardsImage = (CImageView* )m_ImageList.at(i);
		if (cardsImage->getTexture() == m_texture_off)
		{
			cardsImage->runAction(CCSequence::create(
				CCDelayTime::create(1.0f),
				CCScaleTo::create(0.2f,0,1),
				CCCallFuncN::create(this,callfuncN_selector(PairCardsActivityLayer::callFuncNShowPrizePopItemAction)),
				nullptr));
		}
	}
	//设置按钮为再来一次按钮
	m_redPoint->setVisible(false);
	CImageView* image = (CImageView* )m_ui->findWidgetById("StartGoldImage");
	image->setVisible(false);
	CLabel* gold_need = (CLabel* )m_ui->findWidgetById("StartNeed");
	gold_need->setVisible(false);
	CLabel* gold_free = (CLabel* )m_ui->findWidgetById("StartNeed_0");
	gold_free->setVisible(false);
	CLabel* start = (CLabel* )m_ui->findWidgetById("StartLab");
	start->setVisible(false);

	CLabel* again = (CLabel* )m_ui->findWidgetById("AgainLab");
	again->setVisible(true);

	CCLOG("m_gameState_%d",m_gameState);
}
void PairCardsActivityLayer::callFuncNShowPrizePopItemAction(CCNode* node)
{
	CImageView* cardsImage = (CImageView* )node;
	cardsImage->setTexture(m_texture_on);
	cardsImage->setTouchEnabled(true);
	setImageAllChildVisibleTo(cardsImage,true);
	cardsImage->runAction(CCSequence::create(
		CCScaleTo::create(0.2f,1,1),
		CCCallFunc::create(this,callfunc_selector(PairCardsActivityLayer::callFuncSetBtnToTure)),
		nullptr));
}
void PairCardsActivityLayer::callFuncSetBtnToTure()
{
	m_startGameBtn->setEnabled(true);
	m_closeBtn->setEnabled(true);
}

void PairCardsActivityLayer::initNotice()
{
	CCLOG("initNotice");
	CImageView* noticeimage = (CImageView* )m_ui->findWidgetById("Notice");
	noticeimage->setAnchorPoint(ccp(0,0));
	m_minPosY = 5;
	m_maxPosY = noticeimage->getContentSize().height/5*4 + 20;

	for (int i =0;i<m_noticeList.size();i++)
	{
		CLabel* lab = UICloneMgr::cloneLable(m_noticeLabel);
		lab->setVisible(false);
		noticeimage->addChild(lab,10,i);
		lab->setPosition(ccp(5, 0));
		
		CNotice* data = &m_noticeList.at(i);
		const ItemData* itemdate = nullptr;
		itemdate = DataCenter::sharedData()->getItemDesc()->getCfg(data->id);

		CCString* str1 = CCString::createWithFormat(GETLANGSTR(2059));
		CCString* str2 = CCString::createWithFormat("%s",data->player.c_str());
		CCString* str3 = CCString::createWithFormat(GETLANGSTR(2060));
		CCString* str4 = CCString::createWithFormat("%s",itemdate->itemName.c_str());
		CCString* str5 = CCString::createWithFormat("X");
		CCString* str6 = CCString::createWithFormat("%d",data->num);

		CCString* str = CCString::createWithFormat("%s %s %s %s %s %s",
			str1->getCString(),str2->getCString(),str3->getCString(),str4->getCString(),str5->getCString(),str6->getCString());
		lab->setString(str->getCString());

		ccColor3B ccc = ccc3(0,0,0);
		for (int i = 0;i<m_prizeItemList.size();i++)
		{
			if (data->id == m_prizeItemList.at(i).id)
			{
				switch (m_prizeItemList.at(i).color)
				{
				case 1:
					ccc = ccc3(255,255,255);//白
					break;
				case 2:
					ccc = ccc3(0,255,0);//绿
					break;
				case 3:
					ccc = ccc3(0,100,255);//蓝
					break;
				case 4:
					ccc = ccc3(150,0,255);//紫
					break;
				case 5:
					ccc = ccc3(255,150,0);//金黄
					break;
				case 10:
					ccc = ccc3(150,0,255);//紫
					break;
				case 11:
					ccc = ccc3(50,80,150);//淡蓝-书
					break;
				case 12:
					ccc = ccc3(150,0,255);//紫
					break;
				case 13:
					ccc = ccc3(150,0,255);//紫
					break;
				case 14:
					ccc = ccc3(150,0,255);//紫
					break;
				case 15:
					ccc = ccc3(150,0,255);//紫
					break;
				default:
					ccc = ccc3(255,150,0);//金黄
					break;
				}
			}
		}
		lab->setColor(ccc);

		m_noticeLab.push_back(lab);
	}

	if (!m_isUpdateNotice)
	{
		m_isUpdateNotice = true;
		m_currNoticeLab --;
		this->schedule(schedule_selector(PairCardsActivityLayer::updateNoticeLabelShow),1.5f,-1,1);
	}

}
void PairCardsActivityLayer::updateNoticeLabelShow(float delta)
{
	++m_currNoticeLab;
	if (m_currNoticeLab > m_noticeLab.size())
	{
		m_isUpdateNotice = false;
		this->unschedule(schedule_selector(PairCardsActivityLayer::updateNoticeLabelShow));
		return;
	}
	for (int i = 0; i <m_noticeLab.size(); i++)
	{
		if (m_currNoticeLab == i + 1)
		{
			CLabel* lab = m_noticeLab.at(i);
			lab->setPosition(ccp(5,m_minPosY));
			lab->setVisible(true);
			lab->runAction(CCSequence::create(
				CCMoveTo::create(10.0f,ccp(5,m_maxPosY)),
				CCHide::create(),
				nullptr));
		}
	}
}

void PairCardsActivityLayer::judegeHaveSameCardsAndAction(CPrize* item)
{
	//判断是否有相同
	int iNum = 0;
	CImageView* image1 = nullptr;
	CImageView* image2 = nullptr;
	//当前点击卡牌Image
	for (int i= 0;i<m_ImageList.size();i++)
	{
		if (m_cardsIndex -1 == i)
		{
			image1 = (CImageView* )m_ImageList.at(i);
		}
	}
	//判断是否有相同卡牌
	for (int i=0;i<m_showItemList.size() -1;i++)
	{
		CPrize _prize = m_showItemList.at(i);
		if (item->type == _prize.type && item->id == _prize.id &&item->num == _prize.num)
		{
			for (int index =0;index<50;index++)
			{
				if (i == index)
				{
					image2 = (CImageView* )m_ImageList.at(m_index_click[i] - 1);
					image1->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(
						CCScaleTo::create(0.3f,1.05f),CCScaleTo::create(0.3f,1.0f))));
					image2->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(
						CCScaleTo::create(0.3f,1.05f),CCScaleTo::create(0.3f,1.0f))));
					m_getPrizeList.push_back(*item);
				}
			}
		}
	}
}
void PairCardsActivityLayer::setCardsListTouchEnableTo(bool enable)
{
	for (int i =0;i<m_ImageList.size();i++)
	{
		if (enable)
			m_ImageList.at(i)->setTouchEnabled(true);
		else
			m_ImageList.at(i)->setTouchEnabled(false);
	}
}
void PairCardsActivityLayer::setImageAllChildVisibleTo(CImageView* image,bool visible)
{
	CImageView* cards = (CImageView* )image;

	if (image->getChildByTag(10)){
		image->getChildByTag(10)->setVisible(visible);
	}
	if (image->getChildByTag(11)){
		image->getChildByTag(11)->setVisible(visible);
	}
	if(image->getChildByTag(12)){
		image->getChildByTag(12)->setVisible(visible);
	}
	if (image->getChildByTag(13)){
		image->getChildByTag(13)->setVisible(visible);
	}
}
void PairCardsActivityLayer::updateUserData()
{
	UserData* user = DataCenter::sharedData()->getUser()->getUserData();
	for (int i =0;i<m_getPrizeList.size();i++)
	{
		const CPrize& prize = m_getPrizeList.at(i);
		if (prize.type == 3)
			user->setCoin(user->getCoin() + prize.num);
		if(prize.type == 4)
			user->setRoleGold(user->getRoleGold()+ prize.num);
		if(prize.type == 5)
			user->setRoleFood(user->getRoleFood() +prize.num);
	}
	CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
}