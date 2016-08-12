#include "ActivityCollect.h"
#include "tools/UICloneMgr.h"
#include "common/ShaderDataHelper.h"
#include "Resources.h"
#include "common/CGameSound.h"
#include "common/CommonFunction.h"
#include "tollgate/MonsterInfo.h"
#include "tollgate/ItemInfo.h"

CActivityCollectLayer::CActivityCollectLayer():
m_ui(nullptr), m_cellIcon(nullptr), m_cellBlank(nullptr), m_tableView(nullptr),m_pData(nullptr),m_iActId(0)
,m_pMonsterInfo(nullptr), m_pItemInfo(nullptr),m_pCurrentExlist(nullptr),m_iExchangeType(ExchangeTypeNormal)
{

}

CActivityCollectLayer::~CActivityCollectLayer()
{
	CC_SAFE_RELEASE(m_cellIcon);
	CC_SAFE_RELEASE(m_cellBlank);
}

bool CActivityCollectLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//内容
		m_ui = LoadComponent("activityCollect.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setIsShowBlack(false);

		initTableView();

		return true;
	}

	return false;
}


void CActivityCollectLayer::initTableView()
{
	//获取编辑器中设定好的Cell
	m_cellIcon = (CLayout*)m_ui->findWidgetById("Cell_icon");
	m_cellIcon->retain();
	m_ui->removeChild(m_cellIcon);

	m_cellBlank = (CLayout*)m_ui->findWidgetById("Cell_blank");
	m_cellBlank->retain();
	m_ui->removeChild(m_cellBlank);

	//获取空的滑动列表Scroll
	m_tableView = (CTableView*)m_ui->findWidgetById("scroll");
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cellBlank->getContentSize());
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(true);
	m_tableView->setDataSourceAdapter(this, ccw_datasource_adapter_selector(CActivityCollectLayer::tableviewDataSource));

}

void CActivityCollectLayer::onEnter()
{
	BaseLayer::onEnter();
}

void CActivityCollectLayer::onExit()
{
	BaseLayer::onExit();
}


void CActivityCollectLayer::initBaseData( google::protobuf::RepeatedPtrField< ::protos::ExList >& data, int iActId )
{
	m_pData = &data;
	m_iActId =iActId;

	m_tableView->setCountOfCell(m_pData->size());
	m_tableView->reloadData();
}

CCObject* CActivityCollectLayer::tableviewDataSource( CCObject* pConvertCell, unsigned int uIdx )
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

void CActivityCollectLayer::addTableCell( unsigned int uIdx, CTableViewCell * pCell )
{
	//先克隆外框条
	CLayout *lay = UICloneMgr::cloneLayout(m_cellBlank);

	//获取数据
	ExList* pInfo = m_pData->Mutable(uIdx);

	//添加兑换规则
	CCPoint pPos(80, 54);
	int iGapX = 94;
	bool isNotEnough = false;
	switch (m_iExchangeType)
	{
	case CActivityCollectLayer::ExchangeTypeNormal:
		{
			//收集品列表
			isNotEnough = initListCellIcon(pInfo->mutable_oitems(), pPos, iGapX, pCell, true);

			int iOItemSize = pInfo->oitems_size();
			pPos = pPos+ccp(pInfo->oitems_size()*iGapX, 0);


			//箭头
			CCSprite* pSprite = CCSprite::create("activity/jiantou.png");
			//pSprite->setScale(0.8f);
			pSprite->setPosition(ccp(pPos.x, pPos.y));
			pCell->addChild(pSprite, 10);

			pPos = pPos+ccp(iGapX, 0);
		}
		break;
	case CActivityCollectLayer::ExchangeTypeByLevel:
		{
			pPos = pPos+ccp(180, 0);
		}
		break;
	default:
		break;
	}

	//兑换列表
	initListCellIcon(pInfo->mutable_titems(), pPos, iGapX, pCell, false);


	for (unsigned int i=1; i<=6; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		child->setTag(i);
		pCell->addChild(child);
		pCell->setTag(uIdx);

		switch (i)
		{
			//活动底板
		case 1:
			{
				CImageViewScale9* pBoard = dynamic_cast<CImageViewScale9*>(child);
				if(!pBoard)
				{
					return;
				}
				pBoard->setTouchEnabled(true);
				pBoard->setOnClickListener(this, ccw_click_selector(CActivityCollectLayer::collectCellBlankClick));
			}
			break;
			//选中状态-高亮底板
		case 2:
			{
				child->setVisible(uIdx==m_index_selected);
			}
			break;
			//领取按钮
		case 3:
			{
				CButton* pButton = dynamic_cast<CButton*>(child);
				if(!pButton)
				{
					return;
				}
				pButton->setOnClickListener(this, ccw_click_selector(CActivityCollectLayer::collectCellClick));
			}
			break;
		case 4:
			{
				CLabel* pLabel = dynamic_cast<CLabel*>(child);
				if(!pLabel)
				{
					return;
				}
			}
			break;
			//兑换限定
		case 5:
			{
				int iCurrent = pInfo->exlevel();
				CLabel* pLabel = dynamic_cast<CLabel*>(child);
				pLabel->setString(CCString::createWithFormat(GETLANGSTR(2045), iCurrent)->getCString());
			}
			break;
		default:
			break;
		}
	}


	CButton* pButton = dynamic_cast<CButton*>(pCell->getChildByTag(3));
	CLabel* pButtonLab = dynamic_cast<CLabel*>(pCell->getChildByTag(4));
	CImageView* pRight = dynamic_cast<CImageView*>(pCell->getChildByTag(6));
	CLabel* pLabel = dynamic_cast<CLabel*>(pCell->getChildByTag(5));

	switch (m_iExchangeType)
	{
	case CActivityCollectLayer::ExchangeTypeNormal:
		{
			//收集品列表
			CButton* pButton = dynamic_cast<CButton*>(pCell->getChildByTag(3));
			//可用
			if(pInfo->achieve() && !isNotEnough)
			{
				pButton->setEnabled(true);
				pButton->getDisabledImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
			}
			else
			{
				pButton->setEnabled(false);
				pButton->getDisabledImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			}

			//对勾隐藏
			pRight->setVisible(false);
			pLabel->setVisible(false);
		}
		break;
	case CActivityCollectLayer::ExchangeTypeByLevel:
		{
			pLabel->setVisible(true);

			//等级是否达到
			UserData *pUserData = DataCenter::sharedData()->getUser()->getUserData();
			bool isLevelOk = pUserData->getLevel()>=pInfo->exlevel();
			//等级到达，却标记不可领，说明已经领取了
			if(!pInfo->achieve() && isLevelOk)
			{
				//对勾显示，按钮隐藏
				pRight->setVisible(true);
				pButton->setVisible(false);
				pButtonLab->setVisible(false);
			}
			//等级到达，标记可以领，待领取
			else if( pInfo->achieve() && isLevelOk )
			{
				//对勾隐藏，按钮显示
				pRight->setVisible(false);
				pButton->setVisible(true);
				pButtonLab->setVisible(true);

				pButton->setEnabled(true);
				pButton->getDisabledImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
			}
			//等级未到达，标记不可领，不能领
			else if ( !pInfo->achieve() && !isLevelOk  )
			{
				//对勾隐藏，按钮显示
				pRight->setVisible(false);
				pButton->setVisible(true);
				pButtonLab->setVisible(true);

				pButton->setEnabled(false);
				pButton->getDisabledImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			}
		}
		break;
	default:
		break;
	}
}



