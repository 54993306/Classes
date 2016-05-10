#ifndef __CHAPTERDATA_
#define __CHAPTERDATA_
#include "AppUI.h"
#include "bag/bagData.h"
#include "protos/protocol.h"
#include "model/BattleData.h"

struct CChapter
{
	int id;								//章节ID
	string name;					//名称
	bool isOpen;					//是否开启
	int star;							//已获得的星星
    int totalStar;					//星星总数	
	CPrize prizeGet;			//满星后的奖励
	int prize;							//满星后的奖励(0 不可领取，1 可领取，2已领取)
	void read(const Chapter& chap);
};

struct CStage
{
	int id;		        //关卡ID
	string name;		//关卡名称
	int level;		    //可进入等级
	bool isOpen;		//是否开放
	int star;		   //通关评价(星星)
	int action;		//挑战关卡需要消耗的行动力
	int prize;      //关卡奖励道具(0 没有道具，1 有道具、可领取，2 有道具、不可领取)
	void read(const StageInfo& info);
};


struct CStageInfoRes
{
	vector<CMonster> monstList;
	vector<CPrize>  prizeList;
	int action;
	void read(const StageInfoRes& info);
};

struct CGetPrizeRes
{
	bool result;									//奖励领取结果
	vector<CPrize> prizeList;			//奖励道具列表
};

#endif