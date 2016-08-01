#include "HeroDetail.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"

bool CHeroDetail::init()
{
	if (BaseLayer::init())
	{
		this->setIsShowBlack(false);
		m_ui = LoadComponent("HeroDetail.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
	
		return true;
	}
	return false;
}

void CHeroDetail::onClickButton(CCObject* pSender)
{

}

void CHeroDetail::showHeroInfo(CHero* hero)
{
	m_hero = hero;
	CLayout *font = (CLayout*)(m_ui->findWidgetById("font"));
	CLabel *hp = (CLabel*)(font->findWidgetById("hp"));
	hp->setString(ToString(hero->hp));

	CLabel *attack = (CLabel*)(font->findWidgetById("attack"));
	attack->setString(ToString(hero->atk));

	CLabel *deffend = (CLabel*)(font->findWidgetById("defend"));
	deffend->setString(ToString(hero->def));
	deffend->setVisible(hero->def>0||hero->addDef);
	CLabel *def_font = (CLabel*)(font->findWidgetById("def_font"));
	def_font->setVisible(hero->def>0||hero->addDef);

	CLabel *crit = (CLabel*)(font->findWidgetById("crit"));
	crit->setString(CCString::createWithFormat("%d", hero->crit)->getCString());

	CLabel *hit = (CLabel*)(font->findWidgetById("hit"));
	hit->setString(ToString(hero->hit));

	CLabel *dodge = (CLabel*)(font->findWidgetById("dodge"));
	dodge->setString(ToString(hero->dodge));
	
	CLabel *firstAtk = (CLabel*)(font->findWidgetById("firstAtk"));
	firstAtk->setString(ToString(hero->renew));

	CLabel *addhp = (CLabel*)(font->findWidgetById("addhp"));

	CLabel *addatk = (CLabel*)(font->findWidgetById("addatk"));

	CLabel *adddef = (CLabel*)(font->findWidgetById("adddef"));

	CLabel *addcrit = (CLabel*)(font->findWidgetById("addcrit"));

	CLabel *addhit = (CLabel*)(font->findWidgetById("addhit"));

	CLabel *adddodge = (CLabel*)(font->findWidgetById("adddodge"));

	CLabel *dex = (CLabel*)(font->findWidgetById("addact"));

	CLabel *dexf = (CLabel*)(font->findWidgetById("dex"));
	CLabel *adddexf = (CLabel*)(font->findWidgetById("adddex"));
	CLabel *dexfont = (CLabel*)(font->findWidgetById("dex_font"));
	dexf->setString(CCString::createWithFormat("%d",hero->dex)->getCString());

	if (hero->addDex>0)
	{
		adddexf->setString(CCString::createWithFormat("+%d",hero->addDex)->getCString());
	}
	else
	{
		adddexf->setString("");
	}

	if (hero->addHp>0)
	{
		addhp->setString(CCString::createWithFormat("+%d",hero->addHp)->getCString());
	}
	else
	{
		addhp->setString("");
	}
	
	if (hero->addDef>0)
	{
		adddef->setString(CCString::createWithFormat("+%d",hero->addDef)->getCString());
	}
	else
	{
		adddef->setString("");
	}

	if (hero->addAtk>0)
	{
		addatk->setString(CCString::createWithFormat("+%d",hero->addAtk)->getCString());
	}
	else
	{
		addatk->setString("");
	}

	if (hero->addHit>0)
	{
		addhit->setString(CCString::createWithFormat("+%d",hero->addHit)->getCString());
	}
	else
	{
		addhit->setString("");
	}

	if (hero->addCrit>0)
	{
		addcrit->setString(CCString::createWithFormat("+%d",hero->addCrit)->getCString());
	}
	else
	{
		addcrit->setString("");
	}

	if (hero->addDodge>0)
	{
		adddodge->setString(CCString::createWithFormat("+%d",hero->addDodge)->getCString());
	}
	else
	{
		adddodge->setString("");
	}

	if (hero->addRenew>0)
	{
		dex->setString(CCString::createWithFormat("+%d",hero->addRenew)->getCString());
	}
	else
	{
		dex->setString("");
	}

	int count = 8;
	if (hero->def<=0&&hero->addDef<=0)
	{
		count--;
	}
} 

void CHeroDetail::onEnter()
{
	BaseLayer::onEnter();
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO,this,GameMsghandler_selector(CHeroDetail::updateHeroInfo));
}


void CHeroDetail::updateHeroInfo(const TMessage& tMsg)
{
	CHero *hero = (CHero*)tMsg.lParam;
	showHeroInfo(hero);
}

void CHeroDetail::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
}

CHeroDetail::~CHeroDetail()
{
	
}