bool CActivityCollectLayer::initListCellIcon(google::protobuf::RepeatedPtrField<Prize>* prizeList, CCPoint pStartPos, int iGapX , CCNode* pParent, bool bShowMaxData)
{
	bool bAnswer =false;

	for(int i=0; i<prizeList->size(); i++)
	{
		//克隆元素
		CLayout *pIconCell = UICloneMgr::cloneLayout(m_cellIcon);
		pParent->addChild(pIconCell, 10);
		pIconCell->setPosition(ccp(pStartPos.x+iGapX*i, pStartPos.y));

		if(initCellIconByData(pIconCell, prizeList->Mutable(i), bShowMaxData))
		{
			bAnswer = true;
		}
	}

	return bAnswer;
}


bool CActivityCollectLayer::initCellIconByData( CLayout *pCell, Prize* prize, bool bShowMaxData )
{
	bool bAnswer = false;

	//获取数据
	int iType = prize->type();
	int iColor = prize->color();
	int iIconId = prize->thumb();
	int iNum = prize->num();
	int iNumAll = prize->param();
	int iQuality = prize->quality();

	string str = GetImageName(iType, iColor, iIconId);

	//图标
	CImageView* pBgRect = (CImageView *)pCell->getChildByTag(1);
	CImageView *itemSpr = CImageView::create(str.c_str());
	if (!itemSpr)
	{
		itemSpr = CImageView::create("prop/32003.png");
		CCLOG("[ ERROR ] CPopItem::addTableCell Lost Image = %d");
	}
	pBgRect->addChild(itemSpr);
	NodeFillParent(itemSpr);

	//品质+事件
	CImageView *mask= (CImageView *)pCell->findWidgetById("prize");
	SmartSetRectPrizeColor(mask, nullptr, prize);
	mask->setUserData(prize);
	mask->setTag(iType);
	mask->setTouchEnabled(true);
	mask->setOnPressListener(this, ccw_press_selector(CActivityCollectLayer::onPress));

	//数量
	CLabel *pLabelNum = (CLabel*)pCell->findWidgetById("num");
	pLabelNum->setString("");
	if (iNum>0)
	{
		pLabelNum->setVisible(false);
		pCell->removeChild(pLabelNum);
		CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
		haveNum->setAnchorPoint(ccp(0.5f, 0.5f));
		haveNum->setPosition(pLabelNum->getPosition());
		pCell->addChild(haveNum, 99);
		if(!bShowMaxData)
		{
			if(iNum>=1 && iQuality<=0)
			{
				haveNum->setString(CCString::createWithFormat("%d", iNum)->getCString());
			}
		}
		else
		{
			haveNum->setString(CCString::createWithFormat("%d:%d", iNumAll, iNum)->getCString());
			if(iNum>iNumAll)
			{
				//haveNum->setColor(ccc3(255, 0, 0));
				bAnswer = true;
			}
		}
	}
	else
	{
		pLabelNum->setVisible(false);
	}

	//星星
	if(iQuality>0)
	{
		//添加星星
		CPrize cprize;
		cprize.read(*prize);
		CLayout* pStarLayout = SmartGetStarLayout(&cprize);
		mask->addChild(pStarLayout);
	}

	return bAnswer;
}


