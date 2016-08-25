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
#include "Battle/BaseRoleData.h"				//�ع��佫�����ݽṹ��Ͳ���Ҫ������佫�ķ��������ݶ�����
namespace BattleSpace
{
	/************************************************************* 
	 *
	 *		Data : 2016.8.20
	 *	
	 *		Name : �������ϰ�����
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
		if (pRole && pRole->getBaseData()->getProperty() == sRoleNature::eFire)		//�����ǲ�Ӧ�ø��佫�ķ��������ݶ�����ϵ�
			return true;
		return false;
	}

	/************************************************************* 
	 *
	 *		Data : 2016.8.25
	 *	
	 *		Name : �佫�ϰ�����
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
	 *		Name : ����ϰ�����
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
		if (pRole && pRole->getEnemy())				//�����ܹ�����佫���ܹ�
			return true;
		return false;
	}

	/************************************************************* 
	 *
	 *		Data : 2016.8.25
	 *	
	 *		Name : �����ϰ�����
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