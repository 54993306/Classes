#include "RageObject.h"
#include "ActObject.h"
#include "warscene/ConstNum.h"
#include "common/color.h"
#include "scene/alive/HPObject.h"
#include "model/DataCenter.h"
#include "Battle/BattleRole.h"
namespace BattleSpace{
	RageObject::RageObject()
		:m_rage(nullptr)
		,m_bg(nullptr)
		,m_rageval(0)
		,m_rageMax(0)
		,m_percent(0)
	{}

	RageObject::~RageObject()
	{
		if(m_rage) removeChild(m_rage);
		if(m_bg) removeChild(m_bg);
		CC_SAFE_RELEASE(m_rage);
		CC_SAFE_RELEASE(m_bg);
		m_rage = nullptr;
		m_bg = nullptr;
	}

	void RageObject::setSkin(const char* bg,const char* fore)
	{
		do{
			if(m_bg)break;
			if(!bg)break;
			m_bg = CCSprite::create(bg);
			if (m_bg)
			{
				this->addChild(m_bg);
				m_bg->setScale(m_bg->getScale()*0.9f);
				m_bg->retain();
			}
			if(m_rage)break;
			if(!fore)break;
			CCSprite* rg = CCSprite::create(fore);
			rg->setScale(rg->getScale()*0.9f);
			if (!rg)break;
			m_rage = CCProgressTimer::create(rg);
			if (m_rage)
			{
				m_rage->setType(kCCProgressTimerTypeBar);
				setDirection(R_up);
				m_rage->setPercentage(0.0f);
				m_rage->retain();
				this->addChild(m_rage);
			}
		} while (0);
	}

	void RageObject::setDirection(int Direction)
	{
		switch (Direction)
		{
		case R_up:						//向上为增
			{
				m_rage->setBarChangeRate(ccp(0,1));		//变化方向为Y轴
				m_rage->setMidpoint(ccp(0,0));			//中心点,越靠近中心点值越小
			}break;
		case R_Down:
			{
				m_rage->setBarChangeRate(ccp(0,1));
				m_rage->setMidpoint(ccp(0,1));
			}break;
		case R_left:
			{
				m_rage->setBarChangeRate(ccp(1,0));
				m_rage->setMidpoint(ccp(1,0));
			}break;
		case R_Right:
			{
				m_rage->setBarChangeRate(ccp(1,0));
				m_rage->setMidpoint(ccp(0,0));
			}break;
		default:
			break;
		}
	}

	void RageObject::setVal(float rage)
	{
		if(m_rageval == rage)return;
		m_rageval = rage;
		updateRage();
	}

	void RageObject::setRageMax(float rageMax)
	{
		if(m_rageMax == rageMax) return;
		m_rageMax = rageMax;
		updateRage();
	}

	void RageObject::updateRage()
	{
		if (m_rageMax <= 0) return;
		m_percent = m_rageval / m_rageMax *100;
		if(m_rage)
		{
			float oldPe = m_rage->getPercentage();
			CCProgressFromTo* proto = CCProgressFromTo::create(0.3f,oldPe,m_percent);
			m_rage->runAction(proto);
		} 
	}

	void RageObject::setPercent(float pe,float p)
	{
		m_percent += p;
		if(pe) m_rage->setPercentage(pe);
	}

	//显示怒气值变化
	void RageObject::junpNum(CCObject* ob)
	{
		if (!ob)return;
		CCArray*arr = (CCArray*)ob;
		ActObject* aliveOb = (ActObject*)arr->objectAtIndex(0);
		CCInteger* killNum = (CCInteger*)arr->objectAtIndex(1);
		string str = "label/blue.png";
		if (aliveOb->getEnemy())str = "label/purple.png";
		string Num  = "."+string(ToString(killNum->getValue()));
		if (killNum->getValue() < 0)Num = "/"+string(ToString(killNum->getValue()));
		CCLabelAtlas* labAt = CCLabelAtlas::create(Num.c_str(),str.c_str(),30,37,46);
		CCFadeIn* fadein = CCFadeIn::create(1.0f);
		CCFadeOut* fadeout = CCFadeOut::create(1.0f);
		//CCCallFuncN* callfunc = CCCallFuncN::create(this ,callfuncN_selector(HPObject::removethis));
		CCMoveBy* moveBy = CCMoveBy::create(0.4f, ccp(0,200));
		float scale = aliveOb->getScale();	//得到目标的缩放比例
		CCScaleTo* sc = CCScaleTo::create(0.15f,1/scale*1.3f);
		CCScaleTo* sc1 = CCScaleTo::create(0.15f,1/scale*1.0f);
		CCSequence* scSqe = CCSequence::create(sc,sc1,NULL);
		CCSpawn* spw = CCSpawn::create(moveBy ,fadeout,NULL);
		CCSequence* sqe = CCSequence::create(scSqe ,spw ,CCRemoveSelf::create() ,NULL);
		labAt->runAction(sqe);	
		labAt->setPosition(ccp(-60,100));
		aliveOb->addChild(labAt);
	}

	void RageObject::setDisplay(bool display)
	{
		m_rage->setVisible(display);
		m_bg->setVisible(display);
	}

	CCProgressTimer* RageObject::getPro()
	{
		return m_rage;
	}

}
