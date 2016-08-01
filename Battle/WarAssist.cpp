#include "WarAssist.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "tools/ToolDefine.h"
#include "common/CommonFunction.h"
#include "tools/commonDef.h"
#include "common/color.h"
#include "tools/CCShake.h"
#include "model/DataCenter.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/BattleLayer/BattleRoleLayer.h"
#include "Battle/WarControl.h"
#include "tools/ToolDefine.h"
#include "Battle/BaseRole.h"
#include "Battle/RoleObject/RageObject.h"
#include "Battle/EffectObject.h"
#include "Battle/BattleScene/BattleScene.h"
#include "cctk/scenemanager.h"
#include "Battle/ConstNum.h"
#include "Battle/RoleObject/HPObject.h"
#include "model/WarManager.h"
#include "Battle/ComBatLogic.h"
#include "common/CGameSound.h"
#include "Battle/BattleLayer/BattleMapLayer.h"
#include "common/CommonFunction.h"
#include "Global.h"
#include "model/MapManager.h"
#include "Battle/BattleMessage.h"
#include "model/CWholeBodyShowData.h"
#include "Battle/BattleTools.h"

namespace BattleSpace
{
	WarAssist::WarAssist()
	:m_alive(nullptr),m_ui(nullptr),m_scene(nullptr),mManage(nullptr),m_bShowWarning(false),m_bIsBigBoss(false),m_iModel(-1)
	,mBossRole(nullptr)
	{}

	WarAssist::~WarAssist()
	{
		NOTIFICATION->removeAllObservers(this);											//不解绑,map会崩找不到原因
	}

	bool WarAssist::init()
	{
		NOTIFICATION->addObserver(this,callfuncO_selector(WarAssist::PlayBeginAnimation),B_PlayBeginAnimation,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(WarAssist::bigMonsterInBattle),MsgRoleGridChange,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(WarAssist::starActionEnd),MsgStarActionEnd,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(WarAssist::WinEffect),MsgPlayWinEffect,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(WarAssist::displayBossWarningBegin),MsgShowStageWarning,nullptr);
		mManage = DataCenter::sharedData()->getWar();
		return true;
	}

	void WarAssist::setScene(BattleScene* scene)
	{
		m_scene = scene;
		m_ui = scene->getWarUI();
		m_alive = scene->getBattleRoleLayer();
	}

	void WarAssist::displayBossWarningBegin(BaseRole* pRole/*= nullptr*/)
	{
		NOTIFICATION->postNotification(MsgReleaseTouch);									//释放掉触摸消息
		int iModel = 0;
		if (pRole)
		{
			iModel = pRole->getModel();
			mBossRole = pRole;
		}
		//加锁
		if(m_bShowWarning)
		{
			return;
		}
		m_bShowWarning = true;

		//CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.2f);

		//战斗逻辑关闭
		mManage->setLogicState(false);
		m_scene->setMoveState(false);
		//流程不对，不能多线一起执行，考虑单线动画，CCSequence-( CCArray )..动作拼接太麻烦，多写几个callback吧
		//执行动画
		//model!=0 标记为章节最后关卡的大BOSS
		//old-1闪电-警告-2名字3左移4右移
		//new-0 地图缩放至75%  1闪电-警告-科技条 2地图移动 3.boss出场动作 4名字，称号（白光进。白光出）5.地图缩放至87%+白光+震动 6.地图缩放至100%+震动
		
		m_bIsBigBoss = iModel!=0;

		if(m_iModel == -1)
		{
			m_iModel = iModel;
		}

		changeUiVisiable(false);

		//播放BOSS音乐
		PlayBackgroundMusic(BGM_Boss1,true);
		
		CCNode* pMoveLay = m_scene->getMoveLayer();
		pMoveLay->setContentSize(CCSize(DESIGN_WIDTH*2, VIRTUAL_FIXED_HEIGHT));
		m_pPosCopy = pMoveLay->getPosition();

		//-1 补充位置
		bigBossCallback_1();
		////回调-1
		////非引导界面缩放，引导界面，不缩放(补偿偏移)
		//void bigBossCallback_1();
		////回调0
		////非引导界面缩放，引导界面，不缩放(移动到左侧目标位置)
		//void bigBossCallback0();
		////回调1
		////显示warning
		//void bigBossCallback1();
		////回调2
		////拖屏幕,普通关卡才会拖屏幕到最左边//精英。引导关卡,不滑
		//void bigBossCallback2();
		////回调3
		////boss出场动作已经播放了？
		//void bigBossCallback3();
		////回调4
		////4名字称号
		//void bigBossCallback4();
		////回调5
		////地图缩放至87%
		////非精英且非引导才有
		//void bigBossCallback5();
		////回调6
		////地图缩放至100%+震动
		////非引导才有
		//void bigBossCallback6();
		////回调7
		////精英关卡置回右边
		//void bigBossCallback7();
	}



