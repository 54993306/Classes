#pragma once

#include "bag/bagData.h"
#include "protos/lottery_protocol.pb.h"
#include "protos/act_protocol.pb.h"


struct CNotice 
{
	string player;
	int type;
	int num;
	int id;
	CNotice();
	void read(const protos::Notice& notice);
};

enum LotteryType
{
	onceLottery = 1,
	tenLottery
};
