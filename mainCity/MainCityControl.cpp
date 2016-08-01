#include "MainCityControl.h"
#include "mainScene.h"
#include "scene/CPopTip.h"
#include "GMessage.h"
#include "tools/ShowTexttip.h"
#include "mainCity/mainCityBuild.h"

CMainCityControl* CMainCityControl::instance = nullptr;

CMainCityControl* CMainCityControl::getInstance()
{
	if (instance==nullptr)
	{
		instance = new CMainCityControl();
	}
	return instance;
}

CMainCityControl::CMainCityControl()
{
	m_scene =  (CMainScene*)(CSceneManager::sharedSceneManager()->getCurrScene());
	this->onEnter();
	CNetClient::getShareInstance()->registerMsgHandler(EnterMainCity,this,CMsgHandler_selector(CMainCityControl::recMainCityInfo));
	CNetClient::getShareInstance()->registerMsgHandler(GetResource,this,CMsgHandler_selector(CMainCityControl::recCityResource));
}

CMainCityControl::~CMainCityControl()
{
	CNetClient::getShareInstance()->unRegisterAllMsgHandler(this);
}

void CMainCityControl::sendEnterCity()
{
	CNetClient::getShareInstance()->sendDataType(EnterMainCity);
}

void CMainCityControl::sendCityInfo(int cityId)
{
	CityInfo *msg = new CityInfo;
	msg->set_cityid(m_cityMap[cityId].id);
	GetTcpNet->sendData(msg,BuildInfo);
	delete msg;
}

void CMainCityControl::sendGarrisonHero(int cityId, int heroId, int replaceId)
{
	GarrisonHeroRequest *msg = new GarrisonHeroRequest;
	msg->set_cityid(cityId);
	msg->set_heroa(heroId);
	msg->set_herob(replaceId);
	
	GetTcpNet->sendData(msg,GarrisonHero);
	delete msg;
}

void CMainCityControl::sendGetCityResource(int cityId)
{
	HarvestRequest *msg = new HarvestRequest ;
	msg->set_cityid(cityId);
	GetTcpNet->sendData(msg,GetResource);
	delete msg;
}

void CMainCityControl::recMainCityInfo(int type, google::protobuf::Message *msg)
{
	m_scene =  (CMainScene*)(CSceneManager::sharedSceneManager()->getCurrScene());
	cityResponse *res = (cityResponse*)msg;
	memset(&m_cityData,0,sizeof(CityData));
	m_cityData.read(*res);
//  	m_scene->showCityInfo(m_cityData);


// 	CCDelayTime * delayAction = CCDelayTime::create(0.1f);  
// 	CCCallFunc * callFunc = CCCallFunc::create(this, callfunc_selector(CMainCityControl::resCityInfo));  
// 	this->runAction(CCSequence::createWithTwoActions(delayAction, callFunc));  
	resCityInfo();
	for (int i = 0; i < m_cityData.cityList.size(); i++)
	{
		CCity *cb = &(m_cityData.cityList[i]);
		m_cityMap[cb->cityId] = *cb;
	}
}

void CMainCityControl::resCityInfo()
{
 	m_scene = dynamic_cast<CMainScene*>(CSceneManager::sharedSceneManager()->getCurrScene());
	if (m_scene)
	{	
		m_scene->getCityBuildLayer()->showCityInfo(m_cityData);
	}
}

void CMainCityControl::recBuildInfo(int type, google::protobuf::Message *msg)
{
	City* ct = (City*)msg;
	ct->id();

}