	float WarAssist::fixMoveLayBorder(float fGap )
	{
		//如果屏幕左右有临界现象
		{
			CCNode* pMoveLayer = m_scene->getMoveLayer();
			int iRealWidth = pMoveLayer->getContentSize().width*pMoveLayer->getScaleX();
			int iRealHeight = pMoveLayer->getContentSize().height*pMoveLayer->getScaleY();
			//最左边显示位置
			float fLeft = pMoveLayer->getPositionX()-pMoveLayer->getAnchorPoint().x*iRealWidth;
			float fRight = pMoveLayer->getPositionX()+(1-pMoveLayer->getAnchorPoint().x)*iRealWidth;
			//左边预留不够
			float fLOffX = -fGap-fLeft;
			if(fLOffX < 0)
			{
				float fTime = moveLayMoveBy(ccp(fLOffX*1.05f, 0));
				return fTime;
			}

			//右边预留不够
			float fROffX = (VRIGHT+150)-fRight;
			if( fROffX > 0)
			{
				float fTime = moveLayMoveBy(ccp(fROffX*1.05f, 0));
				return fTime;
			}
		}
			return 0.0f;
	}

	float WarAssist::moveLayScaleStart( float fScale, float fTime )
	{
		if(m_bIsBigBoss)
		{
			//重置地图锚点
			CCPoint newAnchor = getMoveLayAnchorpointWithSceneBottomCenter();
			moveLayResetAnchorpointKeepSamePos(newAnchor);
			//地图缩回, 后重置锚点为
			moveLayScaleTo(fScale, fTime);

			return fTime;
		}
		else
		{
			return 0.0f;
		}
	}

	void WarAssist::displayBossWarningEnd()
	{
		
		//保证moveLay的位置和缩放比例
		moveLayResetAnchorPoint();
		CCNode* pMoveLay = m_scene->getMoveLayer();
		pMoveLay->setScale(1.0f);
		pMoveLay->setAnchorPoint(CCPointZero);
		pMoveLay->setPosition(m_pPosCopy);

		changeUiVisiable(true);

		//战斗逻辑开放
		mManage->setLogicState(true);
		m_scene->setMoveState(true);
		mBossRole = nullptr;
		m_bShowWarning = false;

		//CCInteger *pInteger = CCInteger::create(1051);
		//pInteger->retain();
		//m_ui->runAction(CCSequence::create(
		//	CCDelayTime::create(3.0f), 
		//	CCCallFuncO::create(this, callfuncO_selector(WarAssist::displayBossWarningBegin), pInteger),
		//	nullptr));
	}

	CCPoint WarAssist::getMoveLayAnchorpointWithSceneBottomCenter()
	{
		CCNode* pMoveLay = m_scene->getMoveLayer();
		CCPoint centerBottomPosInMoveLay = pMoveLay->convertToNodeSpace(ccp(VCENTER.x, VBOTTOM));

		//获取新的锚点
		CCPoint newAnchor = ccp(
			centerBottomPosInMoveLay.x/(pMoveLay->getContentSize().width*pMoveLay->getScaleX()),
			centerBottomPosInMoveLay.y/(pMoveLay->getContentSize().height*pMoveLay->getScaleY())
			);

		return newAnchor;
	}

	void WarAssist::moveLayResetAnchorpointKeepSamePos( CCPoint anchorpoint )
	{
		CCNode* pMoveLay = m_scene->getMoveLayer();
		int iWidth = pMoveLay->getContentSize().width*pMoveLay->getScaleX();
		int iHeight = pMoveLay->getContentSize().height*pMoveLay->getScaleY();
		CCPoint oldAnchor = pMoveLay->getAnchorPoint();

		CCPoint offPos = ccp(
			(anchorpoint.x-oldAnchor.x)*iWidth, 
			(anchorpoint.y-oldAnchor.y)*iHeight);

		CCPoint newPos = pMoveLay->getPosition() + offPos;
		pMoveLay->setPosition(newPos);
		pMoveLay->setAnchorPoint(anchorpoint);

	}

