#include "CUpdateLayer.h"
#include "jni/CJniHelper.h"
#include "protos/gate_protocol.pb.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "UserDefaultData.h"
#include "common/CommonFunction.h"
#include "ApplicationDefine.h"

CUpdateLayer::CUpdateLayer()
	:m_pAssetManager(nullptr)
	,m_VersionJson(nullptr)
	,m_pathToSave("")
	,m_sVersion("")
	,m_iIndexVersion(0)
{

}

CUpdateLayer::~CUpdateLayer()
{
	CC_SAFE_DELETE(m_pAssetManager);
	CC_SAFE_DELETE(m_VersionJson);
}

bool CUpdateLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("CUpdateLayerMask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		lay->setVisible(false);

		m_ui = LoadComponent("updateLayer.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);
		this->setVisible(true);

		//初始化下载路径
		initDownloadDir();

		m_VersionJson = new CVersionJson;

		return true;
	}
	return false;

}

void CUpdateLayer::setVersion( const char* sVersion )
{
	m_sVersion = sVersion;
}

void CUpdateLayer::onEnter()
{
	BaseLayer::onEnter();

	//CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/9049.plist");
	//AnimationManager::sharedAction()->ParseAnimation("9049");

	//进度条
	m_progress = (CProgressBar*)m_ui->findWidgetById("progress");
	m_progress->setMaxValue(100);

	////僵尸跳
	//CCAnimation *pZombieEffect = AnimationManager::sharedAction()->getAnimation("9049");
	//pZombieEffect->setDelayPerUnit(0.05f);
	//CCAnimate* pAnimate = CCAnimate::create(pZombieEffect);
	//CCSprite* pZombieSprite = CCSprite::create();
	//pZombieSprite->setScale(0.9f);
	//pZombieSprite->setAnchorPoint(ccp(0.5f, 0.0f));
	//pZombieSprite->setPositionY(m_progress->getPositionY()+5);
	//m_ui->addChild(pZombieSprite);
	//pZombieSprite->runAction(CCRepeatForever::create(pAnimate));
	//m_pZombieSprite = pZombieSprite;


	//进度条数字
	m_pLabel = (CLabel*)m_ui->findWidgetById("progress_text");

	//额外信息
	m_pInfoLabel = (CLabel*)m_ui->findWidgetById("text"); 

	//请求版本列表信息
	requestVersionInfo();
}

void CUpdateLayer::onExit()
{
	BaseLayer::onExit();
}

void CUpdateLayer::initDownloadDir()
{
	CCLOG("initDownloadDir");
	m_pathToSave = VERSION_UPDATE_PATH;
	CCLOG("initDownloadDir end");
}

void CUpdateLayer::onError( AssetsManagerErrorCode errorCode )
{
	switch (errorCode)
	{
	case cocos2d::extension::kCreateFile:
		CCLOG("kCreateFile");
		m_pLabel->setString("Create File Error !");
		break;
	case cocos2d::extension::kNetwork:
		CCLOG("kNetwork");
		m_pLabel->setString("Net Error !");
		break;
	case cocos2d::extension::kNoNewVersion:
		CCLOG("kNoNewVersion");
		m_pLabel->setString("Version Error !");
		break;
	case cocos2d::extension::kUncompress:
		CCLOG("kUncompress");
		m_pLabel->setString("Uncompress Error !");
		break;
	default:
		break;
	}
}

void CUpdateLayer::onProgress( int percent )
{
	if (percent < 0)
		return;
	char progress[20];
	snprintf(progress, 20, "%d%%", percent);
	m_pLabel->setString(progress);
	m_progress->setValue(percent);
	//僵尸位置
	//m_pZombieSprite->setPositionX(m_progress->getPositionX()+m_progress->getContentSize().width*percent/100-5);
	CCLOG("%f", percent);
}
//下载成功
void CUpdateLayer::onSuccess()
{
	CCLOG("download success");
	CCLOG("______________________%s", m_pathToSave.c_str());

	//成功更新了一个版本后（阶段性存储），更新下标，继续检测更新版本，没有就重启游戏或者
	//存储当前版本路径（已经成功更新的就存一下）
	const CVersionData& version = m_versionNeedData[m_iIndexVersion];
	CCUserDefault::sharedUserDefault()->setStringForKey(VERSION, version.sVersion);
	CCUserDefault::sharedUserDefault()->flush();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CJniHelper::getInstance()->saveResourcesPath(m_pathToSave.c_str());
	CJniHelper::getInstance()->saveVersion(version.sVersion.c_str());
#endif

	m_iIndexVersion++;									//下标更新
	if(m_iIndexVersion < m_versionNeedData.size())
	{
		downloadVersion();								//下载
	}else{
		m_pInfoLabel->setString("Update Success!");

		runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFunc::create(this, callfunc_selector(CUpdateLayer::callBackForSuccess))));
	}
}

