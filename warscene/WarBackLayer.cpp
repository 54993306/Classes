#include "WarBackLayer.h"
#include "scene/CPopTip.h"
#include "Battle/BattleScene/BattleScene.h"
#include "model/DataCenter.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "netcontrol/CPlayerControl.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "common/CGameSound.h"
#include "jni/CJniHelper.h"
#include "warscene/ComBatLogic.h"
#include "Battle/BattleMessage.h"

WarBackLayer::WarBackLayer()
{
	m_ui = nullptr;
	m_bSoundLock = false;
}
using namespace BattleSpace;
bool WarBackLayer::init()
{
	bool res = BaseLayer::init();
	do{
		CC_BREAK_IF(!res);
		this->setTouchPriority(-12);

		setVisible(true);

		MaskLayer* lay = MaskLayer::create("WarBackLayerMask");
		lay->setContentSize(CCSizeMake(2824,640));
		this->addChild(lay);

		m_ui = LoadComponent("BattleSet.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);

		//退出战斗
		CButton * pExitBattle = (CButton*)(m_ui->findWidgetById("exit"));
		pExitBattle->getSelectedImage()->setScale(1.1f);
		pExitBattle->setOnClickListener(this,ccw_click_selector(WarBackLayer::onExitBattle));

		//特效声音
		CToggleView *effMusic = (CToggleView*)(m_ui->findWidgetById("effMusic"));
		effMusic->setOnCheckListener(this,ccw_check_selector(WarBackLayer::onEffMusic));

		//背景声音
		CToggleView *bgMusic = (CToggleView*)(m_ui->findWidgetById("bgMusic"));
		bgMusic->setOnCheckListener(this,ccw_check_selector(WarBackLayer::onBgMusic));

		bool ison = GameSound->isEffectOn();
		effMusic->setChecked(ison);
		showEffMusicLab(ison);
		ison = GameSound->isMusicOn();
		bgMusic->setChecked(ison);
		showBgMusicLab(ison);
		m_bSoundLock = true;

		//继续战斗
		CButton * pResumeBattle = (CButton*)(m_ui->findWidgetById("resume"));
		pResumeBattle->getSelectedImage()->setScale(1.1f);
		pResumeBattle->setOnClickListener(this,ccw_click_selector(WarBackLayer::onResumeBattle));

	} while (0);
	return res;
}


void WarBackLayer::onEnter()
{
	BaseLayer::onEnter();
}

void WarBackLayer::onExit()
{
	BaseLayer::onExit();
}

void WarBackLayer::touchExit()
{
	CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);

	CPopTip *tip = CPopTip::create();
	LayerManager::instance()->pop();//pop掉masklayer
	tip->setTouchEnabled(true);
	tip->setTouchPriority(-100);
	tip->addContentTip(GETLANGSTR(1010));
	tip->setButtonLisener(this,ccw_click_selector(WarBackLayer::returnCity));
	tip->setTag(tips_tag);
	this->addChild(tip, 999);
}


void WarBackLayer::returnCity(CCObject* ob)
{
	if(m_bSoundLock)
	{
		PlayEffectSound(SFX_Button);
	}

	CButton* btn = (CButton*)ob;
	int tag = btn->getTag();
	if (tag == PopTipCancel)
	{
		this->removeChildByTag(tips_tag);
	}else{

		//如果是在引导,重启游戏
		if( !DataCenter::sharedData()->getWar()->getStageID())
		{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			CJniHelper::getInstance()->restartGame();
#endif
			CCDirector::sharedDirector()->end();
			return;
		}

		//如果是世界BOSS，申请结算
		WarManager* Manage = DataCenter::sharedData()->getWar();
		if(Manage->getWorldBoss())
		{
			CombatLogic* pLogic = Manage->getLogicObj();
			pLogic->combatResult(CCBool::create(false));
			return;
		}

		
		BattleScene* Wscene = (BattleScene*)this->getParent();
		CScene* scene = GETSCENE(LoadBattleResource);
		if (DataCenter::sharedData()->getWar()->getStageID())
		{
			((LoadBattleResource*)scene)->setRelease(true,SkipcityScene);
			CPlayerControl::getInstance().sendBattleFinish(2,false,0);
		}else{
			((LoadBattleResource*)scene)->setRelease(true,SkipLoginScene);
		}
		CSceneManager::sharedSceneManager()->replaceScene(scene);


		//返回章节
		if(DataCenter::sharedData()->getWar()->getLastStageId()!=-1)
		{
			DataCenter::sharedData()->setCityActionType(CA_GoToChapater);

			//预请求关卡信息
			if(DataCenter::sharedData()->getWar()->getNormal())
			{
				CPlayerControl::getInstance().sendChapterList(0);
			}
			else
			{
				CPlayerControl::getInstance().sendChapterList(1);
			}
		}
		else
		{
			DataCenter::sharedData()->setCityActionType(CA_GoToReward);		
		}

	}
}



void WarBackLayer::onExitBattle( CCObject* pSender )
{
	if(m_bSoundLock)
	{
		PlayEffectSound(SFX_Button);
	}

	touchExit();

	//WarScene* Wscene = (WarScene*)this->getParent();
	//CScene* scene = GETSCENE(LoadWar);
	//if (DataCenter::sharedData()->getWar()->getStageID())
	//{
	//	((LoadWar*)scene)->setRelease(true,SkipcityScene);
	//	CPlayerControl::getInstance().sendBattleFinish(2,false,0,0);
	//}else{
	//	((LoadWar*)scene)->setRelease(true,SkipLoginScene);
	//}
	//CSceneManager::sharedSceneManager()->replaceScene(scene);
}

void WarBackLayer::onResumeBattle( CCObject* pSender )
{
	if(m_bSoundLock)
	{
		PlayEffectSound(SFX_Button);
	}

	NOTIFICATION->postNotification(B_ReturnLayer);
}

void WarBackLayer::onEffMusic( CCObject *pSender, bool bChecked )
{
	if(m_bSoundLock)
	{
		PlayEffectSound(SFX_Button);
	}

	showEffMusicLab(bChecked);
}

void WarBackLayer::onBgMusic( CCObject *pSender, bool bChecked )
{
	if(m_bSoundLock)
	{
		PlayEffectSound(SFX_Button);
	}

	showBgMusicLab(bChecked);
}

bool WarBackLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	BaseLayer::ccTouchBegan(pTouch, pEvent);
	if(isVisible())
	{
		return true;
	}
	return false;
}

void WarBackLayer::show()
{
	setVisible(true);
}

void WarBackLayer::hide()
{
	setVisible(false);
	this->removeChildByTag(tips_tag);
}

void WarBackLayer::showEffMusicLab( bool bChecked )
{
	CLabel *efflab = (CLabel*)(m_ui->findWidgetById("effmusicLab"));
	if (bChecked)
	{
		efflab->setString(GETLANGSTR(1006));
	}
	else
	{
		efflab->setString(GETLANGSTR(1007));
	}

	GameSound->setEffectState(bChecked);
}

void WarBackLayer::showBgMusicLab( bool bChecked )
{
	CLabel *bglab = (CLabel*)(m_ui->findWidgetById("bgmusicLab"));
	if (bChecked)
	{
		bglab->setString(GETLANGSTR(1008));
	}
	else
	{
		bglab->setString(GETLANGSTR(1009));
	}

	GameSound->setMusicState(bChecked);
}


void WarBackLayer::setMoneyAndBox( int iMoney, int iBox )
{
	//更新金币和盒子的数量显示

}

