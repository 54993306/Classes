#ifndef __HERO_CONTROL_
#define __HERO_CONTROL_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"

#include "HeroList.h"

#include "HeroSkill.h"

#include "HeroAttribute.h"
#include "HeroDetail.h"
#include "HeroMove.h"
#include "HeroEvolve.h"
#include "HeroData.h"
#include "HeroExp.h"
USING_NS_CC;

class CHeroDelegate
{
public:
	virtual void onHeroInfo(CHero *hero,int showType) = 0;			//英雄信息		
	virtual void onResetLayer() =0;
	virtual void onEvolveHero() = 0;
	virtual void onAddHeroExp(int itemId, int num) = 0;
	virtual void onAddBreak(int food) = 0;
	virtual void onBeforeHero() = 0;
	virtual void onAfterHero() = 0;

private:
};

class CHeroControl: public BaseLayer ,public CHeroDelegate
{
public:
	CHeroControl();
	CREATE_LAYER(CHeroControl);
	virtual bool init();
	void processMessage(int type, google::protobuf::Message *msg);
	void onExit();
	void updateHeroValue(CHero * hero);

	void onHeroInfo(CHero *hero,int showType);
	void onResetLayer();
	void processEvolveMsg(int type, google::protobuf::Message *msg);
	void onEvolveHero();
	void onAddHeroExp(int itemId, int num);
	void onAddBreak(int food);

	void onBeforeHero();
    void onAfterHero();
public:
	//请求英雄进阶任务信息 54
	void sendEvolveInfo(int itemId);
	void sendEvolveHero(int heroId);
	//角色英雄ID ,使用道具ID, 使用道具数量
	void sendAddHeroExp(int heroId, int itemId, int itemNum);
	//英雄突破
	void sendHeroBreak(int heroId, int useFood);
	int findHeroPosIndex(int heroId);
	void setIsHideOperation(bool isHide);
	void recruitMessage(int type, google::protobuf::Message *msg);
protected:
private:
	CHeroList *m_heroLayer;
	vector<CHero> m_heroList;
    CHero m_hero;
	CHeroAttribute *m_heroAttrLayer;
	CHeroSkill *m_skillLayer;
	CHeroDetail *m_detailLayer;
	CHeroMove *m_moveLayer;
	CHeroEvolve *m_evolveLayer;
	CHeroExp* m_expLayer;
	int m_isShowOpr;
	int m_insertIndex;
	int m_iOwnCount;
};


#endif
