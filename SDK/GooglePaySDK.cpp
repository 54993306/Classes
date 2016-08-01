#include "GooglePaySDK.h"
#include "GooglePayJni.h"
#include "net/CNetClient.h"
#include "scene/layer/LayerManager.h"
#include "vip/VipLayer.h"
#include "model/DataCenter.h"

GooglePaySDK *GooglePaySDK::m_pInstance = nullptr;

GooglePaySDK::GooglePaySDK()
	:m_sProductID(""), m_sSignData(""), m_sPurchaseInfo(""),m_iPayTag(GooglePayTagNull)
{

	CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GooglePaySDK::updateForPurchase), this, 0, kCCRepeatForever, 0, false);
}

GooglePaySDK::~GooglePaySDK()
{
	m_pInstance = nullptr;

	CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget(this);
}

GooglePaySDK * GooglePaySDK::getInstance()
{
	if( m_pInstance == nullptr )
	{
		m_pInstance = new GooglePaySDK;
		m_pInstance->init();
	}
	return m_pInstance;
}

void GooglePaySDK::destroy()
{
	//�������ص�����
	if(m_pInstance != nullptr)
	{
		GetTcpNet->unRegisterAllMsgHandler(m_pInstance);
	}
	CC_SAFE_DELETE(m_pInstance);
	m_pInstance = nullptr;
}


void GooglePaySDK::init()
{
	//�󶨻�б�ص�
	GetTcpNet->registerMsgHandler(ValidateBillingMsg, this, CMsgHandler_selector(GooglePaySDK::recvServerMsg));
}


std::string GooglePaySDK::getPublicRsaKey()
{
	CCLOG("[ info ]--GooglePaySDK::getPublicRsaKey");
	return "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAot8GVJt1OckmnMjZB1ZEnpM5td5cCxEzuJj+oltJnImgCZ6MATC5qZ9npPVk+mgPGG4JGT11nRZXvfMkK9jZvZyQxcnn3EeFASOs9E3wTij1GBd/se3N+KSNm2qoc56Aw8O+K7WZuSlmqE972vgQaD2Vk+F/wBpJX5PnUJR9Y0+ya8qvg5sbqQF06W1AVoPQ5mxmJjOQVUIcR4XU3U5rIQiYQSr+MoYFuHV4Uc2TjiIp92O87dhifT5YAEJCtfgfGMmpzDXyl6fRnxCRATFV+raMbqwOgxmXYAylGBXD7/fceTvJT7Y97gHdokigmE2IqPZNO6v2ugbMfpFgLfoq3QIDAQAB";
}

void GooglePaySDK::pay( int iMsg, GAME_PRODUCT productId, int iTag )
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	c_pay(iMsg, GOOGLE_PRODUCT[(int)productId].sProductID.c_str(), iTag);
#endif
}


void GooglePaySDK::concumePurchase(int iMsg, string sPurchaseInfo )
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	c_concume_purchase(iMsg, sPurchaseInfo.c_str());
#endif
}

void GooglePaySDK::checkPurchase( string sProductID, string sSignData, string sPurchaseInfo )
{
	CCLOG("[ info ]--GooglePaySDK::checkPurchase");
	
	m_sPurchaseInfo = sPurchaseInfo;
	m_sProductID = sProductID;
	m_sSignData = sSignData;

	m_iPayTag = GooglePayTagCheckPurchase;
}


void GooglePaySDK::recvServerMsg( int type, google::protobuf::Message *msg )
{
	CCLOG("[ info ]--GooglePaySDK::recvServerMsg");
	if(type==ValidateBillingMsg)
	{
		ValidateRes *pRes = (ValidateRes*)msg;
		checkPurchaseCallback(
			pRes->money(), 
			pRes->result()
			);
	}
}


void GooglePaySDK::checkPurchaseCallback( int iNum, bool bOk )
{
	CCLOG("[ info ]--GooglePaySDK::checkPurchaseCallback");

	//������֤�ɹ�����ʧ�ܣ������͵���׿�˽���������Ʒ��android�Ҳ�����Ӧ�����򲻴���
	concumePurchase( G_CONCUME, m_sPurchaseInfo );

	//��֤�ɹ�
	if(bOk)
	{
		CCLOG("[ info ]--GooglePaySDK::checkPurchaseCallback-success_check_purchase");

		//��ȡ����
		//TODO
		UserData *pData = DataCenter::sharedData()->getUser()->getUserData();
		int iMoneyCurrent = pData->getRoleMoney();
		int iAll = iMoneyCurrent + iNum;
		pData->setRoleMoney(iAll);
		CCLOG("[ info ]--GooglePaySDK::checkPurchaseCallback-setRoleMoneyAdd-%d", iNum);
		CCLOG("[ info ]--GooglePaySDK::checkPurchaseCallback-setRoleMoney-%d", iAll);

		//����UI�� ���Ƕ��һ��������pay���ŵ�pay������
		//TODO
		CVipLayer *pPayLayer = dynamic_cast<CVipLayer*>(LayerManager::instance()->getLayer("CVipLayer"));
		if(pPayLayer)
		{
			pPayLayer->updateMoney(iAll);
		}
	}
	//��֤���ɹ�
	else
	{
		//������֪����������
		//TODO
		CCLOG("[ info ]--GooglePaySDK::checkPurchaseCallback-fail_check_purchase");
	}
	
}

GAME_PRODUCT GooglePaySDK::getGameProductByGoogleProductId( string sProductID )
{
	int iCount = sizeof(GOOGLE_PRODUCT)/sizeof(GooglePayProduct);
	for(int i=0; i<iCount; i++)
	{
		if(GOOGLE_PRODUCT[i].sProductID.compare(sProductID) == 0)
		{
			return (GAME_PRODUCT)i;
		}
	}

	return ERROR_PAY_ID;
}

void GooglePaySDK::payFail(string sProductID)
{
	CCLOG("[ info ]--GooglePaySDK::payFail");
	//��ʾ֧��ʧ��
	//TODO
}

bool GooglePaySDK::isConcumeable( string sProductID )
{
	return true;
}

void GooglePaySDK::updateForPurchase( float dt )
{
	switch (m_iPayTag)
	{
	case GooglePayTagNull:
		break;
	case GooglePayTagCheckPurchase:
		{
			CCLOG("[ info ]--GooglePaySDK::updateForCheckPurchase--case GooglePayTagCheckPurchase");

			//���������֤����
			//������Ϣ�����������ȴ����
			GAME_PRODUCT iGameProductId = getGameProductByGoogleProductId(m_sProductID);

			//TODO
			//��֤����
			ValidateBilling *pReq = new ValidateBilling; 
			pReq->set_inapp_data_signature(m_sSignData);
			pReq->set_inapp_purchase_data(m_sPurchaseInfo);
			GetTcpNet->sendData(pReq, ValidateBillingMsg);
			delete pReq;

			m_iPayTag = GooglePayTagNull;
		}
		break;
	case GooglePayTagPayFailed:
		{
			//����ʧ��

			m_iPayTag = GooglePayTagNull;

		}break;
	default:
		break;
	}
}

void GooglePaySDK::startConsumeTroublePurchaes()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	c_start_consume_trouble_purchaes(START_CONSUME_TROUBLE);
#endif
}

