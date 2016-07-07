#pragma once

#include "cocos2d.h"
#include "PayDefine.h"
#include "protos/protocol.h"


USING_NS_CC;
using namespace std;

struct GooglePayProduct
{
	string		sProductID;			//��ƷID
	float			fPrice;					//�۸�
	int			iNum;					//��������
};

//��Ʒ���壬��google��̨����ͳһ
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
};

//0.ÿ��������Ϸ�������û��û��������Ķ��������貹�䣨����������֤������Ϣ����֤ͬһ�������ᷢ�����Σ�
//
//1.c++ �� ��ȡ����¼� ��ʼִ��֧��-����֧�������android��
//
//2.android �� ����GooglePlaySDK����֧������
//
//3.GooglePlay����ش�֧�������android��
//
//3-1.��Ʒ����ɹ�-��¼��������-�ɹ�
//
//4.�Ȳ����ģ�android��ش�֧�������c++��
//
//5.�ύ֧�����������������֤�Ƿ�ɹ�������
//
//6.�������·�����ɹ�����ʧ����Ϣ-c++��������
//
//6-1.c++ ���ͷ���֪ͨ��android��
//
//6-2.android����յ�֪ͨ��ִ������
//
//7.���ĳɹ���������������


const	int G_PAY					= 0x1001;				//֧��
const	int G_CONCUME		= 0x1002;				//���Ķ���

class GooglePaySDK : public CCObject 
{
public:
	GooglePaySDK();
	~GooglePaySDK();

	static GooglePaySDK *getInstance();
	static void  destroy();

public:

	void init();
	
	//��ȡRSA��Կ
	string getPublicRsaKey();
	
	//����Ʒ�
	void pay(int iMsg, GAME_PRODUCT productId, int iTag);

	//��������֤����
	void checkPurchase(string sProductID, string sSignData, string sPurchaseInfo);

	//���Ķ���
	void concumePurchase(int iMsg, string sPurchaseInfo); 

	//֧��ʧ��
	void payFail(string sProductID);

	//�Ƿ�Ϊ��������Ʒ
	bool isConcumeable(string sProductID);

	void recvServerMsg(int type, google::protobuf::Message *msg);

private:
	
	//���������������֤�ص�
	void checkPurchaseCallback(int iNum, bool bOk);

	//����googlePay����ƷID�ó���Ϸ�ڶ������ƷID
	GAME_PRODUCT getGameProductByGoogleProductId(string sProductID);

private:
	static GooglePaySDK			*m_pInstance;
	string								m_sPurchaseInfo;	//ԭʼ������Ϣ
};