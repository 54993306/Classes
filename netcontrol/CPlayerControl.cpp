#include "CPlayerControl.h"
#include "scene/layer/GameTip.h"
#include "cctk/scenemanager.h"
#include "common/CommonFunction.h"
#include "model/DataCenter.h"
#include "protos/protocol.h"
#include "scene/loadWar.h"
#include "scene/WarScene.h"
#include "GMessage.h"
//#include "model/BattleData.h"
#include "Battle/TempData.h"
#include "task/TaskControl.h"
#include "common/RollLabel.h"
#include "guide/GuideManager.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "model/TerrainManager.h"
#include "warscene/ComBatLogic.h"
#include "warscene/ConstNum.h"
#include "mainCity/mainScene.h"
#include "roleImg/RoleUpdate.h"

using namespace protos;
using namespace protos::common;

CPlayerControl::CPlayerControl()
{
	CNetClient::getShareInstance()->registerMsgHandler(this);
	this->init();
	CTaskControl::getInstance();
}

CPlayerControl::~CPlayerControl()
{
	CNetClient::getShareInstance()->unRegisterMsgHandler(this);
}

bool CPlayerControl::init()
{
	if (CCNode::init())
	{
		CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this,0,false);
	}
	return true;
}


void CPlayerControl::sendChangeHero(int nionId, int pos, int heroId)
{
	//     ChangeHeroRequest *req = new ChangeHeroRequest;
	// 	req->set_unionid(nionId);
	// 	req->set_pos(pos);
	// 	req->set_heroid(heroId);
	// 	GetTcpNet->sendData(req,ChangeHeroMsg);
	// 	delete req;
}

void CPlayerControl::sendGetStagePrize(int prizeType, int stageId)
{
	GetPrizeReq *req = new GetPrizeReq;
	req->set_prizetype(prizeType);
	req->set_id(stageId);
	GetTcpNet->sendData(req,GetStagePrize);
	delete req;
}

void CPlayerControl::sendBattleFinish(int reason, bool res,int roundNum)
{
	BattleFinish *req = new BattleFinish; 
	req->set_reason(reason);		//关卡结束的原因(1:正常结束, 2:中途撤退)
	req->set_ispass(res);			//是否胜利通关
	req->set_blood(roundNum);		//主帅血量值
	//req->set_herodie(aliveNum);		//暂不使用
	GetTcpNet->sendData(req,ExitStage);
	delete req;
}
//世界boss战斗结束
void CPlayerControl::sendWorldBossFinish( int hurt,int checkNum,vector<int>&vec )
{
	BossFinishReq* req = new BossFinishReq;
	req->set_hurt(hurt);
	req->set_checksum(checkNum);
	for (auto i:vec)
		req->add_checkhurt(i);
	GetTcpNet->sendData(req, BossFinishReqMsg);
	delete req;
}

void CPlayerControl::sendMonterList(int batch)// 伏兵
{
	AmbushRequest *req = new AmbushRequest;
	req->set_batch(batch);
	GetTcpNet->sendData(req, AmbushMsg);
	delete req;
}

void CPlayerControl::sendRoleBag(int type, bool equip/*=false*/, int roleHeroId/*=0*/, int pos/*=0*/)
{
	WareHouseRequest *req = new WareHouseRequest;
	req->set_tabindex(type);
	req->set_equip(equip);
	req->set_pos(pos);
	if (roleHeroId!=0)
	{
		req->set_roleheroid(roleHeroId);
	}
	GetTcpNet->sendData(req,RoleBag,true);
	delete req;
}

void CPlayerControl::sendHeroList(int mask,int unionId,int roleId) //请求获取玩家英雄列表
{
	HeroListRequest *req = new HeroListRequest;
	req->set_mark(mask);
	// 	if (unionId!=0)
	// 	{
	// 		req->set_unionid(unionId);
	// 	}
	// 	if (roleId!=0)
	// 	{
	// 		req->set_roleid(roleId);
	// 	}
	GetTcpNet->sendData(req,HeroListMsg,true);
	delete req;
}

void CPlayerControl::SendLotteryType(int type)
{
	LotteryRequest* req = new LotteryRequest;
	req->set_lotttype(type);
	GetTcpNet->sendData(req,LotteryHeroData);
	delete req;
}

