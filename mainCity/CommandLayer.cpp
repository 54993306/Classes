#include "CommandLayer.h"
#include "DefendWuJiang.h"
#include "CityData.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "mainCity/QuickProduct.h"
#include "tools/UICloneMgr.h"
#include "MainCityControl.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "common/color.h"

#include "tools/StringUtil.h"
#include "common/CGameSound.h"
#include "Resources.h"


CCommandLayer::~CCommandLayer()
{
	CC_SAFE_RELEASE(m_cell);
}


bool CCommandLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("MaskLayer");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		
		m_ui = LoadComponent("command.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CCommandLayer::onEnter()
{ 
	BaseLayer::onEnter();

	CButton *btn= (CButton *)m_ui->findWidgetById("close");
	btn->setOnClickListener(this,ccw_click_selector(CCommandLayer::onClose));

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_comLayer = (CLayout*)(m_ui->findWidgetById("pose"));
	m_garrsionLayer = (CLayout*)(m_ui->findWidgetById("garrsion"));

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CCommandLayer::tableviewDataSource));
	m_tableView->reloadData();

	m_selectCellImg = (CCSprite*)(m_ui->findWidgetById("selectImg"));
	m_selectCellImg->retain();

	CSceneManager::sharedSceneManager()->addMsgObserver("updateHero",this, GameMsghandler_selector(CCommandLayer::updateHero));

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	for (int i = 0; i < 2; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CCommandLayer::onSwitchBtn));
	}

	CButton *compose = (CButton *)(m_comLayer->findWidgetById("strengthen"));
	compose->setOnClickListener(this,ccw_click_selector(CCommandLayer::onComposeButton));
	compose->setDisabledImageColor();

	m_ui->setScale(0.0f);
	CCScaleTo *big = CCScaleTo::create(0.2f,1.0f);
	m_ui->runAction(big);

	GetTcpNet->registerMsgHandler(BuildInfo,this,CMsgHandler_selector(CCommandLayer::recBuildInfo));
	GetTcpNet->registerMsgHandler(CommanderMsg,this,CMsgHandler_selector(CCommandLayer::processNetMsg));
	//GetTcpNet->registerMsgHandler(ComposeItemMsg,this,CMsgHandler_selector(CCommandLayer::processNetMsg));
	GetTcpNet->registerMsgHandler(LevelUpMsg,this,CMsgHandler_selector(CCommandLayer::processNetMsg));

	for (int i=1; i<=3; ++i)
	{
		CButton *btn = dynamic_cast<CButton*>(m_garrsionLayer->getChildByTag(i));
		btn->setOnClickListener(this,ccw_click_selector(CCommandLayer::onGarrsion));
	}

	showSelectRadioImg(2);

}

void CCommandLayer::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;
	int selIndex = 2*btn->getTag();

	if (bChecked)
	{
		showSelectRadioImg(selIndex);
		if (btn->getTag()==1)
		{
			m_comLayer->setVisible(true);
			m_garrsionLayer->setVisible(false);
		}
		else
		{
			m_comLayer->setVisible(false);
			m_garrsionLayer->setVisible(true);
			CMainCityControl::getInstance()->sendCityInfo(12);
		}
	}
}

CCObject* CCommandLayer::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell *pCell = (CTableViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CTableViewCell;
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

void CCommandLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CCommandLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateHero",this);
	BaseLayer::onExit();
}


void CCommandLayer::onComposeButton(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (m_selectCity)
	{
		CMainCityControl::getInstance()->sendBuildLevelUp(m_selectCity->id);
	}
}

void CCommandLayer::showCommandList()
{
	m_tableView->setCountOfCell(m_commandData.cityList.size());
	m_tableView->reloadData();

// 	CComposeData &data = m_composeList[0];
// 	
// 	updateCompose(data);
}


void CCommandLayer::onSelectCity(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
// 	m_selectCellImg->setPosition(btn->getParent()->convertToWorldSpace(btn->getPosition()));
	m_selectCellImg->removeFromParent();

	btn->addChild(m_selectCellImg);
	m_selectCellImg->setVisible(true);
	m_selectCellImg->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));

	m_selectCity = (CCity*)btn->getUserData();
	updateSelectCity(m_selectCity);
}

