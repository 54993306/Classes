#ifndef _DATA_DEFINE_H_
#define _DATA_DEFINE_H_
#include "cocos2d.h"
#include <string>
#include <vector>
#include "model/BattleData.h"
#include "tools/commonDef.h"

USING_NS_CC;

#define NOTIFICATION CCNotificationCenter::sharedNotificationCenter()


using namespace std;
class BuffManage;
class ActObject;
class MoveObj;
enum MoveRange
{
	TypeOne,
	TypeTwo,
	TypeThree,
	TypeFour,
	TypeFive, 
};

enum AttributeType
{
	atb_normal = 0,
	atb_attak ,			//1		攻击
	atb_def,			//2		防御
	atb_hp,				//3		血量上限 
	atb_hit,			//4		命中
	atb_crit,			//5		暴击
	atb_agile,			//6		先攻
	atb_move,			//7		移动距离
	atb_addhrt,			//8		增伤属性
	atb_cuther,			//9		减伤属性
	atb_ice_hrt,		//10	冰伤害
	atb_mine_hrt,		//11	雷伤害
	atb_fire_hrt,		//12	火伤害
	atb_ice_def	 = 20,	//20	冰抗性
	atb_mine_def,		//21	雷抗性
	atb_fire_def,		//22	火抗性
	atb_anger	 = 30,	//30	能量回复
	atb_hp_recover,		//31	血量回复
};

enum MoreGridAlive		//多格子武将定义
{
	oneGrid		=1,
	twoGrid		=2,
	threeGrid	=3,
	fourGrid	=4,
	sixGrid		=6,
	eightGrid	=8,
	twelveGrid	=12,
};

enum CALL_Type			//定义武将召唤类型类型,不同类型武将处理不同
{
	CommonType = 0,			//普通类型武将
	AutoMoveType,			//自动移动类型武将
	FixedlyCantFlyType,		//原地不动可被击飞类型
	FixedlyType,			//原地不动不可被击飞类型
	NotAttack,				//石头类不攻击武将
	AutoSkill,				//陨石类即刻释放必杀技武将
	UNAddHP,				//不可被加血类型武将
	OnlyOnce,				//只攻击一次类型
};
enum MST_TYPE				//怪物类型
{
	MST_COMMON = 0,			//普通类型怪物
	MST_HIDE,				//潜行类怪物
	MST_TIME,				//时间召唤类武将
	MST_SKILL,				//出场即释放技能类
};
enum ALIVESTATE
{
	COMMONSTATE = 0,		//正常状态
	UNHIT,					//不可受击状态
	UNATK,					//不可攻击状态
	INVINCIBLE,				//无敌状态
};
/******* 武将基础数据 ********/

