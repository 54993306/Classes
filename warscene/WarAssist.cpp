#include "WarAssist.h"
#include "scene/loadWar.h"
#include "tools/ToolDefine.h"
#include "common/CommonFunction.h"
#include "tools/commonDef.h"
#include "GMessage.h"
#include "common/color.h"
#include "tools/CCShake.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "scene/layer/WarAliveLayer.h"
#include "warscene/WarControl.h"
#include "tools/ToolDefine.h"
#include "model/DataDefine.h"
#include "scene/alive/RageObject.h"
#include "scene/effect/EffectObject.h"
#include "scene/WarScene.h"
#include "cctk/scenemanager.h"
#include "ConstNum.h"
#include "scene/alive/HPObject.h"
#include "model/WarManager.h"
#include "warscene/ComBatLogic.h"
#include "common/CGameSound.h"
#include "scene/layer/WarMapLayer.h"
#include "common/CommonFunction.h"
#include "Global.h"
#include "model/MapManager.h"

WarAssist::WarAssist()
	:m_alive(nullptr)
	,m_ui(nullptr)
	,m_scene(nullptr){}

WarAssist::~WarAssist()
{
	NOTIFICATION->removeAllObservers(this);											//不解绑,map会崩找不到原因
}

bool WarAssist::init()
{
	NOTIFICATION->addObserver(this,callfuncO_selector(WarAssist::PlayBeginAnimation),PLAYERBEGINANIMATION,nullptr);
	return true;
}

