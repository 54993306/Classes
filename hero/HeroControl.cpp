#include "HeroControl.h"
#include "tools/ShowTexttip.h"
#include "netcontrol/CPlayerControl.h"
#include "Battle/AnimationManager.h"

#include "GMessage.h"
#include "HeroPromote.h"
#include "HeroUpdate.h"
#include "HeroBreak.h"
#include "model/DataCenter.h"
#include "guide/GuideManager.h"
#include "common/CommonFunction.h"
#include "hero/HeroCall.h"

CHeroControl::CHeroControl():m_skillLayer(nullptr),m_detailLayer(nullptr),m_moveLayer(nullptr)
	,m_heroAttrLayer(nullptr),m_evolveLayer(nullptr),m_expLayer(nullptr),m_heroLayer(nullptr),m_isShowOpr(false)
	,m_iOwnCount(0)
{
	CNetClient::getShareInstance()->registerMsgHandler(HeroListMsg,this,CMsgHandler_selector(CHeroControl::processMessage));
	CNetClient::getShareInstance()->registerMsgHandler(HeroInfoMsg,this,CMsgHandler_selector(CHeroControl::processMessage));
	CNetClient::getShareInstance()->registerMsgHandler(HeroEvolTaskMsg,this, CMsgHandler_selector(CHeroControl::processEvolveMsg));
	CNetClient::getShareInstance()->registerMsgHandler(HeroEvolveMsg,this, CMsgHandler_selector(CHeroControl::processEvolveMsg));
	CNetClient::getShareInstance()->registerMsgHandler(HeroExpMsg,this, CMsgHandler_selector(CHeroControl::processMessage));
	
	GetTcpNet->registerMsgHandler(RecruitMsg,this,CMsgHandler_selector(CHeroControl::recruitMessage));
}

