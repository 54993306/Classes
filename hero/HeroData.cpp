#include "hero/HeroData.h"

CEvolQuest::CEvolQuest():id(0),action(0),stageId(0),open(false),pass(false)
{

}

void CEvolResult::read(const protos::HeroEvolRes* eq)
{

	if (eq->result())
		this->result = true;
	else
		this->result = false;
	this->hero.readData(eq->hero());
}
