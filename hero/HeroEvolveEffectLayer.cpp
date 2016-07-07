#include "HeroEvolveEffectLayer.h"
#include "HeroEvolve.h"
#include "HeroEvolveDataPanel.h"

CHeroEvolveEffectLayer::CHeroEvolveEffectLayer()
	:m_pUI(nullptr), m_pEffectBase(nullptr), m_pEffectForChange(nullptr), m_pBaseHero(nullptr), m_pAimHero(nullptr)
	,m_pSkillPanel(nullptr), m_eUnLockType(HeroEvolveEffectuUnlockType::UnlockSkill), m_pTarget(nullptr), m_effectCallbackForBallChange(nullptr), m_effectCallbackForAllChange(nullptr)
{

}

bool CHeroEvolveEffectLayer::init()
{
	if (BaseLayer::init())
	{
		m_pUI = LoadComponent("HeroEvolveEffect.xaml");  //
		m_pUI->setPosition(VCENTER);
		this->addChild(m_pUI);

		this->setIsShowBlack(false);
		this->setVisible(false);

		m_pSkillBg = (CImageViewScale9 *)m_pUI->findWidgetById("bg1");

		return true;
	}
	return false;
}

void CHeroEvolveEffectLayer::onEnter()
{
	BaseLayer::onEnter();

	//加载特效
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("heroEvolve/evol.ExportJson");
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("heroEvolve/evol2.ExportJson");
}

void CHeroEvolveEffectLayer::onExit()
{
	BaseLayer::onExit();

	//移除特效
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("heroEvolve/evol.ExportJson");
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("heroEvolve/evol2.ExportJson");
	removeAllPlistFile();
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void CHeroEvolveEffectLayer::initEffect()
{
	//加载资源-两个动画资源
	if(m_pEffectBase!=nullptr)
	{
		m_pEffectBase->removeFromParentAndCleanup(true);
	}
	m_pEffectBase = CCArmature::create("evol2");
	m_pEffectBase->setPosition(ccp(DESIGN_WIDTH/2, VIRTUAL_FIXED_HEIGHT/2));
	m_pUI->addChild(m_pEffectBase, -2);
	m_pEffectBase->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(CHeroEvolveEffectLayer::frameEventCallBack));
	m_pEffectBase->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(CHeroEvolveEffectLayer::movementCallBack));


	if(m_pEffectForChange!=nullptr)
	{
		m_pEffectForChange->removeFromParentAndCleanup(true);
	}
	m_pEffectForChange = CCArmature::create("evol");
	m_pEffectForChange->setPosition(ccp(DESIGN_WIDTH/2, VIRTUAL_FIXED_HEIGHT/2));
	m_pUI->addChild(m_pEffectForChange, -2);
	m_pEffectForChange->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(CHeroEvolveEffectLayer::frameEventCallBack));
	m_pEffectForChange->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(CHeroEvolveEffectLayer::movementCallBack));

}

void CHeroEvolveEffectLayer::showEffect( CHero *pBaseHero, CHero *pHero, CCPoint posAim, CLayout* pSkillPanel, HeroEvolveEffectuUnlockType iType )
{
	this->setVisible(true);

	m_pBaseHero = pBaseHero;
	m_pAimHero = pHero;

	m_posAim = posAim;

	if(pSkillPanel != nullptr)
	{
		if(m_pSkillPanel == nullptr || m_pSkillPanel->getParent()==nullptr)
		{
			if( m_pSkillPanel != nullptr )
			{
				m_pSkillBg->removeChildByTag(255, true);
			}
			m_pSkillPanel = pSkillPanel;
			m_pSkillPanel->setPosition(ccp(m_pSkillBg->getContentSize().width/2, m_pSkillBg->getContentSize().height/2));
			m_pSkillBg->addChild(m_pSkillPanel, 10);
		}
		else
		{
			m_pSkillPanel = pSkillPanel;
		}
	}
	else
	{
		m_pSkillPanel = pSkillPanel;
	}


	m_pSkillBg->setVisible(false);

	
	m_eUnLockType = iType;

	//更新显示数据
	updateData();

	//骨骼1播放-播完-飞到目标位置
	//骨骼2播放1-属性增加动画-需要切入程序界面
	//骨骼2播放2-技能/格子/天赋开启-需要切入程序动画

	//图集选择
	removeAllPlistFile();
	loadPlistFileWithRank(m_pAimHero->quality);

	initEffect();

	//隐藏解锁骨骼动画
	m_pEffectForChange->setVisible(false);

	//基础动画1.1-等待飞入事件传入
	m_pEffectBase->setVisible(true);
	m_pEffectBase->setScale(1.0f);
	m_pEffectBase->setPosition(VCENTER);
	m_pEffectBase->getAnimation()->playWithIndex(0, 0.01f);
}