void WarAssist::setScene(WarScene* scene)
{
	m_scene = scene;
	m_ui = scene->getWarUI();
	m_alive = scene->getWarAliveLayer();
}
void WarAssist::DisplayBossWarning(CCObject* ob,int model/* = 0*/)
{
	//model!=0 标记为章节最后关卡的大BOSS
	//1闪电-警告-2名字3左移4右移
	//run置为false
	m_scene->getCombatLogic()->setRunLogic(false);
	//动画真实执行时间
	float fTimeAnimation = 0;

	//播放声音
	if (ob)
	{
		m_ui = (WarControl*)ob;
	}
	PlayEffectSound(BGM_Boss_Warning);
	PlayBackgroundMusic(BGM_Boss1,true);
	PlayEffectSound(SFX_419);

	CCSize size = CCDirector::sharedDirector()->getWinSize();

	//红色阴影
	CCSprite* pShadow = CCSprite::create("warScene/yinyang.png");
	pShadow->setScale(2);
	pShadow->setZOrder(1);
	pShadow->setAnchorPoint(ccp(0,0));
	pShadow->setVisible(true);
	CCSequence* sqe = CCSequence::create(
		CCRepeat::create(CCSequence::create(CCFadeTo::create(0.25f,100), CCFadeTo::create(0.25f,200), NULL),5), 
		CCRemoveSelf::create(),NULL);
	pShadow->runAction(sqe);
	m_ui->addChild(pShadow, 2);

	//闪电
	EffectObject* pLighting = EffectObject::create("warning",EffectType::Repeat);
	pLighting->setPosition(VCENTER);
	pLighting->setScaleX(2);
	pLighting->setEffAnchorPoint(0.5,0.5);
	pLighting->setZOrder(2);
	pLighting->runAction(CCSequence::create(
		CCDelayTime::create(1.2f),
		CCFadeOut::create(0.3f), 
		CCRemoveSelf::create(), NULL));
	m_ui->addChild(pLighting, 2);

	//警告字样-boss下可能显示为方形图标
	CCSprite* pWarning = CCSprite::create(model != 0?"warScene/warning_2.png":"warScene/warning.png");
	pWarning->setPosition(ccp(size.width/2,-pWarning->getContentSize().height+10));
	CCSequence* sqe1 = CCSequence::create(
		CCMoveTo::create(0.25,ccp(size.width/2,size.height/2)), 
		CCDelayTime::create(1.2f),
		CCFadeOut::create(0.3f), 
		CCRemoveSelf::create(),NULL);
	pWarning->runAction(sqe1);
	pWarning->setZOrder(3);
	m_ui->addChild(pWarning, 2);

	//科技条
	if(model !=0)
	{
		float fTime = 2.4f;
		CCSprite* pUpBar1 = CCSprite::create("warScene/warning_bar.png");
		pUpBar1->setFlipY(true);
		pUpBar1->setAnchorPoint(ccp(0, 1));
		pUpBar1->setPosition(ccp(0, VIRTUAL_FIXED_HEIGHT));
		m_ui->addChild(pUpBar1, 3);
		pUpBar1->runAction(CCSequence::create(
			CCMoveBy::create(fTime, ccp(-DESIGN_WIDTH, 0)), 
			CCRepeat::create(CCSequence::createWithTwoActions(CCMoveBy::create(0, ccp(DESIGN_WIDTH*2, 0)), CCMoveBy::create(fTime*2, ccp(-DESIGN_WIDTH*2, 0))), 30), nullptr));
		CCSprite* pUpBar2 = CCSprite::create("warScene/warning_bar.png");
		pUpBar2->setFlipY(true);
		pUpBar2->setAnchorPoint(ccp(0, 1));
		pUpBar2->setPosition(ccp(DESIGN_WIDTH, VIRTUAL_FIXED_HEIGHT));
		pUpBar2->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(fTime*2, ccp(-DESIGN_WIDTH*2, 0)), CCMoveBy::create(0, ccp(DESIGN_WIDTH*2, 0)))));
		m_ui->addChild(pUpBar2, 3);

		CCSprite* pDownBar1 = CCSprite::create("warScene/warning_bar.png");
		pDownBar1->setAnchorPoint(ccp(1, 0));
		pDownBar1->setPosition(ccp(DESIGN_WIDTH, 0));
		m_ui->addChild(pDownBar1, 3);
		pDownBar1->runAction(CCSequence::create(
			CCMoveBy::create(fTime, ccp(DESIGN_WIDTH, 0)), 
			CCRepeat::create(CCSequence::createWithTwoActions(CCMoveBy::create(0, ccp(-DESIGN_WIDTH*2, 0)), CCMoveBy::create(fTime*2, ccp(DESIGN_WIDTH*2, 0))), 30), nullptr));
		CCSprite* pDownBar2 = CCSprite::create("warScene/warning_bar.png");
		pDownBar2->setAnchorPoint(ccp(1, 0));
		pDownBar2->setPosition(ccp(0, 0));
		m_ui->addChild(pDownBar2, 3);
		pDownBar2->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(fTime*2, ccp(DESIGN_WIDTH*2, 0)), CCMoveBy::create(0, ccp(-DESIGN_WIDTH*2, 0)))));

		pUpBar1->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCRemoveSelf::create()));
		pUpBar2->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCRemoveSelf::create()));
		pDownBar1->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCRemoveSelf::create()));
		pDownBar2->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCRemoveSelf::create()));
	}

	//上面的动画执行1.8s
	fTimeAnimation = 1.8f;

	//可能会有名字出现
	if(model != 0)
	{
		//名字
		CCSprite* pName = CCSprite::create(CCString::createWithFormat("warScene/%d.png", model)->getCString());
		if(pName)
		{
			pName->setPosition(VCENTER);
			pName->setScale(4.0);
			pName->setVisible(false);
			m_ui->addChild(pName, 1);
			pName->runAction(CCSequence::create(
				CCDelayTime::create(1.9f), 
				CCShow::create(),
				CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)),
				CCDelayTime::create(0.9f),
				CCSpawn::createWithTwoActions(CCFadeOut::create(0.3f), CCScaleTo::create(0.3f, 3.0f)),
				CCRemoveSelf::create(),
				nullptr
				));

			//白光
			CCSprite* pLightWhite = CCSprite::create("warScene/skillef.png");
			pLightWhite->setScale(2);
			pLightWhite->setZOrder(2);
			pLightWhite->setAnchorPoint(ccp(0,0));
			pLightWhite->setVisible(false);
			pLightWhite->runAction(CCSequence::create( 
				CCDelayTime::create(2.1f), 
				CCShow::create(), 
				CCFadeTo::create(0.25f,100), 
				CCFadeTo::create(0.25f,200), 
				CCRemoveSelf::create(true), NULL));
			m_ui->addChild(pLightWhite, 1);

			//如果执行播放名字的动画，时间会拉长到3.4s
			fTimeAnimation = 3.4f;
		}
		else
		{
			//没有找到对应图片，就不播名字了
			CCLOG("ERROR DisplayBossWarning %d", model);
		}
	}

	//是否有BOSS出场动画
	//int model = 1037;
	if(model != 0)
	{
		//拖屏幕,普通关卡才会拖屏幕//新手关卡,不滑
		if(DataCenter::sharedData()->getWar()->getNormal() && DataCenter::sharedData()->getWar()->getStageID() != 0)
		{
			CCNode* MoveLayer = m_scene->getMoveLayer();

			int newX1 = 0;
			int newX2 = MAP_MINX(DataCenter::sharedData()->getMap()->getCurrWarMap());

			float dx1 = (newX1 - MoveLayer->getPositionX())*0.3f;
			float dx2 = (newX2 - MoveLayer->getPositionX())*0.3f;

			float layx = 0;
			CLayout *lay = dynamic_cast<CLayout*>(m_scene->getWarMapLayer()->getMaplayer()->findWidgetById("yuanjing"));
			layx = lay->getPositionX();				//记录之前的位置

			MoveLayer->runAction(CCSequence::create(
				CCDelayTime::create(3.5f),
				CCMoveTo::create(0.2f, ccp(newX1, MoveLayer->getPositionY())),
				nullptr));

			lay->runAction(CCSequence::create(
				CCDelayTime::create(3.5f),
				CCMoveTo::create(0.2f, ccp(layx-dx1, lay->getPositionY())),
				nullptr));

			//如果只执行左移的动画，时间会拉长到3.8s
			fTimeAnimation = 3.8f;

		}
	}

	m_ui->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fTimeAnimation), CCCallFunc::create(this, callfunc_selector(WarAssist::callBackForDisplayBossWarning))));
}


