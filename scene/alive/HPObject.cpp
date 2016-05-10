#include "HPObject.h"
#include "ActObject.h"
#include "GMessage.h"
#include "warscene/WarControl.h"
#include "model/DataCenter.h"
#include "warscene/ConstNum.h"
#include "warscene/BattleResult.h"
#include "common/CGameSound.h"
#include "common/ShaderDataHelper.h"
#include "warscene/CHeroSoundData.h"
HPObject::HPObject()
	:m_hp(nullptr)
	,m_bg(nullptr)
	,m_hpVal(0)
	,m_maxHpVal(0)
	,m_percent(0)
{}
HPObject::~HPObject()
{
	if(m_hp) removeChild(m_hp);
	if(m_bg) removeChild(m_bg);
	CC_SAFE_RELEASE(m_hp);
	CC_SAFE_RELEASE(m_bg);
	m_hp = nullptr;
	m_bg = nullptr;
}
void HPObject::setSkin(const char* bgSkin,const char* hpSkin,bool isEnemy)
{
	do{
		if( m_bg != nullptr ) break;
		if( bgSkin == nullptr ) break;
		m_bg = CCSprite::create(bgSkin);
		if( m_bg != nullptr )
		{
			m_bg->retain();
			this->addChild(m_bg);
		}
		this->setContentSize(m_bg->getContentSize());
	} while (0);
	do{
		if(m_hp != nullptr) break;
		if(hpSkin == nullptr ) break;
		CCSprite* hp = CCSprite::create(hpSkin);
		if(hp == nullptr ) break;
		m_hp = CCProgressTimer::create(hp);
		if( m_hp != nullptr )
		{
			m_hp->setType(kCCProgressTimerTypeBar);
			if (isEnemy)
			{
				m_hp->setBarChangeRate(ccp(1,0));
				m_hp->setMidpoint(ccp(0,0));
			}else{
				m_hp->setBarChangeRate(ccp(1,0));
				m_hp->setMidpoint(ccp(1,0));
			}
			m_hp->setPercentage(0.0f);
			m_hp->retain();
			this->addChild( m_hp );
		}
	} while (0);
}
void HPObject::setHp(float hp)
{
	if( m_hpVal == hp )
	{
		CCLOG("[ *TIPS ] HPObject::setHp m_hpVal == hp");
		return;
	}
	m_hpVal = hp;
	if (m_hpVal >= m_maxHpVal)
		m_hpVal = m_maxHpVal;
	updateShow();
}
void HPObject::setMaxHp(float maxHp)
{
	if( m_maxHpVal == maxHp ) return;
	m_maxHpVal = maxHp;
	updateShow();
}
void HPObject::updateShow()
{
	if( m_maxHpVal <= 0 ) return;
	m_percent = m_hpVal / m_maxHpVal * 100;
	if( !m_hp ) 
		return;
	float oldPe = m_hp->getPercentage();
	CCProgressFromTo* proto = CCProgressFromTo::create(0.3f,oldPe,m_percent);
	m_hp->runAction(proto);
}

bool HPObject::hitEffect(ActObject* target)
{
	CCNode* Effect = DataCenter::sharedData()->getRoleData()->getActionEffect(target->getModel());
	if (Effect)
	{
		target->addChild(Effect);
		return false;
	}
	return true;
}

