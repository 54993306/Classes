#include "Battle/RoleBuff.h"

CBuff::CBuff()
	:buffId(0)		//buff ID
	,buffType(0)		//buff影响类型
	,name("")		//buff名称
	,damage(0)		//buff影响数值
	,damage_rate(0) //buff影响百分比
	,useRate(0)		//buff触发概率
	,debuf(false)	//是否为减益buff
	,duration(0)	//buff持续回合数
	,pTarget(0)		//目标类型
	,level(0)		//等级
	,typelimit(0)	//种族限制,1僵尸，2道士，3神将
{}

void CBuff::readData(const protos::common::Buff &buff)
{
	this->buffId = buff.buffid();		
	this->buffType = buff.type();
	this->name = buff.name();	
	this->damage = buff.damage();	
	this->damage_rate = buff.damage_rate();
	this->useRate = buff.userate();
	this->debuf = buff.debuf();
	this->duration = buff.duration();
	this->pTarget = buff.target();
	this->level = buff.level();
	//this->durative = buf.durative();	//来自atbType 属性影响类型，僵尸/道士/神将
	this->typelimit = buff.element();
}