void CHeroControl::processMessage(int type, google::protobuf::Message *msg)
{
	switch (type)
	{
	case HeroListMsg:
		{
			m_heroLayer = CHeroList::create();
			m_heroLayer->setVisible(true);

			LayerManager::instance()->push(m_heroLayer);

			HeroListResponse *res = (HeroListResponse*)msg;
			m_heroList.clear();
			int i=0;
			m_insertIndex = 0;

			m_iOwnCount = 0;
			for (; i<res->herolist_size(); i++)
			{
				Hero hero = res->herolist(i);
				CHero obj;
				obj.readData(hero);
				if (obj.own)
				{
					m_iOwnCount++;
				}
				if (obj.heroid==102&&CGuideManager::getInstance()->getIsRunGuide()
					&&(CGuideManager::getInstance()->getCurrTask()==17))
				{
					m_heroList.insert(m_heroList.begin(),obj);
				}
				else if (!obj.own&&obj.itemNum1>=obj.itemNum2)
				{
					if (CGuideManager::getInstance()->getIsRunGuide()&&(CGuideManager::getInstance()->getCurrTask()==17))
					{
						m_heroList.insert(m_heroList.begin()+1,obj);
					}
					else
					{
						m_heroList.insert(m_heroList.begin(),obj);
					}
				}
				else
				{
					m_heroList.push_back(obj);
				}
			}
			
		    if (CGuideManager::getInstance()->getIsRunGuide()&&(CGuideManager::getInstance()->getCurrTask()==16||CGuideManager::getInstance()->getCurrTask()==13))
			{
				for (int i = 0; i < m_heroList.size(); i++)
				{
					if (m_heroList.at(i).heroid==304&&i!=0)
					{
						CHero hero = m_heroList.at(i);
						m_heroList.erase(m_heroList.begin()+i);
						m_heroList.insert(m_heroList.begin(),hero);
						break;
					}					
				}
			}

			//101,203,301
			if (CGuideManager::getInstance()->getIsRunGuide()&&CGuideManager::getInstance()->getCurrTask()==3)
			{
				for (int i = 0; i < m_heroList.size(); i++)
				{
					CHero hero = m_heroList.at(i);
					if ((hero.heroid==101||(hero.heroid==203)||hero.heroid==301)&&i!=0)
					{
						m_heroList.erase(m_heroList.begin()+i);
						m_heroList.insert(m_heroList.begin(),hero);
						break;
					}					
				}
			}

			i=0;
			for (vector<CHero>::iterator iter = m_heroList.begin();iter!=m_heroList.end();)
			{
				CHero &her = *iter;
				CHero obj;

				//添加分割线
				if (!her.own&&her.itemNum1<her.itemNum2)
				{ 
					m_insertIndex =i;
					CHero hr;
					hr.heroid =0;
					iter = m_heroList.insert(iter,hr);
					hr.heroid =-1;
					iter = m_heroList.insert(iter,hr);
					if (i%2==1)
					{
						hr.heroid=-2;
						iter = m_heroList.insert(iter,hr);
					}
					break;
				}
				else
				{
					iter++;
				}
				i++;
			}

// 			for (; i<res->herolist_size(); i++)
// 			{
// 				Hero hero = res->herolist(i);
// 				CHero obj;
// 				obj.readData(hero);
// // 				if (hero.own())
// // 				{	
// // 					m_heroList.push_back(obj);
// // 				}
// 				//添加分割线
// 				else if (i-1>0&&res->herolist(i-1).own())
// 				{ 
// 					m_insertIndex =i;
// 					CHero hr;
// 					hr.heroid =0;
// 					m_heroList.push_back(hr);
// 					hr.heroid =-1;
// 					m_heroList.push_back(hr);
// 					if (i%2==1)
// 					{
// 						hr.heroid=-2;
// 						m_heroList.push_back(hr);
// 					}
// 					m_heroList.push_back(obj);
// 				}
// 				else
// 				{
// 					m_heroList.push_back(obj);
// 				}
// 			}
			m_heroLayer->updateHeroList(m_heroList, m_insertIndex);
			m_heroLayer->runOpenAnim();
		}
		break;

	case HeroInfoMsg:
		{
			Hero *hero = (Hero*)msg;

			//如果是请求的下一品质数据
			if(m_evolveLayer && m_evolveLayer->getAskNextQualityHero())
			{
				CHero chero;
				chero.readData(*hero);
				m_evolveLayer->updateNextQualityAttr(&chero);
				m_evolveLayer->setAskNextQualityHero(false);
				return;
			}

			CItem item;
			m_hero.armor1 = item;
			m_hero.armor2 = item;
			m_hero.readData(*hero);

			if (!m_heroAttrLayer)
			{
				m_heroAttrLayer = CHeroAttribute::create();	
				m_heroAttrLayer->updateHeroAttr(&m_hero);		
				m_heroAttrLayer->setDelegate(this);
				m_heroAttrLayer->setVisible(true);

				onHeroInfo(&m_hero,1);
				if (m_isShowOpr)
				{
					m_heroAttrLayer->hideHeroOperation();
				}
				LayerManager::instance()->push(m_heroAttrLayer);
			}
			else
			{
				CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HEROINFO,1,nullptr, &m_hero);
				updateHeroValue(&m_hero);

				//如果有进阶界面，请求下一等级数据
				if(m_evolveLayer)
				{
					m_evolveLayer->updateAttr(&m_hero);
					m_evolveLayer->askForData();
					//return;
				}
			}

			//左右按钮
			if (!m_isShowOpr)
			{
				int pos = findHeroPosIndex(m_hero.id);
				if (pos<=0)
				{
					m_heroAttrLayer->updateLeftRightBtn(true,false);
					m_heroAttrLayer->updateLeftRightBtn(false,true);
				}
				else if (pos>=m_iOwnCount-1)
				{
					m_heroAttrLayer->updateLeftRightBtn(true,true);
					m_heroAttrLayer->updateLeftRightBtn(false,false);
				}
				else
				{
					m_heroAttrLayer->updateLeftRightBtn(false,true);
					m_heroAttrLayer->updateLeftRightBtn(true,true);
				}
			}

			//可以屏蔽了，进阶处，不再请求herocall数据
			//if (m_evolveLayer&&m_evolveLayer->isVisible())
			//{
			//	sendEvolveInfo(m_hero.itemId);
			//}
		}
		break;

	case HeroExpMsg:
		{
			HeroResponse *res =(HeroResponse*)msg;
		    bool isSkillup = res->has_skilllvup()&&res->skilllvup()==1;
			if (isSkillup||(res->has_hero()&&m_hero.level<res->hero().level()))
			{
// 				CHeroUpdate *protmo = CHeroUpdate::create();
// 				LayerManager::instance()->push(protmo);		
// 				protmo->levelSkillUp(&m_hero,&res->hero());
				m_heroAttrLayer->levelSkillUp(&m_hero,&res->hero());
			}

			if (res->result()==1&&res->has_hero())
			{
				if (res->hero().level()>m_hero.level)
				{
					m_heroAttrLayer->showUpLevelAnimation();
				}
				m_hero.readData(res->hero());
				CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HEROINFO,0,NULL, &m_hero);
				updateHeroValue(&m_hero);
				m_expLayer->onSetHero(&m_hero);
				m_expLayer->updateItemCount();
			}
			else
			{
				ShowPopTextTip(GETLANGSTR(179));
			}
		}
		break;
	default:
		break;
	}
}