void HPObject::playerNum(ActObject* target,int num,int type)
{
	if (type == nullType)return;
	if (!num&&type!=typeface)
	{
		CCLOG(" [ Tips ] HPObject::playerNum Num = 0!");
		return	;
	}
	int callType = target->getAlive()->getCallType();
	if ( callType== OnlyOnce || callType == AutoSkill)
	{
		this->setVisible(false);
		return;
	}
	showHp(target);
	float scale = target->getScale();								//得到目标的缩放比例
	int moveNum = CCRANDOM_0_1()*170;
	CCScaleTo* sc = CCScaleTo::create(0.15f,1/scale*1.3f);
	CCScaleTo* sc1 = CCScaleTo::create(0.15f,1/scale*1.0f);
	CCMoveBy* moveBy = CCMoveBy::create(0.25f, ccp(0,moveNum));
	CCFadeOut* fadeout = CCFadeOut::create(0.6f);
	CCDelayTime* dely = CCDelayTime::create(0.25f);
	CCSpawn* spw = CCSpawn::create((CCMoveBy*)moveBy->copy(),(CCFadeOut*)fadeout->copy(),NULL);
	CCSequence* sqe = CCSequence::create(sc,sc1,(CCDelayTime*)dely->copy(),spw, CCRemoveSelf::create() , NULL);

	CCScaleTo* crit_sc = CCScaleTo::create(0.05f,1);
	CCScaleTo* crit_sc2 = CCScaleTo::create(0.1f,1.2f);
	CCMoveBy* moveBy2 = CCMoveBy::create(0.05f, ccp(0,moveNum));
	CCSpawn* crit_up = CCSpawn::create(crit_sc,moveBy2,NULL);
	CCFadeOut* fadeout1 = CCFadeOut::create(0.8f);
	CCSequence* crit_sqeBg = CCSequence::create(crit_up,crit_sc2,(CCDelayTime*)dely->copy(),fadeout1,CCRemoveSelf::create(),NULL);

	CCScaleTo* crit_sc3 = CCScaleTo::create(0.05f,1);
	CCScaleTo* crit_sc4 = CCScaleTo::create(0.1f,2.3f);
	CCMoveBy* moveBy3 = CCMoveBy::create(0.2f, ccp(0,moveNum));
	CCSpawn* crit_up2 = CCSpawn::create(crit_sc3,moveBy3,NULL);
	CCFadeOut* fadeout2 = CCFadeOut::create(1.3f);
	CCSequence* crit_sqe = CCSequence::create(crit_up2,crit_sc4,(CCDelayTime*)dely->copy(),fadeout2,CCRemoveSelf::create(),NULL);

	int HpNum = abs(num);
	CCLabelAtlas* NumLabel = nullptr;
	CCSprite* font = nullptr;
	string str ="/"+string(ToString(HpNum));
	switch (type)
	{
	case typeface:
		{
			font = CCSprite::create("label/miss.png");
			font->setScale(1/scale);
			font->runAction(sqe);
			font->setPosition(ccp(-100,0));
			target->addChild(font);
			return;
		}	break;
	case gainType:
		{
			str ="."+string(ToString(HpNum));
			NumLabel = CCLabelAtlas::create(str.c_str(),"label/green.png",31,39,46);
			setHp(m_hpVal + HpNum);								//存在交替变化的清楚出现如果合击的话
			PlayEffectSound(SFX_513);
		}	break;
	case angerType:
		{
			str ="."+string(ToString(HpNum));
			NumLabel = CCLabelAtlas::create(str.c_str(),"label/yellow.png",26,32,46);
		}break;
	case generalType:	{ NumLabel = CCLabelAtlas::create(str.c_str(),"label/number1.png",26,32,46); } break;
	case cutType:		{ NumLabel = CCLabelAtlas::create(str.c_str(),"label/number2.png",26,32,46); } break;
	case addType:		{ NumLabel = CCLabelAtlas::create(str.c_str(),"label/number3.png",26,32,46); } break;
	case genralCritType:{ NumLabel = CCLabelAtlas::create(str.c_str(),"label/number1.png",26,32,46); } break;
	case cutCritType:	{ NumLabel = CCLabelAtlas::create(str.c_str(),"label/number2.png",26,32,46); } break;
	case addCritType:	{ NumLabel = CCLabelAtlas::create(str.c_str(),"label/number3.png",26,32,46); } break;
	default:	break;
	}
	if (type > gainType)										//武将掉血变红
	{
		target->getAlive()->setcloaking(false);					//潜行状态取消
		NumLabel = CCLabelAtlas::create(str.c_str(),"label/myred.png",31,39,46); 
		setHp(m_hpVal - HpNum);	
		if (hitEffect(target))
		{
			CCTintTo* to1 = CCTintTo::create(0.25f,255,0,0);
			CCTintTo* to2 = CCTintTo::create(0,255,255,255);
			target->getArmature()->runAction(CCSequence::create(to1,to2,NULL));
		}
		if (target->getAlive()->getCaptain())					//主帅掉血提示
		{
			NOTIFICATION->postNotification(CAPTAINHIT,target->getAlive());
			NOTIFICATION->postNotification(SHAKE_BYEFFECT,nullptr);
		}
	}
	if (!NumLabel)return;
	if (type >=  genralCritType)
	{
		font = CCSprite::create("warScene/blood_mobile.png");
		CCSequence* runSqu = (CCSequence*)crit_sqe->copy();
		NumLabel->setScale(0.1f);
		NumLabel->runAction(runSqu);
	}else{
		CCSequence* runSqu = (CCSequence*)sqe->copy();
		NumLabel->runAction(runSqu);
	}
	ActObject* act = (ActObject*)target;
	if (act->getEnemy())
	{
		NumLabel->setPosition(ccp(-170,100));
	}else{
		NumLabel->setPosition(ccp(-50,100));
	}
	target->addChild(NumLabel,1);

	if (!font)return;						//暴击的显示方式，字体旋转一定角度，敌我双方显示位置不同
	CCFadeIn* fain = CCFadeIn::create(0.2f);
	CCDelayTime* detime = CCDelayTime::create(0.6f);

	CCSequence* bgsqe = CCSequence::create(fain,detime, CCRemoveSelf::create(), NULL);
	
	CCNode* HpNumBg = target->getChildByTag(CritHpBmg_Sp);
	if (HpNumBg)
	{
		HpNumBg->stopAllActions();
		CCSequence* runSqu = (CCSequence*)bgsqe->copy();
		HpNumBg->runAction(runSqu);
	}else{
		if (act->getEnemy())
		{
			font->setPosition(ccp(-40,120));
		}else{
			font->setPosition(ccp(20,120));
		}
		CCSequence* runSqu = (CCSequence*)crit_sqeBg->copy();
		font->runAction(runSqu);
		font->setTag(CritHpBmg_Sp);
		target->addChild(font);
	}
}

CCProgressTimer* HPObject::getPro()
{
	return m_hp;
}

void HPObject::showHp( CCObject* ob )
{
	this->unschedule(schedule_selector(HPObject::hideHp));
	this->setVisible(true);
	this->scheduleOnce(schedule_selector(HPObject::hideHp),5.0f);
}

void HPObject::hideHp( float dt )
{
	this->setVisible(false);
}


