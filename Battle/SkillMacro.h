
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
namespace BattleSpace
{
	enum struct AffectType 
	{
		eNullArea						= 0,
		eFront							= 1,//前方格子类型				
		eNearby							= 2,//周边范围					
		eVertical						= 3,//纵向格子类型				
		eCross							= 4,//十字						
		eFrontTow						= 5,//前方两边					
		eNearbyUnself					= 6,//周边范围不包含自身			
		eEnemy							= 7,//敌方全体					
		eOur							= 8,//我方全体					
		eAllArea						= 9,//全图					
		ePuncture						= 10,//前方格子类型不贯穿			
		eTArea							= 11,//T型区域				
		//与武将站位格子无关类攻击范围		(从100开始进行特殊化处理)		
		RestArea						= 100,
		eFrontSingle					= 101,//前军单体					
		eCenterSingle					= 102,//中军单体					
		eBackSingle						= 103,//后军单体					
		eFrontDisperse					= 104,//前军分散范围				
		eCenterDisperse					= 105,//中军分散范围				
		eBackDisperse					= 106,//后军分散范围				
		eFrontRow						= 107,//前军n排					
		eCenterRow						= 108,//中军n排					
		eBackRow						= 109,//后军n排					
		eAnyFixGrid						= 110,//随机固定格子				
		eAnyFixGridDisperse				= 111,//随机固定格子分散区域		
		eAnyFixAlive					= 112,//随机固定武将				
		eAnyFixAliveDisperse			= 113,//随机固定武将区域			
		eLowestHp						= 114,//血量最低对象				
		eMapCenterTwoLine				= 115,//地图中间两行区域			
		eFrontFourGrid					= 116,//地图四行区域				
		eLowestHpUnCaptain				= 117,//除主帅外血量最低对象		
		eMapAnyDoubleLine				= 118,//地图中任意两行			
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
}
#endif