void CHeroControl::onHeroInfo(CHero *hero,int showType)
{	
	if (m_detailLayer)
	{
		m_detailLayer->setVisible(false);
	}
	if (m_moveLayer)
	{
		m_moveLayer->setVisible(false);
	}
	if (m_skillLayer)
	{
		m_skillLayer->setVisible(false);
	}
	if (m_evolveLayer)
	{
		m_evolveLayer->setVisible(false);
	}

	if(m_expLayer)
	{
		m_expLayer->setVisible(false);
	}

	switch (showType)
	{
	
	case 1:
		{
			if (!m_detailLayer)
			{
				m_detailLayer = CHeroDetail::create();
				m_detailLayer->setVisible(true);
				m_detailLayer->setTouchPriority(LayerManager::instance()->getCurrLayer()->getTouchPriority()-1);
				m_detailLayer->showHeroInfo(&m_hero);
				m_heroAttrLayer->addChild(m_detailLayer);
			}
			m_detailLayer->setVisible(true);
		}
		break;
	case 5:
		{
			if (!m_moveLayer)
			{
				m_moveLayer = CHeroMove::create();
				m_moveLayer->setVisible(true);
				m_moveLayer->setTouchPriority(LayerManager::instance()->getCurrLayer()->getTouchPriority()-1);
				m_moveLayer->showMoveDirection(&m_hero);
				m_heroAttrLayer->addChild(m_moveLayer);
			}
			m_moveLayer->setVisible(true);
		}
		break;
	case 3:
		{
			if (!m_skillLayer)
			{
				m_skillLayer = CHeroSkill::create();
				m_skillLayer->setVisible(true);
				m_skillLayer->setTouchPriority(LayerManager::instance()->getCurrLayer()->getTouchPriority()-1);
				m_heroAttrLayer->addChild(m_skillLayer);
				/*LayerManager::instance()->push(m_skillLayer);	*/	
			}			
			m_skillLayer->showSkill(&m_hero,1);
			m_skillLayer->setVisible(true);
		}
		break;
	case 4:
		{ 
			if (!m_evolveLayer/*&&m_hero.quality<m_hero.maxQuality*/)
			{
				m_evolveLayer = CHeroEvolve::create();
				m_evolveLayer->setVisible(true);
				m_evolveLayer->setTouchPriority(LayerManager::instance()->getCurrLayer()->getTouchPriority()-1);
				m_evolveLayer->showHeroHead(&m_hero);
				m_evolveLayer->updateAttr(&m_hero);
				m_heroAttrLayer->addChild(m_evolveLayer);
				m_evolveLayer->askForDataCallBack();
			}
			//else if (!m_evolveLayer)
			//{
			//	//当前是最大品质
			//	ShowPopTextTip(GETLANGSTR(181));
			//	return;
			//}
			//sendEvolveInfo(m_hero.itemId);
			m_evolveLayer->setVisible(true);
		}
		break;

	case 2:
		{
			if (!m_expLayer)
			{
				m_expLayer = CHeroExp::create();
				m_expLayer->setVisible(true);
				m_expLayer->setTouchPriority(LayerManager::instance()->getCurrLayer()->getTouchPriority()-1);
				m_heroAttrLayer->addChild(m_expLayer);  		
				CPlayerControl::getInstance().sendRoleBag(4,false,m_hero.heroid);
			}
			m_expLayer->onSetHero(&m_hero);
			m_expLayer->setVisible(true);
		}
		break;
	default:
		break;
	}
}