// void CPlayerControl::sendSkillList(int heroId)//请求英雄所有可用技能列表
// {
// 	HeroSkillRequest *req = new HeroSkillRequest;
// 	req->set_heroid(heroId);
// 	GetTcpNet->sendData(req,HeroSkillMsg);
// 	delete req;
// }
// 
// void CPlayerControl::sendChangeSkill(int type, int skillId,int heroId)//请求更换技能
// {
// 	ChangeSkillRequest *req = new ChangeSkillRequest;
// 	req->set_type(type);
// 	req->set_skillid(skillId);
// 	req->set_heroid(heroId);
// 	GetTcpNet->sendData(req,ChangeSkillMsg);
// 	delete req;
// }

void CPlayerControl::sendChangeArmor(int heroId, int pos,int armorId)
{
	ChangeArmorRequest *req = new ChangeArmorRequest;
	req->set_roleheroid(heroId);
	req->set_pos(pos);
	req->set_armorid(armorId);
	GetTcpNet->sendData(req,ChangeArmor);
	delete req;
}

bool CPlayerControl::ProcessMsg(int type, google::protobuf::Message *msg)
{
	switch (type)
	{
		//角色列表
	case RoleListMsg:
		{
			roleOnlineList(msg);
		}
		return true;
		//角色上线
	case RoleOnlineMsg:
		{
			// 		    Role *role = (Role*)msg;
			// 			DataCenter::sharedData()->getUser()->addRoleAlive(role);
			// 			if (CSceneManager::sharedSceneManager()->getRunningScene()->getClassName()=="CityScene")
			// 			{
			// 				CityScene *ctScene = (CityScene*)CSceneManager::sharedSceneManager()->getRunningScene();
			// 				ctScene->addAlive(role->roleid());
			// 			}
		}
		return true;
		//角色移动
	case RoleMoveMsg:
		{
			roleMove(msg);
		}
		return true;

	case RoleLeaveMsg:
		{
			roleExit(msg);
		}
		return true;
		// 	case StageList:
		// 		{
		// 			stageListResponse(msg);
		// 		}
		// 		return true;
		//战斗信息
	case EnterStage:
		{
			NOTIFICATION->postNotification(RECEIVE_BATTLE_INFO);
			battleInfo(msg);
		}return true;
	case WarRequestMsg:
		{
			NOTIFICATION->postNotification(RECEIVE_BATTLE_INFO);
			BossBattleInfo(msg);
		}return true;
	case UpdateRoleMsg:
		{
			Role *role = (Role *)msg;
			UserData *user = DataCenter::sharedData()->getUser()->getUserData();
			bool isUp = user->getLevel()<role->rolelv()?true:false;
			user->read(*role);
			CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
			CMainScene *mainScene = dynamic_cast<CMainScene*>(CSceneManager::sharedSceneManager()->getCurrScene());
			if (user->getNewStep()>0&&!CGuideManager::getInstance()->getIsRunGuide()&&mainScene)
			{
				CGuideManager::getInstance()->setTaskStep(user->getNewStep(),1);
				CGuideManager::getInstance()->enterGuide();
			}
		}
		return true;

	case BulletMsg:
		{
			Bulletin *bullet = (Bulletin*)msg;
			printf("bullet %s",bullet->text().c_str());

			m_bulletText = bullet->text();
			//	this->scheduleOnce(schedule_selector(CPlayerControl::showBullet),2.0f);
			//	if (CSceneManager::sharedSceneManager()->getRunningScene()->getClassName()!="LoginScene")
			{
				CCScene* scene =  GETCURRSCENE();//CCDirector::sharedDirector()->getRunningScene();

				scene->removeChildByTag(88888);
				CCNode *rollNode = CCNode::create();
				scene->addChild(rollNode,0,88888);
				CCSprite *rollBg = CCSprite::create("mainCity/bulletin-board.png");
				rollBg->setPosition(ccp(VCENTER.x,VCENTER.y+200));
				rollBg->setTag(1200);
				rollNode->addChild(rollBg);
				CRollLabel *rollLab = CRollLabel::create(bullet->text().c_str(),"Arial",20,
					CCRectMake(rollBg->getPositionX()-rollBg->getContentSize().width/2,rollBg->getPositionY()-rollBg->getContentSize().height/2+8,rollBg->getContentSize().width,rollBg->getContentSize().height));
				rollLab->setRollTime(1);
				rollNode->addChild(rollLab);
			}     
		}
		return true;
	default:
		break;
	}
	return false;
}

void CPlayerControl::roleMove(google::protobuf::Message *msg)
{
	//MoveResponse *response = (MoveResponse *)msg;
	//int roleId = response->roleid();
	//if (CSceneManager::sharedSceneManager()->getRunningScene()->getClassName()=="CityScene")
	//{
	//	CityScene *scene = (CityScene *)CSceneManager::sharedSceneManager()->getRunningScene();
	//	scene->roleMoveToPos(roleId,response->movex(),response->movey());
	//	printf("move x:%d, y:%d\n",response->movex(), response->movey());
	//}
}

