#include "HeroData.h"

CEvolQuest::CEvolQuest():id(0),action(0),stageId(0),open(false),pass(false)
{

}

// void CEvolQuest::read(const EvolQuest& eq)
// {
// 	this->id = eq.id();
// 	this->stageId = eq.stageid();
// 	this->open = eq.open();
// 	this->pass = eq.pass();
// 	this->action = eq.action();
// }


// void CEvolResult::read(const EvolResult& eq)
// {
// 	this->result = eq.result();
// 	this->hero.readData(eq.hero());
// 	this->evolInfo.canEvol = eq.canevol();
// 	for (int i = 0; i < eq.quests_size(); i++)
// 	{
// 		CEvolQuest evol;
// 		evol.read(eq.quests(i));
// 		this->evolInfo.questsList.push_back(evol);
// 	}
// }
// 
// void CEvolInfo::read(const EvolInfo& eq)
// {
// 	this->canEvol = eq.canevol();
// 	for (int i = 0; i < eq.quests_size(); i++)
// 	{
// 		CEvolQuest evol;
// 		evol.read(eq.quests(i));
// 		this->questsList.push_back(evol);
// 	}
// }


void CEvolResult::read(const HeroEvolRes& eq)
{

	this->result = eq.result();
	this->hero.readData(eq.hero());
}
