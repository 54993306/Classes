#include "WorldBoss.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "reward/WBossLayer.h"
#include "reward/WRuleLayer.h"
#include "reward/WShopLayer.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "mainCity/MainCityControl.h"
#include "scene/effect/EffectObject.h"
#include "warscene/ArmatureEventDataMgr.h"
#include "tools/CCShake.h"
#include "scene/alive/ActionDef.h"
using namespace BattleSpace;
CWorldBoss::CWorldBoss():
m_ui(nullptr),m_pBossLayer(nullptr), m_pRuleLayer(nullptr),m_pShopLayer(nullptr),m_iType(WorldBossTabNull)
{
	m_pArmature = nullptr;
	m_lastFrame = 0;
	m_armatureEventDataMgr = nullptr;
	m_armatureEventData = nullptr;
}

CWorldBoss::~CWorldBoss()
{
	CC_SAFE_RELEASE(m_armatureEventDataMgr);
}

bool CWorldBoss::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		
		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("WorldBossMaskLayer");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);

		//内容
		m_ui = LoadComponent("worldBoss.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}

	return false;
}

void CWorldBoss::onEnter()
{
	BaseLayer::onEnter();

	//选项卡
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->findWidgetById("radio");
	for (unsigned int i=0; i<WorldBossTabMax; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CWorldBoss::onSwitchBtn));
	}

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CWorldBoss::onClose));
	this->addChild(pClose, 999);

	m_armatureEventDataMgr = ArmatureEventDataMgr::create();
	m_armatureEventDataMgr->retain();

	//默认选择一个
	selectTabPanel(WorldBossTabBoss);
}

void CWorldBoss::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
	//解绑场景事件监听
	CSceneManager::sharedSceneManager()->removeMsgObserver("NULL", this);


	//移除无用的资源
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
	
}

void CWorldBoss::onSwitchBtn( CCObject *pSender, bool bChecked )
{
	CRadioButton* pBtn = (CRadioButton*)pSender;
	int iSelectIndex = pBtn->getTag();

	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		selectTabPanel((WorldBossTabType)iSelectIndex);
	}
}

void CWorldBoss::selectTabPanel( WorldBossTabType iType, bool bForce )
{
	if(m_iType==iType && !bForce)
	{
		return;
	}

	m_iType = iType;

	//更新展示区域
	updateShowArea();
}

void CWorldBoss::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CWorldBoss::updateShowArea()
{
	CCNode* pLayer = (CCNode*)m_ui->findWidgetById("layer_in");

	//由shop切回
	if(m_iType != WorldBossTabShop && m_pShopLayer && m_pShopLayer->isVisible())
	{
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	}
	//切回shop
	if(m_iType == WorldBossTabShop)
	{
		NOTIFICATION->postNotification(HIDE_TOP_LAYER);
	}

	if(m_pBossLayer) m_pBossLayer->setVisible(false);
	if(m_pRuleLayer) m_pRuleLayer->setVisible(false);
	if(m_pShopLayer)m_pShopLayer->setVisible(false);

	switch (m_iType)
	{
	case WorldBossTabBoss:
		{
			if(!m_pBossLayer)
			{
				m_pBossLayer = CWBossLayer::create();
				m_pBossLayer->setTouchPriority(LayerManager::instance()->getPriority()-1);
				pLayer->addChild(m_pBossLayer);
				m_pBossLayer->setPosition(pLayer->convertToNodeSpace(m_pBossLayer->getPosition()));
				AskForWorldBoss();
			}
			else
			{
				m_pBossLayer->setVisible(true);
			}
		}
		break;
	case WorldBossTabRule:
		{
			if(!m_pRuleLayer)
			{
				m_pRuleLayer = CWRuleLayer::create();
				m_pRuleLayer->setTouchPriority(LayerManager::instance()->getPriority()-1);
				pLayer->addChild(m_pRuleLayer);
				m_pRuleLayer->setPosition(pLayer->convertToNodeSpace(m_pRuleLayer->getPosition()));
				AskForRuleReward();
			}
			m_pRuleLayer->setVisible(true);
		}
		break;
	case WorldBossTabShop:
		{
			if(!m_pShopLayer)
			{
				m_pShopLayer = CWShopLayer::create();
				m_pShopLayer->setTouchPriority(LayerManager::instance()->getPriority()-1);
				pLayer->addChild(m_pShopLayer);
				m_pShopLayer->setPosition(pLayer->convertToNodeSpace(m_pShopLayer->getPosition()));
			}			
			CMainCityControl::getInstance()->sendShopRequest(3);
			m_pShopLayer->setVisible(true);
		}
		break;
	case WorldBossTabMax:
		break;
	default:
		break;
	}
}

