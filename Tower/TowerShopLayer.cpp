#include "TowerShopLayer.h"
#include "tools/UICloneMgr.h"

CTowerShopLayer::CTowerShopLayer(void)
{
}
CTowerShopLayer::~CTowerShopLayer(void)
{
}
bool CTowerShopLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* _maskLayer = MaskLayer::create("TowerShopMaskLayer");
		_maskLayer->setContentSize(CCDirector::sharedDirector()->getWinSize());
		LayerManager::instance()->push(_maskLayer);

		m_ui = LoadComponent("towerShop.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}
	return false;
}
void CTowerShopLayer::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout* )m_ui->findWidgetById("Cell");
	m_cell->retain();
	m_ui->removeChild(m_cell);
	//m_cell->removeFromParent();

	m_gridView = (CGridPageView*)m_ui->findWidgetById("scroll");
	m_gridView->setCountOfCell(9);//cell个数
	m_gridView->setDirection(eScrollViewDirectionVertical);//设置方向
	m_gridView->setSizeOfCell(m_cell->getContentSize());//cell大小
	m_gridView->setAnchorPoint(ccp(0,0));//锚点
	m_gridView->setRows(2);
	m_gridView->setColumns(3);//列数
	m_gridView->setAutoRelocate(true);
	m_gridView->setAutoRelocateSpeed(500.0f);
	m_gridView->setDeaccelerateable(false);//重力感应
	m_gridView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CTowerShopLayer::gridviewDataSource));
	m_gridView->reloadData();


}
void CTowerShopLayer::onExit()
{
	BaseLayer::onExit();
}

CCObject* CTowerShopLayer::gridviewDataSource(CCObject* pConvertCell,unsigned int uIdx)
{
	CGridPageViewCell* _cell = (CGridPageViewCell* )pConvertCell;
	if (!_cell)
	{
		_cell = new CGridPageViewCell;
		_cell->autorelease();
		_cell->setTag(uIdx);
	}else
	{
		_cell->removeAllChildren();	
	}
	addGridCell(_cell, uIdx);
	return _cell;
}
void CTowerShopLayer::addGridCell(CGridPageViewCell* pCell, unsigned int uIdx)
{
	CLayout* lay = UICloneMgr::cloneLayout(m_cell);
	for (int i = 1 ;i<= 8 ;i++)
	{
		CCNode* _child = (CCNode* )pCell->getChildByTag(i);
		_child->setVisible(true);
		lay->removeChild(_child);
		pCell->addChild(_child);

		switch (i)
		{
		case 1:
			break;
		case 2:
			break;
		case 3:
			{
				//框
			}
			break;
		case 4:
			{
				//name
			}
			break;
		case 5:
			{
				//price
			}
			break;
		case 6:
			{
				//购买需求图片
			}
			break;
		case 7:
			break;
		case 8:
			break;
		default:
			break;
		}
	}
}