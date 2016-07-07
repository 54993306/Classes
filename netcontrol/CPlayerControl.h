#ifndef _CPlayerControl_H_
#define _CPlayerControl_H_

#include "net/MessageHandler.h"
#include "net/CNetClient.h"

using namespace protos;
using namespace protos::common;

namespace protos
{
	class Member;
}

/************************************************************************/
/*
							接收处理玩家服务器消息
*/
/************************************************************************/
class CPlayerControl:public MessageHandler,public CCNode
{
public:
	static CPlayerControl& getInstance()
	{
		static CPlayerControl playerCtrl;
		return playerCtrl;
	}
	
	~CPlayerControl();
	bool init();
	virtual bool ProcessMsg(int type, google::protobuf::Message *msg);
	void roleMove(google::protobuf::Message *msg);
	void roleOnlineList(google::protobuf::Message *msg);
	void showBullet(float delt);
	

	void sendChapterList(int type);   //获取章节列表  (0 普通关卡，1 精英关卡）
	
	void sendEnterStageForBoss(int hero1,int hero2, int hero3, int hero4, int hero5); //进入世界BOSS关卡

	void sendEnterStage(int stageId,int hero1,int hero2, int hero3, int hero4, int hero5=0, int fid=0, int questId=0); //进入关卡 , //选择参战的好友ID, //选择参战的好友英雄ID
	void sendStageInfo(int stageId); //获取关卡预览信息(64)
	/*0 全部， 1 出战英雄， 2 未出战, 3 可驻守  4 可招募*/
	void sendHeroList(int mask,int unionId=0,int roleId=0); //请求获取玩家英雄列表
	void SendLotteryType(int type);
	void sendSkillList(int heroId);//请求英雄所有可用技能列表
	void sendChangeSkill(int type, int skillId,int heroId);//请求更换技能
	void sendChangeHero(int nionId, int pos, int heroId); //设置出战英雄
	void sendBattleFinish(int reason, bool res,int roundNum);
	void sendWorldBossFinish(int hurt,int checkNum,vector<int>&vec);

	void sendChangeArmor(int heroId, int pos,int armorId);//请求更换装备;
	void sendMonterList(int batch);// 伏兵
	void sendGetHeroInfo(int roleHeroId,int heroId,int roleId=0,int quality=0); //角色英雄ID 英雄ID
	void sendRecruitHero(int heroId);// 招募英雄

	void sendUnion(int stageId=0, int questId=0, bool bBoss=false ); //设置出战队伍 (14)

	void sendRoleBag(int type, bool  equip=false, int roleHeroId=0, int pos=0); //获取背包道具(0 全部道具， 1 装备， 2 符文，3 灵魂)，equip:是否包括已穿戴到英雄身上的装备(强化装备时需要显示已穿戴的装备)
	//强化装备
	void sendStrengthen(int armorId, bool bOneKey=false); //角色装备ID , 是否一键
	//选择强化装备
	void sendStrengthenArmor(int armorId=0, int itemId=0); //角色装备ID ,//铸造秘笈道具ID
	//请求洗练数据
	void askSmeltData(int armorId);
	//洗炼装备 attrVec:1 敏捷dex，2 命中hit，3 暴击crit，4 回复renew，5 闪避dodge
	void sendSmeltArmor(int armorId, const vector<int> &attrVec);

	//请求升星数据
	void askForArmorStartUp(int armorId=0);//装备ID
	//请求升星
	void sendArmorStartUp(int armorId, int itemId, bool useGold);	//装备ID，特殊素材ID，是否用勾玉

	//角色退出
	void roleExit(google::protobuf::Message *msg);
	//关卡列表
	void stageListResponse(Message *msg);

	void battleInfo(Message * msg);		//关卡战斗信息
	void BossBattleInfo(Message * msg);	//世界boss
	//请求签到
	void sendSignReq(bool resign);
	//更改角色名称 (60)
	void sendChangeName(string name);

	//更改角色头像(62))
	void sendChangeHeadImg(int headIcon);

	//请求好友列表(46)
	/* mask:1 好友列表，2 请求加好友列表，3 可添加好友的玩家列表*/
	void sendFriendList(int mark, string name="");
	void sendAddFriend(int roleId);
	void sendDeleteFriend(int friendId);
	void sendAcceptFriend(int roleId, bool isAccept);
	void sendConvert(int type);				//请求兑换信息
	void sendExchange(int type, int way);   //请求兑换资源

	void sendVipInfoByLevel(int level);
	void sendDecomposeArmor(int armorId,int msgType);
	void sendComposeItem(int itemType);  
	void sendGetStagePrize(int prizeType, int stageId);

	void sendBountyHard(int bid);
	void sendExchangeCode(const char* str);	
	void sendItemInfo(int itemId);
	void sendBuyCard(int cardId);
	void sendBuyMoneyCard(string cardId);
	void sendBuyPhoneStep1(int money, string sDeviceId, string sTel);
	void sendBuyPhoneStep2(string sSmsCode, int step);
	void sendArmorList(int itemId,int mask=1);
	void sendAccountBind(const char* strId);
	void sendRank(int type);//排行榜类型(1 世界BOSS排行，2 战斗力排行)	
	void sendCbExchange(string usrname, string pwd);
	void sendSkillLvUp(int id, int skillId, int itemId);
	//请求PVP队伍数据
	void askForPvpTeamData();
	void sendPvpTeamData(int iStatus, const vector<Member*> vec);

	//请求挑战队伍数据
	void askForPvpChallenge(bool bFirst=true);

private:
	CPlayerControl();
	string m_bulletText;
};

#endif