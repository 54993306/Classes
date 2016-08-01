#include "GooglePayJni.h"
#include "cocos2d.h"
#include "GooglePaySDK.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#define CLASS_NAME "com/android/vending/billing/GooglePayHelper"					//��Eclips�����ҵ�src�������Ӧ·��


extern "C"
{

	//����RSA��Կ
	JNIEXPORT jstring JNICALL Java_com_android_vending_billing_GooglePayHelper_getPublicRsaKey(JNIEnv *env, jobject thiz)
	{
		return env->NewStringUTF(GooglePaySDK::getInstance()->getPublicRsaKey().c_str());
	}

	//֧���ɹ�����֤����
	void Java_com_android_vending_billing_GooglePayHelper_checkPurchaseByServer(JNIEnv *env, jobject thiz, jstring sProductId, jstring sSignData, jstring sPurchaseInfo)
	{
		const char* strProductID = env->GetStringUTFChars(sProductId, NULL);
		const char* strSignData = env->GetStringUTFChars(sSignData, NULL);
		const char* strPurchaseInfo = env->GetStringUTFChars(sPurchaseInfo, NULL);
		GooglePaySDK::getInstance()->checkPurchase(strProductID, strSignData, strPurchaseInfo);
	}

	//֧��ʧ��
	void Java_com_android_vending_billing_GooglePayHelper_payFail(JNIEnv *env, jobject thiz, jstring sProductId)
	{
		CCLOG("Java_com_android_vending_billing_GooglePayHelper_payFail1");
		const char* strProductID = env->GetStringUTFChars(sProductId, NULL);
		CCLOG("Java_com_android_vending_billing_GooglePayHelper_payFail2");
		GooglePaySDK::getInstance()->payFail(strProductID);
		CCLOG("Java_com_android_vending_billing_GooglePayHelper_payFail3");
	}

	//�Ƿ�Ϊ��������Ʒ
	JNIEXPORT jboolean JNICALL Java_com_android_vending_billing_GooglePayHelper_isConcumeable(JNIEnv *env, jobject thiz, jstring sProductId)
	{
		const char* strProductID = env->GetStringUTFChars(sProductId, NULL);
		return GooglePaySDK::getInstance()->isConcumeable(strProductID);
	}

	void c_handlMsg( int iMsg, const char *str , int iTag1, int iTag2 )
	{
		JniMethodInfo minfo;
		if(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "handleMsg", "(ILjava/lang/String;II)V")) {
			jstring jStr = minfo.env->NewStringUTF(str);
			minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, iMsg, jStr, iTag1, iTag2);
			minfo.env->DeleteLocalRef(jStr);
		}
	}

	//����֧��
	void c_pay( int iMsg, const char *str , int iTag)
	{
		c_handlMsg( iMsg, str, iTag, 0);
	}

	//�������ĵ�����
	void c_concume_purchase( int iMsg, const char *str )
	{
		c_handlMsg( iMsg, str, 0, 0);
	}


	void c_start_consume_trouble_purchaes( int iMsg )
	{
		c_handlMsg( iMsg, "", 0, 0);
	}


}


#endif