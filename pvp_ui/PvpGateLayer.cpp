#include "PvpGateLayer.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "GMessage.h"
#include "model/DataDefine.h"
#include "tools/ShowTexttip.h"
#include "pvp_ui/SelectChallengeLayer.h"
#include "netcontrol/CPlayerControl.h"
#include "mainCity/mainScene.h"


CPvpGateLayer::CPvpGateLayer():
	m_ui(nullptr), m_pCloudyTranstion(nullptr),m_fogLay(nullptr),m_fogLay1(nullptr),m_fMiddleLayer(nullptr)
{
}

CPvpGateLayer::~CPvpGateLayer()
{

}

bool CPvpGateLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//内容
		m_ui = LoadComponent("pvp_gate_layer.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		setVisible(false);
		setTouchPriority(-2);

		//同步竞技场
		{
			CImageView* pBtn = (CImageView*)m_ui->findWidgetById("click_1");
			pBtn->setTouchEnabled(true);
			pBtn->setOnClickListener(this, ccw_click_selector(CPvpGateLayer::pvpSynchronization));
		}


		//异步竞技场
		{
			CImageView* pBtn = (CImageView*)m_ui->findWidgetById("click_2");
			pBtn->setTouchEnabled(true);
			pBtn->setOnClickListener(this, ccw_click_selector(CPvpGateLayer::pvpAsynchronization));
		}

		//退出
		{
			CImageView* pBtn = (CImageView*)m_ui->findWidgetById("click_3");
			pBtn->setTouchEnabled(true);
			pBtn->setOnClickListener(this, ccw_click_selector(CPvpGateLayer::onClose));
		}
		{
			CImageView* pBtn = (CImageView*)m_ui->findWidgetById("click_4");
			pBtn->setTouchEnabled(true);
			pBtn->setOnClickListener(this, ccw_click_selector(CPvpGateLayer::onClose));
		}

		//雾
		m_fogLay = (CLayout *)m_ui->findWidgetById("fog2");
		m_fogLay1 =(CLayout*)m_ui->findWidgetById("fog3");
		this->runBloodFogAction();

		//云
		runCloud();

		m_fMiddleLayer = (CLayout *)m_ui->findWidgetById("middle");

		return true;
	}

	return false;
}

void CPvpGateLayer::runBloodFogAction()
{
	float width = 1308;//cloud->getContentSize().width;

	CCMoveBy* move = CCMoveBy::create(60.0f,ccp(-width,0));
	CCMoveBy* moveb = CCMoveBy::create(60.0f,ccp(-width,0));
	CCSequence *sequence = CCSequence::create(move,CCCallFuncN::create(this,callfuncN_selector(CPvpGateLayer::relocation)),moveb,nullptr);
	m_fogLay->runAction(CCRepeatForever::create(sequence));

	m_cloudPos = m_fogLay->getPosition();

	// 	CCSprite *cloud1  =  (CCSprite *)(m_fogLay->findWidgetById("cloud1"));
	m_cloudAPos = m_fogLay1->getPosition();

	CCMoveBy* move1 = CCMoveBy::create(120.0f,ccp(-width*2,0));
	CCSequence *sequence1 = CCSequence::createWithTwoActions(move1,CCCallFuncN::create(this,callfuncN_selector(CPvpGateLayer::relocation)));
	m_fogLay1->runAction(CCRepeatForever::create(sequence1));
}

void CPvpGateLayer::relocation( CCNode* pSender )
{
	CLayout *cloud = (CLayout*)pSender;
	float width = 1308;/*cloud->getContentSize().width;*/
	CCPoint pos; 
	if (strcmp(cloud->getId(),"fog2")==0)
	{
		pos = ccpAdd(m_cloudPos,ccp(width,0));
		cloud->setPosition(pos);
	}
	else
	{
		cloud->setPosition(m_cloudAPos);
	}
}

void CPvpGateLayer::onEnter()
{
	BaseLayer::onEnter();
}

void CPvpGateLayer::onExit()
{
	BaseLayer::onExit();

	//移除无用的资源
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void CPvpGateLayer::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);

	//重置锚点
	ResetAnchorPointAndKeepSamePos(m_ui, ccp(0.5f, 0.4f));
	//屏蔽触摸
	setTouchEnabled(false);
	//缩放
	m_ui->runAction(
		CCSpawn::create(
		CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCCallFunc::create(this, callfunc_selector(CPvpGateLayer::showEffectOut))),
		CCScaleTo::create(0.3f, 1.5f),
		nullptr)
		);
}

void CPvpGateLayer::pvpSynchronization( CCObject* pSender )
{
	ShowPopTextTip("open later!");
}

void CPvpGateLayer::pvpAsynchronization( CCObject* pSender )
{
	//进入异步PVP，选择队伍
		/*	CTranstionEffectLayer* pEffect = CTranstionEffectLayer::create();
	pEffect->onOpen(this, 100);
	return*/;
	CSelectChallengeLayer* pChange = CSelectChallengeLayer::create();
	LayerManager::instance()->push(pChange);
	CPlayerControl::getInstance().askForPvpChallenge(false);

	//CSelectDefense *pSelectDefense = CSelectDefense::create();
	//LayerManager::instance()->push(pSelectDefense);
	//CPlayerControl::getInstance().askForPvpTeamData();	
	//return;
}

