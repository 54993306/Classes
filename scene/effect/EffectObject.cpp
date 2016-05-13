#include "EffectObject.h"
#include "scene/AnimationManager.h"
#include "AppUI.h"
#include "warscene/EffectData.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "common/CGameSound.h"
#include "GMessage.h"
#include "Battle/BattleMessage.h"

EffectObject::EffectObject()
	:m_effect(nullptr),m_TotalTime(0),m_Playtime(0),m_Type(EffectType::once),m_Music(0)
	,m_model(""),m_skewing(false),m_bIsNullEffect(false),m_Delaytime(0),m_LoopNum(1)
	,m_DurationTime(0),m_Shake(false),m_LoopInterval(0)
{}
EffectObject::~EffectObject(){}

EffectObject* EffectObject::create(const char* model,EffectType type /*=EffectType::once*/)
{
	EffectObject* pRet = new EffectObject();
	if( pRet && pRet->init() )
	{
		pRet->m_model = model;
		pRet->m_Type = type;
		pRet->autorelease();
		return pRet;
	}else{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool EffectObject::init()
{
	bool bset = false;
	do{
		m_effect = CCSprite::create();					
		bset = true;
		this->addChild(m_effect);
	} while (0);
	return bset;
}

void EffectObject::onEnter()
{
	CCNode::onEnter();
	if (strcmp(m_model.c_str(),"0")==0)
	{
		this->unscheduleAllSelectors();
		this->removeFromParentAndCleanup(true);
	}
	if (m_skewing)
	{
		//修改这个类，不要以m_model为索引，同样的特效可能有不同的效果,特效位置,特效循环次数,音效次数,音效开始播放时间等
		SpecialEffectInfo* spefdata = AnimationManager::sharedAction()->getSpcialEffectData()->getspEff(atoi(m_model.c_str())); 
		if (spefdata)
		{
			m_effect->setPosition(ccp(spefdata->getp_x(),spefdata->getp_y()));
			m_effect->setAnchorPoint(ccp(spefdata->getAcp_x(),spefdata->getAcp_y()));
			m_effect->setScale(spefdata->getscale());
		}
	}
	play();
}
//特效帧数控制音效播放，控制音效播放次数，播放位置
void EffectObject::play()
{
	CCAnimation* Animation = AnimationManager::sharedAction()->getAnimation(m_model.c_str());//获取特效动画的区域赋值给特效对象
	if( Animation == nullptr )
	{
		this->scheduleUpdate();
		m_bIsNullEffect = true;
		return;
	}
	CCAnimate* animate = CCAnimate::create(Animation);
	switch (m_Type)
	{
	case EffectType::once:
		{
			this->scheduleUpdate();
			animate->setTag((int)EffectType::Repeat);
			m_effect->runAction(animate);
			if (m_Music)
				playeEffectMusic(m_Music);
			m_TotalTime = Animation->getDuration();
		}break;
	case EffectType::Repeat:
		{
			CCAction* Action = CCRepeatForever::create(animate);
			Action->setTag((int)EffectType::Repeat);
			m_effect->runAction(Action);
		}break;
	case EffectType::RepeatNum:
		{
			this->scheduleUpdate();
			CCAction* Action = CCRepeat::create(animate,m_LoopNum);
			Action->setTag((int)EffectType::Repeat);
			m_effect->runAction(Action);
			m_TotalTime = Animation->getDuration()*m_LoopNum;
		}break;
	case EffectType::Delay:
		{
			CCDelayTime* delay = CCDelayTime::create(m_Delaytime);
			CCAction* seq = nullptr;
			CCDelayTime* Interval = CCDelayTime::create(m_LoopInterval);
			if (m_Shake)
			{
				CCCallFunc* func = CCCallFunc::create(this,callfunc_selector(EffectObject::ShakeMessage));
				CCCallFunc* func1 = CCCallFunc::create(this,callfunc_selector(EffectObject::PlayerMusic));
				CCSequence* pSeq = CCSequence::create(animate/*,func*/,Interval,nullptr);									//每次重复都震动
				seq = CCSequence::create(delay,func,CCRepeat::create(pSeq,m_LoopNum),CCRemoveSelf::create(),nullptr);		//只震动一次然后播重复动作
			}else{
				CCSequence* pSeq = CCSequence::create(animate,Interval,nullptr);
				seq = CCSequence::create(delay,CCRepeat::create(pSeq,m_LoopNum),CCRemoveSelf::create(),nullptr);
			}
			seq->setTag((int)EffectType::Repeat);
			m_effect->runAction(seq);
			m_TotalTime = Animation->getDuration()*m_LoopNum+m_Delaytime;
		}break;
	case EffectType::Duration:
		{
			CCAction* Action = CCRepeatForever::create(animate);
			Action->setTag((int)EffectType::Repeat);
			m_effect->runAction(Action);
			m_TotalTime = m_DurationTime;
			this->scheduleUpdate();
		}break;
	default:
		break;
	}
}
//非循环类特效播放完毕自动删除
void EffectObject::update(float dt)
{
	m_Playtime += dt;
	if (this->isActionDone() || m_bIsNullEffect ||
		(m_Type == EffectType::Duration && m_Playtime >= m_DurationTime))
	{
		m_effect->stopAllActions();
		this->unscheduleAllSelectors();
		this->removeFromParentAndCleanup(true);
	}
}

bool EffectObject::isActionDone()
{
	bool done = false;
	do{
		if (!m_effect)
		{
			CCLOG("ERROR:EffectObject::isDone()  m_effect IS NULL");
			return true;
		}
		if (m_Type == EffectType::Repeat)
			return done;
		CCAction* Action = m_effect->getActionByTag((int)EffectType::Repeat);
		if(!Action || Action->isDone())		//CCAction的isDone方法被它的不同子类重写过
		{
			done = true;
			break;
		}
	} while (0);
	return done;
}

void EffectObject::setEffAnchorPoint(float x, float y)
{
	m_effect->setAnchorPoint(ccp(x,y));
}

void EffectObject::playeEffectMusic(int musicId)
{
	if (!musicId)return;
	char musicAds[60] = {0};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	sprintf(musicAds,"SFX/Test/%d.mp3",musicId);
#else
	sprintf(musicAds,"SFX/%d.ogg",musicId);
#endif
	PlayEffectSound(musicAds);				//播放效果音效
}

void EffectObject::setShaderEffect( CCGLProgram* pGl )
{
	m_effect->setShaderProgram(pGl);
}

void EffectObject::ShakeMessage()
{
	NOTIFICATION->postNotification(B_Shark,nullptr);
}

void EffectObject::PlayerMusic()
{
	if (m_Music)
		playeEffectMusic(m_Music);
}
