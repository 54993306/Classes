#include "RoleUpdate.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"

#include "ChangeName.h"
#include "ChangeHeadImg.h"

#include "set/SetLayer.h"
#include "common/CShowToBuyResource.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "scene/AnimationManager.h"

CRoleUpdate::CRoleUpdate()
{

}

bool CRoleUpdate::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("RoleUpdate");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("RoleUpdate.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);

		return true;
	}
	return false;

}

void CRoleUpdate::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CRoleUpdate::onEnter()
{
	BaseLayer::onEnter();
	setTouchPriority(-2000);
}


void CRoleUpdate::onExit()
{
	BaseLayer::onExit();
}

CRoleUpdate::~CRoleUpdate()
{
}



bool CRoleUpdate::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	return true;
}

void CRoleUpdate::updateRole(UserData * data, const Role *role)
{
	showRoleData(data,role);
	runAnimation();
}

void CRoleUpdate::runAnimation()
{
	CLayout *up = (CLayout *)(m_ui->findWidgetById("up"));
	CLayout *levelLay1 = (CLayout *)(m_ui->findWidgetById("levelLay1"));
	CLayout *levelLay2 = (CLayout *)(m_ui->findWidgetById("levelLay2"));
	CLayout *levelLay3 = (CLayout *)(m_ui->findWidgetById("levelLay3"));
	CLayout *levelLay4 = (CLayout *)(m_ui->findWidgetById("levelLay4"));
	runMoveAnim(up, 0.2f);
	runMoveAnim(levelLay1, 0.25f);
	runMoveAnim(levelLay2, 0.35f);
	runMoveAnim(levelLay3, 0.4f);
	runMoveAnim(levelLay4, 0.45f);

	CCPoint pos = ccp(VCENTER.x,up->findWidgetById("lev")->getPositionY());
	CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("7035");
	CCSprite *light = createAnimationSprite("skill/7035.png",pos,lightAnim,true);
	if (light)
	{
		m_ui->addChild(light);
	}

	CCNode *lv_up = up->findWidgetById("lv_up");
	CCAnimation *lvAnim = AnimationManager::sharedAction()->getAnimation("7037");
	if (lvAnim)
	{
		lvAnim->setDelayPerUnit(0.25f);
		lv_up->runAction(CCRepeatForever::create(CCAnimate::create(lvAnim)));
	}
}

void CRoleUpdate::runMoveAnim(CLayout * node, float delay)
{
	node->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delay),CCMoveTo::create(0.2f,ccp(node->getPositionX()+VCENTER.x+200,node->getPositionY()))));
}

void CRoleUpdate::showRoleData(UserData * data, const Role * role)
{
	CLabelAtlas *lev = (CLabelAtlas *)(m_ui->findWidgetById("lev"));
	lev->setString(ToString(role->rolelv()));
	for (int i = 1; i <= 3; i++)
	{
		CLabel *level = (CLabel *)(m_ui->findWidgetById(CCString::createWithFormat("level%d",i)->getCString()));
		CLabel *nextlevel = (CLabel *)(m_ui->findWidgetById(CCString::createWithFormat("nextLevel%d",i)->getCString()));
		level->setString(ToString(data->getLevel()));
		nextlevel->setString(ToString(role->rolelv()));
	}
	CLabel *action = (CLabel *)(m_ui->findWidgetById("action"));
	action->setString(ToString(data->getActionLimit()));
	CLabel *nextAction = (CLabel *)(m_ui->findWidgetById("nextAction"));
	nextAction->setString(ToString(role->actionlimit()));

	CImageView *hero = (CImageView *)(m_ui->findWidgetById("hero"));
	CCTexture2D *texture = CCTextureCache::sharedTextureCache()
		->addImage(CCString::createWithFormat("selectRole/%d.png", data->getThumb())->getCString());
	if (texture)
	{
		hero->setTexture(texture);
		hero->setTextureRect(CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
	}
}

void CRoleUpdate::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
	onClose(nullptr);
}



