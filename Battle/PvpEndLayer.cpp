#include "PvpEndLayer.h"
#include "Global.h"
#include "netcontrol/CPlayerControl.h"
#include "common/color.h"
#include "net/CNetClient.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "Battle/WarAssist.h"
#include "common/ProgressLabel.h"
#include "bag/bagData.h"
#include "tollgate/Chapter.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "Battle/RandomBusineseMan.h"
#include "guide/GuideManager.h"
#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"
#include "common/CommonFunction.h"
#include "common/CGameSound.h"
#include "tools/UICloneMgr.h"
#include "mainCity/RecruitData.h"
#include "mainCity/CNewHero.h"
#include "common/CCMixLabelAction.h"
#include "sdk/FaceBookSDK.h"
#include "Global.h"

PvpEndLayer::PvpEndLayer():m_rank(0),m_bIsWin(true)
{
}

PvpEndLayer::~PvpEndLayer(){
}
using namespace BattleSpace;
bool PvpEndLayer::init(){

	bool res = BaseLayer::init();
	
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	//黑底
	MaskLayer* pMaskLayer = MaskLayer::create("PvpEndLayerMasklLayer");
	pMaskLayer->setContentSize(winSize);
	LayerManager::instance()->push(pMaskLayer);
	pMaskLayer->setOpacity(255*0.8f);


	//配置内容
	m_ui = LoadComponent("pvpEndLayer.xaml");
	m_ui->setPosition(VCENTER);
	this->addChild(m_ui);
	this->setIsShowBlack(false);
	
	return res;
}


void PvpEndLayer::onEnter()
{
	BaseLayer::onEnter();

	//退出按钮
	CButton *btnExit = (CButton*)m_ui->findWidgetById("exit");
	btnExit->setOnClickListener(this,ccw_click_selector(PvpEndLayer::onExitClick));
	btnExit->setScale(.1f);
	btnExit->setVisible(false);
	//退出按钮
	btnExit->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCShow::create(), CCScaleTo::create(0.2f, 1.2f), CCScaleTo::create(0.1f, 1), NULL));


	setTouchPriority(-13);

	CButton* fbBtn = (CButton*)m_ui->findWidgetById("fbBtn");
	fbBtn->setOnClickListener(this,ccw_click_selector(PvpEndLayer::shareFb));
#ifdef FACEBOOKSHARE
	fbBtn->setVisible(true);
#else
	fbBtn->setVisible(false);
#endif // FACEBOOKSHARE
	
	HttpLoadImage::getInstance()->bindUiTarget(this);
}

void PvpEndLayer::onExit()
{
	BaseLayer::onExit();

	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);

	HttpLoadImage::getInstance()->bindUiTarget(nullptr);
}



