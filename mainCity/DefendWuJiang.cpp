#include "DefendWuJiang.h"
#include "net/CNetClient.h"
#include "MainCityControl.h"
#include "GMessage.h"
#include "model/DataCenter.h"
#include "netcontrol/CPlayerControl.h"
#include "tools/ShowTexttip.h"
#include "common/CommonFunction.h"
#include "scene/alive/ActionDef.h"
#include "friend/FriendLayer.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "common/CCRollLabelAction.h"

bool CDefendWuJiang::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("MaskLayerDefend");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("wujiangZhushou.xaml"); 
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CDefendWuJiang::onEnter()
{
	BaseLayer::onEnter();

	//确定框
	CButton *btn = dynamic_cast<CButton*>(m_ui->getChildByTag(1));
	btn->setOnClickListener(this,ccw_click_selector(CDefendWuJiang::onClickButton));

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CDefendWuJiang::onClose));
	this->addChild(pClose, 999);
	
	//人物列表
	m_grid = (CGridView*)(m_ui->findWidgetById("gridView"));
	m_grid->setColumns(3);
	m_grid->setCountOfCell(0);
	CCSize size = m_grid->getContentSize();
	m_grid->setSizeOfCell(CCSizeMake(size.width/3,size.width/3));
	m_grid->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CDefendWuJiang::gridViewDataSource));
	m_grid->setBounceable(false);
	m_grid->reloadData();

	//选择框
	m_selectImg = (CCSprite*)(m_ui->findWidgetById("selectImg"));
	CC_SAFE_RETAIN(m_selectImg);
	m_selectImg->removeFromParentAndCleanup(true);
	m_selectImg->setVisible(true);

	//请求数据
	CNetClient::getShareInstance()->registerMsgHandler(HeroListMsg,this,CMsgHandler_selector(CDefendWuJiang::ProcessMsg));
	CNetClient::getShareInstance()->registerMsgHandler(GarrisonHero,this,CMsgHandler_selector(CDefendWuJiang::ProcessMsg));
}

void CDefendWuJiang::onExit()
{
	BaseLayer::onExit();
	CNetClient::getShareInstance()->unRegisterAllMsgHandler(this);
	CC_SAFE_RELEASE(m_selectImg);

	sort(m_VecMode.begin(),m_VecMode.end());
	m_VecMode.erase(unique(m_VecMode.begin(),m_VecMode.end()),m_VecMode.end());	
	vector<int>::iterator iter = m_VecMode.begin();
	for (;iter!=m_VecMode.end();iter++)
	{
		CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(*iter));
		if (animationData)
		{
			char str[60]={0};
			sprintf(str,"BoneAnimation/%d.ExportJson",*iter);
			CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(str);
		}
	}
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void CDefendWuJiang::onClickButton(CCObject* pSender)
{
	int  tag = ((CButton*)pSender)->getTag();

	//驻守
	if (tag==1)
	{
		//清空已有的英雄
		if (m_selectIndex == 0 && m_iStartIndex==1)
		{
 			CMainCityControl::getInstance()->sendGarrisonHero(m_cityId,m_selectIndex,m_replaceId);
		}
		else if (m_heroList.size()>0)
		{
			CMainCityControl::getInstance()->sendGarrisonHero(m_id,m_heroList[m_selectIndex-m_iStartIndex].id, m_replaceId);
		}
	}
}

void CDefendWuJiang::setCityId(int id, int cityId)
{
	m_id = id;
	m_cityId = cityId;
}

void CDefendWuJiang::setReplaceId(int id)
{
	m_replaceId = id;
	if(id!=0)
	{
		m_iStartIndex = 1;
		m_selectIndex = 1;
	}
}

CCObject* CDefendWuJiang::gridViewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;
	if (!pCell)
	{
		pCell = new CGridViewCell();
		pCell->autorelease();
		addTableCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx, pCell);
	}
	return pCell;
}


