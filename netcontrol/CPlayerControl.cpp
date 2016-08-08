#include "CPlayerControl.h"
#include "scene/layer/GameTip.h"
#include "cctk/scenemanager.h"
#include "common/CommonFunction.h"
#include "model/DataCenter.h"
#include "protos/protocol.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "Battle/BattleScene/BattleScene.h"
#include "GMessage.h"
#include "Battle/BaseRoleData.h"
#include "task/TaskControl.h"
#include "common/RollLabel.h"
#include "guide/GuideManager.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "model/TerrainManager.h"
#include "Battle/ComBatLogic.h"
#include "Battle/ConstNum.h"
#include "mainCity/mainScene.h"
#include "roleImg/RoleUpdate.h"
#include "Battle/BattleDataCenter.h"
#include "common/CGameSound.h"
#include "vip/CPaySelect.h"
#include "tools/ShowTexttip.h"
#include "vip/CVipCard.h"
#include "vip/CVipPay.h"

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

void CPlayerControl::sendChangeArmor(int heroId, int pos,int armorId)
{
	ChangeArmorRequest *req = new ChangeArmorRequest;
	req->set_roleheroid(heroId);
	req->set_pos(pos);
	req->set_armorid(armorId);
	GetTcpNet->sendData(req,ChangeArmor);
	delete req;
}
//一直存在一直监听,先进行逐个判断后再判断监听列表
bool CPlayerControl::ProcessMsg(int type, google::protobuf::Message *msg)
{
	switch (type)
	{
	case EnterStage:
		{
			NOTIFICATION->postNotification(RECEIVE_BATTLE_INFO);
			BattleData->initNormalStage(msg);
		}break;
	case WarRequestMsg:
		{
			NOTIFICATION->postNotification(RECEIVE_BATTLE_INFO);
			BattleData->initWordBossStage(msg);
		}break;
	case PvpAsyncBattle:
		{
			NOTIFICATION->postNotification(RECEIVE_BATTLE_INFO);
			BattleData->initPvEData(msg);
		}break;
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
		}break;
	case BulletMsg:
		{
			Bulletin *bullet = (Bulletin*)msg;
			printf("bullet %s",bullet->text().c_str());
			CCScene* scene =  GETCURRSCENE();

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
		}break;

	case RechargeMsg:
		{
			rechargeRes(RechargeMsg,msg);
		}
		break;
	default: return false;
	}
	return true;
}

void CPlayerControl::showBullet(float delt)
{
	LayerManager::instance()->setPriority(-1);
	GameTip *tip = GameTip::create();
	tip->setPosition(VCENTER);
	tip->showTip(m_bulletText.c_str(),VCENTER,CCSizeMake(400,200));
	LayerManager::instance()->push(tip);
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
	GetTcpNet->sendData(batReq,EnterStage);
	delete batReq;
}

