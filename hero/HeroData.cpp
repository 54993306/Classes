#include "hero/HeroData.h"

CEvolQuest::CEvolQuest():id(0),action(0),stageId(0),open(false),pass(false)
{

}


void CEvolResult::read(const protos::HeroEvolRes& eq)
{
	this->result = eq.result();
	this->hero.readData(eq.hero());
}