void CDefendWuJiang::addTableCell( unsigned int uIdx, CGridViewCell * pCell )
{
	pCell->setTag(uIdx);
	SndButton *btn = SndButton::create("common/cell.png","common/cell.png");
	btn->setAnchorPoint(CCPointZero);
	btn->setTag(uIdx);
	btn->setOnClickListener(this,ccw_click_selector(CDefendWuJiang::onSelectHero));
	pCell->addChild(btn);

	if(uIdx == m_selectIndex)
	{
		m_selectImg->removeFromParentAndCleanup(true);
		btn->addChild(m_selectImg, 999);
		NodeFillParent(m_selectImg);
		m_selectImg->setScale(m_selectImg->getScale()*1.15f);
	}

	if(m_iStartIndex == 1 && uIdx==0)
	{
		//绑定点击取消在上面
		CCSprite* pCancel = CCSprite::create("common/guard2.png");
		btn->addChild(pCancel);
		NodeFillParent(pCancel);
		return;
	}

	if (m_heroList.size()>uIdx-m_iStartIndex)
	{
		CHero &hero = m_heroList[uIdx-m_iStartIndex];
		string icon = ToString(hero.thumb);
		if (icon!="")
		{	
			CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png",hero.thumb)->getCString());
			if (!head)
			{
				head = CCSprite::create("headImg/101.png");
				CCLOG("[ ERROR ] CDefendWuJiang::gridViewDataSource Lost Image = %d",hero.thumb);
			}
			btn->addChild(head);
			NodeFillNode(head, btn);
			btn->setOnLongClickListener(this,ccw_longclick_selector(CDefendWuJiang::longSelectHero));
		}

		//边框
		CCSprite *mask = CCSprite::createWithTexture(setItemQualityTexture(hero.iColor));
		btn->addChild(mask, 2);
		NodeFillNode(mask, btn);
		mask->setScale(mask->getScale()*1.05f);
		//添加星级
		CLayout* pStarLayout = getStarLayout(hero.quality);
		mask->addChild(pStarLayout);
	}


}

bool CDefendWuJiang::longSelectHero(CCObject* pSender, CCTouch* pTouch)
{
	CButton *btn = (CButton*)pSender;
	int tag = ((CButton*)pSender)->getTag();

	return true;
}

void CDefendWuJiang::showHeroList()
{
	m_grid->setCountOfCell(m_heroList.size()+m_iStartIndex);
	m_grid->reloadData();
}

void CDefendWuJiang::onSelectHero(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	int tag = ((CButton*)pSender)->getTag();
	m_selectIndex = tag;

	//取消驻守
	if (tag==0 && m_iStartIndex==1)
	{
		/*m_selectIndex = 0;*/
		CMainCityControl::getInstance()->sendGarrisonHero(m_id, m_selectIndex, m_replaceId);
	}
	//更新武将信息
	else 
    {
		//更新选择框位置
		m_selectImg->removeFromParentAndCleanup(true);
		btn->addChild(m_selectImg, 999);
		NodeFillParent(m_selectImg);
		m_selectImg->setScale(m_selectImg->getScale()*1.15f);

	    CHero &hero = m_heroList[tag-m_iStartIndex];

		CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
		const HeroInfoData *c_data = DataCenter::sharedData()->getHeroInfo()->getCfg(hero.thumb);
		if(c_data && c_data->heroName.size() > 0)
			name->setString(c_data->heroName.c_str());
	
		CLabel *state = (CLabel*)(m_ui->findWidgetById("state"));
		state->setString(GETLANGSTR(113));
	
		CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
		level->setString(ToString(hero.level));	

		CLabel *combat = (CLabel*)(m_ui->findWidgetById("power"));
		combat->setString(ToString(hero.combat));
	
		CLabel *effect = (CLabel*)(m_ui->findWidgetById("effect"));
		effect->setString(ToString(hero.prate));

		//战力
		CLabelAtlas *fight = (CLabelAtlas*)(m_ui->findWidgetById("level_fight"));
		fight->setAnchorPoint(ccp(0, 0.5f));
		int iFrom = atoi(fight->getString());
		fight->setString(ToString(hero.combat));
		fight->runAction(CCRollLabelAction::create(0.5f, iFrom, (hero.combat), fight));

		//品质
		if(c_data)
		{
			CCSprite *type = (CCSprite*)(m_ui->findWidgetById("type"));
			type->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png",hero.roletype, c_data->iType2)->getCString()));
		}

		//使用模型代替贴图
		CImageView* pBoard = (CImageView*)m_ui->findWidgetById("board_2");
		int m_ModeID = hero.thumb;
		char ExportJson_str[60] = {0};//"BoneAnimation/101.ExportJson"
		sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",m_ModeID);
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(ExportJson_str);
		CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(m_ModeID));
		if (!animationData)													//模型容错性处理
		{
			CCLOG("[ *ERROR ]  CHeroAttribute::showBaseInfo Model=%d IS NULL",m_ModeID); 
			m_ModeID = 516;
			sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",m_ModeID);
			CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(ExportJson_str);
		}	
		CCArmature* Armature = CCArmature::create(ToString(m_ModeID));
		if (Armature)
		{	
			m_VecMode.push_back(m_ModeID);
			Armature->getAnimation()->play(Stand_Action, 0.01f);
			pBoard->removeChildByTag(989);
			float zoom = hero.zoom*0.01f;
			if (!zoom)
				zoom = 300.0/Armature->getContentSize().height;			//容错性处理
			Armature->setScale(zoom);
			Armature->setTag(989);
			CCPoint pos = ccp(pBoard->getContentSize().width*0.5f, 107);
			Armature->setPosition(pos);
			pBoard->addChild(Armature);
			Armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(CDefendWuJiang::movementCallBack));
			m_pArmature = Armature;
		}	
	}
}

