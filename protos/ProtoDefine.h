#ifndef _PROTO_DEFINE_H_
#define _PROTO_DEFINE_H_

#include <string>
using namespace std;

typedef struct 
{
	int code;
	string typeName;
}ProtoDefine;

enum protoTag
{
	DisconnectMsg				= 0,
	HeartMsg					= 1,
	LoginResponseMsg			= 2,
	InGameResponseMsg			= 3,
	CreateResponseMsg			= 4,
	RoleListMsg					= 5,
	RoleOnlineMsg				= 6,
	RoleMoveMsg					= 7,//角色移动请求
	RoleExitMsg					= 8,
	RoleLeaveMsg				= 9,//角色下线通知
	StageList					= 10,//关卡数据
	EnterStage					= 11,
	HeroListMsg					= 12,
	HeroInfoMsg					= 13,
	ChangeUnionMsg				= 14,
	ChangeHeroMsg				= 15,
	ExitStage					= 16,
	UpdateRoleMsg				= 17,
	BulletMsg					= 18,//公告
	AmbushMsg					= 19,//伏兵
	EnterMainCity				= 20,//进入主城
	RoleBag						= 21,//背包
	BuildInfo					= 22,//获取建筑物信息
	GarrisonHero				= 23,//驻守英雄
	ExtendBag					= 24,//扩展背包
	ResetBag					= 25,//整理背包
	GetResource					= 26,//城市资源
	Shop_Msg					= 27,//商品列表
	Shop_Buy					= 28,//购买道具
	UnionRes					= 29,//军团资源
	ChangeArmor					= 30,//更换装备
	ChapterList					= 31,//章节列表
	RecruitMsg					= 32,//招募
	SelectStrengthMsg			= 33,// 选择强化装备
	ComposeDataMsg				= 34,//调合塔数据
	ComposeItemMsg				= 35,//调合道具
	CommanderMsg				= 37,//返回指挥府信息 (37)
	LevelUpMsg					= 38,//升级建筑物
	TaskListMsg					= 39,//任务列表
	TaskNoticeMsg				= 40,//任务完成通知 
	TaskPrizeMsg				= 41,//任务奖励
	MailListMsg					= 42,//邮件列表
	MailNoticeMsg				= 43,//邮件通知 
	MaillReqMsg					= 44,//请求邮件操作
	SendMailMsg					= 45,//请求发送邮件
	FriendListMsg				= 46,//好友列表
	AddFriendMsg				= 47,//添加好友
	FriendReqNumMsg				= 48,//加为好友的请求数
	AcceptFriendMsg				= 49,//同意、拒绝添加好友请求
	StrengthenMsg				= 50,//强化装备
	SmeltArmorMsg				= 51,//洗练装备
	SmeltDataRequestMsg			= 52,//请求洗练数据
	HeroExpMsg					= 53,//使用经验道具
	HeroEvolTaskMsg				= 54,
	HeroEvolveMsg				= 55,
	SignDataMsg					= 56,// 签到数据
	SignReqMsg					= 57,//签到请求
	HeroLottery					= 58,//武将抽奖
	LotteryHeroData				= 59,//抽奖返回
	ChangeNameMsg				= 60,//请求更改角色名称 (60)
	HeadImgListMsg				= 61,//请求的可用头像(61)
	ChangeHeadImgMsg			= 62,//角色的可用头像(62)
	DeleteFriendMsg				= 63,//删除好友 
	TollgatePreviewMsg			= 64, 
	VipInfoMsg					= 65,
	VipPrivMsg					= 66,
	VipShopMsg					= 67,
	UpdateShopMsg				= 68,
	GetConvertInfo				= 69,//得到兑换的比率信息
	ConvertResult				= 70,//返回兑换的信息结果
	DecomposeData				= 71,//装备分解数据
	DecomposeArmor				= 72,//请求分解装备 
	ContinueStageMsg			= 73,//关卡继续
	GetStagePrize				= 74,//关卡或章节奖励
	BountyDataMsg				= 75,
	BountyHardMsg				= 76,
	ExchangeCodeMsg				= 77,
	ItemInfoMsg					= 78,
	ActListMsg					= 79,//活动内容列表
	BuyGiftMsg					= 80,//购买活动礼包
	BuyCardMsg					= 81,
	ArmorListMsg				= 82,
	AccountBinMsg				= 83,
	BossNoticeMsg				= 84,//世界BOSS战斗开启通知
	BossDataMsg					= 85,//世界BOSS基本信息
	BuyInspireMsg				= 86,//购买鼓舞buff
	ResetBattleTimeMsg			= 87,//重置挑战时间
	WarRequestMsg				= 88,//世界BOSS战斗数据
	BossFinishReqMsg			= 89,//BOSS战斗结束
	RankReqMsg					= 90,
	BossPrizesMsg				= 91,//世界BOSS排名奖励
	RisingDataRequestMsg		= 92,//装备升星数据
	RisingRequestMsg			= 93,//请求装备升星
	CBExchangeMsg				= 94,