void PvpEndLayer::processBattleFinish(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	if(msg==nullptr)
	{
		return;
	}

	PvpOverRes *res = (PvpOverRes*)msg;
	if(res->result() == 2)
	{
		m_bIsWin = false;
	}

	//旋转光
	CCSprite *star = (CCSprite*)m_ui->findWidgetById("light");
	star->runAction(CCRepeatForever::create(CCRotateBy::create(0.2f, 20)));

	//通关字样
	CCSprite *pass = (CCSprite*)m_ui->findWidgetById("finish_level");
	pass->setVisible(true);
	
	
	//成功
	if(m_bIsWin)
	{
		pass->setScale(4.0);
		pass->runAction(CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)));
		PlayEffectSound(SFX_425);
	}
	//失败
	else
	{
		PlayEffectSound(SFX_Fail);
		star->setColor(ccc3(30, 30, 30));
		star->setOpacity(240);
		pass->setTexture(CCTextureCache::sharedTextureCache()->addImage("warscene2/boken_1.png"));
	}


	float fDelayTime = 1.0;

	int iRank = res->my_rank();
	m_rank = iRank;
	//UP
	CLayout* pLayerUp = (CLayout*)m_ui->findWidgetById("up");
	////DOWN
	CLayout* pLayerDown = (CLayout*)m_ui->findWidgetById("down");
	bool bIsFirst = iRank==1?true:false;

	//是否第一名，第一名不显示可超越对象
	if(bIsFirst)
	{
		pLayerUp->setVisible(true);
		pLayerDown->setVisible(false);
		pLayerUp->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fDelayTime), CCMoveBy::create(0.3f, ccp(0, -80))));
	}
	else
	{
		pLayerUp->setVisible(true);
		pLayerDown->setVisible(true);
		CLayout* pCell = (CLayout*)pLayerDown->findWidgetById("Cell");
		pCell->setPositionY(pCell->getPositionY()-300);
		pCell->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fDelayTime), CCMoveBy::create(0.3f, ccp(0, 300))));
	}

	//排名+积分基础+多余积分
	//排名
	CCSprite* pRankIcon = (CCSprite*)(pLayerUp->findWidgetById("rank_icon"));
	if(iRank<=3)
	{
		CCString* pStr = CCString::createWithFormat("worldBoss/rank_%d.png", iRank);
		std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pStr->getCString());
		if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		{
			pRankIcon->setTexture(CCTextureCache::sharedTextureCache()->addImage(pStr->getCString()));
		}
	}
	else
	{
		pRankIcon->setVisible(false);
	}
	//label
	CLabelAtlas* pAtlas = CLabelAtlas::create("", "worldBoss/no_04.png", 26, 43, 48);
	pAtlas->setString(CCString::createWithFormat("%d", iRank)->getCString());
	pAtlas->setPosition(pRankIcon->getPosition()+ccp(0, 25));
	pRankIcon->getParent()->addChild(pAtlas, pRankIcon->getZOrder()+1);
	pAtlas->runAction(MixLabelAction::create(2.0f, pAtlas));

	//基础积分
	//多余积分
	//箭头
	int iBaseScore = res->org_points();
	int iAddScore = res->add_points();
	CLabel* pBaseScore = (CLabel *)pLayerUp->findWidgetById("score_base");
	pBaseScore->setString(ToString(iBaseScore));
	CLabel* pAddScore = (CLabel *)pLayerUp->findWidgetById("score_add");
	pAddScore->setString(CCString::createWithFormat(" %c%d", iAddScore>0?'+':'-', abs(iAddScore))->getCString());
	pAddScore->setPositionX(pBaseScore->getPositionX()+pBaseScore->getContentSize().width+10);
	if(iAddScore==0)
	{
		pAddScore->setVisible(false);
	}

	//箭头方向
	CImageView *pArrow = (CImageView *)pLayerUp->findWidgetById("arrow");
	pArrow->setRotation(iAddScore>0?-90:90);
	pArrow->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.25f, ccp(0, 10)), CCMoveBy::create(0.25f, ccp(0, -10)))));
	pArrow->setPositionX(pAddScore->getPositionX() + pAddScore->getContentSize().width + pArrow->getContentSize().width/2 + 20);
	pArrow->setPositionY(pArrow->getPositionY()+30);
	if(iAddScore==0)
	{
		pArrow->setVisible(false);
	}


	//声望（pvp商店货币）
	CLabel* pSW = (CLabel *)m_ui->findWidgetById("sw_num");
	pSW->setString(CCString::createWithFormat("+%d", res->pvp_coin())->getCString());


	//目标玩家
	if(!bIsFirst)
	{
		int iRank = res->next_rank().rank();
		//名次
		CCSprite* pRankIcon = (CCSprite*)(pLayerDown->findWidgetById("aim_rank"));
		if(iRank<=3)
		{
			CCString* pStr = CCString::createWithFormat("worldBoss/rank_%d.png", iRank);
			std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pStr->getCString());
			if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
			{
				pRankIcon->setTexture(CCTextureCache::sharedTextureCache()->addImage(pStr->getCString()));
			}
		}
		else
		{
			pRankIcon->setVisible(false);
		}
		//label
		CLabelAtlas* pAtlas = CLabelAtlas::create("", "worldBoss/no_04.png", 26, 43, 48);
		pAtlas->setString(CCString::createWithFormat("%d", iRank)->getCString());
		pAtlas->setPosition(pRankIcon->getPosition()+ccp(0, 0));
		pRankIcon->getParent()->addChild(pAtlas, pRankIcon->getZOrder()+1);
		pAtlas->runAction(MixLabelAction::create(2.0f, pAtlas));


		//头像
		CButton* pHeadBg = (CButton*)(pLayerDown->findWidgetById("head"));
		CCSprite* pHead = CCSprite::create(CCString::createWithFormat("headImg/%d.png", res->next_rank().role_thumb())->getCString());
		if( res->next_rank().role_thumb() > 0)
		{
			if (!pHead)
			{
				pHead = CCSprite::create("headImg/101.png");
				CCLOG("[ ERROR ] PvpEndLayer::processBattleFinish Lost Image = %d", res->next_rank().role_thumb());
			}
			pHeadBg->addChild(pHead);
			NodeFillParent(pHead);
		}
		//下载facebook头像
		else
		{
			//加载facebook头像
			string fbName = res->next_rank().fb_id()+".jpg";
			string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
			bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);		
			if(isFileExist)
			{
				CCSprite *sp = CCSprite::create(fbName.c_str());
				pHeadBg->addChild(sp);
				NodeFillParent(sp);
			}
			else
			{
				CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG, res->next_rank().fb_id().c_str());
				HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(), res->next_rank().fb_id().c_str());
				pHeadBg->setId( res->next_rank().fb_id().c_str() );
			}
		}

		//名字
		CLabel* pName = (CLabel*)(pLayerDown->findWidgetById("name"));
		pName->setString(res->next_rank().role_name().c_str());

		//等级
		CLabel* pLevel = (CLabel*)(pLayerDown->findWidgetById("level"));
		pLevel->setString(ToString(res->next_rank().rank()));

		//积分
		CLabelAtlas* pHurt = (CLabelAtlas*)(pLayerDown->findWidgetById("hurt"));
		pHurt->setString(ToString(res->next_rank().points()));
	}
}

