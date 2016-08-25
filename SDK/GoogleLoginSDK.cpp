/******************************************************
*文件名称:	GoogleLoginSDK.cpp
*编写日期:	2016-8-19-19:31
*编写作者:	YPF
*功能描述:	Google登录
*******************************************************/

#include "GoogleLoginSDK.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>

#define CLASS_NAME "com/sdk/GoogleLoginHelper"					//在Eclips里面找到src里面的相应路径
	
#endif


GoogleLoginSDK *GoogleLoginSDK::m_pInstance;

static SEL_CallFuncO	callback_userinfo=NULL;
static CCObject*		target_userinfo=NULL;


GoogleLoginSDK* GoogleLoginSDK::getInstance()
{
	if(NULL==m_pInstance)
	{
		m_pInstance=new GoogleLoginSDK();
	}
	return m_pInstance;
}
void GoogleLoginSDK::purge()
{
	CC_SAFE_DELETE(m_pInstance);
	m_pInstance=nullptr;
}

GoogleLoginSDK::GoogleLoginSDK()
{

}
GoogleLoginSDK::~GoogleLoginSDK()
{

}

//申请授权登录
void GoogleLoginSDK::openAuthor()
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	if(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "signIn", "()V")) {
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	}
#endif
}


void GoogleLoginSDK::setCallbackForUserInfo(CCObject *target,SEL_CallFuncO func)
{
	target_userinfo=target;
	callback_userinfo=func;
}


#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
extern "C"
{
	JNIEXPORT void JNICALL Java_com_sdk_GoogleLoginHelper_googleLoginCallback(JNIEnv* env, jobject thiz, jstring resultText) 
	{
		CCLOG("Java_com_sdk_GoogleLoginHelper_userinfoCallback_1");
		const char* psText = env->GetStringUTFChars(resultText, NULL);
		if(target_userinfo && callback_userinfo)
		{
			CCLOG("Java_com_sdk_GoogleLoginHelper_userinfoCallback_2");
			CCDirector::sharedDirector()->getRunningScene()->runAction(CCCallFuncO::create(target_userinfo, callback_userinfo, CCString::create(psText)));
		}
		env->ReleaseStringUTFChars(resultText, psText);
	}
}
#endif
