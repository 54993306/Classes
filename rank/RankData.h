#ifndef __VIP_DATA_
#define __VIP_DATA_
#include "protos/protocol.h"
#include "cocos2d.h"
#include "model/DataCenter.h"
USING_NS_CC;


//英雄队列
struct CTeams{
	int id;		//英雄ID
	int thumb;	//图标
	int quality;	//星星品质
	int color;	//边框颜色
    void read(const Teams& data);
	CTeams();
};

struct CRankData{
	 int rank;
	 UserData *roleData;
	 int hurt;
	 vector<CTeams> teamList;		
	 void read(const RankData& data);
	 CRankData();
	 ~CRankData();
};

//返回排行榜数据 (90)
struct CRankResponse {
	 CRankData myRank;		//自己的排名
	 vector<CRankData> rankList;		//排行榜
	 void read(const RankResponse& data);
	 ~CRankResponse();
};

#endif