#ifndef __BATTLEMACRO__
#define __BATTLEMACRO__

/** 
 *
 *		Data : 2016.4.12
 *	
 *		Name : BattleMacro
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 定义常用宏
 *
 */

#define CC_PROPERTY_CONSTREAD(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var);

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


enum HeroType
{
	FireType = 1,	//火属性武将
	WaterType,		//水属性武将
	WoodType,		//木属性武将
};
//技能类型(1 普通攻击，2 特殊攻击，3 必杀技，4 召唤技，5 队长技)没有必要分skill1-skill7了,不过已经是分了才加的这个类型
enum ROLESKILLTYPE
{
	NorAtk = 1,
	SpeAtk,
	CriAtk,
	CallAtk,
	CapAtk, 
};
enum struct AliveType
{
	Common,					//普通型武将
	Elite,					//精英型武将
	Boss,					//boss(我方主帅类)
	WorldBoss,				//世界boss
};


#endif