void WarAssist::callBackForDisplayBossWarning()
{
	//run置回true
	m_scene->getCombatLogic()->setRunLogic(true);
}


void WarAssist::WinEffect(CCObject* ob)
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	CCNode* ziti = CCNode::create();
	CCSprite* _win = CCSprite::create("warscene2/win.png");
	/*
	CCSprite* zhan	= CCSprite::create("warscene2/font__03.png");
	CCSprite* dou	= CCSprite::create("warscene2/font__04.png");
	CCSprite* sheng = CCSprite::create("warscene2/font__05.png");
	CCSprite* li	= CCSprite::create("warscene2/font__06.png");
	zhan->setPosition(ccp( -dou->getContentSize().width - zhan->getContentSize().width/2,0));
	dou->setPosition(ccp( -dou->getContentSize().width/2,0));
	sheng->setPosition(ccp(sheng->getContentSize().width/2,0));
	li->setPosition(ccp(sheng->getContentSize().width + li->getContentSize().width/2,0));
	ziti->addChild(zhan);
	ziti->addChild(dou);
	ziti->addChild(sheng);
	ziti->addChild(li);
	*/
	ziti->addChild(_win);
	ziti->setZOrder(1);

	
	CCSprite* bg = CCSprite::create("warScene/point.png");
	bg->setScaleX(size.width);
	bg->setScaleY(size.height);
	bg->setVisible(true);
	CCFadeIn* fadein1 = CCFadeIn::create(0.2f);
	bg->runAction(fadein1);
	m_ui->addChild(bg);

	CCSprite* mo = CCSprite::create("public/mo.png");
	CCFadeIn* fadein = CCFadeIn::create(0.4f);
	mo->runAction(fadein);
	ziti->setPosition(ccp(size.width/2,size.height/2));
	ziti->setScale(2.0f);
	CCScaleTo* cs1 = CCScaleTo::create(0.5f,1.0); 
	CCEaseBounceOut * es= CCEaseBounceOut::create(cs1);
	CCCallFuncO* ef2 = CCCallFuncO::create(this,callfuncO_selector(WarAssist::shake),m_ui);
	CCDelayTime* dly = CCDelayTime::create(1.5f);
	CCCallFuncO* RoundOver = CCCallFuncO::create(m_scene->getCombatLogic(), callfuncO_selector(CombatLogic::CombatResult), CCInteger::create(win));
	CCSequence* sqe = CCSequence::create(es,ef2,dly,RoundOver, NULL);
	//ziti->setPosition(ccp(-size.width/2,size.height/2));
	//CCMoveTo* mt = CCMoveTo::create(0.5f,ccp(size.width/2,size.height/2));
	//CCEaseBounceOut * es= CCEaseBounceOut::create(mt);
	
	//CCSequence* sqe = CCSequence::create(es,dly,RoundOver,NULL);
	ziti->runAction(sqe);
	//CCScaleTo* cs2 = CCScaleTo::create(0.1)
	mo->setAnchorPoint(ccp(0.45,0.5));
	mo->setPosition(ccp(size.width/2,size.height/2));
	m_ui->addChild(ziti);
	m_ui->addChild(mo);

	_win->runAction(CCSequence::create(CCDelayTime::create(1.6f), CCFadeOut::create(1.0f), nullptr));
	mo->runAction(CCSequence::create(CCDelayTime::create(1.6f), CCFadeOut::create(1.0f), nullptr));
}