void CHeroEvolveEffectLayer::hideEffect()
{
	this->setVisible(false);
}

void CHeroEvolveEffectLayer::frameEventCallBack( CCBone* pBone, const char* sEvent, int iTag1, int iTag2 )
{
	//骨骼1飞入目标位置
	if(strcmp(sEvent, "fly_ball")==0)
	{
		m_pEffectBase->runAction(CCSequence::create(
			CCSpawn::create(
				CCSequence::createWithTwoActions(
					CCMoveTo::create(0.15f, m_posAim),
					CCCallFunc::create(this, callfunc_selector(CHeroEvolveEffectLayer::callbackChangeBall))
				),
				CCScaleTo::create(0.3f, 0.5f),
				nullptr
			),
			CCDelayTime::create(0.6f),
			nullptr
			));
	}
	////切入属性增加UI
	else if(strcmp(sEvent, "data_fly_in1")==0)
	{
		showDataFlyInByType(1);
	}
	//属性面板相关东西开始消失
	else if(strcmp(sEvent, "data_fly_out1")==0)
	{
		showDataFlyOutByType(1);
	}
	else if(strcmp(sEvent, "data_fly_in2")==0)
	{
		showDataFlyInByType(2);
	}
	//属性面板相关东西开始消失
	else if(strcmp(sEvent, "data_fly_out2")==0)
	{
		showDataFlyOutByType(2);
	}
	//切入类技能面板
	else if(strcmp(sEvent, "skill_fly_in")==0)
	{
		m_pSkillBg->setVisible(true);
		m_pSkillBg->setPositionY(m_pSkillBg->getPositionY()-470);
		m_pSkillBg->runAction(CCMoveBy::create(0.2f, ccp(0, 470)));
	}
	else if(strcmp(sEvent, "skill_fly_out")==0)
	{
		m_pSkillBg->runAction(
			CCSequence::create(
			CCMoveBy::create(0.2f, ccp(0, -470)),
			CCHide::create(),
			CCMoveBy::create(0.0f, ccp(0, 470)),
			nullptr
			));
	}
}


void CHeroEvolveEffectLayer::movementCallBack( CCArmature *pArmature, MovementEventType type, const char *str )
{
	switch (type)
	{
	case cocos2d::extension::START:
		break;
	case cocos2d::extension::LOOP_COMPLETE:
	case cocos2d::extension::COMPLETE:
		{

			//太极特效
			if(strcmp(str, "taiji") == 0)
			{

				m_pEffectBase->setVisible(false);
				m_pEffectForChange->setVisible(true);

				//骨骼2.1播放效果-属性增加
				m_pEffectForChange->getAnimation()->play("data", 0.01f);


			}
			//属性变化动作结束
			else if(strcmp(str, "data") == 0)
			{
				//进入第2.2阶段特效播放
				//是否有二阶特效
				if( m_pSkillPanel == nullptr )
				{
					//设置回调
					hideEffect();
					callbackChangeAll();
					return;
				}

				//根据类型区分
				switch (m_eUnLockType)
				{
				case HeroEvolveEffectuUnlockType::UnlockSkill:
					{
						m_pEffectForChange->getAnimation()->play("skill", 0.01f);
					}
					break;
				case HeroEvolveEffectuUnlockType::UnlockBox:
					{
						m_pEffectForChange->getAnimation()->play("box", 0.01f);
					}
					break;
				case HeroEvolveEffectuUnlockType::UnlockExtra:
					{
						m_pEffectForChange->getAnimation()->play("skill", 0.01f);
					}
					break;
				default:
					break;
				}
			}
			//所有特效结束
			else if( strcmp(str, "box") == 0 || strcmp(str, "skill") == 0 )
			{
				//设置回调
				//TODO
				hideEffect();
				callbackChangeAll();
			}
		}
		break;
	default:
		break;
	}
}


