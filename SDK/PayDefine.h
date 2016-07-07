#pragma once

//游戏内商品id定义
enum struct GAME_PRODUCT
{
	BUY_NULL					=-1,						
	BUY_50,						//购买50阴阳珠
	BUY_100,						//购买100阴阳珠
	BUY_300,						//购买300阴阳珠
	BUY_500,						//购买500阴阳珠
	BUY_1000						//购买1000阴阳珠
};

//错误的支付ID
#define ERROR_PAY_ID GAME_PRODUCT::BUY_NULL


// 定义支付平台
#define C_PAY_UNKNOWN						0
#define C_PAY_GOOGLE_PLAY					1

//选用支付平台
#define C_PAY_PLATFORM						C_PAY_GOOGLE_PLAY


#if C_PAY_PLATFORM == C_PAY_GOOGLE_PLAY
#include "SDK/GooglePaySDK.h"
#endif