void CWorldBoss::showWorldBossEffect(int iBossId)
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	if(LayerManager::instance()->getLayer("showWorldBossEffect")!=nullptr) return;

	m_armatureEventData = m_armatureEventDataMgr->getArmatureEventData(iBossId);

	//屏蔽层
	{
		MaskLayer* lay = MaskLayer::create("showWorldBossEffect1");
		lay->setContentSize(size);
		lay->setAnchorPoint(ccp(0.5f, 0.5f));
		lay->setPosition(VCENTER);
		lay->setOpacity(255);
		LayerManager::instance()->push(lay);
	}
	MaskLayer* lay = MaskLayer::create("showWorldBossEffect2");
	lay->setContentSize(size);
	lay->setAnchorPoint(ccp(0.5f, 0.5f));
	lay->setPosition(VCENTER);
	lay->setOpacity(255);
	LayerManager::instance()->push(lay);
	lay->getParent()->reorderChild(lay, 101);

	PlayBackgroundMusic(BGM_Boss1,true);

	//警告闪电
	EffectObject* eff = EffectObject::create("warning",PLAYERTYPE::Repeat);
	eff->setPosition(VCENTER);
	eff->setEffAnchorPoint(0.5,0.5);
	eff->setZOrder(2);
	eff->runAction(CCSequence::create(CCDelayTime::create(1.5f), CCFadeOut::create(0.3f), CCRemoveSelf::create(), NULL));
	lay->addChild(eff, 2);
	PlayEffectSound(SFX_419);
	//名字
	CCSprite* pName = CCSprite::create(CCString::createWithFormat("warScene/%d.png", iBossId)->getCString());
	if(!pName)
	{
		CCLOG("ERROR DisplayBossWarning");
		pName = CCSprite::create("warScene/1037.png");
	}
	pName->setPosition(VCENTER);
	pName->setScale(4.0);
	pName->setVisible(false);
	lay->addChild(pName, 3);
	pName->runAction(CCSequence::create(
		CCDelayTime::create(0.3f), 
		CCShow::create(),
		CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)),
		CCDelayTime::create(0.9f),
		CCSpawn::createWithTwoActions(CCFadeOut::create(0.3f), CCScaleTo::create(0.3f, 3.0f)),
		CCRemoveSelf::create(),
		nullptr
		));


	//背景图
	CCSprite* pBg = CCSprite::create("common/bgmission_3.png");
	pBg->setPosition(VCENTER);
	lay->addChild(pBg, 1);
	pBg->setOpacity(0);
	pBg->setTag(iBossId);
	pBg->runAction(CCSequence::create(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f), nullptr));

	//BOSS
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(CCString::createWithFormat("BoneAnimation/%d.ExportJson", iBossId)->getCString());
	CCArmature* pBoss = CCArmature::create(CCString::createWithFormat("%d", iBossId)->getCString());	
	pBoss->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(CWorldBoss::movementCallBack));
	if(pBoss->getAnimation()->getAnimationData()->getMovement("move")!=nullptr)
	{
		pBoss->getAnimation()->play("move");
	}
	pBoss->getAnimation()->setSpeedScale(2.5f);
	showShake(1.5f);
	m_pArmature = pBoss;
	pBoss->setPositionX(250-300);
	pBoss->setPositionY(-50/*+800*/);
	lay->addChild(pBoss, 2);
	pBoss->setOpacity(0);
	if(iBossId==1037)
	{
		pBoss->setPositionX(180-300);
		pBoss->setPositionY(10/*+800*/);
		pBoss->setScale(2.5f);
	}

	pBoss->runAction(CCSequence::create( 
		CCDelayTime::create(2.2f), 
		CCFadeIn::create(0.0f), 
		CCEaseSineIn::create(CCMoveBy::create(1.4f, ccp(300, /*-800*/0))), 
		CCCallFuncN::create(this, callfuncN_selector(CWorldBoss::showBossSkill)),
		nullptr));
}

void CWorldBoss::showWorldBossEffectCallBack(CCNode* pSender)
{
	unschedule(schedule_selector(CWorldBoss::updateFrame));
	
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();

	m_pArmature = nullptr;

	showPanelUp();
}