void CPlayerControl::sendPvEBattleInfo( int pRoleID,bool pAiState,vector<int>& pHeroID, bool bReven )
{
	StartPvpReq *tPvEInfo = new StartPvpReq;
	for (auto tRoleID : pHeroID)
	{
		tPvEInfo->add_battle_hero(tRoleID);
	}
	tPvEInfo->set_opp_id(pRoleID);
	tPvEInfo->set_is_robot(pAiState);
	tPvEInfo->set_reven(bReven);
	GetTcpNet->sendData(tPvEInfo, PvpAsyncBattle);
	delete tPvEInfo;
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

void CPlayerControl::sendGetHeroInfo(int roleHeroId,int heroId,int roleId,int quality)
{
	RequestHeroInfo *req = new RequestHeroInfo;
	if (roleHeroId!=0)
	{
		req->set_roleheroid(roleHeroId);
	}else{
		req->set_heroid(heroId);
	}
	if (roleId!=0)
		req->set_roleid(roleId);
	if (quality!=0)
		req->set_quality(quality);
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
			req->add_attr(attrVec.at(i));
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

void CPlayerControl::sendBuyPhoneStep1( int money, string sDeviceId, string sTel )
{
	RechargeReq *req = new RechargeReq;
	req->set_ctype(1);
	req->set_money(money);
	req->set_mobileno(sTel);
	req->set_step(1);
	GetTcpNet->sendData(req, RechargeMsg);
	delete req;
}

void CPlayerControl::sendBuyPhoneStep2( string sSmsCode, int step )
{
	RechargeReq *req = new RechargeReq;
	req->set_smsno(sSmsCode);
	req->set_ctype(1);
	req->set_step(step);
	GetTcpNet->sendData(req, RechargeMsg);
	delete req;
}

void CPlayerControl::sendBuyMoneyCard(string cardId)
{
	RechargeReq *req = new RechargeReq;
	req->set_cardno(cardId);
	req->set_ctype(2);
	GetTcpNet->sendData(req, RechargeMsg);
	delete req;
}

void CPlayerControl::sendCbExchange(string usrname, string pwd)
{
	CBExchange *req = new CBExchange;
	req->set_username(usrname);
	req->set_password(pwd);
	GetTcpNet->sendData(req, CBExchangeMsg);
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

void CPlayerControl::askForPvpTeamData()
{
	GetTcpNet->sendDataType(AskPvpTeamDataMsg);
}

void CPlayerControl::sendPvpTeamData( int iStatus, const vector<Member*> vec )
{
	PvpTeamData* pReq = new PvpTeamData;
	pReq->set_status(iStatus);
	
	for(auto pMember : vec)
	{
		Member* pNewMember = pReq->add_team();
		pNewMember->CopyFrom(*pMember);
	}

	GetTcpNet->sendData(pReq, SendPvpTeamDataMsg);
	delete pReq;
}

void CPlayerControl::askForPvpChallenge(bool bFirst)
{
	PvpDataReq* pReq = new PvpDataReq;
	pReq->set_refresh(bFirst);
	GetTcpNet->sendData(pReq, PvpDataMsg);
	delete pReq;
}

void CPlayerControl::sendSkillLvUp(int id, int skillId, int itemId)
{
	SkillLevelUp *req = new SkillLevelUp;
	req->set_heroid(id);
	req->set_skillid(skillId);
	req->set_itemid(itemId);
	GetTcpNet->sendData(req,SkillLvUpMsg);
	delete req;
}


void CPlayerControl::rechargeRes(int type, google::protobuf::Message *msg)
{
	RechargeRes *res = (RechargeRes*)msg;

	//获取购买弹框
	CVipPay* pPay = dynamic_cast<CVipPay*>(LayerManager::instance()->getLayer("CVipPay"));

	if(pPay)
	{
		pPay->enableSmsCodeBtn();
	}

	//成功
	if (res->result() == 1)
	{
		PlayEffectSound(SFX_415);

		int iSetp = res->rolemoney()==0?1:2;

		//当前步骤
		if(pPay)
		{
			//第一步验证
			if(iSetp ==1)
			{
				pPay->setStep1Success(true);
				pPay->showTimeClock();
				return;
			}
		}

		if(iSetp == 2)
		{
			pPay->onClose(nullptr);
		}
		if (res->rolemoney()>0)
		{
			UserData *data = DataCenter::sharedData()->getUser()->getUserData();	
			int money = res->rolemoney() - data->getRoleMoney();	
			data->setRoleMoney(res->rolemoney());
			paySuccess(money, res);
			CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
		}
	}
	else if(res->result() == 2)
	{
		string str = GETLANGSTR(1172) + res->errorcode();

		ShowPopTextTip(str.c_str(),ccWHITE,false);
	}
	else if(res->result() == 3)
	{
		string str = GETLANGSTR(1173) +res->errorcode();

		if(pPay)
		{	
			ShowPopTextTip(str.c_str(),ccWHITE,false);
			pPay->showErrorCode(res->errorcode());
		}
		else 
		{
			CPaySelect *paySel = dynamic_cast<CPaySelect*>(LayerManager::instance()->getLayer("CPaySelect"));
			if (paySel&&paySel->getPayType()==CardPay)
			{
				string  str = res->desc();
				ShowPopTextTip(str.c_str(),ccWHITE,false);
				CVipCard *vipCard = dynamic_cast<CVipCard*>(LayerManager::instance()->getLayer("CVipCard"));
				if (vipCard)vipCard->setPayEnable(true);
			}
		}
	}
	else if(res->result() == 4)
	{
		string str = GETLANGSTR(170) + res->errorcode();	
		ShowPopTextTip(str.c_str(),ccWHITE,false);
		if(pPay)
		{
			pPay->showErrorCode(res->errorcode());
		}
		else
		{
			CPaySelect *paySel = dynamic_cast<CPaySelect*>(LayerManager::instance()->getLayer("CPaySelect"));
			if (paySel&&paySel->getPayType()==CardPay)
			{
				string  str = res->desc();
				CVipCard *vipCard = dynamic_cast<CVipCard*>(LayerManager::instance()->getLayer("CVipCard"));
				if (vipCard)
				{
					vipCard->setPayEnable(true);
				}
			}
		}

	}
	else if(res->result() == 5)
	{
		if (pPay)
		{
			pPay->showVerifyCode(res->errorcode().c_str());
		}
	}
}

void CPlayerControl::paySuccess(int money, RechargeRes *res)
{
	CPaySelect *paySel = dynamic_cast<CPaySelect*>(LayerManager::instance()->getLayer("CPaySelect"));
	if (paySel)
	{
		string tip = "";
		switch (paySel->getPayType())
		{
		case GooglePay:
		case PhonePay:
			{
				tip ="ทำรายการสำเร็จแล้ว";
			}
			break;
		case CardPay:
			{
				tip = res->desc();
			}
			break;
		default:
			break;
		}
		ShowPopTextTip(tip.c_str(),ccWHITE,false);
	}
}

void CPlayerControl::sendPveBattleFinish(  bool bBattleResult, int iBattleTime, int iEnemyLeaderHp, int iSelfLeaderHp )
{
	PvpOverReq *pReq = new PvpOverReq;
	pReq->set_battle_result(bBattleResult);
	pReq->set_battle_time(iBattleTime);
	pReq->set_blood_1(iEnemyLeaderHp);
	pReq->set_blood_2(iSelfLeaderHp);
	GetTcpNet->sendData(pReq, PvpOverReqMsg);
	delete pReq;
}
