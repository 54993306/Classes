#ifndef __CITYDATA_H
#define __CITYDATA_H
#include "model/CRoleData.h"
#include "bag/bagData.h"

struct CCity
{
	int id;		//玩家主城建筑ID
	int cityId;	//建筑ID
	string cityName;	//名称
	int level;	//等级
	int lvFood;	//升级需要的粮食数量
	int strenLv; //铁匠铺强化上限等级
	int exp;		//当前经验
	int maxExp;	//下一级升级经验
	int basePE;	//基本产量
	int extPE;	//额外产量
	int cap;		//当前存储量
	int maxCap;	//最大存储量
	int heroNum;	//可驻守英雄数量
	int haveHeroNum;  //当前已驻守的英雄数量
	vector<CHero> heroList;	//驻守英雄列表
// 	bool levelUp;
    bool capFull;
	string note;
	bool  isOpen;
	void read(const City& city);
};

struct CityBuild
{
	int id;		        //玩家主城建筑ID
	int cityId ;	    //建筑ID
	string cityName;	//名称
	int level ;	        //等级
	CityBuild():
		id(0),cityId(0),cityName(""),level(0)
	{

	}
	CityBuild(const CityBuild& cb)
	{
		this->id = cb.id;
		this->cityId = cb.cityId;
		this->level = cb.level;
		this->cityName = cb.cityName;
	}
	void read(const City& res);
};


struct CityData
{
	int roleId ;	//游戏角色ID
	vector<CCity> cityList;  //建筑列表
	
	CityData():roleId(0)
	{
		
	}

	CityData& operator=(CityData& data);

	void read(const cityResponse& res);
};


struct CComposeData
{
	CItem item;		//调合的目标道具
	vector<CItem> itemList;	//需要的素材列表
	bool isCanGet;		//是否满足兑换
	void read(const ComposeData& res);
};

#endif // !__CITYDATA_H