	void WarAssist::moveLayScaleTo( float fScale, float fTime )
	{
		CCNode* pMoveLay = m_scene->getMoveLayer();
		pMoveLay->runAction(
			CCSequence::create(
			CCScaleTo::create(fTime, fScale),
			CCCallFunc::create(this, callfunc_selector(WarAssist::moveLayResetAnchorPoint)),
			nullptr
			)
			);
	}


	float WarAssist::moveLayMoveTo( CCPoint pos  )
	{
		//非boss不做
		if( !m_bIsBigBoss )
		{
			return 0.0f;
		}

		{
			CCNode* MoveLayer = m_scene->getMoveLayer();

			int newX1 = pos.x;

			float dx1 = (newX1 - MoveLayer->getPositionX())*0.3f;

			float fTime = fabs(newX1 - MoveLayer->getPositionX())/800;

			float layx = 0;
			CLayout *lay = dynamic_cast<CLayout*>(m_scene->getBattleMapLayer()->getMaplayer()->findWidgetById("yuanjing"));
			layx = lay->getPositionX();				//记录之前的位置

			MoveLayer->runAction(CCSequence::create(
				
				CCMoveTo::create(fTime, ccp(newX1, MoveLayer->getPositionY())),
				nullptr));

			lay->runAction(CCSequence::create(
				
				CCMoveTo::create(fTime, ccp(layx-dx1, lay->getPositionY())),
				nullptr));

			return fTime;
		}

		return 0.0f;
	}


	float WarAssist::moveLayMoveBy( CCPoint pos )
	{
		if(!m_bIsBigBoss)
		{
			return 0.0f;
		}

		CCNode* MoveLayer = m_scene->getMoveLayer();

		float fRealTime = fabs(pos.x)/800;

		int iOffx = pos.x;

		float dx1 = -iOffx*0.3f;

		CLayout *lay = dynamic_cast<CLayout*>(m_scene->getBattleMapLayer()->getMaplayer()->findWidgetById("yuanjing"));

		MoveLayer->runAction(CCSequence::create(
			CCMoveBy::create(fRealTime, ccp(iOffx, 0)),
			nullptr));

		lay->runAction(CCSequence::create(
			CCMoveBy::create(fRealTime, ccp(dx1, 0)),
			nullptr));

		return fRealTime;
	}


	float WarAssist::addRedShadow( )
	{
		PlayEffectSound(SFX_419);

		//红色阴影
		CCSprite* pShadow = CCSprite::create("warScene/yinyang.png");
		pShadow->setScale(2);
		pShadow->setZOrder(1);
		pShadow->setAnchorPoint(ccp(0,0));
		pShadow->setVisible(false);
		CCSequence* sqe = CCSequence::create(
			CCShow::create(),
			CCRepeat::create(CCSequence::create(CCFadeTo::create(0.25f,100), CCFadeTo::create(0.25f,200), nullptr),5), 
			CCRemoveSelf::create(), nullptr);
		pShadow->runAction(sqe);
		m_ui->addChild(pShadow, 2);

		return 1.25f;
	}

	float WarAssist::addLighting(  )
	{
		PlayEffectSound(BGM_Boss_Warning);

		//闪电
		EffectObject* pLighting = EffectObject::create("warning",sPlayType::eRepeat);
		pLighting->setPosition(VCENTER);
		pLighting->setScaleX(2);
		pLighting->setEffAnchorPoint(0.5,0.5);
		pLighting->setZOrder(2);
		pLighting->setVisible(false);
		pLighting->runAction(CCSequence::create(
			CCShow::create(),
			CCDelayTime::create(1.5f),
			CCFadeOut::create(0.3f),
			CCRemoveSelf::create(), 
			nullptr));
		m_ui->addChild(pLighting, 2);

		return 1.8f;
	}

