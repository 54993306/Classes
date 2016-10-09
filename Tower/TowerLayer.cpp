#include "TowerLayer.h"
#include "tools/UICloneMgr.h"
#include "tools/ShowTexttip.h"

#include "reward/WShopLayer.h"
#include "mainCity/MainCityControl.h"
#include "rank/RankLayer.h"
#include "netcontrol/CPlayerControl.h"
#include "Tower/TowerShopLayer.h"

#define TOWERBOTTOM 1

CTowerLayer::CTowerLayer(void):
	m_currPos(0),
	m_showLayerNum(0)
{
	m_shopLayer = nullptr;
}
CTowerLayer::~CTowerLayer(void)
{
	//CC_SAFE_RELEASE(m_cell);
}
bool CTowerLayer::init()
{
	CCLOG("init");
	if (BaseLayer::init())
	{
		MaskLayer* mask = MaskLayer::create("TowerMaskLayer");
		mask->setContentSize(CCDirector::sharedDirector()->getWinSize());
		mask->setVisible(false);
		LayerManager::instance()->push(mask);

		m_ui = LoadComponent("Tower.xaml");
		m_ui->setPosition(VCENTER);
		this->setVisible(true);
		this->addChild(m_ui);

		m_currPos = 10;
		m_showLayerNum = 100;

		//cocos studio
		////������
		//UILayer * _uilayer = UILayer::create();
		//addChild(_uilayer);
		////����json�ļ�-���ֲ�
		//Layout* _layout = dynamic_cast<Layout* >(GUIReader::shareReader()->widgetFromJsonFile("cocos_studio/NewUi_1.json"));
		//_layout->setTag(1);
		//_uilayer->addWidget(_layout);
		////��ȡ�ļ���Ϸ��
		//Layout* _panel = dynamic_cast<Layout* >(_layout->getChildByName("Panel_14"));
		////���ݱ���
		////����-��ť
		//UIButton* _closeBtn = dynamic_cast<UIButton* >(_panel->getChildByName("closeBtn"));
		//_closeBtn->addTouchEventListener(this,toucheventselector(CTowerLayer::onClose));

		return true;
	}
	return false;
}
void CTowerLayer::onEnter()
{
	CCLOG("onEnter");
	BaseLayer::onEnter();

	CButton* _close = (CButton* )m_ui->findWidgetById("closeBtn");
	_close->getSelectedImage()->setScale(1.1f);
	_close->setPosition(ccp(VLEFT+50,VTOP-50));
	_close->setOnClickListener(this,ccw_click_selector(CTowerLayer::onClose));
	CButton* _startBtn = (CButton* )m_ui->findWidgetById("startBtn");
	_startBtn->getSelectedImage()->setScale(1.1f);
	_startBtn->setOnClickListener(this,ccw_click_selector(CTowerLayer::btncallbackStartGame));

	for (int i = 1;i<eKernelMax;i++)
	{
		CButton* _kernel = (CButton* )m_ui->getChildByTag(i+10);
		_kernel->getSelectedImage()->setScale(1.1f);
		_kernel->setOnClickListener(this,ccw_click_selector(CTowerLayer::btncallbackFourKerbel));
	}
	//
	m_cell = (CLayout* )m_ui->findWidgetById("Cell");
	m_cell->retain();
	m_ui->removeChild(m_cell);
	//m_cell->removeFromParent();

	m_gridView = (CGridView* )m_ui->findWidgetById("Scroll");
	m_gridView->setDirection(eScrollViewDirectionVertical);	//���÷���
	m_gridView->setCountOfCell(m_showLayerNum + TOWERBOTTOM);		//cell����+����
	m_gridView->setSizeOfCell(m_cell->getContentSize());		//����cell��С***
	m_gridView->setColumns(1);											//������
	m_gridView->setAnchorPoint(ccp(0,0));								//ê��
	m_gridView->setDeaccelerateable(true);							//������Ӧ
	m_gridView->setBounceable(false);									//���÷���

	//m_gridView->setAutoRelocate(false);//relocateǨ��,���°���
	//m_gridView->setDragable(true);//�Ƿ�����϶�
	//////m_gridView->setContainerSize(_close->getContentSize());//���ݴ�С
	//////m_gridView->setContentSize(CCSize(119,800));//��ʾ����
	//////m_gridView->setSizeOfCell(m_cell->getChildByTag(1)->getContentSize());//
	//////m_gridView->setContentOffsetToTop();
	m_gridView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CTowerLayer::addCellForScrollView));
	m_gridView->reloadData();

	initData();
}
void CTowerLayer::onExit()
{
	BaseLayer::onExit();
}
void CTowerLayer::onClose(CCObject* obj)
{
		this->setVisible(false);
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
}
void CTowerLayer::onPress(CCObject* obj, CTouchPressState state)
{

}

