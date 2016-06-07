#ifndef __HERO_DATA_
#define __HERO_DATA_
#include "protos/protocol.h"
#include "model/BattleData.h"




struct CEvolQuest 
{
	int id;			//进阶任务ID
	int stageId;		//关联关卡ID（通过该ID可直接进入关卡）
	bool open;		//关卡是否已开启
    bool pass;		//是否已完成
	int action;		//进入关卡需要消耗的行动力
	CEvolQuest();
// 	void read(const EvolQuest& eq);
};

struct CEvolInfo 
{
	bool canEvol;	
	vector<CEvolQuest> questsList;
// 	void read(const EvolInfo& eq);
};


struct CEvolResult
{
	bool result;		//进阶成功 or 失败
	CHero hero;		//进阶成功后更新英雄信息
// 	bool canEvol;		//是否仍可继续进阶
// 	vector<CEvolQuest> questsList;		//下一进阶任务列表
// 	CEvolInfo evolInfo;
	void read(const protos::HeroEvolRes* eq);
};

#endif