void CActivityCollectLayer::updateTableSelected( CTableView* pTable, int iSelected )
{
	CCObject* pObj;
	CCARRAY_FOREACH(pTable->getContainer()->getChildren(), pObj)
	{
		CCNode* pNode = (CCNode*)pObj;
		CImageView* pBoard = (CImageView*)pNode->getChildByTag(2);
		if(pNode->getTag()==iSelected)
		{
			pBoard->setVisible(true);
		}
		else
		{
			pBoard->setVisible(false);
		}
	}
}

void CActivityCollectLayer::collectCellClick( CCObject* pObj )
{
	PlayEffectSound(SFX_Button);

	CCNode* pNode = (CCNode*)pObj;
	int iTag = pNode->getParent()->getTag();
	m_index_selected = iTag;

	ExList* pInfo = m_pData->Mutable(iTag);

	m_pCurrentExlist = pInfo;

	CActivityControl::getInstance()->askForBuyGift(m_iActId, pInfo->exid());	

}

void CActivityCollectLayer::collectCellBlankClick( CCObject* pObj )
{
	PlayEffectSound(SFX_Button);

	CImageView* pImageView = (CImageView*)pObj;
	int iCurrentSelectedIndex = pImageView->getParent()->getTag();
	//如果不是重复点击同一个
	if(m_index_selected != iCurrentSelectedIndex)
	{
		m_index_selected = iCurrentSelectedIndex;
		//更新table
		updateTableSelected(m_tableView, m_index_selected);
	}
}

void CActivityCollectLayer::onPress( CCObject* pSender, CTouchPressState iState )
{
	CImageView* pImage = (CImageView*)pSender;
	Prize* prize = (Prize*)pImage->getUserData();
	int iType = pImage->getTag();

	//1.物品 2.英雄
	switch (iType)
	{
	case 6:
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
			case cocos2d::cocoswidget::CTouchPressOn:
				{
					PlayEffectSound(SFX_429);
					CPrize cprize;
					cprize.read(*prize);
					if(prize->param()!=0)
					{
						cprize.num = prize->param();
					}

					m_pItemInfo->setInfo(&cprize);
					m_pItemInfo->bindPos(pImage);
					m_pItemInfo->changeForSign(&cprize);
					m_pItemInfo->show();
					CCLOG("show");
				}
				break;
			case cocos2d::cocoswidget::CTouchPressOff:
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
			case cocos2d::cocoswidget::CTouchPressOn:
				{
					PlayEffectSound(SFX_429);

					CHero pHero;
					pHero.heroid = prize->id();
					pHero.thumb = prize->thumb();
					pHero.quality = prize->quality();
					pHero.iColor = prize->color();
					pHero.level = 1;
					m_pMonsterInfo->setHero(&pHero);
					m_pMonsterInfo->bindPos(pImage);
					m_pMonsterInfo->show();
					CCLOG("show");
				}
				break;
			case cocos2d::cocoswidget::CTouchPressOff:
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

void CActivityCollectLayer::restData()
{
	if(m_pCurrentExlist!=nullptr)
	{
		switch (m_iExchangeType)
		{
		case CActivityCollectLayer::ExchangeTypeNormal:
			{
				//消耗了哪几种东西, 这几种东西分别剩余多少个
				std::map<int, int> mapConcume;

				int iSize = m_pCurrentExlist->oitems_size();
				for(int i=0; i<iSize; i++)
				{
					Prize* prize= m_pCurrentExlist->mutable_oitems(i);
					int iNeed = prize->num();
					int iAll = prize->param();
					int iRemain = iAll-iNeed;
					iRemain = iRemain<0?0:iRemain;
					mapConcume.insert(std::pair<int, int>(prize->id(), iRemain));
				}

				//拿到所有的列，然后重置物品数量
				for(int i=0; i<m_pData->size(); i++)
				{
					ExList* pInfo = m_pData->Mutable(i);
					int iSize = pInfo->oitems_size();
					for(int j=0; j<iSize; j++)
					{
						Prize* prize= pInfo->mutable_oitems(j);
						std::map<int, int>::iterator iterator = mapConcume.find(prize->id());
						if( iterator != mapConcume.end())
						{
							prize->set_param(iterator->second);
						}
					}
				}
			}
			break;
		case CActivityCollectLayer::ExchangeTypeByLevel:
			{
				m_pCurrentExlist->set_achieve(false);
			}
			break;
		default:
			break;
		}
		
	}

	m_pCurrentExlist = nullptr;
	ReloadTableViewWithSameOffset(m_tableView);
}