void CPlayerControl::showBullet(float delt)
{
	LayerManager::instance()->setPriority(-1);
	GameTip *tip = GameTip::create();
	tip->setPosition(VCENTER);
	tip->showTip(m_bulletText.c_str(),VCENTER,CCSizeMake(400,200));
	LayerManager::instance()->push(tip);
}

//角色列表
void CPlayerControl::roleOnlineList(google::protobuf::Message *msg)
{
	//	RoleListResponse *roleList = (RoleListResponse*)msg;
	//	CityScene *ctScene = (CityScene*)CSceneManager::sharedSceneManager()->getRunningScene();
	//	for (int i=0; i<roleList->rolelist_size(); i++)
	//	{
	//		Role role = roleList->rolelist(i);
	//		DataCenter::sharedData()->getUser()->addRoleAlive(&role);
	//		ctScene->addAlive(role.roleid());
	//	}
	//	//ctScene->onAddAliveData();
	//// 	LayerManager::instance()->setPriority(-1);
	//// 	GameTip *tip = GameTip::create();
	//// 	tip->setPosition(VCENTER);
	//// 	tip->showTip(m_bulletText.c_str(),VCENTER,CCSizeMake(400,200));
	//// 	LayerManager::instance()->push(tip);
}

//角色退出
void CPlayerControl::roleExit(google::protobuf::Message *msg)
{
	//Role *role = (Role*)msg;
	//DataCenter::sharedData()->getUser()->removeRoleAlive(role->roleid());
	//if (CSceneManager::sharedSceneManager()->getRunningScene()->getClassName()=="CityScene")
	//{
	//	CityScene *ctScene = (CityScene*)CSceneManager::sharedSceneManager()->getRunningScene();
	//	ctScene->removeAlive(role->roleid());
	//}
}

//关卡列表
void CPlayerControl::stageListResponse(Message *msg)
{
	//StageListResponse *stList = (StageListResponse *)msg;
	//TollgateScene *scene = (TollgateScene *)CSceneManager::sharedSceneManager()->getRunningScene();
	//
	//DataCenter::sharedData()->tollgate()->clearTollgate();
	//for (int i =0; i<stList->stagelist_size(); i++)
	//{
	//	StageInfo stage = stList->stagelist(i);
	//	TollgateCfg *cfg = new TollgateCfg;
	//	cfg->id = stage.id();
	//	cfg->name = stage.name();
	//	cfg->level = stage.inlevel();
	//	cfg->isOpen= stage.isopen();
	//	cfg->star = stage.star();
	//	DataCenter::sharedData()->tollgate()->addTollgate(cfg);
	//}
	//if (CSceneManager::sharedSceneManager()->getRunningScene()->getClassName()=="TollgateScene")
	//{
	//	scene->showList();
	//}
}


//进入关卡
void CPlayerControl::sendEnterStage(int stageId,int hero1,int hero2, int hero3, int hero4, int hero5/*=0*/, int fid/*=0*/, int questId/*=0*/)
{
	BattleRequest *batReq = new BattleRequest;
	batReq->set_stageid(stageId);
	batReq->set_hero1(hero1);
	batReq->set_hero2(hero2);
	batReq->set_hero3(hero3);
	batReq->set_hero4(hero4);
	batReq->set_hero5(hero5);
	batReq->set_fid(fid);
	if (questId>0)
	{	
		//	batReq->set_questid(questId);
	}
	GetTcpNet->sendData(batReq,EnterStage);
	delete batReq;
}

void CPlayerControl::sendEnterStageForBoss( int hero1,int hero2, int hero3, int hero4, int hero5 )
{
	WarRequest *batReq = new WarRequest;
	batReq->set_hero1(hero1);
	batReq->set_hero2(hero2);
	batReq->set_hero3(hero3);
	batReq->set_hero4(hero4);
	batReq->set_hero5(hero5);
	GetTcpNet->sendData(batReq, WarRequestMsg);
	delete batReq;
}

void CPlayerControl::BossBattleInfo( Message * msg )
{
	WarResponse*batRes=(WarResponse*)msg;
	DataCenter::sharedData()->getWar()->initWordBossData(batRes);
}

//进入关卡战斗信息
void CPlayerControl::battleInfo(Message * msg)
{
	BattleResponse *batRes = (BattleResponse *)msg;
	DataCenter::sharedData()->getWar()->initBattleData(batRes);
}

