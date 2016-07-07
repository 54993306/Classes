#include "PvpRecord.h"
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
#include "scene/CPopTip.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "common/CommonFunction.h"



CPvpRecord::CPvpRecord():m_cell(nullptr)
{

}


bool CPvpRecord::init()
{
	if (BaseLayer::init())
	{
		m_ui = LoadComponent("PvpRecord.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);	

		setIsShowBlack(false);

		return true;
	}
	return false;

}

void CPvpRecord::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CPvpRecord::onToggle(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

}

void CPvpRecord::onEnter()
{
	BaseLayer::onEnter();
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
    m_ui->removeChild(m_cell);

	
	m_heroLay = (CLayout*)(m_ui->findWidgetById("hero"));
	m_heroLay->retain();
	m_ui->removeChild(m_heroLay);

	m_tableView = (CTableView*)m_ui->findWidgetById("scroll");
	m_tableView->setDirection(eScrollViewDirectionVertical);	
	m_tableView->setCountOfCell(0);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setAnchorPoint(ccp(0,0));
	m_tableView->setAutoRelocate(true);
	m_tableView->setDeaccelerateable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CPvpRecord::tableviewDataSource));
	m_tableView->setCountOfCell(5);
	m_tableView->reloadData();
	m_tableSize = m_tableView->getContentSize();


	GetTcpNet->registerMsgHandler(RankReqMsg,this, CMsgHandler_selector(CPvpRecord::ProcessMsg));
}

CCObject* CPvpRecord::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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



void CPvpRecord::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
    CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_VIPINFO,this);
	BaseLayer::onExit();

} 

void CPvpRecord::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	while (lay->getChildrenCount()>0)
	{
		CCNode *child = (CCNode*)lay->getChildren()->objectAtIndex(0);
		child->removeFromParent();
		pCell->addChild(child);
	}
	for (int i = 0; i < 5; i++)
	{
		CLayout *heroLay = UICloneMgr::cloneLayout(m_heroLay);
		while (heroLay->getChildrenCount()>0)
		{
			CCNode *node = (CCNode *) heroLay->getChildren()->objectAtIndex(0);
			node->removeFromParent();
			pCell->addChild(node);
			node->setPositionX(node->getPositionX()+50*i);
		}
	}

	for (int i = 0; i < 5; i++)
	{
		CLayout *heroLay = UICloneMgr::cloneLayout(m_heroLay);
		while (heroLay->getChildrenCount()>0)
		{
			CCNode *node = (CCNode *) heroLay->getChildren()->objectAtIndex(0);
			node->removeFromParent();
			pCell->addChild(node);
			node->setPositionX(node->getPositionX()+50*i+445);
		}
	}
}



CPvpRecord::~CPvpRecord()
{
	CC_SAFE_RELEASE(m_cell);
	CC_SAFE_RELEASE(m_heroLay);
}


void CPvpRecord::ProcessMsg(int type, google::protobuf::Message *msg)
{
	setVisible(true);
}


void CPvpRecord::setOptionType(int type)
{
}



