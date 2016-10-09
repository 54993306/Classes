#pragma once

#include "cocos2d.h"
#include "PayDefine.h"
#include "protos/protocol.h"


USING_NS_CC;
using namespace std;

struct GooglePayProduct
{
	string		sProductID;			//产品ID
	int			iPrice;					//价格
	int			iNum;					//购买数量
};

//商品定义，跟google后台配置统一
const GooglePayProduct GOOGLE_PRODUCT[] = 
{
	{
		"50_yyy", 50, 50
	},
	{
		"100_yyy", 100, 100
	},
	{
		"300_yyy", 300, 300
	}
	,
	{
		"500_yyy", 500, 500
	}
	,
	{
		"1000_yyy", 1000, 1000
	}
	,
	{
		"3000_yyy", 3000, 3000
	}
};

//0.每次启动游戏，检查有没有没处理的消耗订单，给予补充（服务器会验证订单信息，保证同一订单不会发货两次）
//
//1.c++ 层 获取点击事件 开始执行支付-发送支付请求道android层
//
//2.android 层 调用GooglePlaySDK发送支付请求
//
//3.GooglePlay服务回传支付结果到android层
//
//3-1.产品购买成功-记录订单操作-成功
//
//4.先不消耗，android层回传支付结果到c++层
//
//5.提交支付结果到服务器，验证是否成功，发货
//
//6.服务器下发购买成功还是失败消息-c++更新数据
//
//6-1.c++ 发送发货通知到android层
//
//6-2.android层接收到通知后执行消耗
//
//7.消耗成功，购买流程走完


const	int G_PAY												= 0x1001;				//支付
const	int G_CONCUME									= 0x1002;				//消耗订单
const	int START_CONSUME_TROUBLE			= 0x1003;				//登录后开始消耗问题订单

enum GooglePaySdkTag
{
	GooglePayTagNull = -1,						//默认标记
	GooglePayTagCheckPurchase,				//成功，验证订单
	GooglePayTagPayFailed						//购买失败
};

class GooglePaySDK : public CCObject 
{
public:
	GooglePaySDK();
	~GooglePaySDK();

	static GooglePaySDK *getInstance();
	static void  destroy();

	void updateForPurchase( float dt );
public:

	void init();
	
	//获取RSA公钥
	string getPublicRsaKey();
	
	//请求计费
	void pay(int iMsg, GAME_PRODUCT productId, int iTag);

	//服务器验证订单
	void checkPurchase(string sProductID, string sSignData, string sPurchaseInfo);

	//消耗订单
	void concumePurchase(int iMsg, string sPurchaseInfo);

	//登录后开始消耗问题订单
	void startConsumeTroublePurchaes();

	//支付失败
	void payFail(string sProductID);

	//是否为可消耗商品
	bool isConcumeable(string sProductID);

	void recvServerMsg(int type, google::protobuf::Message *msg);

private:
	
	//处理服务器订单验证回调
	void checkPurchaseCallback(int iNum, bool bOk);

	//根据googlePay的商品ID得出游戏内定义的商品ID
	GAME_PRODUCT getGameProductByGoogleProductId(string sProductID);

private:
	static GooglePaySDK			*m_pInstance;
	string								m_sPurchaseInfo;	//原始订单信息
	string								m_sProductID;		//产品ID
	string								m_sSignData;		//签名数据
	GooglePaySdkTag				m_iPayTag;				//pay_tag
};