void CPlayerControl::sendChapterList(int type)
{
	ChapterRequest *req = new ChapterRequest;
	req->set_type(type);
	GetTcpNet->sendData(req,ChapterList,true);
	delete req;
}

void CPlayerControl::sendStrengthen(int armorId, bool bOneKey)
{
	StrengthenRequest *req = new StrengthenRequest;
	req->set_armorid(armorId);
	req->set_oneclick(bOneKey);
	GetTcpNet->sendData(req,StrengthenMsg);
	delete req;
}

void CPlayerControl::sendStrengthenArmor(int armorId, int itemId)
{
	StrengthenRequest *req = new StrengthenRequest;
	if (armorId>=0)
	{	
		req->set_armorid(armorId);
	}
	GetTcpNet->sendData(req,SelectStrengthMsg,true);
	delete req;
}


void CPlayerControl::askForArmorStartUp( int armorId/*=0*/ )
{
	RisingRequest *req = new RisingRequest;
	req->set_armorid(armorId);
	GetTcpNet->sendData(req,RisingDataRequestMsg);
	delete req;
}

void CPlayerControl::sendArmorStartUp( int armorId, int itemId, bool useGold )
{
	RisingRequest *req = new RisingRequest;
	req->set_armorid(armorId);
	req->set_itemid(itemId);
	req->set_usegold(useGold);
	GetTcpNet->sendData(req, RisingRequestMsg);
	delete req;
}

void CPlayerControl::sendRecruitHero(int heroId)
{
	HeroRecruitReq *req = new HeroRecruitReq;
	req->set_heroid(heroId);
	GetTcpNet->sendData(req,RecruitMsg,true);
	delete req;
}

void CPlayerControl::sendItemInfo(int itemId)
{
	ItemInfoReq *req = new ItemInfoReq;
	req->set_itemid(itemId);
	GetTcpNet->sendData(req,ItemInfoMsg,true);
	delete req;
}

void CPlayerControl::sendSkillList(int heroId)
{

}

void CPlayerControl::sendChangeSkill(int type, int skillId,int heroId)
{

}

void CPlayerControl::sendGetHeroInfo(int roleHeroId,int heroId,int roleId,int quality)
{
	RequestHeroInfo *req = new RequestHeroInfo;
	if (roleHeroId!=0)
	{
		req->set_roleheroid(roleHeroId);
	}
	else
	{
		req->set_heroid(heroId);
	}

	if (roleId!=0)
	{
		req->set_roleid(roleId);
	}
	if (quality!=0)
	{
		req->set_quality(quality);
	}
	GetTcpNet->sendData(req,HeroInfoMsg,true);
	delete req;
}

void CPlayerControl::sendUnion(int stageId/*=0*/, int questId/*=0*/, bool bBoss/*=false*/)
{
	UnionRequest *req = new UnionRequest;
	if(stageId>0)
	{
		req->set_stageid(stageId);
	}

	if (questId>0)
	{
		//req->set_questid(questId);
	}

	req->set_boss(bBoss);

	GetTcpNet->sendData(req,UnionRes,true);
	delete req;
}


void CPlayerControl::askSmeltData( int armorId )
{
	SmeltRequest *req = new SmeltRequest;
	req->set_armorid(armorId);
	GetTcpNet->sendData(req, SmeltDataRequestMsg);
	delete req;
}


void CPlayerControl::sendSmeltArmor(int armorId, const vector<int> &attrVec)
{
	SmeltRequest *req = new SmeltRequest;
	req->set_armorid(armorId);
	if (attrVec.size()>0)
	{
		for (int i = 0; i < attrVec.size(); i++)
		{
			req->add_attr(attrVec.at(i));
		}
	}
	GetTcpNet->sendData(req, SmeltArmorMsg);
	delete req;
}

void CPlayerControl::sendSignReq(bool resign)
{
	SignRequest *req = new SignRequest;
	req->set_resign(resign);
	GetTcpNet->sendData(req,SignReqMsg);
	delete req;
}

void CPlayerControl::sendChangeName(string name)
{
	ChangeNameReq *req = new ChangeNameReq;
	req->set_rolename(name);
	GetTcpNet->sendData(req,ChangeNameMsg);
	delete req;
}

void CPlayerControl::sendChangeHeadImg(int headIcon)
{
	ChangeRoleIcon *req = new ChangeRoleIcon;
	req->set_iconid(headIcon);
	GetTcpNet->sendData(req,ChangeHeadImgMsg);
	delete req;
}