void CWorldBoss::showPanelUp()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	//UI
	CLayout* pInfoLayout = (CLayout*)m_ui->findWidgetById("panel");
	pInfoLayout->setVisible(true);
	pInfoLayout->setPositionY(pInfoLayout->getPositionY()-winSize.height);

	//震动
	showShake(0.35f);

	//弹版块
	pInfoLayout->runAction(CCMoveBy::create(0.35f, ccp(0, winSize.height)));
}

void CWorldBoss::movementCallBack( CCArmature *armature, MovementEventType type, const char *data )
{
	switch (type)
	{
	case cocos2d::extension::START:
		{
			if(strcmp(data, "skill")==0)
			{
				schedule(schedule_selector(CWorldBoss::updateFrame));
			}
		}
		break;
	case cocos2d::extension::COMPLETE:
		{
			if(strcmp(data, "skill")==0)
			{
				unschedule(schedule_selector(CWorldBoss::updateFrame));
			}

			{
				//整个层渐隐消失
				showWorldBossEffectCallBack(m_pArmature->getParent());
				PlayEffectSound(SFX_425);
			}
		}
		break;
	case cocos2d::extension::LOOP_COMPLETE:
		break;
	default:
		break;
	}
}

void CWorldBoss::updateFrame( float dt )
{
	if(!LayerManager::instance()->getLayer("showWorldBossEffect2"))
	{
		unschedule(schedule_selector(CWorldBoss::updateFrame));
		return;
	}

	int iCurrentFrameIndex = m_pArmature->getAnimation()->getCurrentFrameIndex();
	if(m_lastFrame!=iCurrentFrameIndex)															//骨骼帧率与游戏帧率不同例如：游戏2帧骨骼才跑了1帧的情况(高效)
	{	
		for(unsigned int frameIndex = m_lastFrame+1; frameIndex<=iCurrentFrameIndex; frameIndex++)	//防止偶尔的掉帧情况出现处理
		{
			vector<ArmatureEvent*>& frameEvents = m_armatureEventData->getEventVector("skill", frameIndex);	//获取当前帧的所有事件
			for(unsigned int i=0; i<frameEvents.size(); i++)
			{
				const ArmatureEvent& armatureEvent = *(frameEvents.at(i));
				string sEventName = armatureEvent.name;
				if(sEventName.size()>0)
				{
					if(sEventName.compare(EVE_SOUND)			==0 )		PlaySound_Event(armatureEvent.sound);				//播放声音
				}
			}
		}
		m_lastFrame = iCurrentFrameIndex;
	}
}

void CWorldBoss::PlaySound_Event( int sData )
{
	char str[60] = {0};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	sprintf(str,"SFX/Test/%d.mp3",sData);											
#else
	sprintf(str,"SFX/%d.ogg",sData);												
#endif
	PlayEffectSound(str);				//播放效果音效
}

void CWorldBoss::showShake(float fTime)
{
	m_ui->setScale(1.02f);
	m_ui->runAction(CCSequence::createWithTwoActions(CCShake::create(fTime, 10.0f), CCCallFunc::create(this, callfunc_selector(CWorldBoss::showShakeCallBack))));
}

void CWorldBoss::showShakeCallBack()
{
	m_ui->setPosition(VCENTER);
	m_ui->setScale(1.0f);
}

void CWorldBoss::showBossSkill( CCNode* pSender )
{
	CCNode* pParent = pSender->getParent();
	//Boss
	if(m_pArmature->getAnimation()->getAnimationData()->getMovement("skill")!=nullptr)
	{
		m_pArmature->getAnimation()->setSpeedScale(1.0f);
		m_pArmature->getAnimation()->play("skill");
	}

	//最多3秒
	pParent->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(4.0f), CCCallFuncN::create(this, callfuncN_selector(CWorldBoss::showWorldBossEffectCallBack))));

	//白光
	CCSprite* ef = CCSprite::create("warScene/skillef.png");
	ef->setScale(2);
	ef->setZOrder(2);
	ef->setAnchorPoint(ccp(0,0));
	ef->setVisible(false);
	ef->runAction(CCSequence::create(CCShow::create(), CCRepeat::create(CCSequence::createWithTwoActions(CCFadeTo::create(0.25f,100), CCFadeTo::create(0.25f,200)), 8), CCRemoveSelf::create(true), NULL));
	pParent->addChild(ef, 3);
}