	float WarAssist::addWarningTip(  )
	{
		CCSize size = CCDirector::sharedDirector()->getWinSize();

		//警告字样-boss下可能显示为方形图标
		CCSprite* pWarning = CCSprite::create(m_bIsBigBoss?"warScene/warning_2.png":"warScene/warning.png");
		pWarning->setPosition(ccp(size.width/2,size.height/2));
		pWarning->setScale(3.0);
		pWarning->setVisible(false);
		CCSequence* sqe1 = CCSequence::create(
			CCShow::create(),
			CCEaseBackOut::create(CCScaleTo::create(0.2f, 1.0f)),
			CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.1f), CCScaleTo::create(0.2f, 1.0f)), 4),
			CCSpawn::createWithTwoActions(CCFadeOut::create(0.2f), CCScaleTo::create(0.2f, 3.0f)),
			CCRemoveSelf::create(),
			nullptr);
		pWarning->runAction(sqe1);
		pWarning->setZOrder(3);
		m_ui->addChild(pWarning, 3);

		return 1.8f;
	}

	float WarAssist::addWhiteLight( float fDelay )
	{
		//白光
		CCSprite* pLightWhite = CCSprite::create("warScene/skillef.png");
		pLightWhite->setScale(2);
		pLightWhite->setZOrder(2);
		pLightWhite->setAnchorPoint(ccp(0,0));
		pLightWhite->setVisible(false);
		pLightWhite->runAction(CCSequence::create(
			CCDelayTime::create(fDelay),
			CCShow::create(), 
			CCFadeTo::create(0.2f,100),
			CCFadeOut::create(0.1f),
			CCFadeTo::create(0.2f,200), 
			CCRemoveSelf::create(true), nullptr));
		m_ui->addChild(pLightWhite, 1);

		return 0.5f;
	}

	float WarAssist::addYellowBar(  )
	{
		if(m_bIsBigBoss)
		{
			float fTime = 1.6f;
			CCSprite* pUpBar1 = CCSprite::create("warScene/warning_bar.png");
			pUpBar1->setFlipY(true);
			pUpBar1->setAnchorPoint(ccp(0, 1));
			pUpBar1->setPosition(ccp(0, VIRTUAL_FIXED_HEIGHT));
			pUpBar1->setVisible(false);
			m_ui->addChild(pUpBar1, 3);
			pUpBar1->runAction(CCSequence::create(
				CCShow::create(),
				CCMoveBy::create(fTime, ccp(-DESIGN_WIDTH, 0)), 
				CCRepeat::create(CCSequence::createWithTwoActions(CCMoveBy::create(0, ccp(DESIGN_WIDTH*2, 0)), CCMoveBy::create(fTime*2, ccp(-DESIGN_WIDTH*2, 0))), 30), nullptr));
			CCSprite* pUpBar2 = CCSprite::create("warScene/warning_bar.png");
			pUpBar2->setFlipY(true);
			pUpBar2->setAnchorPoint(ccp(0, 1));
			pUpBar2->setPosition(ccp(DESIGN_WIDTH, VIRTUAL_FIXED_HEIGHT));
			pUpBar2->setVisible(false);
			pUpBar2->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(fTime*2, ccp(-DESIGN_WIDTH*2, 0)), CCMoveBy::create(0, ccp(DESIGN_WIDTH*2, 0)))));
			m_ui->addChild(pUpBar2, 3);
			pUpBar2->runAction(CCSequence::create(
				CCShow::create(),
				nullptr));


			CCSprite* pDownBar1 = CCSprite::create("warScene/warning_bar.png");
			pDownBar1->setAnchorPoint(ccp(1, 0));
			pDownBar1->setPosition(ccp(DESIGN_WIDTH, 0));
			pDownBar1->setVisible(false);
			m_ui->addChild(pDownBar1, 3);
			pDownBar1->runAction(CCSequence::create(
				CCShow::create(),
				CCMoveBy::create(fTime, ccp(DESIGN_WIDTH, 0)), 
				CCRepeat::create(CCSequence::createWithTwoActions(CCMoveBy::create(0, ccp(-DESIGN_WIDTH*2, 0)), CCMoveBy::create(fTime*2, ccp(DESIGN_WIDTH*2, 0))), 30), nullptr));
			CCSprite* pDownBar2 = CCSprite::create("warScene/warning_bar.png");
			pDownBar2->setAnchorPoint(ccp(1, 0));
			pDownBar2->setPosition(ccp(0, 0));
			pDownBar2->setVisible(false);
			m_ui->addChild(pDownBar2, 3);
			pDownBar2->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(fTime*2, ccp(DESIGN_WIDTH*2, 0)), CCMoveBy::create(0, ccp(-DESIGN_WIDTH*2, 0)))));
			pDownBar2->runAction(CCSequence::create(
				CCShow::create(),
				nullptr));

			pUpBar1->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCRemoveSelf::create()));
			pUpBar2->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCRemoveSelf::create()));
			pDownBar1->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCRemoveSelf::create()));
			pDownBar2->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCRemoveSelf::create()));

			return 1.6f;
		}

		return 0.0f;
	}


	float WarAssist::addInfoPanel( )
	{
		if(!m_bIsBigBoss)
		{
			return 0.0f;
		}

		//背景图
		CCSprite *pBg = CCSprite::create("warScene/boss_bg.jpg");
		pBg->setPosition(ccp(DESIGN_WIDTH/2, VIRTUAL_FIXED_HEIGHT/2));
		pBg->setCascadeColorEnabled(true);
		m_ui->addChild(pBg, 3);

		const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(m_iModel);

		//模型图
		//增加人物贴图
		const CWholeBodyShowInfo& pInfo = CWholeBodyShowData::getInstance()->getDataById(m_iModel);
		CCSprite* pBody = CCSprite::create(CCString::createWithFormat("selectRole/%d.png", m_iModel)->getCString());
		if (!pBody)
		{	
			pBody = CCSprite::create(CCString::createWithFormat("selectRole/%d.png", 111)->getCString());
		}
		pBody->setScale(pInfo.fScale*0.7f);
		pBody->setFlipX(pInfo.bFlipX);
		pBody->setPosition(ccp(DESIGN_WIDTH/2, VIRTUAL_FIXED_HEIGHT/2) + pInfo.pos+ccp(-20, -50));
		pBody->setRotation(pInfo.iRoate);
		pBg->addChild(pBody);

		//红色底
		CCSprite *pRed = CCSprite::create("warScene/hard_03.png");
		pRed->setPosition(ccp(240, 511));
		pRed->setCascadeOpacityEnabled(true);
		pBg->addChild(pRed);
		//称号
		CCSprite *pTitle = CCSprite::create(CCString::createWithFormat("warScene/zombie_0%d.png", data!=nullptr?data->iType3:1)->getCString());
		if(!pTitle)
		{
			pTitle = CCSprite::create("warScene/zombie_01.png");
		}
		pTitle->setPosition(ccp(pRed->getContentSize().width/2, pRed->getContentSize().height/2));
		pRed->addChild(pTitle);

		//名字
		CCSprite *pName = CCSprite::create(CCString::createWithFormat("warScene/%d.png", m_iModel)->getCString());
		if(!pName)
		{
			pName = CCSprite::create("warScene/1037.png");
		}
		pName->setPosition(ccp(DESIGN_WIDTH/2, VIRTUAL_FIXED_HEIGHT/2-150));
		pBg->addChild(pName, 9);


		//名字动作
		pName->setScale(4.0);
		pName->setVisible(false);
		pName->runAction(CCSequence::create(
			CCShow::create(),
			CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)),
			CCDelayTime::create(0.9f),
			CCSpawn::createWithTwoActions(CCFadeOut::create(0.3f), CCScaleTo::create(0.3f, 3.0f)),
			CCRemoveSelf::create(),
			nullptr
			));

		//称号动作
		pRed->setOpacity(0);
		pRed->runAction(
			CCSequence::create(
				CCDelayTime::create(0.3f),
				CCFadeIn::create(0.4f),
				nullptr
			)
			);

		//白光出//TODO 1.2
		addWhiteLight( 1.2f );


		//背景消失
		pBg->setVisible(false);
		pBg->runAction(
			CCSequence::create(
			CCShow::create(),
			CCDelayTime::create(1.6f),
			//CCFadeOut::create(0.2f),
			CCRemoveSelf::create(),
			nullptr
			)

			);

		return 1.6f;
	}

	float WarAssist::moveLayShake( float fDelay )
	{
		CCNode* pMoveLay = m_scene->getMoveLayer();

		CCPoint pPosOld = pMoveLay->getPosition();

		pMoveLay->runAction(CCSequence::create(
			CCDelayTime::create(fDelay),
			CCShake::create(0.2f, 8),
			nullptr
			));

		return 0.2f;
	}

	void WarAssist::bossShowAppearAction()
	{

	}


	void WarAssist::moveLayResetAnchorPoint()
	{
		moveLayResetAnchorpointKeepSamePos(CCPointZero);
	}

	float WarAssist::addFightText(  )
	{
		CCSprite *pFight = CCSprite::create("warScene/fight.png");
		//名字动作
		pFight->setScale(4.0);
		pFight->setVisible(false);
		pFight->setPosition(ccp(DESIGN_WIDTH/2, VIRTUAL_FIXED_HEIGHT/2));
		m_ui->addChild(pFight, 2);
		pFight->runAction(CCSequence::create(
			CCShow::create(),
			CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)),
			CCDelayTime::create(0.9f),
			//CCSpawn::createWithTwoActions(CCFadeOut::create(0.3f), CCScaleTo::create(0.3f, 3.0f)),
			CCFadeOut::create(0.5f),
			CCRemoveSelf::create(),
			nullptr
			));

		return 1.7f;
	}


	void WarAssist::bigBossCallback_1()
	{
		//非引导界面才做 ， 引导界面，不缩放,
		if(mManage->getStageID())
		{
			float fDelay = fixMoveLayBorder(190);	//(1138/2)/3
			m_ui->runAction(
				CCSequence::createWithTwoActions(
				CCDelayTime::create(fDelay),
				CCCallFunc::create(this, callfunc_selector(WarAssist::bigBossCallback0))
				));
		}
		else
		{
			bigBossCallback0();
		}
	}

	void WarAssist::bigBossCallback0()
	{
		//0
		//非引导界面缩放，引导界面，不缩放
		if(mManage->getStageID())
		{
			float fDelay =  moveLayScaleStart(0.75f, 0.5f);

			m_ui->runAction(
				CCSequence::createWithTwoActions(
				CCDelayTime::create(fDelay),
				CCCallFunc::create(this, callfunc_selector(WarAssist::bigBossCallback1))
				));
		}
		else
		{
			bigBossCallback1();
		}
	}

	void WarAssist::bigBossCallback1()
	{
		//1
		{
			//显示warning
			float fDelay1 = addRedShadow();
			float fDelay2 = addLighting();
			float fDelay3 = addWarningTip();
			//科技条
			float fDelay4 = /*addYellowBar()*/0;

			float fDelay = max(max(max(fDelay1, fDelay2), fDelay3), fDelay4);
			m_ui->runAction(
				CCSequence::createWithTwoActions(
				CCDelayTime::create(fDelay),
				CCCallFunc::create(this, callfunc_selector(WarAssist::bigBossCallback2))
				));
		}
	}

	void WarAssist::bigBossCallback2()
	{
		//2
		//拖屏幕,普通关卡才会拖屏幕到最左边//精英。引导关卡,不滑
		if(mManage->getNormal() && mManage->getStageID())
		{
			//地图移动
			float fDelay = moveLayMoveTo(ccp(0, 0));
			m_ui->runAction(
				CCSequence::createWithTwoActions(
				CCDelayTime::create(fDelay),
				CCCallFunc::create(this, callfunc_selector(WarAssist::bigBossCallback3))
				));
		}
		else
		{
			bigBossCallback3();
		}
	}

	void WarAssist::bigBossCallback3()
	{
		//3
		{
			if(m_bIsBigBoss)
			{
				//boss出场动作已经播放了？
				mBossRole->getRoleObject()->setVisible(true);
				mBossRole->getRoleObject()->TurnStateTo(sStateCode::eEnterState);
			}
			else
			{
				bigBossCallback4();
			}
		}
	}

	void WarAssist::bigBossCallback4()
	{
		//4名字称号, 非引导
		if(m_bIsBigBoss && mManage->getStageID())
		{
			float fDelay = addInfoPanel();
			m_ui->runAction(
				CCSequence::createWithTwoActions(
				CCDelayTime::create(fDelay),
				CCCallFunc::create(this, callfunc_selector(WarAssist::bigBossCallback5))
				));
		}
		else
		{
			bigBossCallback5();
		}
	}

	void WarAssist::bigBossCallback5()
	{
		//5地图缩放至87%
		//非引导才有
		if( mManage->getStageID() )
		{
			float fDealyB = addWhiteLight();

			float fDelayA1 = moveLayScaleStart(0.87f, 0.25f);
			float fDelayA2 = moveLayShake(fDelayA1);
			float fDelayA = fDelayA1 + fDelayA2 + 0.05f;
			m_ui->runAction(
				CCSequence::createWithTwoActions(
				CCDelayTime::create(fDelayA),
				CCCallFunc::create(this, callfunc_selector(WarAssist::bigBossCallback6))
				));
		}
		else
		{
			bigBossCallback6();
		}
	}

	void WarAssist::bigBossCallback6()
	{

		//6.地图缩放至100%+震动
		//非引导才有
		if( mManage->getStageID() )
		{
			//战斗
			float fDelayC = addFightText();

			float fDelayB = addWhiteLight();

			float fDelayA1 = moveLayScaleStart(1.0f, 0.25f);
			float fDelayA2 = moveLayShake(fDelayA1);
			float fDelayA = fDelayA1 + fDelayA2;

			float fDelay = max(max(fDelayA, fDelayB), fDelayC);
			m_ui->runAction(
				CCSequence::createWithTwoActions(
				CCDelayTime::create(fDelayA),
				CCCallFunc::create(this, callfunc_selector(WarAssist::bigBossCallback7))
				));
		}
		else
		{
			bigBossCallback7();
		}
	}

	void WarAssist::bigBossCallback7()
	{
		//7
		//精英关卡置回右边
		if( !mManage->getNormal() && DataCenter::sharedData()->getWar()->getStageID())
		{
			float fDelay = fixMoveLayBorder(0);
			m_ui->runAction(
				CCSequence::createWithTwoActions(
				CCDelayTime::create(fDelay),
				CCCallFunc::create(this, callfunc_selector(WarAssist::displayBossWarningEnd))
				));
		}
		else
		{
			displayBossWarningEnd();
		}
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
		CCCallFunc* win = CCCallFunc::create(this,callfunc_selector(WarAssist::battleWin));
		CCSequence* sqe = CCSequence::create(es,ef2,dly,win, nullptr);
		//ziti->setPosition(ccp(-size.width/2,size.height/2));
		//CCMoveTo* mt = CCMoveTo::create(0.5f,ccp(size.width/2,size.height/2));
		//CCEaseBounceOut * es= CCEaseBounceOut::create(mt);

		//CCSequence* sqe = CCSequence::create(es,dly,RoundOver,nullptr);
		ziti->runAction(sqe);
		//CCScaleTo* cs2 = CCScaleTo::create(0.1)
		mo->setAnchorPoint(ccp(0.45,0.5));
		mo->setPosition(ccp(size.width/2,size.height/2));
		m_ui->addChild(ziti);
		m_ui->addChild(mo);

		_win->runAction(CCSequence::create(CCDelayTime::create(1.6f), CCFadeOut::create(1.0f), nullptr));
		mo->runAction(CCSequence::create(CCDelayTime::create(1.6f), CCFadeOut::create(1.0f), nullptr));
	}

	void WarAssist::battleWin()
	{
		NOTIFICATION->postNotification(MsgBattleOver,CCBool::create(true));
	}

	void WarAssist::playerMusic(CCObject* ob)
	{
		CCInteger* music = (CCInteger*)ob;
		char musicAds[60] = {0};
#if BATTLE_TEST
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
			CCLOG("[ *ERROR ] WarAssist::PlayBeginAnimation animationData nullptr");
			bNotification->postNotification(MsgCreateStory,CCInteger::create(1));
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
				bNotification->postNotification(MsgCreateStory,CCInteger::create(beginStory));
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

	void WarAssist::changeUiVisiable( bool bVisiable )
	{
		CCObject *pObj;
		CCARRAY_FOREACH(m_ui->getChildren(), pObj)
		{
			CLayout *pNode = dynamic_cast<CLayout *>(pObj);
			if(pNode != nullptr)
			{
				pNode->setVisible(bVisiable);
				break;
			}
			//pNode->setPosition(poin);
		}
	}

	void WarAssist::bigMonsterInBattle( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (!isBigBoss(tRole)									||
			!tRole->getRoleObject()->getFirstBattle()			||
			converCol(tRole->getGridIndex())	 < C_GRID_COL_MIN	)
			return;
		displayBossWarningBegin(tRole);
	}

	void WarAssist::starActionEnd( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (isBigBoss(tRole))
			bigBossCallback4();
	}

	bool WarAssist::isBigBoss( BaseRole* pRole )
	{
		if(pRole == nullptr || !pRole->getRoleObject())
			return false;
		if (pRole->getEnemy()&&pRole->getMonsterSpecies() == sMonsterSpecies::eBoss	)
			return true;
		return false;
	}

};