
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
#include "model/BattleData.h"

class WarAlive;

class CaptainSkill : public CCObject
{
public:
	CaptainSkill();
	~CaptainSkill();
	virtual bool init();
	static CaptainSkill* create();
	void ExecuteCaptainSkill();
	CCArray* RaceJudgeCap(CCArray* arr,int type);
	CCArray* RaceJudge(CCArray* arr,int type,bool exclude = false,bool And = false);
	CCArray* AttributeJudgeArrCap(CSkill& skill,CCArray* arr);
	bool AttributeJudge(WarAlive* alive,int type,int rate);
	CCArray* PositionJudgeArrCap(CCArray* arr,int type);
	bool PositionJudge(bool Enemy,int grid,int type);
	CCArray* getTargetArrCap(CEffect&effect,CCArray*targetArr,CCArray*arr);			//得到执行队长技效果的目标数组并执行效果
	void ExecuteArrCap(CEffect&effect,CCArray* arr);
	void AliveExecute(CEffect&effect,WarAlive*alive);
	void AddArr(CCArray* target,CCArray*arr);
private:

};
//先判断种族，后判断属性，后根据效果的种族或方位限制，改变目标属性 
//注：带 * 为在原有的基数上进行百分比变化,否则为直接相加
enum CaptainType
{
	allAtb = 0,		//不限制属性
	Move1,			//前方=1
	Move2,			//前斜方=2
	Move3,			//上下方=3
	Move4,			//後斜方=4
	Move5,			//後方=5
	DefRate,		//防禦=6*
	AtkRate,		//攻擊=7*
	NedHpRate,		//血量=8*
	AgilityRate,	//攻速=9*
	RenewRate,		//回復=10*
	DogeRate,		//闪避=11
	HitRate,		//命中=12
	CritRate,		//暴擊=13
	HpMaxRate,		//血量上限=14*
	BackAtkRate,	//反击几率=15
};
enum CaptainAliveType				//判断队长技能是否可执行的处理
{
	Fire=1,							//火						1僵尸
	Water,							//水						2道士
	Wood ,							//木						3神将
	WaterAndWood,					//水或木					4
	FireAndWater,					//火或水					5
	FireAndWodd,					//火或木					6
	WoodAndWater,					//木且水					7
	WaterAndFire,					//水且火					8
	WoodAndFire,					//木且火					9
	AllAliveType,					//所有类型武将同时存在	10
	AnyType,						//不限制武将类型			11
	usAll,							//满足条件的所有目标		12
	usFire,							//满足条件的火			13
	usWood,							//满足条件的神木			14
	usWater,						//满足条件的道士			15
	CapfrontOne,					//前军单体				16
	CapcenterOne,					//中	军单体				17
	CapbackOne,						//后	军单体				18
	CapfrontArea,					//前军分散范围			19
	CapcenterArea,					//中军分散范围			20
	CapbackArea,					//后军分散范围			21
	CapfrontRowArea,				//前军n排				22
	CapbackRowArea,					//后军n排				23
	CapcenterRowArea,				//中军n排				24			 	
};
#endif