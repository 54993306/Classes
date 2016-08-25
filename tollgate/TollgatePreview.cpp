#include "TollgatePreview.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"
#include "MonsterInfo.h"
#include "bag/BagInfoTip.h"

#include "SelectFriend.h"
#include "tools/ShowTexttip.h"
#include "SelectArmy.h"
#include "common/CheckMoney.h"
#include "tollgate/ItemInfo.h"
#include "common/CShowToBuyResource.h"
#include "model/CWholeBodyShowData.h"
#include "common/CommonFunction.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "model/DataPool.h"
#include "pvp_ui/SelectPvpArmy.h"
#include "battle/AnimationManager.h"
#include "common/ShaderDataHelper.h"

using namespace BattleSpace;

CTollgatePreview::CTollgatePreview():
	m_stageId(0),m_tableView(nullptr)
	,m_questId(0)
	,m_iCostFood(0)
	,m_pMonsterInfo(nullptr)
	,m_pItemInfo(nullptr)
	,m_bIsNormal(false)
	,m_pBody(nullptr)
	,m_bLastStage(false)
	,m_iStar(-1)
	,m_iStarLevel(0)
	,m_iNeedHeroType(0)
	,m_iNeedHeroTypeLevel(-1)
	,m_iNeedHeroNum(5)
	,m_iNeedHeroNumLevel(-1)
{

}

bool CTollgatePreview::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("TollgatePreviewr");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		m_pMaskLayer = lay;

		m_ui = LoadComponent("warPreview.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}
	return false;
}

void CTollgatePreview::onCombat(CCObject* pSender)
{
	//检查粮食是否足够
	if(!CheckActionPower(m_iCostFood))
	{
		//ShowPopTextTip(GETLANGSTR(240));
		CShowToBuyResource* pShow = CShowToBuyResource::create();
		pShow->showToBuyResourceByType(ShowBuyResourceAction);
		return;
	}

	CCNode *pNode = (CCNode*)pSender;
	int iTag = pNode->getTag();
	m_iStarLevel = iTag;

	if (m_iStarLevel != m_iNeedHeroTypeLevel)
	{
		m_iNeedHeroType = 0;
	}

	if (m_iStarLevel != m_iNeedHeroNumLevel)
	{
		m_iNeedHeroNum = 5;
	}

	onCombatEffect();
}

void CTollgatePreview::onEnter()
{
	BaseLayer::onEnter();
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	CButton *combat = (CButton *)(m_ui->findWidgetById("combat"));
	combat->setOnClickListener(this,ccw_click_selector(CTollgatePreview::onCombat));
	//combat->getSelectedImage()->setScale(1.1f);
	//combat->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.3f, 1.1f), CCScaleTo::create(0.3f, 1.0f))));
	//combat->setAnchorPoint(ccp(0.5f, 0.5f));
	//combat->setPosition(ccpAdd(combat->getPosition(), ccp(combat->getContentSize().width/2, combat->getContentSize().height/2)));

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CTollgatePreview::onClose));
	this->addChild(pClose, 999);

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionHorizontal);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CTollgatePreview::tableviewDataSource));
	m_tableView->reloadData();

	//人物背景光环旋转
	CImageView* pBgEffect = (CImageView*)m_ui->findWidgetById("bg_effect");
	pBgEffect->setScale(1.36f);
	pBgEffect->runAction(CCRepeatForever::create(CCRotateBy::create(1.5f, 360)));

	NOTIFICATION->addObserver(this, callfuncO_selector(CTollgatePreview::exitMsg), WARPREVIEW_EXIT, nullptr);

	GetTcpNet->registerMsgHandler(TollgatePreviewMsg,this, CMsgHandler_selector(CTollgatePreview::processNetMsg));

	//雾
	m_fogLay = (CLayout *)m_ui->findWidgetById("fog2");
	m_fogLay1 =(CLayout*)m_ui->findWidgetById("fog3");
	this->runCloudAction();

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(CTollgatePreview::roleUpdate));

}

void CTollgatePreview::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

CCObject* CTollgatePreview::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell *pCell = (CTableViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CTableViewCell;
		pCell->autorelease();
		tableCell(pCell, uIdx);
	}
	else
	{
		pCell->removeAllChildren();
		tableCell(pCell,uIdx);
	}
	return pCell;
}