void CPlayerControl::sendStageInfo(int stageId)
{
	StageInfoReq *req = new StageInfoReq;
	req->set_stageid(stageId);
	GetTcpNet->sendData(req, TollgatePreviewMsg);
	delete req;
}

void CPlayerControl::sendFriendList(int mark, string name)
{
	FriendListRequest *req = new FriendListRequest;
	req->set_mark(mark);
	if (name!="")
	{
		req->set_name(name);
	}
	GetTcpNet->sendData(req,FriendListMsg,true);
	delete req;
}

void CPlayerControl::sendAddFriend(int roleId)
{
	AddFriend* req = new AddFriend;
	req->set_roleid(roleId);
	GetTcpNet->sendData(req,AddFriendMsg);
	delete req;
}

void CPlayerControl::sendDeleteFriend(int friendId)
{
	RemoveFriend *req = new RemoveFriend;
	req->set_friendid(friendId);
	GetTcpNet->sendData(req, DeleteFriendMsg);
	delete req;
}

void CPlayerControl::sendAcceptFriend(int roleId, bool isAccept)
{
	AcceptFriend *req = new AcceptFriend;
	req->set_roleid(roleId);
	req->set_accept(isAccept);
	GetTcpNet->sendData(req,AcceptFriendMsg);
	delete req;
}

void CPlayerControl::sendConvert(int type)
{
	ExchangeInfoReq *req = new ExchangeInfoReq;
	req->set_type(type);
	GetTcpNet->sendData(req,GetConvertInfo,true);
	delete req;
}

void CPlayerControl::sendExchange(int type, int way)
{
	ExchangeReq *req = new ExchangeReq;
	req->set_type(type);
	req->set_way(way);
	GetTcpNet->sendData(req,ConvertResult);
	delete req;
}

void CPlayerControl::sendVipInfoByLevel(int level)
{
	VipPrivReq *req = new VipPrivReq;
	req->set_level(level);
	GetTcpNet->sendData(req,VipPrivMsg,true);
	delete req;
}

void CPlayerControl::sendDecomposeArmor(int armorId,int msgType)
{
	DecomposeReq *req = new DecomposeReq;
	req->set_armorid(armorId);
	GetTcpNet->sendData(req, msgType);
	delete req;
}

void CPlayerControl::sendComposeItem(int itemType)
{
	ComposeRequest *req = new ComposeRequest;
	req->set_composeid(itemType);
	GetTcpNet->sendData(req, ComposeDataMsg,true);
	delete req;
}

void CPlayerControl::sendBountyHard(int bid)
{
	HardModeReq *req = new HardModeReq;
	req->set_bid(bid);
	GetTcpNet->sendData(req,BountyHardMsg);
	delete req;
}

void CPlayerControl::sendExchangeCode(const char* str)
{
	CardExchangeReq *req = new CardExchangeReq;
	req->set_card_no(str);
	GetTcpNet->sendData(req,ExchangeCodeMsg);
	delete req;
}

void CPlayerControl::sendBuyCard(int cardId)
{
	BuyCardReq *req = new BuyCardReq;
	req->set_cardid(cardId);
	GetTcpNet->sendData(req,BuyCardMsg);
	delete req;
}

void CPlayerControl::sendBuyCardStep1( int iCardId, string sDeviceId, string sTel )
{
	BuyCardReq *req = new BuyCardReq;
	req->set_cardid(iCardId);
	req->set_deviceid(sDeviceId);
	req->set_mobileno(sTel);
	req->set_step(1);
	GetTcpNet->sendData(req, BuyCardMsg);
	delete req;
}

void CPlayerControl::sendBuyCardStep2( string sSmsCode )
{
	BuyCardReq *req = new BuyCardReq;
	req->set_smsno(sSmsCode);
	req->set_step(2);
	GetTcpNet->sendData(req, BuyCardMsg);
	delete req;
}

void CPlayerControl::sendArmorList(int itemId,int mask)
{
	ArmorListReq *req = new ArmorListReq;
	req->set_itemid(itemId);
	req->set_mark(mask);
	GetTcpNet->sendData(req,ArmorListMsg);
	delete req;
}

void CPlayerControl::sendAccountBind(const char* strId)
{
	AccountBindReq *req = new AccountBindReq;
	req->set_openid(strId);
	GetTcpNet->sendData(req,AccountBinMsg);
	delete req;
}

void CPlayerControl::sendRank(int type)
{
	RankRequest *req = new RankRequest;
	req->set_ranktype(type);
	GetTcpNet->sendData(req,RankReqMsg);
	delete req;
}

