// ***************************************************************
//  FileName: commonDef.h
//  Version:  1.0   
//  Date:  2014.7.8 10:04  
//  Author: linxiancheng
//  Copyright (C) 2014 U1
//  --------------------------------------------------------------
//  Description:  commonDef.h
//  -------------------------------------------------------------
//  History: 
//  -------------------------------------------------------------
// ***************************************************************

#pragma once
#include"cocos2d.h"
#define FONT_NAME								"SimHei"					//"Thonburi"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define SCALE_FACTOR							(2.0f / 3.0f)
////////////字体大小//////////////
#define NORMAL_FONT_SIZE						(23)
#define BIG_FONT_SIZE							(30)
#define SMALL_FONT_SIZE							(14)
#define BUTTON_FONT_SIZE						(19)
#define TITTLE_TAB_FONT_SIZE					(24)
#else
#define SCALE_FACTOR							(1.0f)
////////////字体大小//////////////
#define NORMAL_FONT_SIZE						(25)
#define BIG_FONT_SIZE							(32)
#define SMALL_FONT_SIZE							(16)
#define BUTTON_FONT_SIZE						(20)
#define TITTLE_TAB_FONT_SIZE					(25)
#endif
#define supremeTouchPriority					(kCCMenuHandlerPriority - 1)
#define commonBtnWidth							(198)
#define commonBtnHeight							(47)
#define maxIDStrCount							(8)	
#define minIDStrCount							(6)	
#define maxLoadingTimeOut						(15)

typedef struct
{
	cocos2d::CCSprite* normalSpr;
	cocos2d::CCSprite* selectedSpr;
}tabMenuItem;

//获取修正坐标
inline cocos2d::CCPoint getScaledPoint(float x, float y)
{
	return cocos2d::ccpMult(ccp(x, y), SCALE_FACTOR);
}
#define accountLoginViewTag      (10000)