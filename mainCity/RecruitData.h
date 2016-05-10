#ifndef _RECRUITDATA_
#define _RECRUITDATA_

#include "net/CNetClient.h"
struct HeroLotteryData
{
	HeroLotteryData();
	int id;			//英雄ID or 道具ID
	int quality;	//品质1白、2绿、3蓝、4紫、5橙（对英雄来说是星级）
	int thumb;		//图片
	int type;		//奖励类型
	bool isnew;		//是否新品
	int heroType;	//英雄类型(为0则为秘籍，1僵尸2道士3神将)
	string name;	//武将名称
	int m_iNum;	//数量
	int iColor;		//颜色
	void readData(const protos::Lottery &lottery);
};
enum ResTag
{
	cell1 = 10,
	cell1_icon,
	cell2 = 20,
	cell_name,
	cell_Img,
	cell_quality,
	cell_newIcon,
	cell_mijiIcon,
	cell_bg,
	resultLayerRes = 200,		//
	resImgRes = 202,						//标记位置。鬼火
	resViewRes,								//特效的view
	resTenLabRes,							//购买十个成功
	resOenLabRes,							//购买1个成功
	heroIconLayerRes = 300,
	heroViewRes,
	commonLayerRes = 310,
	comHeroNameRes,
	comHeroImgRes,
	comHeroQuality,
	heruTurnLayerRes = 320,
	newHeroLayerRes = 350,		//抽奖结算界面
	newGoldIconRes,						//勾玉图标
	newheartIconRes,					//友情点图标
	newNumLabRes,						//抽奖消耗
	newBuyBtnRes,							//再抽一次按钮
	newButTenLabRes,					//十连抽文本
	newButOenLabRes,					//单抽文本
	newSureBtnRes,						//确定按钮
	newMijiIconRes,						//秘籍图标
	newNewIconRes,						//new图标
	newTurnLabRes,						//
	newGetLabRes,
};


enum LotteryResultType		//抽奖结果反馈
{
	LotterySucceed = 1,
	NeedFriendship,
	NeedGold,
	DataError,
};

enum LotteryType			//抽奖类型
{
	FriendshipOnes = 1,
	GoldOnes,
	GoldTen,
};

enum RecruitTag				//界面TAG
{
	RecruiLayerRec = 200,		//招募面板
	FriendshipNumRec,				//友情点数字
	OnceGoldNumRec,				//招募1次使用金币数
	TenGoldNumRec,				//招募10次使用金币数
	FriendshipBtnRec,				//友情点抽奖按钮
	OnecBtnRec,							//金币一次抽奖按钮
	TenBtnRec,							//金币10次抽奖按钮
	One_ImgRec = 210,				//位置1武将图片
	One_NewImgRec,				//位置1新武将图标(NEW图标)
	One_TypeRec,						//位置1武将类型(僵尸1、道士2、神将3)
	One_HeroType=218,			//武将品质(1白、2绿、3蓝、4紫、5橙)	
};
#endif