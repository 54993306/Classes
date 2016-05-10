#include "CityData.h"


void CCity::read(const City& city)
{
	this->id = city.id();
	this->cityId = city.cityid();
	this->cityName = city.cityname();
	this->level = city.level();
 	this->exp = 0;
 	this->maxExp = 0;
	this->lvFood = city.lvfood();
	this->strenLv = city.strenlv();

	this->basePE = city.basepe();
	this->extPE = city.extpe();
	this->cap = city.cap();
	this->maxCap = city.maxcap();
	this->heroNum = city.heronum();
	this->haveHeroNum = city.hero();
/*	this->levelUp =  city.levelup();*/
	this->capFull = city.capfull();
	this->note = city.note();
	for (int i = 0; i < city.herolist_size(); i++)
	{
		Hero hr = city.herolist(i);
		CHero hero;
		hero.readData(hr);
		this->heroList.push_back(hero);
	}
	this->isOpen = city.isopen();
}

void CityData::read(const cityResponse& res)
{
	this->roleId = res.roleid();
	for (int i = 0; i < res.citylist().size(); i++)
	{
		City city =res.citylist(i);
		CCity cb;
		cb.read(city);
		this->cityList.push_back(cb);
	}
}

void CityBuild::read(const City& res)
{
	this->cityId = res.cityid();
	this->cityName = res.cityname();
	this->id = res.id();
	this->level = res.level();
}

CityData& CityData::operator=(CityData& data)
{
	this->roleId = data.roleId;
	for (int i=0; i<data.cityList.size(); ++i)
	{
		this->cityList.push_back(data.cityList[i]);
	}
	return *this;
}

void CComposeData::read(const ComposeData& res)
{
	item.read(res.item());
	for (int i = 0; i < res.itemlist_size(); i++)
	{
		CItem it;
		it.read(res.itemlist(i));
		if (it.itemType==2)
		{
			itemList.insert(itemList.begin(),it);
		}
		else
		{
			itemList.push_back(it);
		}
	}
	isCanGet = res.canget();
}