void CDefendWuJiang::ProcessMsg(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	switch (type)
	{
	//英雄列表
	case HeroListMsg:
		{
			HeroListResponse *res = (HeroListResponse*)msg;
			m_heroList.clear();
			for (int i=0; i<res->herolist_size(); i++)
			{
				Hero hero = res->herolist(i);
				CHero obj;
				obj.readData(hero);
				m_heroList.push_back(obj);
			}
			showHeroList();
			//默认选择
			onSelectHero(getItemByIndex(m_selectIndex));
		}
		break;

	case GarrisonHero:
		{
			GarrisonHeroResponse* res = (GarrisonHeroResponse*)msg;
			if (res->retval())
			{
				CCLOG("Garrison %s","true");	

				if (!res->has_extpe())
				{
					res->set_extpe(0);
				}

				//清空已有的英雄
				if (m_selectIndex==0 && m_iStartIndex==1)
				{
					CSceneManager::sharedSceneManager()->PostMessageA("updateHero", -1, nullptr, nullptr,res->extpe());
					CSceneManager::sharedSceneManager()->PostMessageA(GARRSION_HERO,m_cityId,nullptr,nullptr);
				}
				else
				{
					CHero *her = new CHero;
					*her = m_heroList[m_selectIndex-m_iStartIndex];
					CSceneManager::sharedSceneManager()->PostMessageA("updateHero", m_selectIndex, nullptr, her,res->extpe());
					CHero *hero = new CHero;
					*hero = m_heroList[m_selectIndex-m_iStartIndex];
					CSceneManager::sharedSceneManager()->PostMessageA(GARRSION_HERO,m_cityId,nullptr,hero);

				}
			}
			else
			{
				ShowPopTextTip(GETLANGSTR(235));
			}
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
		break;
	}
}

void CDefendWuJiang::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);
	//关闭
	LayerManager::instance()->pop();
	LayerManager::instance()->close("MaskLayerDefend");
}

CCNode* CDefendWuJiang::getItemByIndex( int iIndex )
{
	CCObject* pObj;
	CCARRAY_FOREACH(m_grid->getContainer()->getChildren(), pObj)
	{
		CCNode* pNode = (CCNode*)pObj;
		if(pNode->getTag() == iIndex)
		{
			return pNode->getChildByTag(iIndex);
		}
	}
	CCLOG("ERROR  CDefendWuJiang::getItemByIndex");
	return (CCNode*)(m_grid->getContainer()->getChildren()->objectAtIndex(0));
}

void CDefendWuJiang::movementCallBack( CCArmature* pAramture, MovementEventType type, const char* sData )
{
	switch (type)
	{
	case cocos2d::extension::START:
		break;
	case cocos2d::extension::LOOP_COMPLETE:
	case cocos2d::extension::COMPLETE:
		{
			//动作1
			float fRand = CCRANDOM_0_1();
			if(fRand>0.7)
			{
				if(m_pArmature->getAnimation()->getAnimationData()->getMovement(Stand_Action))
				{
					m_pArmature->getAnimation()->play(Stand_Action, 0.01f);
					return;
				}
			}
			else if(fRand>0.5)
			{
				if(m_pArmature->getAnimation()->getAnimationData()->getMovement(Attack_Action))
				{
					m_pArmature->getAnimation()->play(Attack_Action, 0.01f);
					return;
				}
			}
			else if(fRand>0.3f)
			{
				if(m_pArmature->getAnimation()->getAnimationData()->getMovement(SpAttack_Action))
				{
					m_pArmature->getAnimation()->play(SpAttack_Action, 0.01f);
					return;
				}
			}
			else if(fRand>0.1f)
			{
				if(m_pArmature->getAnimation()->getAnimationData()->getMovement(Win_Action))
				{
					m_pArmature->getAnimation()->play(Win_Action, 0.01f);
					return;
				}
			}

			if(m_pArmature->getAnimation()->getAnimationData()->getMovement(Walk_Action))
			{
				m_pArmature->getAnimation()->play(Walk_Action, 0.01f);
			}
		}
		break;
	default:
		break;
	}
}




