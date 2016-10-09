/******************************************************
*文件名称:	HandBookHero.cpp
*编写日期:	2016-8-23-16:03
*编写作者:	YPF
*功能描述:	英雄图鉴
*******************************************************/

#include "HandBookHero.h"
#include "model/DataDefine.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "tools/UICloneMgr.h"
#include "common/CommonFunction.h"
#include "handBook/HandBookEffectTip.h"
#include "HandBookControl.h"
#include "common/ShaderDataHelper.h"


CHandBookHeroLayer::CHandBookHeroLayer():
m_ui(nullptr), m_pTableView(nullptr), m_pCell(nullptr),m_pCellBlank(nullptr)
{

}

CHandBookHeroLayer::~CHandBookHeroLayer()
{
	CC_SAFE_RELEASE(m_pCell);
	CC_SAFE_RELEASE(m_pCellBlank);
}

bool CHandBookHeroLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//内容
		m_ui = LoadComponent("handBookHero.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		setIsShowBlack(false);

		//保存Cell
		m_pCell = (CLayout*)findWidgetById("Cell");
		m_pCell->retain();
		m_pCell->removeFromParent();

		m_pCellBlank = (CLayout*)m_ui->findWidgetById("CellBg");
		m_pCellBlank->retain();
		m_pCellBlank->removeFromParent();

		//获取空的滑动列表Scroll
		m_pTableView = (CTableView*)m_ui->findWidgetById("scroll");
		m_pTableView->setDirection(eScrollViewDirectionVertical);
		m_pTableView->setSizeOfCell(m_pCellBlank->getContentSize());
		m_pTableView->setCountOfCell(0);
		m_pTableView->setBounceable(true);
		m_pTableView->setDataSourceAdapter(this, ccw_datasource_adapter_selector(CHandBookHeroLayer::tableviewDataSource));


		setVisible(false);

		return true;
	}

	return false;
}

void CHandBookHeroLayer::onEnter()
{
	BaseLayer::onEnter();

	//绑定活动列表回调
	GetTcpNet->registerMsgHandler(AtlasRequestMsg, this, CMsgHandler_selector(CHandBookHeroLayer::processNetMsg));
}

void CHandBookHeroLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	//GetTcpNet->unRegisterAllMsgHandler(this);
}

void CHandBookHeroLayer::processNetMsg( int type, google::protobuf::Message *msg )
{
	if(!isVisible())
	{
		this->setVisible(true);
		LayerManager::instance()->getLayer("CHandBookPanel")->setVisible(true);
	}

	AtlasResponse *pRes = dynamic_cast<AtlasResponse *>(msg);

	if (pRes)
	{
		//数据存储
		m_AtlasResponse = *pRes;

		//图鉴列表
		updateUI( );
		

	}
}

void CHandBookHeroLayer::updateUI( )
{
	//图鉴收集比例
	int iCurrentN = CompleteInHandBook(&m_AtlasResponse);
	int iMax = m_AtlasResponse.atlas_list_size();
	CLabel *pText = (CLabel *)m_ui->findWidgetById("text");
	pText->setString(CCString::createWithFormat("%d/%d", iCurrentN, iMax)->getCString());


	m_pTableView->setCountOfCell(m_AtlasResponse.atlas_list_size());
	m_pTableView->reloadData();

}

CCObject* CHandBookHeroLayer::tableviewDataSource( CCObject* pConvertCell, unsigned int uIdx )
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

