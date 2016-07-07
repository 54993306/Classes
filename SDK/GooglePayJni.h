#pragma once

extern "C"
{
	//对接的消息接收
	void c_handlMsg( int iMsg, const char *str , int iTag1, int iTag2 );

	//发送支付请求到android层
	//1.str-商品ID 2.标记tag
	void c_pay(int iMsg, const char *str, int iTag);

	//请求消耗掉订单
	void c_concume_purchase(int iMsg, const char *str);
}