/************************************************************* 
*
*
*		Data : 2016.9.20
*	
*		Name : Buff_Attack
*
*		Author : Lin_Xiancheng
*
*		Description : 对属性产生影响类buff
*
*
*************************************************************/

#ifndef __Buff_Attack_H_
#define __Buff_Attack_H_
#include "Battle/Buff/BattleBuff.h"

namespace BattleSpace
{
	/*******************************************************************/
	class Buff_Blood : public BattleBuff
	{
	protected:
		Buff_Blood(const BuffData* pData,BaseRole* pRole);
	public:
		static Buff_Blood* CreateBuff(const BuffData* pData,BaseRole* pRole);
	protected:
		virtual void ExcuteLogic(bool pExcute);
	private:
		int bloodChange();
	};

	/*******************************************************************/
	class Buff_Attack : public BattleBuff
	{
	protected:
		Buff_Attack(const BuffData* pData,BaseRole* pRole);
	public:
		static Buff_Attack* CreateBuff(const BuffData* pData,BaseRole* pRole);
	protected:
		virtual void ExcuteLogic(bool pExcute);
	};

	/*******************************************************************/
	class Buff_Defense : public BattleBuff
	{
	protected:
		Buff_Defense(const BuffData* pData,BaseRole* pRole);
	public:
		static Buff_Defense* CreateBuff(const BuffData* pData,BaseRole* pRole);
	protected:
		virtual void ExcuteLogic(bool pExcute);
	};

	/*******************************************************************/
	class Buff_Crit : public BattleBuff
	{
	protected:
		Buff_Crit(const BuffData* pData,BaseRole* pRole);
	public:
		static Buff_Crit* CreateBuff(const BuffData* pData,BaseRole* pRole);
	protected:
		virtual void ExcuteLogic(bool pExcute);
	};

	/*******************************************************************/
	class Buff_Dodge : public BattleBuff
	{
	protected:
		Buff_Dodge(const BuffData* pData,BaseRole* pRole);
	public:
		static Buff_Dodge* CreateBuff(const BuffData* pData,BaseRole* pRole);
	protected:
		virtual void ExcuteLogic(bool pExcute);
	};

	/*******************************************************************/
	class Buff_HitRate : public BattleBuff
	{
	protected:
		Buff_HitRate(const BuffData* pData,BaseRole* pRole);
	public:
		static Buff_HitRate* CreateBuff(const BuffData* pData,BaseRole* pRole);
	protected:
		virtual void ExcuteLogic(bool pExcute);
	};

	/*******************************************************************/
	class Buff_MoveSpeed : public BattleBuff
	{
	protected:
		Buff_MoveSpeed(const BuffData* pData,BaseRole* pRole);
	public:
		static Buff_MoveSpeed* CreateBuff(const BuffData* pData,BaseRole* pRole);
	protected:
		virtual void ExcuteLogic(bool pExcute);
	};

	/*******************************************************************/
	class Buff_AttackSpeed : public BattleBuff
	{
	protected:
		Buff_AttackSpeed(const BuffData* pData,BaseRole* pRole);
	public:
		static Buff_AttackSpeed* CreateBuff(const BuffData* pData,BaseRole* pRole); 
	protected:
		virtual void ExcuteLogic(bool pExcute);
	};
};
#endif