	RechargeMsg					= 100,
	AskPvpTeamDataMsg			= 101,//101获取
	SendPvpTeamDataMsg			= 102,//102设置
	PvpDataMsg					= 103,//请求PVP战斗数据
	PvpPrizeListMsg				= 104,
	PvpOppTeamMsg				= 105,
	SkillLvUpMsg				= 106,
	ValidateBillingMsg			= 107,//验证订单
	PvpAsyncBattle				= 108,//异步PVP
	PvpOverReqMsg				= 109,//异步PVP结算
	PvpRecordResMsg				= 110,//异步PVP记录
	VerifyReceiptMsg			= 111,//支付验证
	PvpCityDataMsg				= 112,//PVP主城数据
	LuckyWheelMsg				= 113,//获取转盘信息
	DrawRequestMsg				= 114,//抽奖请求
	AtlasRequestMsg				= 115,//获取图鉴信息
	AtlasPrizeReqMsg			= 116,//领取图鉴奖励
	OnlinePrizeMsg				= 117,//在线礼包请求
	CardListMsg			= 118,//请求卡牌列表
	CardResMsg			= 119,//返回抽卡结果
	CardStartMsg		= 120,//开始抽卡游戏
	GameTipMsg					= 1000,
	Reconnect					= 1001,//断线重连
	ServerListMsg				= 1111,//版本号+服务器列表
};
//使用数组存储信息查找的效率更高
static ProtoDefine protoDef[] =
{
	{DisconnectMsg,"protos.Disconnect"},
	{HeartMsg,"protos.Protocol"},
	{LoginResponseMsg,"protos.LoginResponse"},
	{InGameResponseMsg,"protos.InGameResponse"},
	{CreateResponseMsg,"protos.CreateResponse"},
	{RoleListMsg,"protos.RoleListResponse"},
	{RoleOnlineMsg,"protos.common.Role"},
	{RoleMoveMsg,"protos.MoveResponse"},
	{RoleExitMsg,""},
	{RoleLeaveMsg,"protos.common.Role"},
	{StageList,"protos.StageListResponse"},
	{EnterStage,"protos.BattleResponse"},				//进入关卡返回战斗信息
	{HeroListMsg,"protos.HeroListResponse"},
	{HeroInfoMsg,"protos.common.Hero"},
	{ChangeUnionMsg,"protos.ChangeUnion"},
	{ChangeHeroMsg,"protos.ChangeHeroResponse"},
	{ExitStage,"protos.BattleFinishRep"},				//退出关卡
	{UpdateRoleMsg,"protos.common.Role"},				//更新角色信息
	{BulletMsg,"protos.Bulletin"},						//系统公告信息
	{AmbushMsg,"protos.BattleResponse"},				//伏兵信息
	{EnterMainCity,"protos.cityResponse"},				//进入主城
	{RoleBag,"protos.WareHouseResponse"},				//请求角色背包数据
	{BuildInfo,"protos.common.City"},					//获取建筑物信息
	{GarrisonHero,"protos.GarrisonHeroResponse"},		//驻守英雄
	{ExtendBag,"protos.ExtendResponse"},				//扩展背包
	{ResetBag,"protos.ResetResponse"},					//整理背包
	{GetResource,"protos.HarvestResponse"},				//城市资源
	{Shop_Msg,"protos.ShopResponse"},
	{Shop_Buy,"protos.BuyItemResponse"},
	{UnionRes,"protos.UnionResponse"},					//军团系统数据
	{ChangeArmor,"protos.ChangeArmorResponse"},			//更换装备 
	{ChapterList,"protos.ChapterResponse"},				//章节列表
	{RecruitMsg,"protos.HeroRecruitRes"},
	{SelectStrengthMsg,"protos.StrengthenResponse"},	//选择强化装备
	{ComposeDataMsg,"protos.ComposeResponse"},			//获得调合塔数据 
	{ComposeItemMsg,"protos.BuyItemResponse"},			//调合道具
	{36,""},
	{CommanderMsg,"protos.CommanderResponse"},			//v指挥府信息
	{LevelUpMsg,"protos.LevelUpResponse"},				//城市升级
	{TaskListMsg,"protos.TaskListResponse"},			//任务列表
	{TaskNoticeMsg,"protos.TaskNotice"},				//任务通知
	{TaskPrizeMsg,"protos.PrizeResponse"},				//任务奖励
	{MailListMsg,"protos.MailListResponse"},			//邮件列表
	{MailNoticeMsg,"protos.MailNotice"},				//邮件通知 
	{MaillReqMsg,"protos.MailResponse"},				//请求邮件操作
	{SendMailMsg,"protos.MailResponse"},				//请求发送邮件
	{FriendListMsg,"protos.FriendListResponse"},	
	{AddFriendMsg,"protos.FriendRes"},
	{FriendReqNumMsg,"protos.FriendRes"},				//加为好友的请求数
	{AcceptFriendMsg,"protos.FriendRes"},
	{StrengthenMsg,"protos.StrengthenResponse"},		//强化装备
	{SmeltArmorMsg,"protos.SmeltResponse"},				//洗练装备
	{SmeltDataRequestMsg,"protos.common.Armor"},		//洗练数据
	{HeroExpMsg,"protos.HeroResponse"},
	{HeroEvolTaskMsg,"protos.EvolInfo"},				//进阶信息
	{HeroEvolveMsg,"protos.HeroEvolRes"},				//进阶
	{SignDataMsg,"protos.SignData"},        
	{SignReqMsg,"protos.SignResponse"},
	{HeroLottery,"protos.LotteryData"},
	{LotteryHeroData,"protos.LotteryResponse"},
	{ChangeNameMsg,"protos.RoleCtrlRes"},
	{HeadImgListMsg,"protos.RoleIcons"},
	{ChangeHeadImgMsg,"protos.RoleCtrlRes"},
	{DeleteFriendMsg,"protos.FriendRes"},
	{TollgatePreviewMsg,"protos.StageInfoRes"},			//关卡预览信息(64)
	{VipInfoMsg,"protos.VipInfoRes"},  
	{VipPrivMsg,"protos.VipData"},						//VIP特权信息
	{VipShopMsg,"protos.VipShopRes"},
	{UpdateShopMsg,"protos.UpdateShopRes"},
	{GetConvertInfo,"protos.ExchangeInfoRes"},
	{ConvertResult,"protos.ExchangeRes"},
	{DecomposeData,"protos.DecomposeRes"},
	{DecomposeArmor,"protos.DecomposeRes"},
	{ContinueStageMsg,"protos.StageContinueRes"},
	{GetStagePrize,"protos.GetPrizeRes"},
	{BountyDataMsg,"protos.BountyDataRes"},
	{BountyHardMsg,"protos.HardModeRes"},
	{ExchangeCodeMsg,"protos.CardExchangeRes"},
	{ItemInfoMsg,"protos.ItemInfoRes"},
	{ActListMsg,"protos.ActListRes"},
	{BuyGiftMsg,"protos.BuyGiftRes"},
	{BuyCardMsg,"protos.BuyCardRes"},
	{ArmorListMsg,"protos.ArmorListRes"},
	{AccountBinMsg,"protos.AcountBindRes"},
	{BossNoticeMsg,"protos.BossNotice"},
	{BossDataMsg,"protos.BossData"},
	{BuyInspireMsg,"protos.BuyInspire"},
	{ResetBattleTimeMsg,"protos.ResetBattleTime"},
	{WarRequestMsg,"protos.WarResponse"},
	{BossFinishReqMsg,"protos.BossFinishRes"},
	{RankReqMsg,"protos.RankResponse"},
	{BossPrizesMsg,"protos.BossPrizes"},
	{RisingDataRequestMsg,"protos.RisingResponse"},
	{RisingRequestMsg,"protos.RisingResponse"},
	{CBExchangeMsg,"protos.CardExchangeRes"},
	{95,""},{96,""},{97,""},{98,""},{99,""},
	{RechargeMsg,"protos.RechargeRes"},						//充值
	{AskPvpTeamDataMsg, "protos.PvpTeamData"},				//玩家PVP防守阵容
	{SendPvpTeamDataMsg, "protos.PvpTeamData"},				//玩家PVP防守阵容
	{PvpDataMsg, "protos.PvpDataRes"},						//PVP战斗数据(103)
	{PvpPrizeListMsg,"protos.PvpPrizeList"},
	{PvpOppTeamMsg,"protos.OppTeamRes"},
	{SkillLvUpMsg,"protos.DoResult"},
	{ValidateBillingMsg, "protos.ValidateRes"},				//验证订单
	{PvpAsyncBattle,"protos.StartPvpRes"},					//PvE数据返回
	{PvpOverReqMsg,"protos.PvpOverRes"},					//Pvp结算
	{PvpRecordResMsg,"protos.PvpRecordRes"},				//异步Pvp战斗记录
	{VerifyReceiptMsg, "protos.VerifyRes"},			//支付验证
	{PvpCityDataMsg,"protos.PvpCityData"},					//pvp天空场景数据
	{LuckyWheelMsg,"protos.LuckyWheel"},					//获取转盘信息
	{DrawRequestMsg,"protos.DrawResponse"},					//抽奖请求
	{AtlasRequestMsg,"protos.AtlasResponse"},				//获取图鉴信息
	{AtlasPrizeReqMsg,"protos.AtlasPrizeRes"},				//领取图鉴奖励
	{OnlinePrizeMsg,"protos.OnlinePrize"},					//抽奖请求
	{CardListMsg,"protos.CardListRes"},			//请求卡牌列表
	{CardResMsg,"protos.GetCardRes"},			//返回抽卡结果
	{CardStartMsg,"protos.DrawCardStart"}	,	//开始抽卡游戏
	{GameTipMsg,"protos.GameTips"},
	{Reconnect,"protos.ReconnectResponse"},					//断线重连
	{ServerListMsg,"protos.ServerList"}						//请求服务器列表
};
#endif // !_PROTO_DEFINE_H_
