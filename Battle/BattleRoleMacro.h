/************************************************************* 
 *
 *
 *		Data : 2016.6.2
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#ifndef __BATTLEROLEMACRO__
#define __BATTLEROLEMACRO__
namespace BattleSpace{
	enum E_ATTRIBUTETYPE
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

	enum E_CALLTYPE				//定义武将召唤类型类型,不同类型武将处理不同
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

	enum E_MONSTERTYPE			//怪物类型
	{
		MST_COMMON = 0,			//普通类型怪物
		MST_HIDE,				//潜行类怪物
		MST_TIME,				//时间召唤类武将
		MST_SKILL,				//出场即释放技能类
	};

	enum E_ALIVESTATE
	{
		COMMONSTATE = 0,		//正常状态
		UNHIT,					//不可受击状态
		UNATK,					//不可攻击状态
		INVINCIBLE,				//无敌状态
	};

	enum E_HEROTYPE
	{
		FireType = 1,			//火属性武将
		WaterType,				//水属性武将
		WoodType,				//木属性武将
	};

	enum struct E_ALIVETYPE
	{
		eCommon,				//普通型武将
		eElite,					//精英型武将
		eBoss,					//boss(我方主帅类)
		eWorldBoss,				//世界boss
	};
};
#endif	//__BATTLEROLEMACRO__