void CTollgatePreview::onExit()
{
	BaseLayer::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
	NOTIFICATION->removeAllObservers(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
}

void CTollgatePreview::processNetMsg(int type, google::protobuf::Message *msg)
{
	StageInfoRes *res =(StageInfoRes*)msg;
	CStageInfoRes stageInfo;
	stageInfo.read(*res);

	//普通/精英战斗关卡不缓存
	if( m_iStar <= 0 )
	{
		DATAPOOL->setStageInfoById(m_stageId,stageInfo);
	}
	
	showStageInfo(stageInfo);
	return;
}

void CTollgatePreview::tableCell(CTableViewCell *pCell, unsigned int uIdx)
{
	CLayout *clone = UICloneMgr::cloneLayout(m_cell);
	CPrize &prize = m_prizeList.at(uIdx);
	for (int i=1;i<=2;++i)
	{
		CCNode *child = clone->getChildByTag(i);
		clone->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			CButton *btn = (CButton*)child;
			btn->setOnPressListener(this, ccw_press_selector(CTollgatePreview::onPressItem));
			btn->setUserData(&m_prizeList.at(uIdx));
			CImageView *prop = getPrizeSprite(&prize);
			if (prop)
			{
				prop->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
				btn->addChild(prop);
				NodeFillParent(prop);
			}
		}
		else if (i==2)
		{
			CCSprite *mask = (CCSprite*)child;
			SmartSetRectPrizeColor(mask, &prize);

			//添加星星
			if(prize.quality > 0)
			{
				CLayout* pStarLayout = SmartGetStarLayout(&prize);
				mask->addChild(pStarLayout);
			}
		}
	}
}

void CTollgatePreview::setStage(int stageId, const char* name)
{
	m_stageId = stageId;
	//const TaskData *task = DataCenter::sharedData()->getTask(Evole_Task)->getCfg(stageId);
	//if(task&&task->taskName.size()>0)
	//{
	//	CLabel *stageName = (CLabel *)(m_ui->findWidgetById("stageName"));
	//	stageName->setString(task->taskName.c_str());
	//}
	//if(task&&task->taskDesc.size()>0)
	//{
	//	CLabel *stageDesc = (CLabel *)(m_ui->findWidgetById("stage_desc"));
	//	stageDesc->setString(task->taskDesc.c_str());
	//}
}

void CTollgatePreview::setQuestId(int questId)
{
	m_questId = questId;
}

void CTollgatePreview::onPressItem( CCObject* pSender, CTouchPressState iState )
{
	CButton *btn = (CButton*)pSender;

	//如果弹窗没够构建
	if(m_pItemInfo==nullptr)
	{
		m_pItemInfo = CItemInfo::create();
		this->addChild(m_pItemInfo);
	}

	switch (iState)
	{
	case cocos2d::cocoswidget::CTouchPressOn:
		{
			PlayEffectSound(SFX_429);

			m_pItemInfo->setInfo((CPrize*)btn->getUserData());
			m_pItemInfo->bindPos(btn);
			m_pItemInfo->show();
			CCLOG("show");
		}
		break;
	case cocos2d::cocoswidget::CTouchPressOff:
		{
			m_pItemInfo->hide();
			CCLOG("hide");
		}
		break;
	default:
		break;
	}
}

void CTollgatePreview::onPressMonster( CCObject* pSender, CTouchPressState iState )
{
	CButton *btn = (CButton*)pSender;

	//如果弹窗没够构建
	if(m_pMonsterInfo==nullptr)
	{
		m_pMonsterInfo = CMonsterInfo::create();
		this->addChild(m_pMonsterInfo);
	}

	switch (iState)
	{
	case cocos2d::cocoswidget::CTouchPressOn:
		{
			PlayEffectSound(SFX_429);

			m_pMonsterInfo->setMonster((CMonster*)btn->getUserData());
			m_pMonsterInfo->bindPos(btn);
			m_pMonsterInfo->show();
			CCLOG("show");
		}
		break;
	case cocos2d::cocoswidget::CTouchPressOff:
		{
			m_pMonsterInfo->hide();
			CCLOG("hide");
		}
		break;
	default:
		break;
	}
}

