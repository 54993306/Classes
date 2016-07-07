
//#include "JNITool.h"
#include "FaceBookSDK.h"
#include "model/DataCenter.h"
#include "common/MultLanguage.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	#include "platform/android/jni/JniHelper.h"
	#include <android/log.h>
	extern "C"
	{
		jobject getActivityObject(void);
		jobject getFaceBookObject(void);
		jobject getMainActivityObject(void);
	}
	#define Java_Class_Name "com/sdk/THFaceBook" 
#define  Java_Main_Name "com/ui/dashisha/DaShiSha" 
#endif


FaceBookSDK *FaceBookSDK::g_Instance;

static SEL_CallFuncO	callback_login=NULL;
static CCObject*		target_login=NULL;

static SEL_CallFuncO	callback_userinfo=NULL;
static CCObject*		target_userinfo=NULL;

static SEL_CallFuncO	callback_friendList=NULL;
static CCObject*		target_friendList=NULL;

FaceBookSDK* FaceBookSDK::sharedInstance()
{
	if(NULL==g_Instance)
	{
		g_Instance=new FaceBookSDK();
	}
	return g_Instance;
}
void FaceBookSDK::purge()
{
	if(g_Instance)
	{
		delete g_Instance;
		g_Instance=NULL;
	}
}
FaceBookSDK::FaceBookSDK()
{

}
FaceBookSDK::~FaceBookSDK()
{

}
//ÉêÇëÊÚÈ¨µÇÂ¼
void FaceBookSDK::openAuthor()
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	if(JniHelper::getMethodInfo(minfo,Java_Main_Name,"openAuthor","()V"))
	{
		minfo.env->CallVoidMethod(getMainActivityObject(),minfo.methodID);
	}
#endif
}
//¹Ø±ÕÊÚÈ¨µÇÂ¼
void FaceBookSDK::closeAuthor()
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	if(JniHelper::getMethodInfo(minfo,Java_Class_Name,"closeAuthor","()V"))
	{
		minfo.env->CallVoidMethod(getFaceBookObject(),minfo.methodID);
	}
#endif
}

void FaceBookSDK::onOpenPickFriends()
{
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();
	string appName = GETLANGSTR(1258);
	CCString *str = CCString::createWithFormat(GETLANGSTR(1260),ToString(data->getRoleID()+10000));
	string appDesc = GETLANGSTR(1259)+ string(str->getCString());
	string imgUrl ="";
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	
	JniMethodInfo minfo;
	if(JniHelper::getMethodInfo(minfo,Java_Main_Name,"onOpenPickFriends","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring stringName = minfo.env->NewStringUTF(appName.c_str());
		jstring stringDesc = minfo.env->NewStringUTF(appDesc.c_str());
		jstring stringImgUrl = minfo.env->NewStringUTF(imgUrl.c_str());
		minfo.env->CallVoidMethod(getMainActivityObject(),minfo.methodID,stringName,stringDesc,stringImgUrl);
	//	minfo.env->DeleteLocalRef(stringName);
	//  ¡¡minfo.env->DeleteLocalRef(stringDesc);
	}
#endif
}

void FaceBookSDK::onShareToFb(const char* desc, const char* imgUrl)
{
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();
	string appName = GETLANGSTR(1258);
	string strdesc = desc;
	string strimg = imgUrl;

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)

	JniMethodInfo minfo;
	if(JniHelper::getMethodInfo(minfo,Java_Main_Name,"onOpenPickFriends","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring stringName = minfo.env->NewStringUTF(appName.c_str());
		jstring stringDesc = minfo.env->NewStringUTF(strdesc.c_str());
		jstring stringImgUrl = minfo.env->NewStringUTF(strimg.c_str());
		minfo.env->CallVoidMethod(getMainActivityObject(),minfo.methodID,stringName,stringDesc,stringImgUrl);
	}
#endif
}

void FaceBookSDK::setCallbackForAuthor(CCObject *target,SEL_CallFuncO func)
{
	target_login=target;
	callback_login=func;
}
void FaceBookSDK::setCallbackForUserInfo(CCObject *target,SEL_CallFuncO func)
{
	target_userinfo=target;
	callback_userinfo=func;
}

void FaceBookSDK::setCallbackForFriendList(CCObject *target,SEL_CallFuncO func)
{
	target_friendList = target;
	callback_friendList = func;
}

void FaceBookSDK::openFriendList()
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	if(JniHelper::getMethodInfo(minfo,Java_Class_Name,"getFriendList","()V"))
	{
		minfo.env->CallVoidMethod(getFaceBookObject(),minfo.methodID);
	}
#endif
}

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
extern "C"
{
	jobject getFaceBookObject(void)
	{
		JniMethodInfo minfo;
		bool isHave = JniHelper::getStaticMethodInfo(minfo,Java_Class_Name,"getJavaObject","()Ljava/lang/Object;");
		jobject jobj=NULL;
		if (isHave)
		{
			jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		}
		CCAssert(jobj!=NULL,"");
		return jobj;
	}
	
	jobject getMainActivityObject(void)
	{
		JniMethodInfo minfo;
		bool isHave = JniHelper::getStaticMethodInfo(minfo,Java_Main_Name,"getObject","()Ljava/lang/Object;");
		jobject jobj=NULL;
		if (isHave)
		{
			jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		}
		CCAssert(jobj!=NULL,"");
		return jobj;
	}

	JNIEXPORT void JNICALL Java_com_sdk_THFaceBook_loginCallback(JNIEnv* env, jobject thiz, jstring resultText) 
	{
		const char* psText = env->GetStringUTFChars(resultText, NULL);
		if(target_login && callback_login)
		{
			CCDirector::sharedDirector()->getRunningScene()->runAction(CCCallFuncO::create(target_login,callback_login,CCString::create(psText)));
			//(target_login->*callback_login)(CCString::create(psText));
		}
		env->ReleaseStringUTFChars(resultText, psText);
	}
	 //com_sdk_THFaceBook
	JNIEXPORT void JNICALL Java_com_sdk_THFaceBook_userinfoCallback(JNIEnv* env, jobject thiz, jstring resultText) 
	{
		const char* psText = env->GetStringUTFChars(resultText, NULL);
		if(target_userinfo && callback_userinfo)
		{
			CCDirector::sharedDirector()->getRunningScene()->runAction(CCCallFuncO::create(target_userinfo,callback_userinfo,CCString::create(psText)));
			//(target_userinfo->*callback_userinfo)(CCString::create(psText));
		}
		env->ReleaseStringUTFChars(resultText, psText);
	}

	JNIEXPORT void JNICALL Java_com_sdk_THFaceBook_friendListCallback(JNIEnv* env, jobject thiz, jstring resultText) 
	{
		const char* psText = env->GetStringUTFChars(resultText, NULL);
		if(target_friendList && callback_friendList)
		{
			CCDirector::sharedDirector()->getRunningScene()->runAction(CCCallFuncO::create(target_userinfo,callback_userinfo,CCString::create(psText)));
			//(target_userinfo->*callback_userinfo)(CCString::create(psText));
		}
		env->ReleaseStringUTFChars(resultText, psText);
	}
}
#endif
