/************************************************************* 
 *
 *
 *		Data : 2016.8.19
 *	
 *		Name : ObstacleTrap
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 障碍类地形
 *
 *
*************************************************************/

#ifndef __ObstacleTrap_H_
#define __ObstacleTrap_H_
#include "Battle/Landform/BattleTrap.h"
namespace BattleSpace
{
	/*******************************************************************/
	class FireObstacle : public BattleTrap
	{
	public:
		static FireObstacle* createTrap(const TrapData* pData);
		virtual bool throughJudge(BaseRole* pRole = nullptr);
	protected:
		FireObstacle(const TrapData* pData);
	};

	/*******************************************************************/
	class ObstacleTrap : public BattleTrap
	{
	public:
		static ObstacleTrap* createTrap(const TrapData* pData);
		virtual bool throughJudge(BaseRole* pRole = nullptr);
	protected:
		virtual void ExcuteAI(){};									//障碍类地形不执行AI
		ObstacleTrap(const TrapData* pTrap);
	};

	/*******************************************************************/
	class PlayerObstacle : public BattleTrap
	{
	public:
		static PlayerObstacle* createTrap(const TrapData* pData);
		virtual bool throughJudge(BaseRole* pRole  = nullptr );
	protected:
		virtual void ExcuteAI(){};	
		PlayerObstacle(const TrapData* pData);
	};

	/*******************************************************************/
	class MonsterObstacle : public BattleTrap
	{
	public:
		static MonsterObstacle* createTrap(const TrapData* pData);
		virtual bool throughJudge(BaseRole* pRole  = nullptr );
	protected:
		virtual void ExcuteAI(){};	
		MonsterObstacle(const TrapData* pData);
	};
};
#endif