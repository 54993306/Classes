/************************************************************* 
*
*
*		Data : 2016.8.20
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/
#include "Battle/Landform/NatureTrap.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
#include "Battle/RoleMacro.h"
namespace BattleSpace
{
	FireTrap::FireTrap( const TrapData* pData ):BattleTrap(pData){}

	FireTrap* FireTrap::createTrap( const TrapData* pData )
	{
		FireTrap* tTrap = new FireTrap(pData);
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

	bool FireTrap::RoleJudge()	//火属性的武将站立则不执行效果
	{
		if (mTriggerRole->getBaseData()->getProperty() == sRoleNature::eFire)
			return false;
		return true;
	}

	WaterTrap::WaterTrap( const TrapData* pData ):BattleTrap(pData){}

	WaterTrap* WaterTrap::createTrap( const TrapData* pData )
	{
		WaterTrap* tTrap = new WaterTrap(pData);
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

	bool WaterTrap::RoleJudge()	//水属性武将不执行AI
	{
		if (mTriggerRole->getBaseData()->getProperty() == sRoleNature::eWater)
			return false;
		return true;
	}

	WoodTrap::WoodTrap( const TrapData* pData ):BattleTrap(pData){}

	WoodTrap* WoodTrap::createTrap( const TrapData* pData )
	{
		WoodTrap* tTrap = new WoodTrap(pData);
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

	bool WoodTrap::RoleJudge()
	{
		if (mTriggerRole->getBaseData()->getProperty() == sRoleNature::eWood)
			return false;
		return true;
	}
}