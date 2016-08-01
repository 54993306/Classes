#ifndef _DATA_DEFINE_H_
#define _DATA_DEFINE_H_
#include "cocos2d.h"
#include <string>
#include <vector>
#include "tools/commonDef.h"
#include <protos/common/role_common.pb.h>
USING_NS_CC;
using namespace std;
#define NOTIFICATION CCNotificationCenter::sharedNotificationCenter()

class BaseObject : public CCObject
{
public:
	BaseObject();
	virtual ~BaseObject(){};
public:
	CC_SYNTHESIZE(unsigned int,m_AliveID,AliveID);
	CC_SYNTHESIZE(int,m_Model,Model);
	inline void setName(const char* name){m_Name = name;}
	inline string getName(){return m_Name;}
protected:
	inline bool init(){return true;}
protected:
	string m_Name;
};

class CityAlive:public BaseObject
{
public:
	CityAlive(){};
	virtual ~CityAlive(){};
	CREATE_FUNC(CityAlive);
public:
	CC_SYNTHESIZE(float,m_x,X);
	CC_SYNTHESIZE(float,m_y,Y);
};

class UserData : public BaseObject
{
public:
	UserData();
	virtual ~UserData(){};
	CREATE_FUNC(UserData);
public:
	CC_SYNTHESIZE(int,m_raceType,RaceType);			//种族
	CC_SYNTHESIZE(int,m_roleId,RoleID);				//种族
	CC_SYNTHESIZE(int,m_level,Level);				//等级
	CC_SYNTHESIZE(int,m_coin,Coin);					//金币
	CC_SYNTHESIZE(int,m_exp,Exp);					//经验
	CC_SYNTHESIZE(int,m_nextExp,NextExp);
	CC_SYNTHESIZE(int,m_roleAction,RoleAction);
	CC_SYNTHESIZE(int,m_mapId,MapId);
	CC_SYNTHESIZE(int,m_roleGold,RoleGold);
	CC_SYNTHESIZE(string,m_roleName,RoleName);
	CC_SYNTHESIZE(int,m_roleFood, RoleFood);
	CC_SYNTHESIZE(int,m_Friends, Friends);			//友情点
	CC_SYNTHESIZE(int,m_actionLimit,ActionLimit);	//行动值上限
	CC_SYNTHESIZE(time_t,m_actionTime,ActionTime);	//下一次行动值回复时间
	CC_SYNTHESIZE(int,m_interval,Interval);	//行动力加复间隔(分钟)
	CC_SYNTHESIZE(int,m_vip, Vip);	      //VIP等级
	CC_SYNTHESIZE(int,m_combat, Combat);  //角色战斗力
	CC_SYNTHESIZE(int,m_thumb, Thumb);   //头像编号
	CC_SYNTHESIZE(int,m_newStep, NewStep);   //新手教程步骤
	CC_SYNTHESIZE(bool,m_mobileShop,MobileShop);
	CC_SYNTHESIZE(int,m_inviteFriend, InviteFriend);   //新手教程步骤
	CC_SYNTHESIZE(bool,m_isFBLogin,IsFBLogin);
	CC_SYNTHESIZE(string,m_fbId,FbId);
	CC_SYNTHESIZE(bool, m_bFirstLogin, FirstLogin);
	CC_SYNTHESIZE(bool, m_firstMobileShop, FirstMobileShop);
	CC_SYNTHESIZE(int,m_roleMoney, RoleMoney);   //充值金币
	CC_SYNTHESIZE(int,m_rolePoints,RolePoints);  //PVP勋章
	CC_SYNTHESIZE(bool, m_bGoogleBilling, GoogleBilling);  //GooglePay
	CC_SYNTHESIZE(int, m_roleNation, RoleNation);  //GooglePay

	void read(const protos::common::Role& role);
};
//武将的信息最后由服务器发送过来，定义好一个结构体去接收服务器发送的信息
class WuJiangCfg
{
public:
	int id;					//武将ID
	string name;			//名称
	char quality;			//品质
	int capacity;			//资质
	char type;				//类型
	string model;			//形象模型
	string icon;			//头像icon
	string cardIcon;		//卡片插画
	string desc;
	vector<int> subSkill;	//普通技能
	vector<int> mainSkill;	//主帅技
	vector<int> critSkill;	//必杀技
	char dragVal;			//拖拽范围
};

class SkillCfg
{
public:
	int id;
	string name;
	int icon;
	string model;
	string desc;
	char type;
	int area;				//技能区域需要从配置表里面去读取
};

class SkillEffectCfg
{
public:
	int id;
	int type;
	vector<string> names;
};

class TollgateCfg
{
public:
	int id;
	string name;
	int boss;				//bossID
	vector<int> wuJiang;	//参战武将ID
	int mapID;				//战斗场景地图ID
	int star;				//关卡星级
	int level;				//开放等级
	string openIcon;		//已开放的关卡ICON
	string closeIcon;		//未开放的关卡ICON
	bool isOpen;
};
#endif // !_DATA_DEFINE_H_