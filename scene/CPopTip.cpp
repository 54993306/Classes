#include "CPopTip.h"
#include "Global.h"
#include "common/CGameSound.h"
CPopTip::CPopTip():m_listener(nullptr)
	,m_handler(nullptr)
	,m_tipContent(nullptr)
	,m_title(nullptr)
	,m_isOutClose(false)
	,m_bottomOffset(10.0f)
	,m_textTip(nullptr)
{
	m_btnList = CCArray::create();
	m_btnList->retain();
}

CPopTip::~CPopTip()
{
	m_listener = nullptr;
	m_handler = nullptr;
	m_btnList->release();
	m_btnList = nullptr;
}
/*
CPopTip* CPopTip::create()
{
	CPopTip* pRet = new CPopTip();
	if( pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return nullptr;
}
*/

bool CPopTip::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("CPopTipmask");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer, true);

		m_ui = LoadComponent("popTip.xaml");
		m_ui->setTag(1);
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);

		//背景图片
		m_pBackgroundImage = (CCNode*)m_ui->findWidgetById("bg");

		//提示信息
		m_tipContent = (CLabel*)m_ui->findWidgetById("info");
		m_tipContent->setDimensions(m_tipContent->getContentSize());
		m_tipContent->setHorizontalAlignment(kCCTextAlignmentCenter);
		m_tipContent->setVerticalAlignment(kCCVerticalTextAlignmentCenter);

		//按钮绑定
		CButton* pConfirm = (CButton*)m_ui->findWidgetById("confirm");
		pConfirm->setTag(PopTipConfirm);
		m_btnList->addObject(pConfirm);
		CButton* pCancel = (CButton*)m_ui->findWidgetById("cancel");
		pCancel->setTag(PopTipCancel);
		m_btnList->addObject(pCancel);
		this->setVisible(true);
		return true;
	}
	return false;
}

void CPopTip::setBackgroundImage(const char* pFile) 
{ 
	//m_pBackgroundImage = CCSprite::create(pFile);
	//m_layout->addChild(m_pBackgroundImage);
	//m_pBackgroundImage->setPosition(VCENTER);

}

void CPopTip::setBackImgScale(float scalex, float scaley) 
{
	//m_pBackgroundImage->setScaleX(scalex);
	//m_pBackgroundImage->setScaleY(scaley);
}

void CPopTip::onEnter()
{
	BaseLayer::onEnter();
	PlayEffectSound(SFX_420);
}

void CPopTip::onExit()
{
	BaseLayer::onExit();	
}

void CPopTip::setTitle(const char *title, const ccColor3B &color,char* fontName, float size)
{
	//m_title = CLabel::create(title,fontName,size);
	//m_title->setColor(color);
	//m_layout->addChild(m_title);
}

void CPopTip::addButton(char *norImg, char *selectImg, char *text,int tag, float scale)
{
	//SndButton *button = SndButton::create(norImg,selectImg);
	//if (text)
	//{
	//	button->setText(text);
	//}
	//if (tag!=0)
	//{
	//	button->setTag(tag);
	//}
	//button->setScale(scale);
	//m_layout->addChild(button);
	//m_btnList->addObject(button);
}


void CPopTip::addContentTip(const char *content, const ccColor3B &color,char* fontName, float span, float size)
{
	m_tipContent->setString(content);
	//m_tipContent->setFontName(fontName);
	//m_tipContent->setFontSize(size);
	//m_tipContent->setColor(color);
}

void CPopTip::setButtonLisener(CCObject* pListener, SEL_ClickHandler pHandler)
{
	m_listener = pListener;
	m_handler = pHandler;
	for (int i=0;i<m_btnList->count(); ++i)
	{
		CButton *btn = dynamic_cast<CButton*>(m_btnList->objectAtIndex(i));
		btn->setOnClickListener(pListener,pHandler);
	}
}

void CPopTip::setIsTouchOutClose(bool isClose)
{
	m_isOutClose = isClose;
}

bool CPopTip::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch, pEvent);
	if (res)
	{
		PlayEffectSound(SFX_Ensure);
	}
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

void CPopTip::setBottomOffset(float offset)
{ 
	//m_bottomOffset = offset; 
}

void CPopTip::addBeforeImage(char *img, float offsetX, float offsetY)
{
	//CCSprite *bgSpr = CCSprite::create(img);
	//m_layout->addChild(bgSpr);
	//bgSpr->setPosition(ccpAdd(m_pBackgroundImage->getPosition(),ccp(offsetX,offsetY)));
}

void CPopTip::buttonAddImage(char *img, int tag)
{
	//CCSprite *bgSpr = CCSprite::create(img);
	//CButton* button = dynamic_cast<CButton*>(m_layout->getChildByTag(tag));
	//button->addChild(bgSpr);
	//bgSpr->setPosition(ccp(button->getContentSize().width/2,button->getContentSize().height/2));
}

void CPopTip::runScaleAction()
{
	m_ui->setScale(0.0f);
	m_ui->runAction(CCScaleTo::create(0.2f, 1.0f));
}

CCNode *CPopTip::getConfirmButton()
{
	if (m_btnList)
	{
		CCNode *child = (CCNode*)(m_btnList->objectAtIndex(0));
		if (child->getTag() == PopTipConfirm)
		{
			return child;
		}
	}
	return nullptr;
}

//添加富文本
void CPopTip::addTextRichTip(const char *content, const ccColor3B &color, char* fontName,float size)
{
	if (!m_textTip)
	{
		m_textTip = CTextRich::create();
		m_ui->addChild(m_textTip);
	}
	m_textTip->insertElement(content,fontName,size,color);
	/*m_textTip->reloadData();*/
}

void CPopTip::addRichImage(char *img)
{
	if (!m_textTip)
	{
		m_textTip = CTextRich::create();
		m_ui->addChild(m_textTip);
	}
	CCSprite *imageSpr = CCSprite::create(img);
	m_textTip->insertElement(imageSpr,1);
}

void CPopTip::reloadTextRich(const CCPoint &pos)
{
	if (m_textTip)
	{
		m_textTip->reloadData();
		m_textTip->setPosition(pos);
	}
}

//void CPopTip::removeSelf(CCObject * pSender)
//{
//	this->stopAllActions();
//	((CCNode*)pSender)->getParent()->getParent()->removeFromParent();
//}

void CPopTip::showConfirmOnly()
{
	CButton* pConfirm = (CButton*)m_ui->findWidgetById("confirm");
	CButton* pCancel = (CButton*)m_ui->findWidgetById("cancel");
	pCancel->setVisible(false);

	pConfirm->setPosition((pConfirm->getPosition()+pCancel->getPosition())/2);

	CLabel* pTextConfirm = (CLabel*)m_ui->findWidgetById("text_confirm");
	CLabel* pTextCancel = (CLabel*)m_ui->findWidgetById("text_cancel");
	pTextCancel->setVisible(false);

	pTextConfirm->setPosition((pTextConfirm->getPosition()+pTextCancel->getPosition())/2);

}

void CPopTip::onClose( CCObject * pSender )
{
	this->stopAllActions();
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CPopTip::runActionWithUi( CCAction* pAction )
{
	m_ui->runAction(pAction);
}