void CMainCityControl::recCityResource(int type, google::protobuf::Message *msg)
{
	HarvestResponse *res = (HarvestResponse*)msg;
	if(res->result())
	{
		const Role &role = res->role();
		CCLog("coin %d",role.rolecoin());
		UserData *user = DataCenter::sharedData()->getUser()->getUserData();
		int coin = role.rolecoin() - user->getCoin();
		int food = role.rolefood() - user->getRoleFood();

		CCDelayTime *delay = CCDelayTime::create(2.0f);
		CCScaleTo *smal = CCScaleTo::create(0.2f,0.0f);
		CCSequence *seque = CCSequence::create(delay,smal, CCCallFuncN::create(this,callfuncN_selector(CMainCityControl::removeTip)),nullptr);
		
		if (coin>0)
		{
// 			CPopTip *pop = CPopTip::create("tips/bg.png");
// 			pop->addBeforeImage("tips/blackbg.png",0,20);
// 			pop->addButton("public/btn_yellow_befor.png","public/btn_yellow_after.png",nullptr,3,1);
// 			pop->buttonAddImage("public/font_queiding.png",3);
// 		//	CCString *str = CCString::createWithFormat("已獲取%d份金幣",coin);
// 
// 			pop->addTextRichTip(U8(str->getCString()),ccWHITE,"Arail",18.0f);
// 			pop->addRichImage("mainCity/icon_11.png");
// 			pop->reloadTextRich(ccp(VCENTER.x,VCENTER.y+50));
// 
// 			pop->setBottomOffset(-10);
// 			pop->setTouchEnabled(true);
// 			pop->setTouchPriority(-100);
// // 			pop->setButtonLisener(this,ccw_click_selector(CQuickProduct::onUserStone));
// 			m_scene->addChild(pop,10,2);
// 			pop->runScaleAction();
// 
// 			pop->runDefAction(seque);
			
			CSceneManager::sharedSceneManager()->PostMessageA(CULTIVATE_GET_MONEY, -1, nullptr, nullptr, coin);
		}
		else if (food>0)
		{
			CCString *str = CCString::createWithFormat(GETLANGSTR(256),food); //已獲取%d份糧食
// 			CPopTip *pop = CPopTip::create("tips/bg.png");
// 			pop->addBeforeImage("tips/blackbg.png",0,20);
// 			pop->addButton("public/btn_yellow_befor.png","public/btn_yellow_after.png",nullptr,3,1);
// 			pop->buttonAddImage("public/font_queiding.png",3);
// 			
// 			pop->addTextRichTip(str->getCString(),ccWHITE,"Arail",18.0f);
// 			pop->addRichImage("mainCity/tili.png");
// 			pop->reloadTextRich(ccp(VCENTER.x,VCENTER.y+50));
// 
// 			pop->setBottomOffset(-10);
// 			pop->setTouchEnabled(true);
// 			pop->setTouchPriority(-100);
// 			/*pop->setButtonLisener(this,ccw_click_selector(CQuickProduct::onUserStone));*/
// 			m_scene->addChild(pop,10,2);
// 			pop->runScaleAction();
// 			pop->runDefAction(seque);
			ShowPopTextTip(str->getCString());
		}
		
		user->read(role);

		//user->setRoleAction(role->a
// 		CSceneManager::sharedSceneManager()->PostMessageA("updateRole",0,nullptr,nullptr);
		CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

	}
}


void CMainCityControl::removeTip(CCNode * node)
{
	node->getParent()->removeFromParent();
}

void CMainCityControl::sendShopRequest(int shopType)
{
	ShopRequest *req = new ShopRequest();
	req->set_shop(shopType);
 	GetTcpNet->sendData(req,Shop_Msg,true);
	delete req;
}

void CMainCityControl::sendShopUpdate(int shopType)
{
	UpdateShopReq *req = new UpdateShopReq;
	req->set_shop(shopType);
	GetTcpNet->sendData(req,UpdateShopMsg);
	delete req;
}


void CMainCityControl::sendShopBuyItem(int itemId, int buyNum, int type/*=1*/)
{
	BuyItemRequest *req = new BuyItemRequest();
	req->set_itemid(itemId);
	req->set_buynum(buyNum);
	req->set_shop(type);
	GetTcpNet->sendData(req,Shop_Buy);
	delete req;
}

void CMainCityControl::sendComposeItem(int itemId, int armorId)
{
	ComposeItemRequest *req = new ComposeItemRequest;
	req->set_item_id(itemId);
	req->set_armor_id(armorId);
	GetTcpNet->sendData(req,ComposeItemMsg);
	delete req;
}

void CMainCityControl::updateCityHero(int cityId, CCity& city)
{
	m_cityMap[cityId] = city;
}

vector<int> CMainCityControl::getGarrsionHeroId()
{
	vector<int> heroIdVec;
	map<int,CCity>::iterator iter = m_cityMap.begin();

	for (; iter!= m_cityMap.end(); iter++)
	{
		CCity &city = iter->second;
		for (int i = 0; i < city.heroList.size(); i++)
		{
			if (city.heroList[i].id >0)
			{
				heroIdVec.push_back(city.heroList[i].thumb);
			}
		}
	}
	return heroIdVec;
}


CHero* CMainCityControl::getGarrsionHeroIdByCityId( int cityId )
{
	vector<int> heroIdVec;
	CCity &city = m_cityMap[cityId];
	for (int i = 0; i < city.heroList.size(); i++)
	{
		if (city.heroList[i].id >0)
		{
			return &city.heroList[i];
		}
	}
	return nullptr;
}


void CMainCityControl::sendBuildLevelUp(int cityId)
{
	LevelUpRequest *req =new LevelUpRequest;
	req->set_cityid(cityId);
	GetTcpNet->sendData(req,LevelUpMsg);
	delete req;
}

bool CMainCityControl::isCityOpen(int cityId)
{
	map<int,CCity>::iterator iter = m_cityMap.find(cityId);
	if (iter!=m_cityMap.end())
	{
		return iter->second.isOpen;
	}
	return false;
}

bool CMainCityControl::isJustOpen(CCity * cb)
{
	map<int,CCity>::iterator iter = m_cityMap.find(cb->cityId);
	bool justOpen = false;
	if (iter!=m_cityMap.end()&&!iter->second.isOpen&&cb->isOpen&&cb->cityId!=13)
	{
		justOpen = true;
	}
	return justOpen;
}

const CCity& CMainCityControl::getCityDataById( int iId )
{
	return m_cityMap[iId];
}

