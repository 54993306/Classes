
/************************************************************************/
/*                         
				//  [4/14/2015 xc_lin]
				用于实现队长技管理
				先判断种族，后判断属性，后根据效果的种族或方位限制，改变目标属性
*/
/************************************************************************/

#ifndef _CAPTAINSKILL_
#define _CAPTAINSKILL_
#include "AppUI.h"
namespace BattleSpace
{
	class BaseRole;
	class RoleSkill;
	class skEffectData;
	class CaptainSkill : public CCObject
	{
	public:
		CaptainSkill(){};
		virtual bool init(){return true;}
		CREATE_FUNC(CaptainSkill);
		void ExecuteSkill(bool pOther = false);
		CCArray* RaceJudgeCap(CCArray* arr,int type);
		CCArray* RaceJudge(CCArray* arr,int type,bool exclude = false,bool And = false);
		CCArray* AttributeJudgeArrCap(RoleSkill& skill,CCArray* arr);
		bool AttributeJudge(BaseRole* alive,int type,int rate);
		CCArray* PositionJudgeArrCap(CCArray* arr,int type);
		bool PositionJudge(bool Enemy,int grid,int type);
		CCArray* getTargetArrCap(const skEffectData*pEffect,CCArray*targetArr,CCArray*arr);			//得到执行队长技效果的目标数组并执行效果
		void ExecuteArrCap(const skEffectData*pEffect,CCArray* arr);
		void AliveExecute(const skEffectData*pEffect,BaseRole*alive);
		void AddArr(CCArray* target,CCArray*arr);
	private:

	};
	//先判断种族，后判断属性，后根据效果的种族或方位限制，改变目标属性 
	//注：带 * 为在原有的基数上进行百分比变化,否则为直接相加

	enum CaptainAliveType				//判断队长技能是否可执行的处理
	{
		Fire							= 1,//火						1僵尸
		Water							= 2,//水						2道士
		Wood							= 3,//木						3神将
		WaterAndWood					= 4,//水或木					4
		FireAndWater					= 5,//火或水					5
		FireAndWodd						= 6,//火或木					6
		WoodAndWater					= 7,//木且水					7
		WaterAndFire					= 8,//水且火					8
		WoodAndFire						= 9,//木且火					9
		AllAliveType					= 10,//所有类型武将同时存在	10
		AnyType							= 11,//不限制武将类型			11
		usAll							= 12,//满足条件的所有目标		12
		usFire							= 13,//满足条件的火			13
		usWood							= 14,//满足条件的神木			14
		usWater							= 15,//满足条件的道士			15
		CapfrontOne						= 16,//前军单体				16
		CapcenterOne					= 17,//中	军单体				17
		CapbackOne						= 18,//后	军单体				18
		CapfrontArea					= 19,//前军分散范围			19
		CapcenterArea					= 20,//中军分散范围			20
		CapbackArea						= 21,//后军分散范围			21
		CapfrontRowArea					= 22,//前军n排				22
		CapbackRowArea					= 23,//后军n排				23
		CapcenterRowArea				= 24,//中军n排				24			 	
	};
};
#endif