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
namespace BattleSpace
{
#define bNotification CCNotificationCenter::sharedNotificationCenter()

	enum struct sMoveType				//武将行走类型
	{
		eNull						= 0,
		eWalk						= 1,//普通行走
		eBurrow						= 2,//钻地移动
		eFight						= 3,//飞行移动
	};

	enum struct sStandType
	{
		eNull						= 0,
		eGround						= 1,//当前位置在地面
		eUnderground				= 2,//当前位置在地下
		eSky						= 3,//当前位置在天空
		eWater						= 4,//当前位置在水面
	};

	enum struct sRoleCamp
	{
		eNULL						= 0,//初始化值
		eHeroRole					= 1,//英雄类角色
		eMonsterRole				= 2,//怪物类角色
		eRivalRole					= 3,//对手类角色
		eChildRole					= 4,//召唤类角色
		eNeutrality					= 5,//中立类角色
	};

	enum struct sCallType				//定义武将召唤类型类型,不同类型武将处理不同
	{
		eCommon						= 0,//普通类型武将
		eAutoMove					= 1,//自动移动类型武将
		eFixedlyCantFly				= 2,//原地不动可被击飞类型
		eNotAttack					= 4,//石头类不攻击武将
		eAutoSkill					= 5,//陨石类即刻释放必杀技武将
		eUNAddHP					= 6,//不可被加血类型武将
		eOnlyOnce					= 7,//只攻击一次类型
		eBoxHaveRole				= 8,//盒子类怪物,死亡时角色被召唤出来
	};

	enum struct sBehavior				//怪物类型
	{
		eNormal						= 0,//普通类型怪物
		eHide						= 1,//潜行类怪物
		eAutoSkill					= 3,//出场即释放技能类
	};

	enum struct sLogicState
	{
		eNormal						= 0,//正常状态
		eUnHit						= 1,//不可受击状态
		eUnAttack					= 2,//不可攻击状态
		eInvincible					= 3,//无敌状态
		eFree						= 4,//游离状态(可移动,不可攻击不可受击)
	};

	enum struct sRoleNature
	{
		eNull						= 0,//默认初值
		eFire						= 1,//火属性武将
		eWater						= 2,//水属性武将
		eWood						= 3,//木属性武将
	};

	enum struct sMonsterSpecies
	{
		eCommon							= 0,//普通型武将
		eElite							= 1,//精英型武将
		eBoss							= 2,//boss(我方主帅类)
		eWorldBoss						= 3,//世界boss
	};
};
#endif	//__BATTLEROLEMACRO__
