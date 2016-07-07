#pragma once

//��Ϸ����Ʒid����
enum struct GAME_PRODUCT
{
	BUY_NULL					=-1,						
	BUY_50,						//����50������
	BUY_100,						//����100������
	BUY_300,						//����300������
	BUY_500,						//����500������
	BUY_1000						//����1000������
};

//�����֧��ID
#define ERROR_PAY_ID GAME_PRODUCT::BUY_NULL


// ����֧��ƽ̨
#define C_PAY_UNKNOWN						0
#define C_PAY_GOOGLE_PLAY					1

//ѡ��֧��ƽ̨
#define C_PAY_PLATFORM						C_PAY_GOOGLE_PLAY


#if C_PAY_PLATFORM == C_PAY_GOOGLE_PLAY
#include "SDK/GooglePaySDK.h"
#endif