void CHeroControl::onResetLayer()
{
 	m_heroAttrLayer = nullptr;
	m_moveLayer = nullptr;
	m_detailLayer = nullptr;
	m_skillLayer = nullptr;
	m_evolveLayer = nullptr;
	m_expLayer = nullptr;
	if (m_heroLayer)
	{
		m_heroLayer->updateHeroList(m_heroList, m_insertIndex);
	}
}

void CHeroControl::sendEvolveInfo(int itemId)
{
// 	HeroEvolReq *evol = new HeroEvolReq;
// 	evol->set_heroid(heroId);
// 	GetTcpNet->sendData(evol,HeroEvolTaskMsg);
// 	delete evol;
	ItemInfoReq *req = new ItemInfoReq;
	req->set_itemid(itemId);
	GetTcpNet->sendData(req,ItemInfoMsg);
	delete req;
}

void CHeroControl::sendEvolveHero(int heroId)
{
	HeroEvolReq *evol = new HeroEvolReq;
	evol->set_heroid(heroId);
	GetTcpNet->sendData(evol,HeroEvolveMsg);
	delete evol;
}

void CHeroControl::processEvolveMsg(int type, google::protobuf::Message *msg)
{
	switch (type)
	{
	case HeroEvolTaskMsg:
		{
// 		    EvolInfo *res = (EvolInfo*)msg;
// 			CEvolInfo evolinfo;
// 			evolinfo.read(*res);
// 
// 			if (m_evolveLayer)
// 			{
// 				m_evolveLayer->evolveTask(evolinfo);
// 			}
		}
		break;

	case HeroEvolveMsg:
		{
			HeroEvolRes *res = (HeroEvolRes*)msg;
			CEvolResult evoRes; 
			evoRes.read(res);
			if (evoRes.result==1)
			{
				if (res->has_hero())
				{


					m_evolveLayer->levelSkillUp(&m_hero,&res->hero());
					m_evolveLayer->showUpLevelAnimation();
					m_hero.readData(res->hero());
					m_evolveLayer->showHeroQuality(&m_hero);
					m_evolveLayer->updateAttr(&m_hero);
					m_evolveLayer->askForData();

					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HEROINFO,0,nullptr, &m_hero);
					updateHeroValue(&m_hero);

					if(m_heroAttrLayer)
					{
						m_heroAttrLayer->updateStarRank(&m_hero);
					}

					//进阶成功后，更新觉醒技能
					if(m_skillLayer)
					{
						m_skillLayer->showSkill(&m_hero,1);
					}
				}

// 				if (res->quests_size()>0)
// 				{
// 					m_evolveLayer->evolveTask(evoRes.evolInfo);
// 				}	
			}
			else
			{
				ShowPopTextTip(GETLANGSTR(182));
			}
		}
		break;
	default:
		break;
	}
}


