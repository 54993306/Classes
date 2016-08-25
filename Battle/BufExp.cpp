#include "BufExp.h"
#include "Battle/ConstNum.h"
#include "Battle/BaseRole.h"
#include "Battle/BuffManage.h"
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "Battle/EffectData.h"
#include "Battle/RoleObject/HPObject.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/EffectObject.h"
#include "Battle/BattleMessage.h"
#include "Battle/BuffData.h"
#include "Battle/Config/ConfigManage.h"
namespace BattleSpace{

	BufExp::BufExp():m_hpSize(CCPointZero),m_interval(0){}

	BufExp::~BufExp()
	{
		bNotification->removeAllObservers(this);											//不解绑,map会崩找不到原因
	}

	bool BufExp::init()
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("warScene/bufficon.plist");
		bNotification->addObserver(this,callfuncO_selector(BufExp::AddBuffExcute),B_AddBuff,nullptr);
		bNotification->addObserver(this,callfuncO_selector(BufExp::updatePosition),B_RemoveBuff,nullptr);
		bNotification->addObserver(this,callfuncO_selector(BufExp::upBuffEffect),B_UpdateBuffEffect,nullptr);
		return true;
	}
	//每个buff新添加时都发送一条消息
	void BufExp::AddBuffExcute( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (m_hpSize.equals(CCPointZero))
		{
			m_hpSize.x = tRole->getRoleObject()->getHp()->getContentSize().width;
			m_hpSize.y = tRole->getRoleObject()->getHp()->getContentSize().height-GRID_HEIGHT/2;
		}
		BuffMap* buffMap = tRole->getBuffManage()->getBuffMap();
		for (auto i:*buffMap)
		{
			if (!i.second->getAddFirst())										//找到数组中没有处理过添加效果的buff
				continue;
			i.second->setAddFirst(false);
			const BuffEffect* effect = BattleConfig->getBuffData()->getBuffEffect(i.second->getBuffType(),i.second->getIsDBuff());
			vector<CCNode*> VecEffect;
			if (effect->getEffect_up())
			{
				EffectObject* ef = EffectObject::create(ToString(effect->getEffect_up()));
				ef->setSkewing(true);		
				ef->setScale(1/tRole->getZoom());
				if (effect->getup_Loop())						//定身类的效果需要持续显示
				{
					ef->setPlayerType(sPlayType::eRepeat);
					VecEffect.push_back(ef);
				}
				ef->setPosition(ccp(0,30));
				tRole->getRoleObject()->getBody()->addChild(ef);
			}
			if (effect->getEffect_down())
			{
				EffectObject* ef = EffectObject::create(ToString(effect->getEffect_down()));
				ef->setSkewing(true);
				ef->setScale(1/tRole->getZoom());
				if (effect->getdown_Loop())						//定身类的效果需要持续显示
				{
					ef->setPlayerType(sPlayType::eRepeat);
					VecEffect.push_back(ef);
				}
				ef->setPosition(ccp(0,30));
				tRole->getRoleObject()->getBody()->addChild(ef,-1);
			}
			CCSprite* smallIcon = CreateSmallIcon(i.second,VecEffect);			//创建小图标
			tRole->getBuffManage()->AddEffectVec(i.second->getBuffID(),VecEffect);	//加入到map才能刷新位置
			if (smallIcon)
			{
				tRole->getRoleObject()->getBody()->addChild(smallIcon);
				updatePosition(tRole);											//刷新小图标位置
			}
			CreateBigIcon(i.second,tRole->getRoleObject()->getBody());			//创建大图标
			return;																//每一条添加消息只处理一个buff的效果
		}
	}

	CCSprite* BufExp::CreateSmallIcon( BuffInfo* info,vector<CCNode*>&Vec )
	{
		char smalstr[30] = {0};
		sprintf(smalstr,"icon_%d.png",info->getBuffType());
		CCSpriteFrame *pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(smalstr);
		if (pFrame)
		{
			CCSprite* smallIcon = CCSprite::createWithSpriteFrame(pFrame);
			float x = smallIcon->getContentSize().width;
			if (!info->getIsDBuff())
				smallIcon->setFlipY(true);
			Vec.push_back(smallIcon);
			return smallIcon;
		}else{
			if ((sBuffType)info->getBuffType() > sBuffType::eCurrBlood )
				CCLOG("[ ERROR ] BufExp::CreateSmallIcon Lost Small Icon type=%d",info->getBuffType());
		}
		return NULL;
	}
	//创建大图标
	void BufExp::CreateBigIcon( BuffInfo* info,CCSprite* body )
	{
		m_interval+=0.3f;
		CCSprite* bigIcon = nullptr;	
		char bigstr[30]={0};
		CCDelayTime* dely = CCDelayTime::create(1.0f);
		if (info->getIsDBuff())		//减益大图标由上往下
		{
			sprintf(bigstr,"%d_down.png",info->getBuffType());
			CCSpriteFrame *bFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(bigstr);		
			if (bFrame)
			{
				bigIcon = CCSprite::createWithSpriteFrame(bFrame);
				bigIcon->setScale(1/body->getScale());
				bigIcon->setPosition(ccp(0,150));
				CCMoveBy* mt = CCMoveBy::create(m_interval,ccp(0,-100));
				CCScaleTo* scb = CCScaleTo::create(m_interval,1);
				CCSpawn* spa = CCSpawn::create(mt,scb,NULL);
				bigIcon->runAction(CCSequence::create(dely,spa,CCRemoveSelf::create(),NULL));
				body->addChild(bigIcon,100-m_interval*10);
			}else{
				if ((sBuffType)info->getBuffType() > sBuffType::eCurrBlood )
					CCLOG("[ ERROR ] BufExp::CreateBigIcon Lost Big Icon type=%d",info->getBuffType());
			}
		}else{
			sprintf(bigstr,"%d_up.png",info->getBuffType());
			CCSpriteFrame *bFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(bigstr);
			if (bFrame)
			{
				bigIcon = CCSprite::createWithSpriteFrame(bFrame);
				bigIcon->setPosition(ccp(0,0));
				CCMoveBy* mt = CCMoveBy::create(m_interval,ccp(0,150));
				CCScaleTo* scb = CCScaleTo::create(m_interval,1/body->getScale());
				CCSpawn* spa = CCSpawn::create(mt,scb,NULL);
				bigIcon->runAction(CCSequence::create(dely,spa,CCRemoveSelf::create(),NULL));
				body->addChild(bigIcon,100-m_interval*10);
			}else{
				if ((sBuffType)info->getBuffType() > sBuffType::eCurrBlood )
					CCLOG("[ ERROR ] BufExp::CreateBigIcon Lost Big Icon type=%d",info->getBuffType());
			}
		}
	}
	//刷新移除后个个图标的位置、最多同时显示4个小图标
	void BufExp::updatePosition( CCObject* ob )
	{
		BaseRole* alive = (BaseRole*)ob;
		if (m_hpSize.equals(CCPointZero))
		{
			m_hpSize.x = alive->getRoleObject()->getHp()->getContentSize().width;
			m_hpSize.y = alive->getRoleObject()->getHp()->getContentSize().height-GRID_HEIGHT/2+3;
		}
		BuffEffectMapList* EffectMap = alive->getBuffManage()->getEffectMap();
		int index = 0;
		for (auto i:*EffectMap)
		{
			for (auto j:i.second)
			{
				CCSprite* smallIcon = dynamic_cast<CCSprite*>(j);	
				if (!smallIcon)
					continue;
				int x = smallIcon->getContentSize().width+3;
				smallIcon->setScale(1/alive->getZoom());
				if (index>3)
					smallIcon->setVisible(false);		//最多同时显示4个小图标
				if (alive->getEnemy())
				{
					smallIcon->setPosition(ccp(m_hpSize.x/2-index*x,m_hpSize.y));//根据已存在的icon个数 size 做偏移处理
				}else{
					smallIcon->setPosition(ccp(-m_hpSize.x/2+index*x,m_hpSize.y));
				}
				index++;
				break;
			}
		}
	}
	//中毒类效果每次执行时刷新显示一次buff的效果,让buff执行起来的效果更加明显一点
	void BufExp::upBuffEffect( CCObject* ob )
	{

	}

	void BufExp::ResetInterval( float dt )
	{
		if (m_interval>0)
			m_interval -= dt;
	}

};