void WarAssist::playerMusic(CCObject* ob)
{
	CCInteger* music = (CCInteger*)ob;
	char musicAds[60] = {0};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	sprintf(musicAds,"BGM/Test/%d.mp3",music->getValue());
#else
	sprintf(musicAds,"BGM/%d.ogg",music->getValue());
#endif	
	PlayEffectSound(SFX_514);				//播放效果音效
}
//震动实现
void WarAssist::shake(CCObject* ob)
{
	if(!ob)return;
	CCNode* node = (CCNode*)ob;
	node->getParent()->runAction(CCShake::create(0.3f,10.0f));
}

bool WarAssist::lastStory()
{
	char str[60] = {0};
	sprintf(str,"csv/story/last_%d.csv",DataCenter::sharedData()->getWar()->getStageID());
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(str);
	if (file)return true;
	return false;
}

void WarAssist::ActStandExcute( CCArray* arr )
{
	if (arr)
	{
		CCObject* obj= nullptr;
		CCARRAY_FOREACH(arr,obj)
		{
			ActObject* act = (ActObject*)obj;
			act->setMoveState(0);
			act->TurnStateTo(Stand_Index);
		}
	}
}

void WarAssist::PlayBeginAnimation(CCObject* obj)
{
	CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData("taiwen");
	if (animationData)
	{
		CCArmature*Armature = CCArmature::create("taiwen");
		Armature->getAnimation()->play("open");
		Armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(WarAssist::animationEvent));
		Armature->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(WarAssist::frameEvent));
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		Armature->setPosition(ccp(size.width/2,size.height/2));
		m_scene->addChild(Armature);

		PlayBackgroundMusic(BGM_Prologue, true);
	}
	else
	{
		CCLOG("[ *ERROR ] WarAssist::PlayBeginAnimation animationData NULL");
		CCNotificationCenter::sharedNotificationCenter()->postNotification(LAYERMOVEEND,CCInteger::create(1));
	}
}

void WarAssist::animationEvent( CCArmature *armature,MovementEventType movementType, const char *movementID )
{
	std::string id = movementID;
	id = strRemoveSpace(id);
	if (movementType == LOOP_COMPLETE || COMPLETE==movementType)	//动作结束的标记
	{
		armature->removeFromParentAndCleanup(true);
		if (id.compare("open")==0)
		{
			int beginStory = 1;
			CCNotificationCenter::sharedNotificationCenter()->postNotification(LAYERMOVEEND,CCInteger::create(beginStory));
		}
	}
}

void WarAssist::frameEvent( CCBone *pBone, const char *str, int a, int b )
{
	if(strcmp(str, "boss_come"))
	{
		PlayEffectSound(SFX_544);
	}
	else if(strcmp(str, "boss_words"))
	{
		PlayEffectSound(SFX_543);
	}
}