void CHeroControl::recruitMessage(int type, google::protobuf::Message *msg)
{
	HeroRecruitRes *res = (HeroRecruitRes*)msg;
	if (res->result()==1&&res->has_hero())
	{
		CHero hr;
		hr.readData(res->hero());

		//m_heroList.insert(m_heroList.begin(),hr);
		for (int i = 0; i < m_heroList.size(); i++)
		{
			CHero &her = m_heroList.at(i);
			if (her.heroid==hr.heroid)
			{
				m_heroList[i]= hr;
				break;
			}
		}
// 		vector<CHero>::iterator iter = m_heroList.begin();
// 		for (;iter!=m_heroList.end();)
// 		{
// 			if (iter->heroid<=0)
// 			{
// 				iter = m_heroList.erase(iter);
// 			}
// 			else
// 			{
// 				++iter;
// 			}
// 		}
// 
// 		int i=0;
// 		m_insertIndex = 0;
// 		for (iter = m_heroList.begin();iter!=m_heroList.end();)
// 		{
// 			CHero &her = m_heroList.at(i);
// 			CHero obj;
// 	
// 			//添加分割线
// 			if (!her.own&&her.itemNum1<her.itemNum2)
// 			{ 
// 				m_insertIndex =i;
// 				CHero hr;
// 				hr.heroid =0;
// 				iter = m_heroList.insert(iter,hr);
// 				hr.heroid =-1;
// 				iter = m_heroList.insert(iter,hr);
// 				if (i%2==1)
// 				{
// 					hr.heroid=-2;
// 					iter = m_heroList.insert(iter,hr);
// 				}
// 				break;
// 			}
// 			else
// 			{
// 				iter++;
// 			}
// 		}
		m_heroLayer->updateHeroList(m_heroList, m_insertIndex);
		m_heroLayer->showNewHeroEffect(&hr);

		UserData *user = DataCenter::sharedData()->getUser()->getUserData();
		user->setCoin(user->getCoin()-1000);
		CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
	}
	else
	{
		ShowPopTextTip(GETLANGSTR(236));
	}
}

void CHeroControl::onEvolveHero()
{
	sendEvolveHero(m_hero.id);
}

void CHeroControl::onAddHeroExp(int itemId, int num)
{
	sendAddHeroExp(m_hero.id,itemId,num);
}

void CHeroControl::sendAddHeroExp(int heroId, int itemId, int itemNum)
{
	HeroExpRequest *req = new HeroExpRequest;
	req->set_heroid(heroId);
	req->set_itemid(itemId);
	req->set_itemnum(itemNum);
	GetTcpNet->sendData(req,HeroExpMsg);
	delete req;
}

void CHeroControl::sendHeroBreak(int heroId, int useFood)
{
// 	HeroBreakRequest *req = new HeroBreakRequest;
// 	req->set_heroid(heroId);
// 	req->set_usefood(useFood);
// 	GetTcpNet->sendData(req,HeroBreakMsg);
// 	delete req;
}

void CHeroControl::onAddBreak(int food)
{
	sendHeroBreak(m_hero.id, food);
}

int CHeroControl::findHeroPosIndex(int heroId)
{
	for (int i = 0; i < m_heroList.size(); i++)
	{
		if (m_heroList.at(i).id == heroId)
		{
			return i;
		}
	}
	return 0;
}

void CHeroControl::onBeforeHero()
{
    int pos = findHeroPosIndex(m_hero.id);
	if (pos<=0)
	{
		return;
	}
	CPlayerControl::getInstance().sendGetHeroInfo(m_heroList.at(pos-1).id,0);
	if (m_expLayer&&m_expLayer->isVisible())
	{
		CPlayerControl::getInstance().sendRoleBag(4,false,m_heroList.at(pos-1).heroid);
	}
}

void CHeroControl::onAfterHero()
{
	int pos = findHeroPosIndex(m_hero.id);
	if (pos>=m_heroList.size()-1||m_heroList.at(pos+1).id==0)
	{
		return;
	}
	CPlayerControl::getInstance().sendGetHeroInfo(m_heroList.at(pos+1).id,0);
	if (m_expLayer&&m_expLayer->isVisible())
	{
		CPlayerControl::getInstance().sendRoleBag(4,false,m_heroList.at(pos+1).heroid);
	}
}

void CHeroControl::updateHeroValue(CHero * hero)
{
	int pos = findHeroPosIndex(hero->id);
	m_heroList.at(pos) = *hero;
}

void CHeroControl::setIsHideOperation(bool isHide)
{ 
	m_isShowOpr = isHide;
}