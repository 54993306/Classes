/************************************************************* 
 *
 *
 *		Data : 2016.7.15
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "Battle/BattleLayer/BattleTips.h"
namespace BattleSpace
{
	BattleTips::BattleTips()
	:m_ui(nullptr),m_pBackgroundImage(nullptr),m_tipContent(nullptr),m_listener(nullptr)
	,m_handler(nullptr),m_isOutClose(true)
	{}

	BattleTips::~BattleTips()
	{}

	bool BattleTips::init()
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		m_ui = LoadComponent("popTip.xaml");
		m_ui->setPosition(VCENTER);
		m_ui->setTouchEnabled(true);
		this->setTouchPriority(-9999);
		this->addChild(m_ui, 2);

		//背景图片
		m_pBackgroundImage = (CCNode*)m_ui->findWidgetById("bg");
		//提示信息
		m_tipContent = (CLabel*)m_ui->findWidgetById("info");
		m_tipContent->setDimensions(m_tipContent->getContentSize());
		m_tipContent->setHorizontalAlignment(kCCTextAlignmentCenter);
		m_tipContent->setVerticalAlignment(kCCVerticalTextAlignmentCenter);

		return true;
	}

	void BattleTips::showConfirmOnly()
	{
		CButton* pConfirm = (CButton*)m_ui->findWidgetById("confirm");
		CButton* pCancel = (CButton*)m_ui->findWidgetById("cancel");
		pCancel->setVisible(false);

		pConfirm->setPosition((pConfirm->getPosition()+pCancel->getPosition())/2);

		CLabel* pTextConfirm = (CLabel*)m_ui->findWidgetById("text_confirm");
		CLabel* pTextCancel = (CLabel*)m_ui->findWidgetById("text_cancel");
		pTextCancel->setVisible(false);

		pTextConfirm->setPosition((pTextConfirm->getPosition()+pTextCancel->getPosition())/2);
		pConfirm->setOnClickListener(this,ccw_click_selector(BattleTips::removeSelf));
		CCDelayTime *delay = CCDelayTime::create(1.5f);
		CCScaleTo *smal = CCScaleTo::create(0.2f,0.0f);
		CCSequence *seque = CCSequence::create(delay,smal, CCCallFuncO::create(this,callfuncO_selector(BattleTips::removeSelf), nullptr), nullptr);
		m_ui->runAction(seque);
	}

	void BattleTips::setButtonLisener( CCObject* pListener, SEL_ClickHandler pHandler )
	{
		m_listener = pListener;
		m_handler = pHandler;
		CButton* pConfirm = (CButton*)m_ui->findWidgetById("confirm");
		pConfirm->setOnClickListener(pListener,pHandler);

		CButton* pCancel = (CButton*)m_ui->findWidgetById("cancel");
		pCancel->setOnClickListener(pListener,pHandler);
	}

	bool BattleTips::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
	{
		bool res = CWidgetWindow::ccTouchBegan(pTouch, pEvent);
		if (!res)
		{
			CCPoint locPoint = convertToNodeSpace(pTouch->getLocation());
			if (m_pBackgroundImage->boundingBox().containsPoint(locPoint))
			{
				res = true;
			}
			//点击背景框外
			else if (m_isOutClose)
			{
				this->removeFromParent();
				res = true;
			}
		}
		return true;
	}

	void BattleTips::setIsTouchOutClose( bool isClose )
	{
		m_isOutClose = isClose;
	}

	void BattleTips::addContentTip( const char *content )
	{
		m_tipContent->setString(content);
	}

	void BattleTips::removeSelf( CCObject* ob )
	{
		this->removeFromParentAndCleanup(true);
	}

}