void CHeroEvolveEffectLayer::removeAllPlistFile()
{
	for(int i=0; i<HERO_EVOLVE_RANK_MAX; i++)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(
			CCString::createWithFormat("heroEvolve/taiji%d.plist", i+1)->getCString()
			);
	}
}

void CHeroEvolveEffectLayer::loadPlistFileWithRank( int iRank )
{
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(
		CCString::createWithFormat("heroEvolve/taiji%d.plist", iRank)->getCString()
		);
}

void CHeroEvolveEffectLayer::updateData( )
{
	//面板数据
	int iValue[][2] = { 
		{m_pBaseHero->combat, m_pAimHero->combat}, 
		{m_pBaseHero->hp, m_pAimHero->hp}, 
		{m_pBaseHero->atk, m_pAimHero->atk}, 
		{m_pBaseHero->def, m_pAimHero->def},
		{m_pBaseHero->dex, m_pAimHero->dex}, 
		{m_pBaseHero->hit, m_pAimHero->hit}, 
		{m_pBaseHero->crit, m_pAimHero->crit}, 
		{m_pBaseHero->dodge, m_pAimHero->dodge},
		{m_pBaseHero->renew, m_pAimHero->renew}
	};

	//更新数据
	for(int i=0; i<9; i++)
	{
		CLayout *pLay = (CLayout *)m_pUI->findWidgetById(CCString::createWithFormat("data%d", i+1)->getCString());
		pLay->setVisible(false);
		CLabel *pBaseValue = (CLabel *)pLay->findWidgetById(CCString::createWithFormat("base_value%d", i+1)->getCString());
		pBaseValue->setString(ToString(iValue[i][0]));
		CLabel *pAimValue = (CLabel *)pLay->findWidgetById(CCString::createWithFormat("aim_value%d", i+1)->getCString());
		pAimValue->setString(ToString(iValue[i][1]));
	}
}

void CHeroEvolveEffectLayer::showDataFlyInByType( int iType )
{
	for(int i=0; i<9; i++)
	{
		if( (iType==1 && i<4) || (iType==2 && i>=4))
		{
			CLayout *pLay = (CLayout *)m_pUI->findWidgetById(CCString::createWithFormat("data%d", i+1)->getCString());
			pLay->setPositionX(pLay->getPositionX()-600);
			pLay->setVisible(false);
			pLay->setOpacity(0);
			pLay->runAction(CCSequence::create(
				CCDelayTime::create(0.05f*i),
				CCSpawn::createWithTwoActions(
				CCMoveBy::create(0.2f, ccp(600, 0)),
				CCSequence::create(
				CCDelayTime::create(0.05f),
				CCShow::create(),
				CCFadeIn::create(0.1f),
				nullptr
				)
				),
				nullptr
				));
		}
	}
}

void CHeroEvolveEffectLayer::showDataFlyOutByType( int iType )
{
	for(int i=0; i<9; i++)
	{
		if( (iType==1 && i<4) || (iType==2 && i>=4))
		{
			CLayout *pLay = (CLayout *)m_pUI->findWidgetById(CCString::createWithFormat("data%d", i+1)->getCString());
			pLay->runAction(CCSequence::create(
				CCDelayTime::create(0.05f*i),
				CCMoveBy::create(0.15f, ccp(-600, 0)),
				CCHide::create(),
				CCMoveBy::create(0.0f, ccp(600, 0)),
				nullptr
				));
		}
	}
}


void CHeroEvolveEffectLayer::callbackChangeBall()
{
	if(m_pTarget != nullptr && m_effectCallbackForBallChange!=nullptr)
	{
		(m_pTarget->*m_effectCallbackForBallChange)();
	}
}


void CHeroEvolveEffectLayer::callbackChangeAll()
{
	if(m_pTarget!=nullptr && m_effectCallbackForAllChange!=nullptr)
	{
		(m_pTarget->*m_effectCallbackForAllChange)();
	}
}

void CHeroEvolveEffectLayer::bindBallChangeCallBack( CCObject* pTarget, EvolveEffectCallbackForChange callback )
{
	m_pTarget = pTarget;
	m_effectCallbackForBallChange = callback;
}

void CHeroEvolveEffectLayer::bindAllEffectChangeCallBack( CCObject* pTarget, EvolveEffectCallbackForChange callback )
{
	m_pTarget = pTarget;
	m_effectCallbackForAllChange = callback;
}


