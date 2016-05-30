
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

//技能施放区域类型
enum SkillAreaType{
	nullArea = 0,
	front,							//前方格子类型				1
	roundArea,						//周边范围					2
	lengthwaysArea,					//纵向格子类型				3(计算所有武将站位格子)
	crossArea,						//十字						4
	slantArea,						//前方两边					5
	roundExclude,					//周边范围不包含自身			6
	EnemyArea,						//敌方全体					7	
	usArea,							//我方全体					8
	allArea,						//全图						9
	frontAreaVia,					//前方格子类型不贯穿			10
	tTypeArea,						//T型区域					11
	//与武将站位格子无关类攻击范围		(从100开始进行特殊化处理)		
	RestArea = 100,
	frontOne,						//前军单体					101
	centerOne,						//中	军单体					102
	backOne,						//后	军单体					103
	frontArea,						//前军分散范围				104
	centerArea,						//中军分散范围				105
	backArea,						//后军分散范围				106
	frontRowArea,					//前军n排					107
	centerRowArea,					//中军n排					108
	backRowArea,					//后军n排					109
	anyFixGrid,						//随机固定格子				110
	anyFixGridArea,					//随机固定格子分散区域		111
	anyFixAlive,					//随机固定武将				112
	anyFixAliveArea,				//随机固定武将区域			113
	allTypeLowestHp,				//血量最低对象				114
	frontTwoType,					//地图中间两行区域			115
	frontFourType,					//地图四行区域				116		
	TypeLowestHp,					//除主帅外血量最低对象		117
	DoubleLine,						//地图中两行					118
};
//警戒区域
enum GuardType
{
	GuardFront =1,					//前方范围
	GuardBack,						//后方范围
	GuardFrontAndBack,				//前后范围
	GuardUpDown,					//警戒区域为上下范围
};

enum AreaMark
{
	ustarget	= 1,		//我方目标
	enemyTyep,				//敌方目标
	allType,				//双方目标

	FrontArea		= 11,		//前
	CentenArea		= 12,		//中
	BackArea		= 13,		//后

	RowArea			= 20,		//行
	DisperseArea	= 30,		//扩散

	Hit_Alive	= 100,
	Atk_Area	= 200,
};
#endif