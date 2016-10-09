/************************************************************* 
 *
 *
 *		Data : 2016.8.20
 *	
 *		Name : NatureTrap
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __NatureTrap_H_
#define __NatureTrap_H_
#include "Battle/Landform/BattleTrap.h"

namespace BattleSpace
{
	/*******************************************************************/
	class BloodTrap : public BattleTrap
	{
	public:
		static BloodTrap* createTrap(const TrapData* pData);
	protected:
		BloodTrap(const TrapData* pData);
		virtual bool RoleJudge();
	};

	/*******************************************************************/
	class FireTrap : public BattleTrap
	{
	public:
		static FireTrap* createTrap(const TrapData* pData);
	protected:
		FireTrap(const TrapData* pData);
		virtual bool RoleJudge();
	};

	/*******************************************************************/
	class WaterTrap : public BattleTrap
	{
	public:
		static WaterTrap* createTrap(const TrapData* pData);
	protected:
		WaterTrap(const TrapData* pData);
		virtual bool RoleJudge();
	};

	/*******************************************************************/
	class WoodTrap : public BattleTrap
	{
	public:
		static WoodTrap* createTrap(const TrapData* pData);
	protected:
		WoodTrap(const TrapData* pData);
		virtual bool RoleJudge();
	};
};
#endif