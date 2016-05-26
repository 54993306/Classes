#include "Battle/BattleField.h"

BattleField::BattleField()
	:id(0)
	,trapType(0)			//效果类型 (1 风暴，2 陨石，3 死者，4 黑暗，5 泥地，6 复活)
	,name("")			//效果名称
	,interval(0)		//发动间隔(秒)
	,limit(0)			//最大发动次数
	,param1(0)			//参数1
	,param2(0)			//参数2
	,batch(0)
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