class AliveBase : public CCObject
{
public:
	AliveBase();
	virtual ~AliveBase(){};
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

//重构方向，这个类还是一样作为数据存储的存在，跟显示对象aliveObject组合构成一个完整的武将。Actobject(包含武将独立逻辑和公用逻辑的地方)
//太多地方引用了导致整个类都不敢修改，实际上应该以接口的方式来处理的。接口修改少，但是继承接口的实现就难说了
/******************* 战场武将数据 ********************/

class WarAlive:public AliveBase					//定义数据的顺序结构是为了方便查看数据
{
public:
	WarAlive();
	virtual ~WarAlive();
	CREATE_FUNC(WarAlive);		//父类存在init方法，所有这里就不需要了。
	void ResetAttackState();					//重置武将当前状态信息
	void ExcuteNextEffect();					//重置武将下执行下一个效果状态
	ROLE* role;									//服务器角色信息
	vector<int> grids;							//多格子站位
	vector<int> moves;							//各方向移动范围
	vector<int> TouchGrids;						//移动状态下武将多格子处理
	vector<int> AtkArea;						//多个攻击格子
	vector<WarAlive*> AliveS;					//多个攻击目标
	vector<int>AtkGrid;							//存储武将当前攻击区域
	vector<WarAlive*> AtkAlive;					//存储攻击区域内受击目标
	vector<WarAlive*> HittingAlive;				//受击目标中被击中对象				(用于做buff和受击目标死亡结算处理)
	CC_PROPERTY(BuffManage*,m_BuffManage,BuffManage);
	CC_SYNTHESIZE(ActObject*,m_ActObject,ActObject);
	CC_SYNTHESIZE(MoveObj*,m_MoveObj,MoveObj);			//设置移动对象
	CC_SYNTHESIZE(bool,m_IsEnemy,Enemy);				//怪物
	CC_PROPERTY(bool,m_NorAtk,NorAtk);					//普通攻击状态
	CC_SYNTHESIZE(bool,m_SpecialAtk,SpeAtk);			//特殊攻击
	CC_SYNTHESIZE(bool,m_CritSkill,CriAtk);				//必杀技
	CC_SYNTHESIZE(int,m_MstType,MstType);				//怪物类型
	CC_PROPERTY(int,m_CallType,CallType);				//武将召唤类型(我方武将均为召唤类型武将)
	CC_SYNTHESIZE(bool,m_AIState,AIState);				//武将是否在AI状态
	CC_SYNTHESIZE(bool,m_DieState,DieState);			//武将是否在AI状态
	CC_PROPERTY(int,m_GridIndex,GridIndex);				//位置
	CC_PROPERTY(int,m_MoveGrid,MoveGrid);				//移动目标格子
	CC_SYNTHESIZE(bool,m_TouchState,TouchState);		//武将当前为移动状态下
	CC_PROPERTY(int,m_TouchGrid,TouchGrid);				//移动状态下当前位置
	CC_SYNTHESIZE(float,m_MoveSpeed,MoveSpeed);				//移动速度(格/S)
	CC_SYNTHESIZE(int,m_UILayout,UiLayout);				//对应的UI控制面板ID
	CC_PROPERTY(float,m_Delaytime,Delaytime);			//武将出现延迟时间
	CC_PROPERTY(float,m_AtkDelay,AtkDelay);				//武将攻击延迟时间
	CC_PROPERTY(float,m_StateDelay,StatDelay);			//武将状态延迟时间
	CC_PROPERTY(int,m_AliveState,AliveStat);			//武将状态
	CC_PROPERTY(float,m_Atktime,Atktime);				//攻击间隔时间
	CC_PROPERTY(float,m_CritTime,CritTime);				//必杀技时间
	CC_SYNTHESIZE(float,m_AtkInterval,AtkInterval);		//攻速(次/秒)
	CC_SYNTHESIZE(float,m_TimePercent,TimePercent);		//攻速增减百分比
	CC_SYNTHESIZE(int,m_initCost,InitCost);				//初始 cost
	CC_SYNTHESIZE(int,m_CostMax,Costmax);				//cost Max
	CC_SYNTHESIZE(float,m_AddCost,AddCost);				//cost 秒变化率
	CC_PROPERTY(int,m_AtkNum,AtkNum);					//记录攻击次数(3次释放特殊攻击)
	CC_SYNTHESIZE(int,m_Batch,Batch);					//批次
	CC_SYNTHESIZE(int,m_EfGroup,EffectGroup);			//效果组
	CC_SYNTHESIZE(int,m_EffectIndex,EffIndex);			//效果Index
	CC_PROPERTY(int,m_Hp,Hp);							//血量
	CC_SYNTHESIZE(unsigned int,m_MaxHp,MaxHp);			//血量Max
	CC_SYNTHESIZE(int,m_Atk,Atk);						//攻击
	CC_PROPERTY(int,m_Def,Def);							//防御
	CC_SYNTHESIZE(int,m_Hit,Hit);						//命中
	CC_SYNTHESIZE(int,m_Doge,Doge);						//闪避
	CC_SYNTHESIZE(int,m_Crit,Crit);						//暴击
	CC_SYNTHESIZE(AliveType,m_AliveType,AliveType);		//角色品质等级
	CC_SYNTHESIZE(unsigned int,m_Renew,Renew);			//回复
	CC_SYNTHESIZE(bool,m_Battle,Battle);				//上阵状态
	CC_SYNTHESIZE(int,m_SortieNum,SortieNum);			//当前回造成伤害次数
	CC_SYNTHESIZE(int,m_FatherID,FatherID);				//被召唤武将存储父武将ID
	CC_SYNTHESIZE(int,m_CallAliveNum,CallAliveNum);		//record can call alive number use int alive is captain
	CC_SYNTHESIZE(bool,m_Captain,Captain);				//是否为队长
	CC_SYNTHESIZE(bool,m_CritEffect,CritEffect);		//必杀技播放过特效
	CC_SYNTHESIZE(bool,m_ExecuteCap,ExecuteCap);		//队长技执行标记
	CC_SYNTHESIZE(bool,m_Negate,Negate);				//标记是否转向攻击
	CC_SYNTHESIZE(bool,m_Move,Move);					//是否可以移动
	CC_SYNTHESIZE(bool,m_cloaking,Cloaking);			//隐身状态
	CC_SYNTHESIZE(int,m_Hrt,Hrt);						//伤害减免效果(正加深,负减少)
	CC_SYNTHESIZE(int,m_HrtPe,Hrtpe);					//伤害百分比(正加深,负减少)
	CC_SYNTHESIZE(float,m_Zoom,Zoom);					//缩放比
	CC_SYNTHESIZE(int,m_TerType,TerType);				//地形影响属性类型
	CC_SYNTHESIZE(int,m_TerTypeNum,TerTypeNum);			//地形影响属性数值
	CC_SYNTHESIZE(bool,m_LastAlive,LastAlive);			//隐身状态
	bool canSummonAlive();								//是否可以召唤武将
};

class CityAlive:public AliveBase
{
public:
	CityAlive(){};
	virtual ~CityAlive(){};
	CREATE_FUNC(CityAlive);
public:
	CC_SYNTHESIZE(float,m_x,X);
	CC_SYNTHESIZE(float,m_y,Y);
};

class UserData : public AliveBase
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