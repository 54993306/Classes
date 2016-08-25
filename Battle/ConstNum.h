

#ifndef  __CONSTNUMBER__
#define	 __CONSTNUMBER__
#include "cocos2d.h"
using namespace cocos2d;

//地图相关常量
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
const CCPoint	INVALID_POINT		= CCPoint(50,50);		//测试用可以看得见
#else
const CCPoint	INVALID_POINT		= CCPoint(-200,-200);
#endif
const int		C_GRIDOFFSET_X		= 4;				//格子间隙
const int		C_GRIDOFFSET_Y		= 2;				//格子间隙
const int		C_GRID_ROW			= 4;
const int		C_GRID_COL_MIN		= 16;				//可处理列数
const int		C_GRID_COL			= 32;
const int		C_MOSTERATKGRID		= 60;				//开始计算怪物攻击格子
const int		C_BEGINGRID			= 64;				//前面的格子为不可显示范围
const int		C_STANDGRID			= 96;				//武将怒气值增加的半屏
const int		C_CAPTAINGRID		= 124;				//主帅可移动范围格子
const int		C_CAPTAINSTAND		= 125;				//主帅初始站立格子
const int		C_ENDGRID			= 127;					

const CCRect	NULL_RECT			= CCRect(0,0,0,0);
const int		GRID_WIDTH			= 118;				//格子大小108*92
const int		GRID_HEIGHT			= 105;
const char		INVALID_GRID		= 0;


//武将相关
const int		C_BatchMonst		= 2000;				//伏兵ID
const int		C_CallHero			= 4000;				//我方英雄召唤武将
const int		C_CallMonst			= 5000;				//敌方召唤怪物

const int		C_PVEStage			= 10;
const int		C_PVEStopGrid		= 68;			
const int		C_OtherCaptain		= 65;
const int		C_PVPCol			= 17;
const int		C_PVPMINGRID		= 64;
const int		C_PVEOtherArea		= 83;

const int		C_TrapSpace			= 1;				//地形1S触发一次效果
#endif