void CTollgatePreview::showEffect()
{
	//背景层隐藏
	CLayout* pBgLayer = (CLayout*)findWidgetById("layer_back");
	pBgLayer->setVisible(false);

	//前景层隐藏
	CLayout* pFgUpLayer = (CLayout*)findWidgetById("layer_front_up");
	pFgUpLayer->setVisible(false);
	CLayout* pFgDownLayer = (CLayout*)findWidgetById("layer_front_down");
	pFgDownLayer->setVisible(false);

	//动作
	//默认显示石像
	CImageView* pStone1 = (CImageView*)m_ui->findWidgetById("stone_1");
	CImageView* pStone2 = (CImageView*)m_ui->findWidgetById("stone_2");

	//石像出现
	pStone1->setPositionX(pStone1->getPositionX()-300);
	pStone2->setPositionX(pStone2->getPositionX()+300);

	pStone1->runAction(CCMoveBy::create(0.2f, ccp(300, 0)));
	pStone2->runAction(CCMoveBy::create(0.2f, ccp(-300, 0)));


	//背景层出现
	pBgLayer->runAction(CCSequence::create(
		CCDelayTime::create(0.5f),
		CCShow::create(),
		CCFadeIn::create(0.2f),
		nullptr));

	//前景弹出
	pFgUpLayer->setPositionY(pFgUpLayer->getPositionY()+200);
	pFgUpLayer->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCShow::create(), CCMoveBy::create(0.2f, ccp(0, -200)), nullptr));

	pFgDownLayer->setPositionY(pFgDownLayer->getPositionY()-400);
	pFgDownLayer->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCShow::create(), CCMoveBy::create(0.2f, ccp(0, 400)), nullptr));

	//人物出现
	m_pBody->setVisible(false);
	m_pBody->setScale(2.0);
	m_pBody->runAction(CCSequence::create(CCDelayTime::create(0.6f), CCShow::create(), CCCallFunc::create(this, callfunc_selector(CTollgatePreview::callBackForSound425)),CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)), nullptr));
	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.2f), CCCallFunc::create(this, callfunc_selector(CTollgatePreview::showEffectCallBack))));
	
// 	CCAnimation *bgAnim = AnimationManager::sharedAction()->getAnimation("8041");
// 	CCSprite *bg = createAnimationSprite("skill/8041.png",VCENTER,bgAnim,true);
// 	bg->setScale(5.0f);
// 	pBgLayer->addChild(bg,-1);
// 
// 	CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("8040");
// 	CCSprite *light = createAnimationSprite("skill/8040.png",VCENTER,lightAnim,true);
// 	light->setScale(2.2f);
// 	pBgLayer->addChild(light,-1);

}

void CTollgatePreview::showEffectCallBack()
{
	setTouchEnabled(true);

	m_pBody->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.6f, 1.04f), CCScaleTo::create(0.6f, 1.0f))));
}

void CTollgatePreview::showEffectNormal()
{
	setTouchEnabled(false);

	//默认隐藏石像
	CImageView* pStone1 = (CImageView*)m_ui->findWidgetById("stone_1");
	pStone1->setVisible(false);
	CImageView* pStone2 = (CImageView*)m_ui->findWidgetById("stone_2");
	pStone2->setVisible(false);

	//人物出现
	m_pBody->setScale(2.0);
	m_pBody->runAction(CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)));

	PlayEffectSound(SFX_425);

	//下层弹出
	CLayout* pLayerFrontDown = (CLayout*)m_ui->findWidgetById("layer_front_down");
	pLayerFrontDown->setPositionY(pLayerFrontDown->getPositionY()-400);
	pLayerFrontDown->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCMoveBy::create(0.2f, ccp(0, 400))));

	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFunc::create(this, callfunc_selector(CTollgatePreview::showEffectNormalCallBack))));
	
	CLayout* pBgLayer = (CLayout*)findWidgetById("layer_back");

// 	CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("7038");
// // 	lightAnim->setDelayPerUnit(0.15f);
// 	CCSprite *light = createAnimationSprite("skill/7038.png",VCENTER,lightAnim,true);
// 	light->setScale(2.1f);
// 	pBgLayer->addChild(light);

// 	CCSprite *circle = CCSprite::create("warPreview/s1.png");
// 	circle->setPosition(VCENTER);
// 	circle->setScale(1.8f);
// 	circle->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleBy::create(0.4,1.6),CCScaleTo::create(0.4,1.8f))));
// 	m_ui->addChild(circle);
}

