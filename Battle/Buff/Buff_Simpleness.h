/************************************************************* 
*
*
*		Data : 2016.9.6
*	
*		Name : Buff_ClearDbuff
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __Buff_ClearDbuff_H_
#define __Buff_ClearDbuff_H_
#include "Battle/Buff/BattleBuff.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	/*******************************************************************/
	class Buff_ClearDbuff : public BattleBuff
	{
	private:
		Buff_ClearDbuff(const BuffData* pData,BaseRole* pRole); 
	public:
		static Buff_ClearDbuff* CreateBuff( const BuffData* pData,BaseRole* pRole);
	protected:
		virtual void ExcuteLogic(bool pExcute);
	};

	/*******************************************************************/
	class Buff_UnMove : public BattleBuff
	{
	private:
		Buff_UnMove(const BuffData* pData,BaseRole* pRole);
	public:
		static Buff_UnMove* CreateBuff( const BuffData* pData,BaseRole* pRole);
	protected:
		virtual void ExcuteLogic(bool pExcute);
	};
};
#endif