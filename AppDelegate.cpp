#include "AppDelegate.h"
#include "net/CNetClient.h"
#include "tools/commonDef.h"
#include "model/DataCenter.h"
#include "tools/ToolDefine.h"
#include "SimpleAudioEngine.h"
#include "scene/SceneDefine.h"
#include "protos/protocol.pb.h"
#include "common/MultLanguage.h"
#include "protos/auth_protocol.pb.h" 
#include "common/ShaderDataHelper.h"
#include "common/CGameSound.h"
#include "model/CWholeBodyShowData.h"
#include "jni/CJniHelper.h"
#include "update/CUpdateLayer.h"
#include "CResEncrypt.h"
#include "C2DXShareSDK/C2DXShareSDK.h"
#include "UserDefaultData.h"
#include "reward/WBossLayer.h"
#include "ApplicationDefine.h"

USING_NS_CC;
using namespace CocosDenshion;
using namespace cn::sharesdk;

AppDelegate::AppDelegate()
{

}

AppDelegate::~AppDelegate() 
{
	CResEncrypt::getInstance()->destroy();
	CSceneManager::sharedSceneManager()->release();
	AnimationManager::sharedAction()->release();
	DataCenter::sharedData()->release();
	FileUtils::sharedFileUtils()->release();
	VirtualScreen::Instance()->release(); 
	LayerManager::instance()->release();
	SimpleAudioEngine::end();
	google::protobuf::ShutdownProtobufLibrary();
	ShaderDataMgr->destroy();
	GameSound->destroy();
	CWholeBodyShowData::destroy();
	CJniHelper::destroy();
}

bool AppDelegate::applicationDidFinishLaunching() 
{
    // initialize director
	//初始化ShareSDK
//	C2DXShareSDK::open(CCString::create("efe5bc28978c"), false);

	//初始化社交平台信息
//	this -> initPlatformConfig(); 

	CCDirector* pDirector = CCDirector::sharedDirector();
	pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
	pDirector->setAnimationInterval(1.0 / 50);

#if DISPLAY_INFO
	pDirector->setDisplayStats(true);
#else
	pDirector->setDisplayStats(false);
#endif

#if ENCRYPT_IMAGE
	//设置图片加密开关
	CResEncrypt::getInstance()->setIsImageEncrypt(true);
#endif
	
	//注册所有场景
	REGISTE_SCENE_CLASS(LogoScene);
	REGISTE_SCENE_CLASS(LoadScene);
	REGISTE_SCENE_CLASS(LoginScene);
	REGISTE_SCENE_CLASS(WarScene);
	REGISTE_SCENE_CLASS(LoadWar);
	REGISTE_SCENE_CLASS(SelectHeroScene);
	REGISTE_POPUPBOX_CLASS(WaitLayer);
	REGISTE_SCENE_CLASS(CMainScene);

	//设置语言文件类型
	CMultLanguage::getInstance()->setLangType((int)APP_LANGUAGE);
	if(RES_PATH_WITH_LANG[(int)APP_LANGUAGE].compare("")!=0)
	{
		vector<string> pathVec = CCFileUtils::sharedFileUtils()->getSearchPaths();
		pathVec.insert(pathVec.begin(), RES_PATH_WITH_LANG[(int)APP_LANGUAGE].c_str());
		CCFileUtils::sharedFileUtils()->setSearchPaths(pathVec);
	}

	//添加分包更新目录到搜索路径
	AddPathToSearchPath(SUB_PACKAGE_PATH);

	//添加版本更新目录到搜索路径
	AddPathToSearchPath(VERSION_UPDATE_PATH);

	//读取语言配置文件
	CMultLanguage::getInstance()->parseFromJson("language.json");

	//初始化声音管理
	GameSound->init();
	//初始化shader管理
	ShaderDataMgr->loadShaderFile();
	//读取人物立绘配置
	CWholeBodyShowData::getInstance()->reloadFile();

	//关闭拆分包下载
#if !DOWNLOAD_EXTRA_PACKAGE
	CCUserDefault::sharedUserDefault()->setBoolForKey(PACKAGE_DOWNLOAD_OPEN, false);
	CCUserDefault::sharedUserDefault()->flush();
#endif
	
	CCUserDefault::sharedUserDefault()->setBoolForKey(ACCESS_UI_OPEN, true);
	CCUserDefault::sharedUserDefault()->flush();

	CSceneManager::sharedSceneManager()->runScene(GETSCENE(LogoScene));

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

	GameSound->enterBackground();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

	//如果是在游戏场景，默认显示暂停界面
//	if (CSceneManager::sharedSceneManager()->getRunningScene()->getClassName()=="WarScene")
	{
		//
	}

	//如果在游戏里, 世界BOSS在读秒倒计时，重新请求一次数据
	if(LayerManager::instance()->getLayer("CWorldBoss"))
	{
		//AskForWorldBoss();
	}

	GameSound->enterForeground();
}