void CTollgatePreview::showEffectNormalCallBack()
{
	setTouchEnabled(true);

	m_pBody->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.6f, 1.04f), CCScaleTo::create(0.6f, 1.0f))));
}

void CTollgatePreview::onCombatEffect()
{
	setTouchEnabled(false);

	float fTime = 0.45f;

	//人物背景光环旋转
	CImageView* pBgEffect = (CImageView*)m_ui->findWidgetById("bg_effect");
	pBgEffect->runAction(CCRepeatForever::create(CCRotateBy::create(1.0f, 360)));

	//前景层隐藏
	CLayout* pFgUpLayer = (CLayout*)findWidgetById("layer_front_up");
	pFgUpLayer->runAction(CCSpawn::create(CCScaleTo::create(fTime, 0.0f), CCFadeOut::create(fTime), CCRotateBy::create(fTime, 150), nullptr));

	CLayout* pFgDownLayer = (CLayout*)findWidgetById("layer_front_down");
	m_tableView->runAction(CCSequence::create(
		CCFadeOut::create(0.25f),
		CCHide::create(),
		nullptr
		));
	pFgDownLayer->runAction(CCSpawn::create(CCScaleTo::create(fTime, 0.0f), CCFadeOut::create(fTime), CCRotateBy::create(fTime, 150), nullptr));
	pFgDownLayer->setCascadeOpacityEnabled(true);

	m_pBody->stopAllActions();
	m_pBody->runAction(CCSpawn::create(CCScaleTo::create(fTime, 0.0f), CCFadeOut::create(fTime), CCRotateBy::create(fTime, 150), nullptr));

	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fTime), CCCallFunc::create(this, callfunc_selector(CTollgatePreview::onCombatEffectCallBack))));

	PlayEffectSound(SFX_422);
}

void CTollgatePreview::onCombatEffectCallBack()
{
	setTouchEnabled(true);

	//onClose(nullptr);

 	CSelectArmy *selArmy = CSelectArmy::create();
	selArmy->setStarLevel(m_iStarLevel);
	selArmy->setHeroTypeNeed(m_iNeedHeroType);
	selArmy->setLimitHeroNum(m_iNeedHeroNum);
 	LayerManager::instance()->push(selArmy);
	CPlayerControl::getInstance().sendUnion( m_stageId, m_questId, false, m_iStarLevel );
	selArmy->setStagId(m_stageId, m_questId);
	m_pBody->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.6f, 1.04f), CCScaleTo::create(0.6f, 1.0f))));
}

void CTollgatePreview::runCloudAction()
{
	//云运动
	// 	CCSprite *cloud  =  (CCSprite *)(m_fogLay->findWidgetById("cloud"));

	float width = 1308;//cloud->getContentSize().width;

	CCMoveBy* move = CCMoveBy::create(60.0f,ccp(-width,0));
	CCMoveBy* moveb = CCMoveBy::create(60.0f,ccp(-width,0));
	CCSequence *sequence = CCSequence::create(move,CCCallFuncN::create(this,callfuncN_selector(CTollgatePreview::relocation)),moveb,nullptr);
	m_fogLay->runAction(CCRepeatForever::create(sequence));

	m_cloudPos = m_fogLay->getPosition();

	// 	CCSprite *cloud1  =  (CCSprite *)(m_fogLay->findWidgetById("cloud1"));
	m_cloudAPos = m_fogLay1->getPosition();

	CCMoveBy* move1 = CCMoveBy::create(120.0f,ccp(-width*2,0));
	CCSequence *sequence1 = CCSequence::createWithTwoActions(move1,CCCallFuncN::create(this,callfuncN_selector(CTollgatePreview::relocation)));
	m_fogLay1->runAction(CCRepeatForever::create(sequence1));
}

void CTollgatePreview::relocation( CCNode* pSender )
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

void CTollgatePreview::exitMsg(CCObject* pObj)
{
	//onClose(nullptr);
	LayerManager::instance()->removeLayer(m_pMaskLayer);
	LayerManager::instance()->removeLayer(this);
}

void CTollgatePreview::roleUpdate( const TMessage& tMsg )
{
	CLabel *action = (CLabel*)(m_ui->findWidgetById("action"));
	//检查货币数量
	if(!CheckActionPower(atoi(action->getString())))
	{
		action->setColor(RGB_RED);
	}
	else
	{
		action->setColor(RGB_WHITE);
	}
}

