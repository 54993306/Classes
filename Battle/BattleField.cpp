#include "Battle/BattleField.h"

BattleField::BattleField()
:id(0),trapType(0),name(""),interval(0),limit(0)
,param1(0),param2(0),batch(0)
{}

void BattleField::readData(const protos::FieldEff &fieldeff)
{
	this->id = fieldeff.id();
	this->trapType = fieldeff.type();
	this->name = fieldeff.name();
	this->interval = fieldeff.interval();
	this->limit = fieldeff.limit();
	this->param1 = fieldeff.param1();
	this->param2 = fieldeff.param2();
	this->batch = fieldeff.batch();
	//this->monster.readData(fieldeff.monster());
}