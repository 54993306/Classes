
#include "Battle/BattleTrap.h"
#include "Battle/BuffData.h"

BattleTrap::BattleTrap()
:id(0),terrainId(0),name(""),terrainType(0),pro_type(0),pro_rate(0),
damage(0),posX(0),posY(0),triggerNum(0),existNum(0),batch(0)
{}

void BattleTrap::readData(const protos::common::Trap &terr)
{
	this->id = terr.id();
	this->terrainId = terr.trapid();
	this->name =terr.name();
	this->terrainType =terr.type();
	this->pro_type =terr.pro_type();
	this->pro_rate = terr.pro_rate();
	this->damage = terr.damage();
	this->posX = terr.posx();
	this->posY = terr.posy();
	this->triggerNum = terr.round();
	this->existNum = terr.touch();
	this->batch = terr.batch();
	//if (terr.has_buff())
	//	this->buff.readData(terr.buff());
}