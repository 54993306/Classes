/************************************************************* 
 *
 *
 *		Data : 2016.9.6
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "Battle/Buff/Buff_Simpleness.h"
#include "Battle/BattleMessage.h"

namespace BattleSpace
{
	Buff_ClearDbuff::Buff_ClearDbuff( const BuffData* pData,BaseRole* pRole )
		:BattleBuff(pData,pRole)
	{

	}

	Buff_ClearDbuff* Buff_ClearDbuff::CreateBuff( const BuffData* pData,BaseRole* pRole )
	{
		Buff_ClearDbuff* tBuff = new Buff_ClearDbuff(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}
	//在处理buff逻辑处做特殊的处理，这个buff是一个特殊的buff会对其他的buff产生影响。
	void Buff_ClearDbuff::ExcuteLogic(bool pExcute)
	{
		if (pExcute)
			getRole()->clearDbuff();
	}

	/*******************************************************************/
	Buff_UnMove::Buff_UnMove( const BuffData* pData,BaseRole* pRole ):BattleBuff(pData,pRole)
	{

	}

	Buff_UnMove* Buff_UnMove::CreateBuff( const BuffData* pData,BaseRole* pRole )
	{
		Buff_UnMove* tBuff = new Buff_UnMove(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}

	void Buff_UnMove::ExcuteLogic( bool pExcute )
	{
		if (pExcute)
		{
			getRole()->setMove(false);
		}else{
			getRole()->setMove(true);
		}
	}

}
