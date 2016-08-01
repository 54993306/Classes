#include "FaceBookImg.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"
#include "common/CommonFunction.h"
#include "Global.h"
#include "common/CGameSound.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "tools/CCShake.h"
#include "common/ShaderDataHelper.h"
#include "Global.h"

int CFacebookImg::TableViewOffSet = 0;
using namespace BattleSpace;
bool CFacebookImg::init()
{
	if (BaseLayer::init())
	{

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("CFacebookImgMaskLayer");
		LayerManager::instance()->push(pMaskLayer);
		pMaskLayer->setVisible(false);

		m_ui = LoadComponent("FacebookImg.xaml");  //  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;

}

CFacebookImg::CFacebookImg():m_pos(0),m_iCurrentChapterId(0)
{

}

void CFacebookImg::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);
	
	m_pageView = (CPageView *)m_ui->findWidgetById("scroll");
	m_pageView->setDirection(eScrollViewDirectionHorizontal);	
	m_pageView->setCountOfCell(0);
	m_pageView->setSizeOfCell(m_cell->getContentSize());
	m_pageView->setAnchorPoint(ccp(0,0));
	m_pageView->setAutoRelocate(true);
	m_pageView->setDeaccelerateable(false);
	m_pageView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CFacebookImg::tableviewDataSource));
	m_pageView->reloadData();
	

	CImageView *left = (CImageView*)m_ui->findWidgetById("left");
	left->setTouchEnabled(true);
	left->setOnClickListener(this,ccw_click_selector(CFacebookImg::onLeft));
	CImageView *right = (CImageView*)m_ui->findWidgetById("right");
	right->setTouchEnabled(true);
	right->setOnClickListener(this,ccw_click_selector(CFacebookImg::onRight));
	HttpLoadImage::getInstance()->bindUiTarget(this);
}

CCObject* CFacebookImg::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CFacebookImg::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);	

	for (int i = 1; i <=2; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		switch (i)
		{
		case 2:
			{
				string fbName = m_stage.friendList.at(uIdx)+".jpg";
				string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
				bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
				if(isFileExist)
				{
					CCSprite* spr = CCSprite::create(fullName.c_str());
					if (spr)
					{
						child->addChild(spr);
						NodeFillParent(spr);
					}
				}
				else
				{
// 					CCSprite* spr = CCSprite::create("headImg/156.png");
// 					if (spr)
// 					{
// 						child->addChild(spr);
// 						NodeFillParent(spr);
// 						spr->setScale(0.8*spr->getScale());
// 					}
					CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG,m_stage.friendList.at(uIdx).c_str());
					HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(),m_stage.friendList.at(uIdx).c_str());
				}			
				pCell->setUserData(&m_stage.friendList.at(uIdx));
			}
			break;
		default:
			break;
		}
	}
}

void CFacebookImg::onExit()
{
	BaseLayer::onExit();

	TableViewOffSet = m_pageView->getContentOffset().y;
	HttpLoadImage::getInstance()->bindUiTarget(nullptr);
}



void CFacebookImg::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CFacebookImg::onHero(CCObject* pSender)
{
	RewardDataGM *data = DataCenter::sharedData()->getRewardDataGM();
	int size = data ->getStoryLength();
	for (int i = 0; ; i++)
	{
		int pos = CCRANDOM_0_1()*(size-1);
		if (pos!=m_pos)
		{
			m_pos = pos;
			break;
		}
	}
	CLabel *story = (CLabel*)m_ui->findWidgetById("story");
	story->setString(data->getStoryStr(m_pos));
}

void CFacebookImg::callBackForShake()
{
	m_ui->setScale(1.02f);
	m_ui->runAction(CCSequence::createWithTwoActions(CCShake::create(0.08f, 8), CCCallFunc::create(this, callfunc_selector(CFacebookImg::showShakeCallBack))));
}

void CFacebookImg::showShakeCallBack()
{
	m_ui->setScale(1.0f);
}

void CFacebookImg::showEffectStart()
{
	
}

void CFacebookImg::showEffectCallBack()
{
}

void CFacebookImg::onLeft(CCObject* pSender)
{
	CCPoint pos = m_pageView->getContentOffset();
	pos.x +=  m_cell->getContentSize().width;
	if (pos.x > m_pageView->getMaxOffset().x)
	{
		pos.x  = m_pageView->getMaxOffset().x;
	}
	m_pageView->setContentOffset(pos);
}

void CFacebookImg::onRight(CCObject* pSender)
{
	CCPoint pos = m_pageView->getContentOffset();
	pos.x -=  m_cell->getContentSize().width;
	if (pos.x < m_pageView->getMinOffset().x)
	{
		pos.x  = m_pageView->getMinOffset().x;
	}
	m_pageView->setContentOffset(pos);
}

void CFacebookImg::setStage(const CStage& stage)
{
	m_stage = stage;
	m_pageView->setCountOfCell(stage.friendList.size());
	m_pageView->reloadData();
}

void CFacebookImg::imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer)
{
	CCImage* img = new CCImage;
	img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
	CCTexture2D* texture = new CCTexture2D();
	texture->initWithImage(img);

	string path = HttpLoadImage::getInstance()->getStoragePath("download/fbImg",sTag.c_str())+".jpg";
	string buff(pBuffer->begin(), pBuffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
	fclose(fp);

	CCSprite *headImg = CCSprite::createWithTexture(texture);
	int i = 0;
	for (; i < m_stage.friendList.size(); i++)
	{
		if (m_stage.friendList.at(i)==sTag)
		{
			break;
		}
	}
	CTableViewCell *pCell = m_pageView->cellAtIndex(i);
	if (pCell)
	{
		pCell->getChildByTag(2)->addChild(headImg);
		NodeFillParent(headImg);
		headImg->setScale(0.8*headImg->getScale());
	}
	img->release();
}

bool CFacebookImg::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	if( !res )
	{
		CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());
		CImageViewScale9 *bg = (CImageViewScale9*)(m_ui->findWidgetById("bg"));
		if( bg->boundingBox().containsPoint(pTouchPos))
		{
			res = true;
		}
		else
		{
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
	}
	return true;
}