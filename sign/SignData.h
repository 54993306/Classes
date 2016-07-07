#pragma once

/******************************************************
*文件名称:	SignData.h
*编写日期:	2016-6-14-11:45
*编写作者:	YPF
*功能描述:	签到数据
*******************************************************/

#include "bag/bagData.h"
#include "protos/protocol.h"

struct CSign
{
	int day;				//签到天数 (1 ~ 30 天)
	int status;			////状态 (1 已签，2 未签) (1 已领取，2 未领取)
	CPrize prize;			//签到获得的道具
	int vip;				//VIP双倍等级
	CSign();
	void read(const protos::Sign& sign);
};

struct CSignData
{
	int resign;	//可补签天数
	int sign;	//已签到天数
	vector<CSign> signList;	//签到数据列表
	vector<CSign> totalList;	//累计签到数据列表
	int resign_gold;	//可补签天数补签价格(元宝)
	bool bCanResign;		//是否可补签
	bool bCanSign;				//是否可签	
	CSignData();
	void read(const SignData& signData);
};

struct CSignRes
{
	int result;	//签到结果(1 成功，2 补签元宝数量不足，3 数据错误)
	int resignGold;	//下一次补签花费
	CSignRes();
	void read(const SignResponse& res);
};