void CCommandLayer::processNetMsg(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	switch (type)
	{
	case CommanderMsg:
		{
			 CommanderResponse *res = (CommanderResponse*)msg;
			 m_commandData.cityInfo.read(res->info());
			 for (int i = 0; i < res->citylist_size(); i++)
			 {
				 CCity city;
				 city.read(res->citylist(i));
				 m_commandData.cityList.push_back(city);
			 }
			 showCommandList();
		}
		break;
	case LevelUpMsg:
		{
			LevelUpResponse *res = (LevelUpResponse*)msg;
			//1 升级成功，2 粮食不足，3 指挥府等级限制，4 已达到最大等级，5 数据错误

			switch (res->result())
			{
			case 1:
				ShowPopTextTip(GETLANGSTR(244));		
				if (res->has_city())
				{
					UserData *user = DataCenter::sharedData()->getUser()->getUserData();
					int food = user->getRoleFood() - m_selectCity->lvFood;
					user->setRoleFood(food);
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

					m_selectCity->read(res->city());
					
					CCity *city = new CCity;
					*city = *m_selectCity;

					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_BUILD,0,nullptr, city);

					//更新指挥塔数据
					if (m_selectCity->cityId==m_commandData.cityInfo.cityId)
					{
						m_commandData.cityInfo = *m_selectCity;
					}
					m_tableView->setContentOffset(m_tableView->getContentOffset());
					m_tableView->reloadData();
					updateSelectCity(m_selectCity);

				}
				break;
			case 2:
				ShowPopTextTip(GETLANGSTR(241));		

				break;
			case 3:
				ShowPopTextTip(GETLANGSTR(242));		

				break;
			case 4:
				ShowPopTextTip(GETLANGSTR(243));		

				break;
			case 5:
				ShowPopTextTip(GETLANGSTR(170),RGB_RED);
				break;

			default:
				break;
			}
		}
		break;

	default:
		break;
	}
}

void CCommandLayer::removeAnimation(CCNode* node)
{
	node->removeFromParent();
}

void CCommandLayer::updateSelectCity(CCity *data)
{
	CCSprite *build = (CCSprite*)(m_comLayer->findWidgetById("build"));
    CCTexture2D *textTure = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("command/%d.jpg",data->cityId*10+1)->getCString());
	if (textTure)
	{
		build->setTexture(textTure);
	}

	CLabel *food = (CLabel*)(m_comLayer->findWidgetById("food"));
	food->setString(ToString(data->lvFood));

	CLabel *heronum = (CLabel*)(m_comLayer->findWidgetById("heronum"));
	heronum->setString(CCString::createWithFormat("%d/%d",data->haveHeroNum,data->heroNum)->getCString());

	CLabel *note1 = (CLabel*)(m_comLayer->findWidgetById("func1"));

	CLabel *note2 = (CLabel*)(m_comLayer->findWidgetById("func2"));

    CCArray *strArr = strSplit(data->note.c_str(),"|");
	
	if (strArr->count()==1)
	{
		note1->setString(((CCString*)strArr->objectAtIndex(0))->getCString());
		note2->setString("");
	}
	else if (strArr->count()==2)
	{
		note1->setString(((CCString*)strArr->objectAtIndex(0))->getCString());
		note2->setString(((CCString*)strArr->objectAtIndex(1))->getCString());
	}

	CButton *command = (CButton*)(m_comLayer->findWidgetById("strengthen"));

	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	if (data->cityId != m_commandData.cityInfo.cityId)
	{
		if (user->getRoleFood()>=data->lvFood&&data->level< m_commandData.cityInfo.level)
		{
			command->setEnabled(true);
		}
		else
		{
			command->setEnabled(false);
		}
	}
	else
	{
		if (user->getRoleFood()>=data->lvFood)
		{
			command->setEnabled(true);
		}
		else
		{
			command->setEnabled(false);
		}
	}
}

void CCommandLayer::onGarrsion(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	m_selectHero = btn->getTag();
	vector<CHero> &heroList = m_city.heroList;
	if (heroList.size()>=btn->getTag())
	{
		CHero hero = heroList[btn->getTag()-1];
		CDefendWuJiang *layer =CDefendWuJiang::create();
		layer->setCityId(m_city.id,m_city.cityId);
		layer->setReplaceId(hero.id);
		LayerManager::instance()->push(layer);
		CPlayerControl::getInstance().sendHeroList(3);
	}
	else
	{
		CDefendWuJiang *layer =CDefendWuJiang::create();
		layer->setCityId(m_city.id,m_city.cityId);
		layer->setReplaceId(0);
		LayerManager::instance()->push(layer);
		CPlayerControl::getInstance().sendHeroList(3);
	}
}

void CCommandLayer::showBuildInfo(const CCity& ct)
{
	int i=0;
	for (; i<ct.heroList.size()&&i<3; i++)
	{
		CButton *btn = (CButton*)(m_garrsionLayer->getChildByTag(i+1));
		const CHero &hero = ct.heroList[i];
		if (hero.thumb!=0)
		{
			CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png",hero.thumb)->getCString());
			if (!head)
			{
				head = CCSprite::create("headImg/101.png");
				CCLOG("[ ERROR ] CCommandLayer::showBuildInfo Lost Image = %d",hero.thumb);
			}
			head->setPosition(ccp(btn->getContentSize().width/2, btn->getContentSize().height/2));
			head->setTag(1);		
			head->setScale(82.0f/100.0f);
			btn->addChild(head);
			if (!hero.remove)
			{
				btn->setEnabled(false);
				head->setColor(RGB_GRAY_COLOR);
			}
		}
		CCNode *lab = (CCNode*)m_garrsionLayer->findWidgetById(CCString::createWithFormat("lab%d",i+1)->getCString());
		lab->setVisible(false);
	}

	for (i=ct.heroNum;i<3;i++)
	{
		CButton *btn = (CButton*)(m_garrsionLayer->getChildByTag(i+1));
		btn->setEnabled(false);
		CCNode *lab = (CCNode*)m_garrsionLayer->findWidgetById(CCString::createWithFormat("lab%d",i+1)->getCString());
		lab->setVisible(false);
	}

	//runFadeAction(ct);
}