void CPvpGateLayer::showEffectIn()
{
	//TODO
	//PVP场景进场动画
	//云进入
	//云动作
	//延迟0.5f生成建筑物
	//云出
	//动画完成

	setVisible(true);

	m_ui->setVisible(false);

	showCloundyTranstion();

	runAction(CCSequence::create(
		CCDelayTime::create(0.5f),
		CCCallFunc::create(this, callfunc_selector(CPvpGateLayer::postMsgToCleanMainCityRes)),
		CCCallFunc::create(this, callfunc_selector(CPvpGateLayer::showBuilding)),
		nullptr
		));
}

void CPvpGateLayer::showEffectOut()
{
	//TODO
	//PVP场景进场动画
	//云进入
	//云动作
	//延迟0.5f隐藏建筑物
	//云出
	//摧毁当前层
	//动画完成

	showCloundyTranstion(true);
	runAction(CCSequence::create(
		CCDelayTime::create(0.5f),
		CCCallFunc::create(this, callfunc_selector(CPvpGateLayer::postMsgToRebuildMainCityRes)),
		CCCallFunc::create(this, callfunc_selector(CPvpGateLayer::hideBuilding)),
		nullptr
		));
}

void CPvpGateLayer::initCloundyTranstion()
{
	SkeletonAnimation *pSkeletonAnimation = SkeletonAnimation::createWithFile("pvp/yun.json", "pvp/yun.atlas", 1);
	pSkeletonAnimation->setPosition(ccp(VCENTER.x, 0));
	this->addChild(pSkeletonAnimation);
	pSkeletonAnimation->completeListener = std::bind(&CPvpGateLayer::SpineComplete, this, std::placeholders::_1, std::placeholders::_2);

	m_pCloudyTranstion = pSkeletonAnimation;
}

void CPvpGateLayer::showCloundyTranstion( bool bFlipY )
{
	if(m_pCloudyTranstion == nullptr)
	{
		initCloundyTranstion();
	}

	m_pCloudyTranstion->setScaleY(bFlipY?-1:1);
	m_pCloudyTranstion->setVisible(true);
	m_pCloudyTranstion->setAnimation(0, "stand", false);
}

void CPvpGateLayer::hideCloundyTranstion()
{
	m_pCloudyTranstion->setVisible(false);
}


void CPvpGateLayer::initBuilding()
{
	//建筑1
	{
		SkeletonAnimation *pSkeletonAnimation = SkeletonAnimation::createWithFile("pvp/zuojian.json", "pvp/zuojian.atlas", 1);
		pSkeletonAnimation->setPosition(ccp(180, 0));
		pSkeletonAnimation->setAnimation(0, "stand", true);
		m_fMiddleLayer->addChild(pSkeletonAnimation, 1);
	}

	//建筑2
	{
		SkeletonAnimation *pSkeletonAnimation = SkeletonAnimation::createWithFile("pvp/zhongjian.json", "pvp/zhongjian.atlas", 1);
		pSkeletonAnimation->setPosition(ccp(DESIGN_WIDTH/2, 0));
		pSkeletonAnimation->setAnimation(0, "stand", true);
		m_fMiddleLayer->addChild(pSkeletonAnimation, 3);
	}
	
	//建筑3
	{
		SkeletonAnimation *pSkeletonAnimation = SkeletonAnimation::createWithFile("pvp/youjian.json", "pvp/youjian.atlas", 1);
		pSkeletonAnimation->setPosition(ccp(900, 0));
		pSkeletonAnimation->setAnimation(0, "stand", true);
		m_fMiddleLayer->addChild(pSkeletonAnimation, 2);
	}
}

void CPvpGateLayer::showBuilding()
{
	m_ui->setVisible(true);

	initBuilding();
}


void CPvpGateLayer::hideBuilding()
{
	m_ui->setVisible(false);
}

void CPvpGateLayer::postMsgToCleanMainCityRes()
{
	//清除主场景资源，其实不清楚也行，没有怎么样，反而搞来搞去会卡顿
	//TODO
	NOTIFICATION->postNotification("CMainScene::removeCityBuildLayer");

}

void CPvpGateLayer::postMsgToRebuildMainCityRes()
{
	//TODO
	NOTIFICATION->postNotification("CMainScene::addCityBuildLayer");
}


void CPvpGateLayer::SpineComplete( int trackIndex, int loopCount )
{
	SkeletonAnimation* skeletonNode = dynamic_cast<SkeletonAnimation*>(m_pCloudyTranstion);
	if (skeletonNode)
	{
		CCLOG("[ info ]	 CPvpGateLayer::SpineComplete");
		m_pCloudyTranstion->setVisible(false);

		//场景都不可见了，移除掉，退出动画常用
		if(!m_ui->isVisible())
		{
			NOTIFICATION->postNotification("CMainScene::removePvpGateLayer");
		}
	}
}

void CPvpGateLayer::runCloud()
{
	for(int i=0; i<3; i++)
	{
		CImageView *pImage = (CImageView *)m_ui->findWidgetById(CCString::createWithFormat("cloud%d", i+1)->getCString());
		pImage->runAction(
				CCRepeatForever::create(
					CCSequence::create(
						CCMoveBy::create(1500/(50+CCRANDOM_0_1()*50), ccp(1500, 0)),
						CCMoveTo::create(0, ccp(-200, pImage->getPositionY())),
						nullptr
					)
				)
			);
	}
}
