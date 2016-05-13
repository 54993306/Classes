#include "DropItem.h"
#include "GMessage.h"
#include "scene/alive/AliveDefine.h"
#include "model/WarManager.h"
#include "common/CommonFunction.h"
#include "common/ProgressLabel.h"
#include "model/DataCenter.h"
#include "warscene/WarControl.h"
#include "scene/layer/WarAliveLayer.h"
#include "common/CGameSound.h"
#include "Battle/BattleMessage.h"

DropItem::DropItem()
	:m_ui(nullptr)
	,m_alive(nullptr)
{

}

DropItem::~DropItem()
{
	RemoveEnent();
}

bool DropItem::init()
{
	AddEvent();
	return true;
}

void DropItem::AddEvent()
{
	NOTIFICATION->addObserver(this,callfuncO_selector(DropItem::ItemDropDispose),Msg_DropItem,nullptr);
}

void DropItem::RemoveEnent()
{
	NOTIFICATION->removeAllObservers(this);
}

void DropItem::setWarUi(WarControl* ui,WarAliveLayer*layer)
{
	if (outPutERRORMsg("DropItem::setWarUi",ui))return;
	m_ui = ui;
	m_alive = layer;
}

void DropItem::ItemDropDispose(CCObject* ob)
{
	if (outPutERRORMsg("DropItem::ItemDropDispose",ob))return;

	AliveObject* aliveOb = (AliveObject*)ob;
	if (aliveOb->getDropItem()<=0)return;

	CCPoint pointBox = m_ui->getBoxIconPos();
	
	CCPoint pointCoin = m_ui->getGoldIconPos();
	//超过10则为金币，否则为道具类

	if(aliveOb->getDropItem() < 10)
	{
		CCNode* node = CCSprite::create("warScene/NewUI/baoxiang.png");
		node->setScale(1.4f);
		node->setAnchorPoint(ccp(0.5f, 0.5f));
		node->setPosition(aliveOb->getParent()->convertToWorldSpace(ccpAdd(aliveOb->getPosition(),ccp(0, 9))));

		CCJumpBy* jumpBy = CCJumpBy::create(0.5f, CCPoint(CCRANDOM_0_1()*30, 0), 50, 2);
		float fTime = ccpDistance(node->getPosition(), pointBox)/1100;
		CCSpawn* mb =CCSpawn::createWithTwoActions(CCMoveTo::create(fTime , pointBox), CCScaleTo::create(fTime, 1.0f));
		CCScaleTo* scaleTo = CCScaleTo::create(0.2f, 1.6f);
		CCFadeOut* fout = CCFadeOut::create(0.1f);
		CCCallFuncO* cgNum = CCCallFuncO::create(this,callfuncO_selector(DropItem::changeGoldNum),CCInteger::create(aliveOb->getDropItem()));
		node->runAction(CCSequence::create(jumpBy, mb, scaleTo, fout,cgNum,CCRemoveSelf::create(), nullptr));		//出现后执行淡出操作
		m_ui->addChild(node, 10);

	}
	else
	{
		//创建batch层
		PlayEffectSound(SFX_408);
		CCSpriteBatchNode* pEffectBatch = m_ui->getBatchNodeEffect();
		int iRandomCount = CCRANDOM_0_1()*6+6;
		float fTimeCallBack = 0.0f;
		for(unsigned int i=0; i<iRandomCount; i++)
		{
			CCSprite* pNode = CCSprite::create("common/icon_11.png");
			pNode->setPosition(aliveOb->getParent()->convertToWorldSpace(ccpAdd(aliveOb->getPosition(),ccp((CCRANDOM_0_1()-0.5f)*100, (CCRANDOM_0_1()-0.5f)*40))));
			pNode->setVisible(false);
			pNode->setScale(0.5f);
			CCDelayTime* delay = CCDelayTime::create(i*0.03f);
			CCShow* show = CCShow::create();
			CCJumpBy* jumpBy = CCJumpBy::create(0.5f, CCPoint((CCRANDOM_0_1()-0.8f)*100, (CCRANDOM_0_1()-0.5f)*80), CCRANDOM_0_1()*20+20, CCRANDOM_0_1()*2+2);
			float fTime = ccpDistance(pNode->getPosition(), pointCoin)/1100;
			CCSpawn* mb =CCSpawn::createWithTwoActions(CCEaseSineOut::create(CCMoveTo::create(fTime , pointCoin)), CCScaleTo::create(fTime, 1.0f));
			CCScaleTo* scaleTo = CCScaleTo::create(0.2f, 1.2f);
			CCFadeOut* fout = CCFadeOut::create(0.1f);
			pNode->runAction(CCSequence::create(delay, show, jumpBy, mb, scaleTo, fout, CCRemoveSelf::create(), nullptr));		//出现后执行淡出操作
			pEffectBatch->addChild(pNode);

			if(i==iRandomCount-1)
			{
				fTimeCallBack += i*0.03f;
				fTimeCallBack += 0.5f;
				fTimeCallBack += fTime;
			}
		}
		CCCallFuncO* cgNum = CCCallFuncO::create(this,callfuncO_selector(DropItem::changeGoldNum),CCInteger::create(aliveOb->getDropItem()));
		pEffectBatch->runAction(CCSequence::create(CCDelayTime::create(fTimeCallBack), cgNum, nullptr));
	}

	aliveOb->setDropItem(0);
}

void DropItem::changeGoldNum(CCObject*ob)
{
	CCInteger* num = (CCInteger*)ob;
	if (num->getValue() < 10)
	{
		PlayEffectSound(SFX_407);
	}else{
		PlayEffectSound(SFX_404);
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(B_DropItem,ob);
}
