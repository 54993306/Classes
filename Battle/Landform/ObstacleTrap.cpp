/************************************************************* 
 *
 *
 *		Data : 2016.8.19
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/Landform/ObstacleTrap.h"
#include "Battle/RoleMacro.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"				//重构武将的数据结构后就不需要再耦合武将的服务器数据对象了
namespace BattleSpace
{
	/************************************************************* 
	 *
	 *		Data : 2016.8.20
	 *	
	 *		Name : 火属性障碍地形
	 *
	 *************************************************************/
	
	FireObstacle::FireObstacle( const TrapData* pData ):BattleTrap(pData){}

	FireObstacle* FireObstacle::createTrap( const TrapData* pData )
	{
		FireObstacle* tTrap = new FireObstacle(pData);
		if (tTrap)
		{
			tTrap->autorelease();
			return tTrap;
		}else{
			delete tTrap;
			tTrap = nullptr;
			return nullptr;
		}
	}

	bool FireObstacle::throughJudge(BaseRole* pRole /*=nullptr*/)
	{
		if (pRole && pRole->getBaseData()->getProperty() == sRoleNature::eFire)		//这里是不应该跟武将的服务器数据对象耦合的
			return true;
		return false;
	}

	/************************************************************* 
	 *
	 *		Data : 2016.8.25
	 *	
	 *		Name : 武将障碍地形
	 *
	 *************************************************************/

	ObstacleTrap::ObstacleTrap( const TrapData* pData ):BattleTrap(pData) {}

	ObstacleTrap* ObstacleTrap::createTrap( const TrapData* pData )
	{
		ObstacleTrap* tTrap = new ObstacleTrap(pData);
		if (tTrap)
		{
			tTrap->autorelease();
			return tTrap;
		}else{
			delete tTrap;
			tTrap = nullptr;
			return nullptr;
		}
	}

	bool ObstacleTrap::throughJudge(BaseRole* pRole /*=nullptr*/)
	{
		return false;
	}

	/************************************************************* 
	 *
	 *		Data : 2016.8.25
	 *	
	 *		Name : 玩家障碍地形
	 *
	 *************************************************************/

	PlayerObstacle::PlayerObstacle( const TrapData* pData ):BattleTrap(pData){}

	PlayerObstacle* PlayerObstacle::createTrap( const TrapData* pData )
	{
		PlayerObstacle* tTrap = new PlayerObstacle(pData);
		if (tTrap)
		{
			tTrap->autorelease();
			return tTrap;
		}else{
			delete tTrap;
			tTrap = nullptr;
			return nullptr;
		}
	}

	bool PlayerObstacle::throughJudge( BaseRole* pRole /*= nullptr */ )
	{
		if (pRole && pRole->getEnemy())				//怪物能过玩家武将不能过
			return true;
		return false;
	}

	/************************************************************* 
	 *
	 *		Data : 2016.8.25
	 *	
	 *		Name : 怪物障碍地形
	 *
	 *************************************************************/

	MonsterObstacle::MonsterObstacle( const TrapData* pData ):BattleTrap(pData){}

	MonsterObstacle* MonsterObstacle::createTrap( const TrapData* pData )
	{
		MonsterObstacle* tTrap = new MonsterObstacle(pData);
		if (tTrap)
		{
			tTrap->autorelease();
			return tTrap;
		}else{
			delete tTrap;
			tTrap = nullptr;
			return nullptr;
		}
	}

	bool MonsterObstacle::throughJudge( BaseRole* pRole /*= nullptr */ )
	{
		if ( pRole && !pRole->getEnemy())
			return true;
		return false;
	}
}