void PvpEndLayer::onExitClick(CCObject *pSender)
{
	CCMenuItem *item = (CCMenuItem*)pSender;
	item->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.2f), 
		CCScaleTo::create(0.1f, 1.f), 
		CCCallFuncN::create(this, callfuncN_selector(PvpEndLayer::onBackClick)),
		NULL));
}

void PvpEndLayer::shareFb(CCObject *pSender)
{
	int stageId = DataCenter::sharedData()->getWar()->getStageID();
	const ShareData *data = DataCenter::sharedData()->getShareData()->getCfg(10);
	CCString *desc = CCString::createWithFormat(data->desc.c_str(),m_rank);
	CCString *url = CCString::createWithFormat(data->url.c_str(),stageId);
	FaceBookSDK::sharedInstance()->onShareToFb(desc->getCString(),url->getCString());
}

void PvpEndLayer::onBackClick(CCNode* ob)
{
	//返回章节
	DataCenter::sharedData()->setCityActionType(CA_PVP);

	CScene* scene = GETSCENE(LoadBattleResource);
	((LoadBattleResource*)scene)->setRelease(true, SkipcityScene);
	CSceneManager::sharedSceneManager()->replaceScene(scene);
}


bool PvpEndLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	BaseLayer::ccTouchBegan(pTouch,pEvent);

	return true;
}

void PvpEndLayer::imageLoadSuccessCallBack( string sTag, vector<char>* pBuffer )
{
	CCImage* img = new CCImage;
	img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
	CCTexture2D* texture = new CCTexture2D();
	texture->initWithImage(img);
	//保存facebook 头像
	string path = HttpLoadImage::getInstance()->getStoragePath("download/fbImg",sTag.c_str())+".jpg";
	string buff(pBuffer->begin(), pBuffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
	fclose(fp);

	CImageView* pHeadBg = (CImageView*)m_ui->findWidgetById(sTag.c_str());
	if (pHeadBg)
	{
		CCSprite *pHead = CCSprite::createWithTexture(texture);
		pHeadBg->addChild(pHead);
		NodeFillParent(pHeadBg);
	}
	img->release();
}

void PvpEndLayer::setBattleResult( bool bResult )
{
	m_bIsWin = bResult;
}
