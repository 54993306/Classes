
#ifndef __SKILLMACRO_H__
#define __SKILLMACRO_H__

/************************************************************* 
 *
 *
 *		Data : 2016.5.30
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#define eLoopNumberMax 200

//技能施放区域类型
enum SkillAreaType{
	nullArea = 0,
	eFront,								//前方格子类型				1
	eNearby,							//周边范围					2
	eVertical,							//纵向格子类型				3(计算所有武将站位格子)
	eCross,								//十字						4
	eFrontTow,							//前方两边					5
	eNearbyUnself,						//周边范围不包含自身			6
	eEnemy,								//敌方全体					7	
	eOur,								//我方全体					8
	eAllArea,							//全图						9
	ePuncture,							//前方格子类型不贯穿			10
	eTArea,								//T型区域					11
	
	//与武将站位格子无关类攻击范围		(从100开始进行特殊化处理)		
	RestArea = 100,
	eFrontOne,						//前军单体					101
	eCenterOne,						//中	军单体					102
	eBackOne,						//后	军单体					103
	eFrontDisperse,					//前军分散范围				104
	eCenterDisperse,				//中军分散范围				105
	eBackArea,						//后军分散范围				106
	eFrontRow,						//前军n排					107
	eCenterRow,						//中军n排					108
	eBackRow,						//后军n排					109
	eAnyFix,						//随机固定格子				110
	eAnyFixDisperse,				//随机固定格子分散区域		111
	eAnyAlive,						//随机固定武将				112
	eAnyAliveDisperse,				//随机固定武将区域			113
	eLowestHp,						//血量最低对象				114
	eMapCenterTwoLine,				//地图中间两行区域			115
	eFrontFourGrid,					//地图四行区域				116		
	LowestHpUnCaptain,				//除主帅外血量最低对象		117
	eMapAnyDoubleLine,				//地图中任意两行				118
};

enum AreaMark
{
	eUsType	= 1,			//我方目标
	eEnemyType,				//敌方目标
	eAllType,				//双方目标

	eFrontDirection			= 11,		//前
	eCentenDirection		= 12,		//中
	eBackDirection			= 13,		//后
};
#endif