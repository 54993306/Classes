#ifndef __EXCHANGDATA_H
#define __EXCHANGDATA_H
#include "model/BattleData.h"
#include "bag/bagData.h"

struct CExchangeInfo
{
	int gold ;	//兑换元宝数
	int exchange;	//可兑换的铜钱或体力
	int exTime;	//当前可兑换次数
	int maxTime;	//可兑换的最大次数
	CExchangeInfo();
	void read(const ExchangeInfoRes& res);
};

struct CExRes{
	int gold;	//使用元宝数
	int exchange;	//兑换数
	int crit;	//暴击倍数
	void read(const ExRes& res);
};


struct CExchangeRes
{
	int status;	//兑换结果(1 成功，2 兑换次数不足，3 元宝不足，4 数据错误)
	int gold;	//下次兑换需要的元宝
	int exTime;	//剩下兑换次数
	int exchange;
	vector<CExRes> resultList;	//兑换结果
	void read(const ExchangeRes& res);
};


#endif // !__CITYDATA_H
