#include "SignData.h"


CSign::CSign():day(0),status(0),vip(0)
{

}

void CSign::read(const protos::Sign& sign)
{
	this->day = sign.day();
	this->status = sign.status();
	this->vip = sign.vip();
	this->prize.read(sign.prize());
}

CSignData::CSignData():resign(0),sign(0),resign_gold(0),bCanResign(false),
	bCanSign(false)
{

}

void CSignData::read(const SignData& signData)
{
	this->resign = signData.resign();
	this->sign = signData.sign();
	this->resign_gold = signData.resign_gold();
	this->bCanResign = signData.can_resign();
	this->bCanSign = signData.can_sign();

	for (int i = 0; i < signData.signlist_size(); i++)
	{
		CSign sign;
		sign.read(signData.signlist(i));
		this->signList.push_back(sign);
	}
	for (int i = 0; i < signData.totallist_size(); i++)
	{
		CSign sign;
		sign.read(signData.totallist(i));
		this->totalList.push_back(sign);
	}
}

CSignRes::CSignRes():result(0), resignGold(0)
{

}

void CSignRes::read(const SignResponse& res)
{
	this->result = res.result();
	this->resignGold = res.resigngold();
}