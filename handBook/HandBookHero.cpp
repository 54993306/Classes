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


CHandBookHeroLayer::CHandBookHeroLayer():
m_ui(nullptr), m_pScroll(nullptr), m_pCell(nullptr)
{

}

CHandBookHeroLayer::~CHandBookHeroLayer()
{
	CC_SAFE_RELEASE(m_pCell);
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

		setVisible(false);

		return true;
	}

	return false;
}

void CHandBookHeroLayer::onEnter()
{
	BaseLayer::onEnter();

	//绑定活动列表回调
	GetTcpNet->registerMsgHandler(BossPrizesMsg, this, CMsgHandler_selector(CHandBookHeroLayer::processNetMsg));
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
}