void CTollgatePreview::callBackForSound425()
{
	PlayEffectSound(SFX_425);
}

void CTollgatePreview::setIsLastStageInChapter( bool bLastStage )
{
	m_bLastStage = bLastStage;
}

void CTollgatePreview::showStageInfo(CStageInfoRes &stageInfo)
{
	this->setVisible(true);
	m_monstList = stageInfo.monstList;
	m_prizeList = stageInfo.prizeList;
	if(m_monstList.size() == 0)
	{
		CCLOG("CTollgatePreview::processNetMsg---Data Error");
		onClose(nullptr);
		return;
	}
	m_tableView->setCountOfCell(m_prizeList.size());
	m_tableView->reloadData();

	for (int i = 0; i < 5; i++)
	{
		CButton *hero = (CButton *)m_ui->findWidgetById(CCString::createWithFormat("hero%d",i+1)->getCString());
		CCSprite *mask = (CCSprite *)m_ui->findWidgetById(CCString::createWithFormat("mask%d",i+1)->getCString());

		if (i<m_monstList.size())
		{
			CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", m_monstList.at(i).thumb)->getCString());
			if (!head)
			{
				head = CCSprite::create("headImg/101.png");
				CCLOG("[ ERROR ] CTollgatePreview::processNetMsg Lost Image = %d",m_monstList.at(i).thumb);
			}
			if (head)
			{
				//head->setScale(0.82f);
				head->setPosition(ccp(hero->getContentSize().width/2, hero->getContentSize().height/2));
				hero->addChild(head);
				NodeFillParent(head);
				hero->setOnPressListener(this,ccw_press_selector(CTollgatePreview::onPressMonster));
				((SndButton*)hero)->setPlaySound(false);
				hero->setUserData(&m_monstList.at(i));
			}


			const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(m_monstList.at(i).thumb);
			if(data)
			{

				mask->setTexture(SetRectColor(data->iType1));

				CCTexture2D* pTexture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png", m_monstList.at(i).roletype, data->iType2)->getCString());
				if(pTexture)
				{
					CCSprite* pMonsterType = CCSprite::createWithTexture(pTexture);
					pMonsterType->setPosition(ccp(mask->getContentSize().width, mask->getContentSize().height));
					mask->addChild(pMonsterType);
				}
			}

		}
		else
		{
			hero->setVisible(false);
			mask->setVisible(false);
		}
	}

	CLabel *action = (CLabel*)(m_ui->findWidgetById("action"));
	m_iCostFood = stageInfo.action;
	action->setString(ToString(m_iCostFood));

	//检查货币数量
	if(!CheckActionPower(atoi(action->getString())))
	{
		action->setColor(RGB_RED);
	}
	else
	{
		action->setColor(RGB_WHITE);
	}


	//更新名字，等级
	CMonster& monster = m_monstList.at(0);
	CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
	const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(monster.thumb);
	if(data)
	{
		name->setString(data->heroName.c_str());
	}
	CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
	level->setString(ToString(monster.level));

	//怪物类型
	if(data)
	{
		CCSprite* pType = (CCSprite*)(m_ui->findWidgetById("type"));
		CCString* pStr = CCString::createWithFormat("common/type_%d_%d.png", monster.roletype, data->iType2);
		std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pStr->getCString());
		if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		{
			pType->setTexture(CCTextureCache::sharedTextureCache()->addImage(pStr->getCString()));
		}
		else
		{
			pType->setVisible(false);
		}
	}


	//增加人物贴图
	const CWholeBodyShowInfo& pInfo = CWholeBodyShowData::getInstance()->getDataById(monster.thumb);
	CCSprite* pNodePos = (CCSprite*)m_ui->findWidgetById("monster_pos");
	pNodePos->setVisible(true);
	pNodePos->setOpacity(0);
	CCPoint pPos = pNodePos->getPosition();
	CCSprite* pBody = CCSprite::create(CCString::createWithFormat("selectRole/%d.png", monster.thumb)->getCString());
	if (!pBody)
	{	
		pBody = CCSprite::create(CCString::createWithFormat("selectRole/%d.png", 111)->getCString());
	}
	pBody->setScale(pInfo.fScale);
	pBody->setFlipX(pInfo.bFlipX);
	pBody->setPosition(pInfo.pos);
	pBody->setRotation(pInfo.iRoate);
	pNodePos->addChild(pBody, 100, 100);
	if(m_bLastStage)
	{
		pBody->setColor(ccc3(0, 0, 0));
		pBody->setOpacity(230);
		pBody->setScale(pBody->getScale()*1.2f);
	}

	m_pBody = pBody;

	if(m_bIsNormal)
	{
		showEffectNormal();
	}
	else
	{
		showEffect();
	}

	showCombatWithStar(stageInfo);
}