void AppDelegate::initPlatformConfig()
{
	//Facebook
	CCDictionary *fbConfigDict = CCDictionary::create();
	fbConfigDict -> setObject(CCString::create("802924199830394"), "api_key"); // api_key
	fbConfigDict -> setObject(CCString::create("9a262839fabeb02f646604b6d6cb0e77"), "app_secret");
	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeFacebook, fbConfigDict);

	//新浪微博
// 	CCDictionary *sinaConfigDict = CCDictionary::create();
// 	sinaConfigDict -> setObject(CCString::create("568898243"), "app_key");
// 	sinaConfigDict -> setObject(CCString::create("38a4f8204cc784f81f9f0daaf31e02e3"), "app_secret");
// 	sinaConfigDict -> setObject(CCString::create("http://www.sharesdk.cn"), "redirect_uri");
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeSinaWeibo, sinaConfigDict);
// 
// 	//腾讯微博
// 	CCDictionary *tcConfigDict = CCDictionary::create();
// 	tcConfigDict -> setObject(CCString::create("801307650"), "app_key");
// 	tcConfigDict -> setObject(CCString::create("ae36f4ee3946e1cbb98d6965b0b2ff5c"), "app_secret");
// 	tcConfigDict -> setObject(CCString::create("http://www.sharesdk.cn"), "redirect_uri");
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeTencentWeibo, tcConfigDict);
// 
// 	//短信
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeSMS, NULL);
// 
// 	//QQ空间
// 	CCDictionary *qzConfigDict = CCDictionary::create();
// 	qzConfigDict -> setObject(CCString::create("100371282"), "app_id");
// 	qzConfigDict -> setObject(CCString::create("aed9b0303e3ed1e27bae87c33761161d"), "app_key");
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeQZone, qzConfigDict);
// 
// 	//微信
// 	CCDictionary *wcConfigDict = CCDictionary::create();
// 	wcConfigDict -> setObject(CCString::create("wx4868b35061f87885"), "app_id");
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeWeixiSession, wcConfigDict);
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeWeixiTimeline, wcConfigDict);
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeWeixiFav, wcConfigDict);
// 
// 	//QQ
// 	CCDictionary *qqConfigDict = CCDictionary::create();
// 	qqConfigDict -> setObject(CCString::create("100371282"), "app_id");
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeQQ, qqConfigDict);
// 
// 	
// 	//Twitter
// 	CCDictionary *twConfigDict = CCDictionary::create();
// 	twConfigDict -> setObject(CCString::create("mnTGqtXk0TYMXYTN7qUxg"), "consumer_key");
// 	twConfigDict -> setObject(CCString::create("ROkFqr8c3m1HXqS3rm3TJ0WkAJuwBOSaWhPbZ9Ojuc"), "consumer_secret");
// 	twConfigDict -> setObject(CCString::create("http://www.sharesdk.cn"), "redirect_uri");
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeTwitter, twConfigDict);
// 
// 	//Google+
// 	CCDictionary *gpConfigDict = CCDictionary::create();
// 	gpConfigDict -> setObject(CCString::create("232554794995.apps.googleusercontent.com"), "client_id");
// 	gpConfigDict -> setObject(CCString::create("PEdFgtrMw97aCvf0joQj7EMk"), "client_secret");
// 	gpConfigDict -> setObject(CCString::create("http://localhost"), "redirect_uri");
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeGooglePlus, gpConfigDict);

// 	//邮件
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeMail, NULL);
// 
// 	//打印
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeAirPrint, NULL);
// 
// 	//拷贝
// 	C2DXShareSDK::setPlatformConfig(C2DXPlatTypeCopy, NULL);
}
