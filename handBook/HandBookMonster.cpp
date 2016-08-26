/******************************************************
*文件名称:	HandBookMonster.cpp
*编写日期:	2016-8-23-16:03
*编写作者:	YPF
*功能描述:	英雄图鉴
*******************************************************/

#include "HandBookMonster.h"
#include "model/DataDefine.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "tools/UICloneMgr.h"


CHandBookMonsterLayer::CHandBookMonsterLayer():
	m_ui(nullptr), m_pScroll(nullptr), m_pCell(nullptr)
{

}

CHandBookMonsterLayer::~CHandBookMonsterLayer()
{
	CC_SAFE_RELEASE(m_pCell);
}

bool CHandBookMonsterLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//内容
		m_ui = LoadComponent("handBookMonster.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		setIsShowBlack(false);

		//保存Cell
		m_pCell = (CLayout*)findWidgetById("Cell");
		m_pCell->retain();
		m_pCell->removeFromParent();

		setVisible(true);

		return true;
	}

	return false;
}

void CHandBookMonsterLayer::onEnter()
{
	BaseLayer::onEnter();

	//绑定活动列表回调
	//GetTcpNet->registerMsgHandler(BossPrizesMsg, this, CMsgHandler_selector(CHandBookMonsterLayer::processNetMsg));
	//测试
	processNetMsg(0, nullptr);
}

void CHandBookMonsterLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	//GetTcpNet->unRegisterAllMsgHandler(this);
}

void CHandBookMonsterLayer::processNetMsg( int type, google::protobuf::Message *msg )
{

	if(!isVisible())
	{
		this->setVisible(true);
	}

	//switch (type)
	//{
	//case BossPrizesMsg:
	//	{
	//	}
	//	break;
	//default:
	//	break;
	//}
}