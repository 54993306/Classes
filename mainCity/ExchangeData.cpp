#include "ExchangeData.h"

CExchangeInfo::CExchangeInfo()
	:gold(0),exchange(0),exTime(0),maxTime(0)
{

}

void CExchangeInfo::read(const ExchangeInfoRes& res)
{
	this->gold = res.gold();
	this->exchange = res.exchange();
	this->exTime = res.extime();
	this->maxTime = res.maxtime();
}

void CExRes::read(const ExRes& res)
{
	this->gold = res.gold();
	this->exchange = res.exchange();
	this->crit = res.crit();
}

void CExchangeRes::read(const ExchangeRes& res)
{
	this->exTime = res.extime();
	this->gold = res.gold();
	this->status =  res.status();
	this->exchange = res.exchange();
	for (int i = 0; i < res.result_size(); i++)
	{
		CExRes er;
		er.read(res.result(i));
		this->resultList.push_back(er);
	}
}
