 
#include "WarScene.h"
#include "GMessage.h"
#include "tools/commonDef.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "warscene/DropItem.h"
#include "model/terrainLayer.h"
#include "warscene/WarControl.h"
#include "scene/layer/BackLayer.h"
#include "warscene/ComBatLogic.h"
#include "scene/layer/StoryLayer.h"
#include "scene/layer/WarMapLayer.h"
#include "scene/layer/WarAliveLayer.h"
#include "warscene/CombatGuideData.h"
#include "warscene/CombatGuideLayer.h"
#include "warscene/CombatGuideManage.h"
#include "common/CommonFunction.h"
#include "warscene/EffectData.h"
#include "tools/CCShake.h"
WarScene::WarScene()
	:m_MapLayer(nullptr),m_AliveLayer(nullptr),m_StoryLayer(nullptr)
	,m_MoveLayer(nullptr),m_UILayer(nullptr),_dropItem(nullptr)
	,m_Loginc(nullptr)
{}
WarScene::~WarScene()
{
	CC_SAFE_RELEASE(_dropItem);
	_dropItem = nullptr;
}
void WarScene::onEnter()
{
	CScene::onEnter();
	DataCenter::sharedData()->getUser()->setoldLevel();									//设置开始战斗前等级
	m_Loginc->initMember();
	LayerMoveEnd(CCInteger::create((int)StoryType::beginStory));						//这个方法不应该放在这里的发条消息也能解决的问题
	NOTIFICATION->addObserver(this,callfuncO_selector(WarScene::ShakeLayer),SHAKE_BYEFFECT,nullptr);
}

void WarScene::onEnterTransitionDidFinish()
{
	m_GuideLayer->initScene(this);
}

void WarScene::onExit()
{
	CScene::onExit();
	this->unscheduleUpdate();
	RemoveEvent();
}

void WarScene::onCreate()
{
	WarMapData* mapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
	m_MoveLayer = CCNode::create();					//可以移动的节点包含地图和武将
	addChild(m_MoveLayer);

	m_MapLayer = WarMapLayer::create();
	m_MoveLayer->addChild(m_MapLayer);

	m_TerrainLayer = TerrainLayer::create();
	m_MoveLayer->addChild(m_TerrainLayer);
	
	m_AliveLayer = WarAliveLayer::create();
	m_MoveLayer->addChild(m_AliveLayer);

	m_UILayer = WarControl::create();
	this->addChild(m_UILayer);

	_dropItem = DropItem::create();
	_dropItem->retain();
	_dropItem->setWarUi(m_UILayer,m_AliveLayer);

	BackLayer* bkLayer = BackLayer::create();
	this->addChild(bkLayer);

	m_GuideLayer = CombatGuideLayer::create();
	m_GuideLayer->setTouchEnabled(false);
	this->addChild(m_GuideLayer);
	DataCenter::sharedData()->getCombatGuideMg()->setGuideLayer(m_GuideLayer);

	m_StoryLayer = StoryLayer::create();
	m_StoryLayer->setVisible(false);
	m_StoryLayer->setTouchEnabled(false);
	addChild(m_StoryLayer, 99);

	m_Loginc = CombatLogic::create();
	addChild(m_Loginc);									//添加到父类才能开启定时器
	DataCenter::sharedData()->getWar()->setLogicObj(m_Loginc);
}
//添加事件处理监听等
void WarScene::AddEvent()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,AliveLayerPriority,true);
	if( m_UILayer ) m_UILayer->AddEvent();
	if( m_AliveLayer ) m_AliveLayer->addEvent();
	if( m_MapLayer ) m_MapLayer->addEvent();
}
//移除事件
void WarScene::RemoveEvent()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCNotificationCenter::sharedNotificationCenter()->removeAllObservers(this);
	if( m_UILayer ) m_UILayer->RemoveEvent();
	if( m_AliveLayer) m_AliveLayer->removeEvent();
	if( m_MapLayer ) m_MapLayer->removeEvent();
}

bool WarScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	m_StartPos = pTouch->getLocation();
	return true;
}
//拖动地图
void WarScene::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCPoint pMove = this->convertToNodeSpace(pTouch->getLocation());
#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
	if (!DataCenter::sharedData()->getWar()->getNormal())
		return;