void CTowerLayer::btncallbackStartGame(CCObject* obj)
{
	//�����������ݽ���

	//����ƫ��-λ��
	m_gridView->setContentOffset(ccp(0,(m_currPos - 1) * 97));

	//����һ��ʱ���ƶ�������
	//m_gridView->setContentOffsetToTopInDuration(1.0f);
	//�����ƶ�������-ʱ��-����
	//m_gridView->setContentOffsetToTopEaseIn(1.0f,2.0f);
	
	//����һ��ʱ��ƫ�Ƶ�ĳ��λ��-λ��-ʱ��
	//m_gridView->setContentOffsetInDuration(ccp(0,0),1.0f);//�ƶ����ײ�
	//�����ƶ���ĳ��λ��-λ��-ʱ��-����
	//m_gridView->setContentOffsetEaseIn(ccp(0,0),1.0f,2.0f);
}
void CTowerLayer::btncallbackFourKerbel(CCObject* obj)
{
	CCNode* node = (CCNode* )obj;
	int type = node->getTag() - 10;

	switch (type)
	{
	case eRule:
		{
			//���򵯿�
			CCString* str = CCString::createWithFormat("rule_rule_rule_rule");
			ShowPopTextTipandTitle(str->getCString());
		}
		break;
	case eRank:
		{
			//��ʷ��������
			CRankLayer* _rank = CRankLayer::create();
			LayerManager::instance()->push(_rank);
			CPlayerControl::getInstance().sendRank(2);
		}
		break;
	case eShop:
		{
			//�����̵굯��
			CTowerShopLayer* _shopLayer = CTowerShopLayer::create();
			LayerManager::instance()->push(_shopLayer);

			//if (!m_shopLayer)
			//{
			//	m_shopLayer = CWShopLayer::create();
			//	m_shopLayer->loadUiByType(3);
			//	m_shopLayer->setPosition(ccp(0,0));
			//	//m_shopLayer->setVisible(true);
			//	m_shopLayer->setTouchPriority(LayerManager::instance()->getPriority() -1);
			//	addChild(m_shopLayer);
			//	m_shopLayer->setVisible(true);
			//}
			//CMainCityControl::getInstance()->sendShopRequest(3);
			
		}
		break;
	case eQuick:
		{
			//��������
			//��ʾ����߲���
		}
		break;
	default:
		break;
	}
}
void CTowerLayer::btnRuleBombBox(CCObject* obj)
{

}

CCObject* CTowerLayer::addCellForScrollView(CCObject* cell,unsigned int uIdx)
{
	CGridViewCell* _cell = (CGridViewCell* )cell;
	if (!cell)
	{
		_cell = new CGridViewCell;
		_cell->autorelease();
		_cell->setTag(m_showLayerNum - uIdx);
	}else
	{
		_cell->removeAllChildren();
	}
	_cell->setVisible(true);
	CLayout* lay = UICloneMgr::cloneLayout(m_cell);
	for (int i =1;i<=6;i++)
	{
		CCNode* _child = lay->getChildByTag(i);
		_child->setVisible(true);
		lay->removeChild(_child);
		_cell->addChild(_child);
		
		switch (i)
		{
		case 1:
			{
				//����ͼ
				CImageView* _img = (CImageView* )_child;
				if (m_showLayerNum- uIdx != 0)
					_img->setVisible(true);
				else
					_img->setVisible(false);
			}
			break;
		case 2:
			{
				//������
				CLabel* lab = (CLabel* )_child;
				lab->setString(ToString(m_showLayerNum- uIdx));
			}
			break;
		case 3:
			{
				//����ͼƬ
				CCSprite* sp = (CCSprite* )_child;
				CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage("common/luo.png");
				sp->setTexture(texture);
				//SmartSetRectPrizeColor();
			}
			break;
		case 4:
			{
				//��ʶ��ǰλ��
				CImageView* userPos = (CImageView* )_child;
				//userPos->setVisible(true);
				if (m_showLayerNum - uIdx == m_currPos)
					userPos->setVisible(true);
				else
					userPos->setVisible(false);
			}
			break;
		case 5:
			{
				//����λ��
				CImageView* _friend = (CImageView* )_child;
				_friend->setVisible(false);
			}
			break;
		case 6:
			{
				//����
				CImageView* _img = (CImageView* )_child;
				if (m_showLayerNum - uIdx == 0)
					_img->setVisible(true);
				else
					_img->setVisible(false);
			}
			break;
		default:
			break;
		}
	}
	return _cell;
}
void CTowerLayer::initData()
{
	//m_gridView->setContentOffset(ccp(0,200));
	CImageView* _img = (CImageView* )m_ui->findWidgetById("maskTcouhscale9");
	//_img->setTouchEnabled(true);
}
