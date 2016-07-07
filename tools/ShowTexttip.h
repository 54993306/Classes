// ***************************************************************
//  FileName: ShowTexttip.h
//  Version:  1.0   
//  Date: 
//  Author: linxiancheng
//  Copyright (C) 2014 U1
//  --------------------------------------------------------------
//  Description:  文字提示
//  -------------------------------------------------------------
//  History: 
//  -------------------------------------------------------------
// ***************************************************************

#pragma once

#ifndef  _SHOWTEXTTIP_
#define  _SHOWTEXTTIP_
 // ! _SHOWTEXTTIP_

#include "commonDef.h"

#include "cocos2d.h"
#include "common/color.h"

#include "AppUI.h"
#include "bag/bagData.h"
#include "scene/CPopTip.h"

using namespace cocos2d;

enum
{
	MTB_TEXT = 0,                           //文本警告
	MTB_WARNING,                            //警告  失败
	MTB_OK,                                 //警告  成功
	ROUNDNUM,								//回合数
};

void ShowTexttip(const char * content,								//提示的内容
				 const ccColor3B & color,							//颜色
				 int type = MTB_TEXT,								//类型，默认为文本
				 const CCPoint & startPoint = ccp(480, 320),		//位置
				 float startFlyTime = 0.0f,							//停留时间
				 float flyingTime = 2.0f,							//运动时间
				 float offsetx = 0.0f,								//飞行偏移量,相对于当前点的位置
				 float offsety = 200.0f,
				 const char * fontName = FONT_NAME,					//字体名称
				 int fontSize = 20);								//字体大小

void ShowTexttip(CCNode * content,									//显示的提示节点
				 const ccColor3B & color,							
				 int type = MTB_OK,
				 const CCPoint & startPoint = ccp(480, 320),
				 float startFlyTime = 0.0f,
				 float flyingTime = 2.0f,
				 float offsetx = 0.0f,								//飞行偏移量,相对于当前点的位置
				 float offsety = 200.0f);

void showScrollTip(const char* str, CCNode * _parent = NULL);                        

CPopTip* ShowPopTextTip(const char* str,const ccColor3B& color =ccWHITE, bool isAutoClose = true);
void ShowConfirmTextTip(const char* str, CCObject *pSender, SEL_ClickHandler pHandler, const ccColor3B& color =ccWHITE);
void showItemTip(const CItem *item);
#endif