#endif
	
	CCPoint pMove = pTouch->getLocation();
	float dx = pMove.x - m_StartPos.x;							//地图只能x轴移动
	float newX = dx + m_MoveLayer->getPositionX();
	WarMapData* mapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
	if( newX < MAP_MINX(mapData) )
		newX = MAP_MINX(mapData);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if( newX > MAP_MAXX(mapData) )
		newX = MAP_MAXX(mapData);
	//if (newX > 0)newX = 0;
#else
	if (newX > 0)newX = 0;
#endif
	m_StartPos = pMove;											//不断更新触摸初始值
	if (DataCenter::sharedData()->getCombatGuideMg()->IsGuide())//引导状态进行锁屏处理
	{
		CombatGuideStep* step = DataCenter::sharedData()->getCombatGuideMg()->getCurrStep();
		if (step->getLockLayer())
			return ;
	}
	float px = (newX - m_MoveLayer->getPositionX()) * 0.3f;	//已经偏移了量太多了偏移少一些回来
	float layx = 0;
	CLayout *lay = dynamic_cast<CLayout*>(m_MapLayer->getMaplayer()->findWidgetById("yuanjing"));
	layx = lay->getPositionX();	
	m_MoveLayer->setPositionX(newX);	
	lay->setPositionX(layx-px);					
}
void WarScene::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	return;
	CCPoint endP = pTouch->getLocation();
	float dx = endP.x - m_StartPos.x;							//地图只能x轴移动
	float time  = 0.5f;
	if (dx > 130)
	{
		dx = dx +300;
		time += 0.5f;
	}else if (dx < - 130)
	{
		dx = dx -300;
		time += 0.5f;
	}
	CCPoint layerP = m_MoveLayer->getPosition();
	float newX = dx + layerP.x;
	WarMapData* mapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
	if( newX < MAP_MINX(mapData) )
		newX = MAP_MINX(mapData);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if( newX > MAP_MAXX(mapData) )
		newX = MAP_MAXX(mapData);
	//if (newX > 0)newX = 0;
#else
	if (newX > 0)newX = 0;
#endif
	CCMoveTo* moveTo = CCMoveTo::create(time,ccp(newX,m_MoveLayer->getPositionY()));
	CCActionInterval * easeExponentialIn= CCEaseExponentialOut::create(moveTo); //缓慢终止
	m_MoveLayer->runAction(easeExponentialIn);
}		

CCArray* WarScene::getTaskArray()
{
	if (m_Loginc->getTaskArray())
	{
		return m_Loginc->getTaskArray();
	}else{
		CCLOG("[ *ERROR ] WarScene::getTaskArray");
		return nullptr;
	}
}

void WarScene::LayerMoveEnd(CCObject* ob)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	CCNotificationCenter::sharedNotificationCenter()->postNotification(LAYERMOVEEND,ob);
	//if (DataCenter::sharedData()->getWar()->getStageID())
	//{
	//	CCNotificationCenter::sharedNotificationCenter()->postNotification(LAYERMOVEEND,ob);
	//}else{
	//	if (((CCInteger*)ob)->getValue() == beginStory)
	//		CCNotificationCenter::sharedNotificationCenter()->postNotification(PLAYERBEGINANIMATION);
	//	else
	//		CCNotificationCenter::sharedNotificationCenter()->postNotification(LAYERMOVEEND,ob);
	//}
#else
	if (DataCenter::sharedData()->getWar()->getStageID())
	{
		CCNotificationCenter::sharedNotificationCenter()->postNotification(LAYERMOVEEND,ob);
	}else{
		if (((CCInteger*)ob)->getValue() == (int)StoryType::beginStory)
			CCNotificationCenter::sharedNotificationCenter()->postNotification(PLAYERBEGINANIMATION);
		else
			CCNotificationCenter::sharedNotificationCenter()->postNotification(LAYERMOVEEND,ob);
	}
#endif
}

void WarScene::ShakeLayer( CCObject* ob )
{
	//if (m_MoveLayer->getActionByTag(100))
	//	return;
	//CCShake* shake = CCShake::create(0.4f,13.0f);
	//shake->setTag(100);
	//m_MoveLayer->runAction(shake);
}
