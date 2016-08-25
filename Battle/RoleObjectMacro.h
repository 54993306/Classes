/************************************************************* 
 *
 *
 *		Data : 2016.8.21
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#ifndef __PLAYHPTYPE__H_
#define __PLAYHPTYPE__H_

namespace BattleSpace
{
	enum struct PlayHpType
	{
		nullType			= 0,		//不显示掉血内容
		typeface			= 1,		//显示字体类内容；1：闪避
		gainType			= 2,		//加血
		angerType			= 3,		//怒气类型
		generalType			= 4,		//一般型掉血
		cutType				= 5,		//减少型掉血
		addType				= 6,		//增加型掉血
		genralCritType		= 7,		//一般型暴击
		cutCritType			= 8,		//减少型暴击
		addCritType			= 9,		//增加型暴击
	};


}

#endif
