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
	
	enum struct sRoleType
	{
		eBaseRole				= 0,
		eHeroRole				= 1,
		eMonsterRole			= 2,
	};

	enum struct sProperty
	{
		eNull						= 0,
		eAttack						= 1,	//攻击
		eDefense					= 2,	//防御
		eBlood						= 3,	//血量上限 
		eHit						= 4,	//命中
		eCrit						= 5,	//暴击
	};

	enum struct sCallType					//定义武将召唤类型类型,不同类型武将处理不同
	{
		eCommon						= 0,	//普通类型武将
		eAutoMove					= 1,	//自动移动类型武将
		eFixedlyCantFly				= 2,	//原地不动可被击飞类型
		eNotAttack					= 4,	//石头类不攻击武将
		eAutoSkill					= 5,	//陨石类即刻释放必杀技武将
		eUNAddHP					= 6,	//不可被加血类型武将
		eOnlyOnce					= 7,	//只攻击一次类型
	};

	enum struct sBehavior					//行为类型
	{
		eNormal						= 0,	//普通类型怪物
		eHide						= 1,	//潜行类怪物
		eAutoSkill					= 3,	//出场即释放技能类
	};

	enum struct sLogicState					//角色逻辑状态
	{
		eNormal						= 0,	//正常状态
		eUnHit						= 1,	//不可受击状态
		eUnAttack					= 2,	//不可攻击状态
		eInvincible					= 3,	//无敌状态
	};

	enum struct sMonsterSpecies				//怪物种类
	{
		eCommon						= 0,	//普通型武将
		eElite						= 1,	//精英型武将
		eBoss						= 2,	//boss(我方主帅类)
		eWorldBoss					= 3,	//世界boss
	};
	//火1>木3; 木3>水2; 水2>火1
	enum struct sAttribute					//角色属性
	{
		eNull						= 0,
		eFire						= 1,	//火属性武将
		eWater						= 2,	//水属性武将
		eWood						= 3,	//木属性武将
	};
};
#endif	//__BATTLEROLEMACRO__