#include "RecruitData.h"

HeroLotteryData::HeroLotteryData()
	:id(0),quality(0),thumb(0),type(0),isnew(false),heroType(-1),name("")
	,m_iNum(0),iColor(1)
{}

void HeroLotteryData::readData(const protos::Lottery &lottery)
{
	this->id = lottery.id();
	this->quality = lottery.quality();
	this->thumb = lottery.thumb();
	this->type	= lottery.type();
	this->isnew = lottery.isnew();
	this->heroType = lottery.herotype();
	this->name = lottery.name();
	this->m_iNum = lottery.num();
	this->iColor = lottery.color();
}