void CTollgatePreview::showCombatWithStar( CStageInfoRes &stageInfo )
{
	//消耗的位置
	CLayout *pConsumeLay = (CLayout *)m_ui->findWidgetById("consume");
	//战斗按钮状态
	CButton *pBaseCombat = (CButton *)m_ui->findWidgetById("combat");
	//星级战斗按钮
	CLayout *pCombatLay = (CLayout *)m_ui->findWidgetById("combat_lay");

	bool isStarStage = stageInfo.bHasStar;

	//其他非星级关卡
	if (!isStarStage)
	{
		pBaseCombat->setVisible(true);
		pBaseCombat->setTag(0);
		pCombatLay->setVisible(false);
	}
	//星级关卡，精英和普通
	else
	{
		pConsumeLay->setPosition(pConsumeLay->getPosition()-ccp(690, 150));
		pBaseCombat->setVisible(false);
		pCombatLay->setVisible(true);

		const StageStar* cStageStar[3] = {&stageInfo.pStageStar1, &stageInfo.pStageStar2, &stageInfo.pStageStar3};

		for( int i=0; i<3; i++)
		{
			//按钮
			CButton *pButton = (CButton *)pCombatLay->findWidgetById(CCString::createWithFormat("combat_%d", i+1)->getCString());
			//图标
			CImageView *pIcon = (CImageView *)pCombatLay->findWidgetById(CCString::createWithFormat("title_%d", i+1)->getCString());
			//文字
			CLabel *pText = (CLabel *)pCombatLay->findWidgetById(CCString::createWithFormat("tips_%d", i+1)->getCString());
			//对勾
			CImageView *pRight = (CImageView *)pCombatLay->findWidgetById(CCString::createWithFormat("got_%d", i+1)->getCString());

			//TODO
			//文字描述
			updateStarLimitTips(pText, cStageStar[i]->type(), cStageStar[i]->param(), i+1);

			//对勾
			pRight->setVisible(m_iStar > i);

			//可以进去打的
			if (cStageStar[i]->open())
			{
				pButton->setTag(i+1);
				pButton->setTouchEnabled(true);
				pButton->setOnClickListener(this, ccw_click_selector(CTollgatePreview::onCombat));
			}
			//不能进去打的
			else
			{
				pButton->setEnabled(false);
				pButton->getDisabledImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
				pIcon->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			}
		}
	}

}

void CTollgatePreview::updateStarLimitTips( CLabel *pLabel, int iType, int iData, int iLevelStar )
{
	//条件类型(1 指定时间内通关，2 限制英雄数量，3 限定英雄属性，4 指定英雄)
	switch (iType)
	{
	case 1:
		{
			pLabel->setString(CCString::createWithFormat(GETLANGSTR(2047), iData)->getCString());
		}break;
	case 2:
		{
			m_iNeedHeroNumLevel = iLevelStar;
			m_iNeedHeroNum = iData;
			pLabel->setString(CCString::createWithFormat(GETLANGSTR(2048), iData)->getCString());
		}break;
	case 3:
		{
			m_iNeedHeroTypeLevel = iLevelStar;
			m_iNeedHeroType = iData;
			pLabel->setString(CCString::createWithFormat(GETLANGSTR(2049), GETLANGSTR(1189+iData))->getCString());
		}break;
	case 4:
		{
			//根据id获取英雄信息
			const HeroInfoData *pData = DataCenter::sharedData()->getHeroInfo()->getCfg( iData );
			pLabel->setString(CCString::createWithFormat(GETLANGSTR(2050), pData->heroName.c_str())->getCString());
		}break;
	default:
		{
			pLabel->setString(GETLANGSTR(2052));
		}
		break;
	}
}
