#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "Resources.h"
static const char* DEFAULT_FONT = "Arial";

#define PUBLIC_ROOT(__URL__) ("public/" __URL__ )
#define CSV_ROOT(__URL__) ("csv/" __URL__)
#define CSV_STORY_ROOT(__URL__) ("csv/story" __URL__)
#define MAP_ROOT(__URL__) ("map/" __URL__)
#define STORY_ROOT(__URL__) ("story/" __URL__)

enum GameRaceType
{
	RaceType_1 = 1,	//一眉道姑
	RaceType_2,		//化骨龙
	RaceType_3		//我道行
};

#define WINSIZE (CCDirector::sharedDirector()->getWinSize())
#define random_range(low, high) ((rand() % (high - low + 1)) + low)
#define ROUND(X) (int)(X+0.5)
//#define max(a,b) (((a) > (b)) ? (a) : (b))
//#ifndef min
//#define min(a,b) (((a) > (b)) ? (b) : (a))
//#endif // !min

#define abs(a) ((a)>0?(a):-(a))
#define REMOVE_TARGET(target) \
	if(target && target->getParent()) \
	{                                 \
		target->getParent()->removeChild(target);\
	}

enum WarEventPriority
{
	WarUIEventPriority,//最上层UI界面
	WarAliveEventPrority,//人物层
	WarRootEventPrority,//地图，人物容器
	WarMapEventPrority//地图层
};
#endif // !_GLOBAL_H_