//判断当前版本是否需要更新
bool CUpdateLayer::checkUpdate( const char* sVersion )
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	return false;
#endif

#if !VERSION_UPDATE
	return false;
#endif
	//默认版本号
	std::string sDefalutVersion = CJniHelper::getInstance()->getVersionName();
	std::string sCurrentVersion = CCUserDefault::sharedUserDefault()->getStringForKey(VERSION, sDefalutVersion);
	if(sCurrentVersion.compare(sVersion)<0)
	{
		return true;
	}
	return false;
}

//请求版本的json文件
void CUpdateLayer::requestVersionInfo()
{
	std::string sUpdateServer = CCUserDefault::sharedUserDefault()->getStringForKey(SERVER_FOR_UPDATE);
	CCString* pUrl = CCString::createWithFormat("%s%s?r=%ld", sUpdateServer.c_str(), VERSION_FILE, getCurrentTime());
	CCHttpRequest* request = new CCHttpRequest();
	request->setUrl(pUrl->getCString());
	request->setRequestType(CCHttpRequest::kHttpGet);
	request->setResponseCallback(this, httpresponse_selector(CUpdateLayer::requestVersionInfoCallBack));
	CCHttpClient::getInstance()->send(request);
	request->release();
}
//解析json
void CUpdateLayer::requestVersionInfoCallBack( cocos2d::extension::CCHttpClient *sender, cocos2d::extension::CCHttpResponse *response )
{
	if (!response)
	{
		requestVersionInfo();
		return;
	}
		
	if (!response->isSucceed())
	{
		ShowPopTextTip(GETLANGSTR(287));
		requestVersionInfo();
		return;
	}

	//请求到的数据
	vector<char> *buffer = response->getResponseData();  

	// 数据转存本地
	string path = m_pathToSave+"/" +VERSION_FILE;
	string buff(buffer->begin(), buffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, buffer->size(),  fp);
	fclose(fp);

	//加载json数据
	m_VersionJson->reloadFile(path);

	//开始更新
	startDownload();
}
//根据json内容获取版本间需要更新的版本内容
void CUpdateLayer::startDownload()
{
	//获取需要更新的所有版本
	std::string sDefalutVersion = CJniHelper::getInstance()->getVersionName();
	std::string sCurrentVersion = CCUserDefault::sharedUserDefault()->getStringForKey(VERSION, sDefalutVersion);
	m_versionNeedData = m_VersionJson->getUpdateList(sCurrentVersion, m_sVersion);						//判断间隔了几个版本信息

	//开始依次更新版本
	m_iIndexVersion = 0;

	downloadVersion();
}
//下载版本内容
void CUpdateLayer::downloadVersion()
{
	const CVersionData& version = m_versionNeedData[m_iIndexVersion];		//获取当前加载版本

	if(m_pAssetManager)					//保证Asstmanage是最新的
	{
		CC_SAFE_DELETE(m_pAssetManager);
	}
	std::string sUpdateServer = CCUserDefault::sharedUserDefault()->getStringForKey(SERVER_FOR_UPDATE);
	m_pAssetManager = new AssetsManager(CCString::createWithFormat("%s%s?r=%ld", sUpdateServer.c_str(), version.sFile.c_str(), getCurrentTime())->getCString() , m_pathToSave.c_str(), "http://www.baidu.com/version.php");
	m_pAssetManager->setZipFileName(CCString::createWithFormat("%s", version.sFile.c_str())->getCString());
	m_pAssetManager->setDelegate(this);
	//m_pAssetManager->setConnectionTimeout(3);			//3S内没有回调则报错调用 onError
	m_pAssetManager->update();

	m_pInfoLabel->setString(CCString::createWithFormat("Update To Version %s", version.sVersion.c_str())->getCString());
}

void CUpdateLayer::callBackForSuccess()
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LogoScene));
#else
	CJniHelper::getInstance()->restartGame();
	CCDirector::sharedDirector()->end();
#endif
}
