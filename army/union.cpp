#include "union.h"


void CUnion::read(const UnionResponse& nion)
{

	if (nion.has_hero1())
	{
		CHero hero;
		hero.readData(nion.hero1());
		this->heroList.push_back(hero);
	}
	else
	{
		CHero hero;
		this->heroList.push_back(hero);
	}
	
	if (nion.has_hero2())
	{
		CHero hero;
		hero.readData(nion.hero2());
		this->heroList.push_back(hero);
	}
	else
	{
		CHero hero;
		this->heroList.push_back(hero);
	}

	if (nion.has_hero3())
	{
		CHero hero;
		hero.readData(nion.hero3());
		this->heroList.push_back(hero);
	}
	else
	{
		CHero hero;
		this->heroList.push_back(hero);
	}

	if (nion.has_hero4())
	{		
		CHero hero;
		hero.readData(nion.hero4());
		this->heroList.push_back(hero);
	}
	else
	{
		CHero hero;
		this->heroList.push_back(hero);
	}

	if (nion.has_hero5())
	{
		CHero hero;
		hero.readData(nion.hero5());
		this->heroList.push_back(hero);
	}
	else
	{
		CHero hero;
		this->heroList.push_back(hero);
	}
}