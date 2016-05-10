#include "FilterItem.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"

CFilterItem::CFilterItem():m_ui(nullptr), m_noEquip(true),m_yesEquip(true)
{

}


CFilterItem::~CFilterItem()
{

}

bool CFilterItem::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		MaskLayer* lay = MaskLayer::create("CFilterItemMaskLayer");
		lay->setContentSize(winSize);
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("FilterItem.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);
		return true;
	}
	return false;

}

void CFilterItem::onEnter()
{
	BaseLayer::onEnter();

	for (int i=1; i<=7; ++i)
	{
		CCheckBox *checkBtn = (CCheckBox*)(m_ui->getChildByTag(i));
		checkBtn->setOnCheckListener(this,ccw_check_selector(CFilterItem::onCheckFilter));
	}
}

void CFilterItem::onExit()
{
	BaseLayer::onExit();
}

void CFilterItem::setFilterType(int type, bool isCheck)
{
	if (isCheck)
	{
		int i=0;
		for (; i<m_filterType.size();i++)
		{
			if (m_filterType.at(i)==type)
			{
				break;
			}
		}
		if (i==m_filterType.size())
		{
			m_filterType.push_back(type);
		}
	}
	else
	{
		vector<int>::iterator iter = m_filterType.begin();
		for (; iter!=m_filterType.end();iter++)
		{
			if (*iter==type)
			{
				m_filterType.erase(iter);
				break;
			}
		}
	}
}

void CFilterItem::setFilterType(const vector<int>& typeVec)
{
	m_filterType = typeVec;

	for (int i=1; i<=7; ++i)
	{
		CCheckBox *checkBtn = (CCheckBox*)(m_ui->getChildByTag(i));
		checkBtn->setChecked(false);
	}

	for (int i=0; i<typeVec.size(); ++i)
	{
		CCheckBox *checkBtn = (CCheckBox*)(m_ui->getChildByTag(typeVec.at(i)));
		checkBtn->setChecked(true);
	}
}

void CFilterItem::onCheckFilter(CCObject *pSender, bool bChecked)
{
	PlayEffectSound(SFX_429);

	CCheckBox *checkBox = (CCheckBox*)pSender;
	int tag = checkBox->getTag();
	//1-5 为道具类型//6:未装备//7:已装备
	if (tag<=5)
	{
		setFilterType(tag,bChecked);
	}
	else if (tag==6)
	{
		m_noEquip = bChecked;
		setFilterType(tag,bChecked);
	}
	else
	{
		m_yesEquip = bChecked;
		setFilterType(tag,bChecked);
	}

	requestUpdate();
}

bool CFilterItem::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	if( !res )
	{
		CCSprite *bgView = (CCSprite*)(m_ui->findWidgetById("bgView"));

		CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

		if( bgView->boundingBox().containsPoint(pTouchPos))
		{
			res = true;
		}
		else
		{
			PlayEffectSound(SFX_Ensure);

			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
	}
	return true;
}

void CFilterItem::requestUpdate()
{
	int type =0;

	if (m_noEquip&&!m_yesEquip)
	{
		type=0;
	}
	else if (!m_noEquip&&m_yesEquip)
	{
		type=1;
	}
	else
	{
		type = 2;
	}
	CSceneManager::sharedSceneManager()->PostMessageA("updateFilter",type, nullptr, &m_filterType);
}