void CCommandLayer::recBuildInfo(int type, google::protobuf::Message *msg)
{
	City* ct = (City*)msg;
	//memset(&m_city,0,sizeof(CCity));
	m_city.heroList.clear();
	m_city.read(*ct);
	showBuildInfo(m_city);
}

void CCommandLayer::updateHero(const TMessage& tMsg)
{
	//清空
	if (tMsg.nMsg ==0)
	{
		CButton *btn = (CButton*)(m_garrsionLayer->getChildByTag(m_selectHero));
		btn->removeChildByTag(1);
		CHero hero;
		hero.id= 0;
		// 		memset((void*)&hero,0,sizeof(CHero));
		m_city.heroList[m_selectHero-1] = hero;
		// 
		CCNode *lab = (CCNode*)m_garrsionLayer->findWidgetById(CCString::createWithFormat("lab%d",m_selectHero)->getCString());
		lab->setVisible(true);
	}
	else
	{
		CButton *btn = (CButton*)(m_garrsionLayer->getChildByTag(m_selectHero));
		CHero *hero = (CHero*)tMsg.lParam;
		string icon = ToString(hero->thumb);

		btn->removeChildByTag(1);
		CCNode *lab = (CCNode*)m_garrsionLayer->findWidgetById(CCString::createWithFormat("lab%d",m_selectHero)->getCString());
		lab->setVisible(false);

		CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png",hero->thumb)->getCString());
		if (!head)
		{
			head = CCSprite::create("headImg/101.png");
			CCLOG("[ ERROR ] CCommandLayer::updateHero Lost Image = %d",hero->thumb);
		}
		head->setPosition(ccp(btn->getContentSize().width/2, btn->getContentSize().height/2));
		head->setTag(1);
		head->setScale(82.0f/100.0f);
		btn->addChild(head);

		btn->setEnabled(false);
		head->setColor(RGB_GRAY_COLOR);

		if (m_selectHero>m_city.heroList.size())
		{
			m_city.heroList.push_back(*hero);
		}
		else
		{
			m_city.heroList[m_selectHero-1] = *hero;
		}
		delete hero;
	}
}

void CCommandLayer::showSelectRadioImg(int selIndex)
{
	for (int i=1; i<=4; ++i)
	{
		CCNode *spr = nullptr;
		if (i%2==0)
		{
			spr= (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("switch%d",i)->getCString());
			spr->setVisible(true);
		}
		else
		{
			spr = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("switch%d",i)->getCString());
			spr->setVisible(false);
		}
		if (selIndex ==i)
		{
			spr->setVisible(false);
		}
		else if (selIndex==i+1)
		{
			spr->setVisible(true);
		}
	}
}

void CCommandLayer::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CCity &data = m_commandData.cityList[uIdx];

	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	for (int i=1;i<=6;++i)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		if (i==1)
		{
			child->setTag(uIdx+100);
			child->setUserData((void*)&m_commandData.cityList[uIdx]);
			((CButton*)child)->setOnClickListener(this,ccw_click_selector(CCommandLayer::onSelectCity));
		}
		else if(i==3)
		{
			CLabel *name = (CLabel*)child;
			name->setString(data.cityName.c_str());
		}
		else if (i==2)
		{
			((CButton*)child)->setEnabled(false);
			CCSprite *prop = CCSprite::create(CCString::createWithFormat("command/%d.jpg",data.cityId*10)->getCString());
			if (prop)
			{
				prop->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
				child->addChild(prop,0,1);
			}
			else 
			{
				prop = CImageView::create("command/30.jpg");
				prop->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
				child->addChild(prop,0,1);
			}
		}

		else if(i==4)
		{
			CLabel *level = (CLabel*)child;
			level->setString(CCString::createWithFormat(GETLANGSTR(230),data.level)->getCString());
		}
		else if (i==5)
		{
			UserData *user = DataCenter::sharedData()->getUser()->getUserData();
			if (user->getRoleFood()>=data.lvFood)
			{
				child->setVisible(false);
			}
		}
		else if (i==6)
 		{
			UserData *user = DataCenter::sharedData()->getUser()->getUserData();
			if (m_commandData.cityInfo.level>=data.level&&m_commandData.cityInfo.cityId!=data.cityId&&user->getRoleFood()>=data.lvFood)
			{
				child->setVisible(false);
			}
			else if(m_commandData.cityInfo.cityId==data.cityId&&user->getRoleFood()>=data.lvFood)
			{
				child->setVisible(false);
			}
 		}
	}
}