void CHandBookHeroLayer::addTableCell( unsigned int uIdx, CTableViewCell * pCell )
{
	//先克隆外框条
	CLayout *pLay = UICloneMgr::cloneLayout(m_pCellBlank);

	//对应数据
	AtlasIns *pAtlasIns = m_AtlasResponse.mutable_atlas_list()->Mutable(uIdx);

	//赋值
	for (unsigned int i=1; i<=10; i++)
	{
		CCNode *child = pLay->getChildByTag(i);
		pLay->removeChild(child);
		child->setTag(i);
		pCell->addChild(child);
		pCell->setTag(uIdx);
	}

	//1-活动底板
	{
		CImageView* pBoard = dynamic_cast<CImageView*>(pCell->getChildByTag(1));
		pBoard->setTouchEnabled(true);
		pBoard->setOnClickListener(this, ccw_click_selector(CHandBookHeroLayer::onClickBlank));

		if ( uIdx==m_iSelectIndex )
		{
			pBoard->setTexture(CCTextureCache::sharedTextureCache()->addImage("handBook/bookbg_2.png"));
		}
		else
		{
			pBoard->setTexture(CCTextureCache::sharedTextureCache()->addImage("handBook/bookbg_1.png"));
		}
	}

	//2-组合名字
	{
		CLabel* pLabel = dynamic_cast<CLabel*>(pCell->getChildByTag(2));
	}

	//3-//组合描述
	{
		CLabel* pLabel = dynamic_cast<CLabel*>(pCell->getChildByTag(3));
	}

	//4-礼品背景
	//5-礼品
	//10-文字
	{
		CImageView* pGiftBg = dynamic_cast<CImageView*>(pCell->getChildByTag(4));
		CImageView* pGift = dynamic_cast<CImageView*>(pCell->getChildByTag(5));
		CLabel* pLabel = dynamic_cast<CLabel*>(pCell->getChildByTag(10));

		pLabel->setVisible(false);
		pGift->stopAllActions();

		//奖励领取标志 0未领取(不可领取)，1未领取(可领取状态)，2 已领取
		switch (pAtlasIns->get_prize())
		{
		case 0:
			{
				pGiftBg->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
				pGift->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));

			}break;
		case 1:
			{
				pGiftBg->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
				pGift->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
				ResetAnchorPointAndKeepSamePos(pGift, CCPointCenter);
				pGift->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.3f, 0.75f), CCScaleTo::create(0.3f, 0.7f))));

			}break;
		case 2:
			{
				pGiftBg->setVisible(false);
				pGift->setVisible(false);
				pLabel->setVisible(true);
			}break;
		default:
			break;
		}
	}


	//7-进度条
	//8-进度数字
	//9-进度图标
	{
		CProgressBar* pProgress = dynamic_cast<CProgressBar*>(pCell->getChildByTag(7));

		CLabel* pLabel = dynamic_cast<CLabel*>(pCell->getChildByTag(8));
		pLabel->setTouchEnabled(true);
		pLabel->setOnClickListener(this, ccw_click_selector(CHandBookHeroLayer::onClickScoreIcon));

		CImageView* pIcon = dynamic_cast<CImageView*>(pCell->getChildByTag(9));
		pIcon->setTouchEnabled(true);
		pIcon->setOnClickListener(this, ccw_click_selector(CHandBookHeroLayer::onClickScoreIcon));
	}


	//收集列表
	CCPoint pPos(80, 54);
	int iGapX = 94;
	initListCellIcon( pAtlasIns, pPos, iGapX, pCell );

}


void CHandBookHeroLayer::initListCellIcon( AtlasIns *pAtlasIns, CCPoint startPos, int iGapX, CTableViewCell *pCell )
{
	int iSize = pAtlasIns->data_list_size();
	for(int i=0; i<iSize; i++)
	{
		//克隆元素
		CLayout *pIconCell = UICloneMgr::cloneLayout(m_pCell);
		pIconCell->setPosition(ccp(startPos.x+iGapX*i, startPos.y));
		pCell->addChild(pIconCell, 10);

		initCellIconByData( pAtlasIns->mutable_data_list()->Mutable(i),  pIconCell );
	}

}

void CHandBookHeroLayer::initCellIconByData( AData *pAData, CLayout *pCell )
{
	//message AData{
	//	optional int32 type = 1;		//类型 1英雄，2装备，3怪物
	//	optional int32 thumb = 2;		
	//	optional int32 color = 3;		//边框颜色
	//	optional int32 process = 4;		//英雄、装备代表进阶数，怪物代表击杀数
	//	optional int32 max_process = 5;
	//	optional int32 param = 6;
	//}


	Prize *prize = new Prize; 
	//获取数据
	int iType = 1;
	if ( pAData->type()==1 || pAData->type()==3 )
	{
		iType = 2;
	}
	int iColor = pAData->color();
	int iIconId = pAData->thumb();
	int iRank = pAData->process();

	string str = GetImageName(iType, iColor, iIconId);

	//图标
	CImageView* pBgRect = (CImageView *)pCell->findWidgetById("headBg");
	CImageView *itemSpr = CImageView::create(str.c_str());
	if (!itemSpr)
	{
		itemSpr = CImageView::create("prop/32003.png");
		CCLOG("[ ERROR ] CPopItem::addTableCell Lost Image = %d");
	}
	pBgRect->addChild(itemSpr);
	NodeFillParent(itemSpr);

	//品质+事件
	CImageView *mask= (CImageView *)pCell->findWidgetById("head");
	mask->setTexture(setItemQualityTexture(iColor));
	mask->setUserData(pAData);
	mask->setTag(iType);
	mask->setTouchEnabled(true);
	mask->setOnClickListener(this, ccw_click_selector(CHandBookHeroLayer::onShowRankUp));

	//进阶
	for ( int i=0; i<5; i++)
	{
		CImageView *pStar = (CImageView *)pCell->getChildByTag(i+3);
		//阶级到，点亮
		if (i<iRank)
		{
			pStar->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
		}
		else
		{
			pStar->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
		}
	}
}


void CHandBookHeroLayer::onClickBlank( CCObject *pSender )
{
	//TODO
	CHandBookEffectTip *pTip = CHandBookEffectTip::create();
	LayerManager::instance()->push(pTip);
}

void CHandBookHeroLayer::onClickScoreIcon( CCObject *pSender )
{
	//TODO
}

void CHandBookHeroLayer::onShowWhereGet( CCObject *pSender )
{
	//TODO
}

void CHandBookHeroLayer::onShowRankUp( CCObject *pSender )
{
	//TODO
}
