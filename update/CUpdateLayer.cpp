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
	,m_iCurrentIPixelndex(0)
	,m_iMaxPixel(0)
	,m_iPercent(0)
	,m_iErrorTimes(5)
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

		schedule(schedule_selector(CUpdateLayer::updateForChangePicture), 30);
		scheduleUpdate();

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
	m_iMaxPixel = m_progress->getContentSize().width;
	m_progress->setMinValue(0);
	m_progress->setMaxValue(m_iMaxPixel);

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
}

void CUpdateLayer::onExit()
{
	BaseLayer::onExit();

	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
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

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	showHelpTips(GETLANGSTR(2024), false);
#endif
}

void CUpdateLayer::onProgress( int percent )
{
	if ( percent < 0 || percent>100 )
	{
		return;
	}
	CCLOG("%d", percent);	

	m_iPercent = percent;
	//显示百分比
	char progress[20];
	snprintf(progress, 20, "%d%%", m_iPercent);
	if(m_pLabel!=nullptr)
	{
		m_pLabel->setString(progress);
	}

	//转换为像素百分比
	m_iCurrentIPixelndex = (m_iMaxPixel*m_iPercent)/100;
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

	//如果没有拿到版本信息,重新拿几次，几次后还是拿不到，显示version-error, 2s钟后重启游戏
	if( m_versionNeedData.size() <= 0 )
	{
		if (m_iErrorTimes > 0)
		{
			m_iErrorTimes--;
			//重新请求数据
			requestVersionInfo();
		}
		else
		{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			showHelpTips(GETLANGSTR(2024), false);
#endif
		}
		return;
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	if(CJniHelper::getInstance()->getVersionName().compare("1.0.2")<=0)
	{
		//跳转下载
		showHelpTips(GETLANGSTR(2025), true);
		return;
	}

	//判断是否有一个强制更新版本
	if( isThereAForceApkVersion(sDefalutVersion) )
	{
		showHelpTips(GETLANGSTR(2025), true);
		return;
	}
#endif


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

	onProgress(0);

	std::string sUpdateServer = CCUserDefault::sharedUserDefault()->getStringForKey(SERVER_FOR_UPDATE);
	m_pAssetManager = new AssetsManager(CCString::createWithFormat("%s%s?r=%ld", sUpdateServer.c_str(), version.sFile.c_str(), getCurrentTime())->getCString() , m_pathToSave.c_str());
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

void CUpdateLayer::updateForChangePicture( float dt )
{
	CCSprite* pSprite = dynamic_cast<CCSprite*>(m_ui->findWidgetById("bg"));
	changeRandPicture(pSprite);
}

void CUpdateLayer::update( float dt )
{
	if(m_iCurrentIPixelndex==0 || m_iCurrentIPixelndex==m_iMaxPixel)
	{
		m_progress->setValue(m_iCurrentIPixelndex);
		return;
	}

	int iValue = m_progress->getValue();
	if(iValue<m_iCurrentIPixelndex)
	{
		int iGap = (m_iCurrentIPixelndex-iValue)/30;//确保到达目标值的时间控制在30帧
		iGap +=1;
		iValue += iGap;
		if(iValue<=m_iMaxPixel)
		{
			m_progress->setValue(iValue);
		}
	}

	//僵尸位置
	//m_pZombieSprite->setPositionX(m_progress->getPositionX()+m_progress->getContentSize().width*percent/100-5);
}

void CUpdateLayer::showHelpTips( const char *str, bool bShowOkOnly )
{
	if( CCDirector::sharedDirector()->getRunningScene()->getChildByTag(4562) != nullptr )
	{
		return;
	}
	CPopTip *pop = CPopTip::create();
	LayerManager::instance()->pop();
	pop->setVisible(true);
	pop->addContentTip(str);
	pop->setTouchEnabled(true);
	pop->setTouchPriority(-100);
	if(bShowOkOnly)
	{
		pop->showConfirmOnly();
	}
	pop->setButtonLisener(this, ccw_click_selector(CUpdateLayer::showHelpTipsClick));
	CCDirector::sharedDirector()->getRunningScene()->addChild(pop, 4562);
}

void CUpdateLayer::showHelpTipsClick( CCObject *pSender )
{
	//点击取消重启，点击确定下载新的apk
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()!=PopTipConfirm)
	{
		//确定
		runAction(CCCallFunc::create(this, callfunc_selector(CUpdateLayer::callBackForSuccess)));
	}                                    
	else
	{
		//默认版本
		std::string sDefalutVersion = CJniHelper::getInstance()->getVersionName();
		if(sDefalutVersion.compare("1.0.2")>0)
		{
			//跳转下载
			CJniHelper::getInstance()->downloadNewVersionFromGooglePlay();
		}
		else
		{
			//确定
			runAction(CCCallFunc::create(this, callfunc_selector(CUpdateLayer::callBackForSuccess)));
		}
	}
}

bool CUpdateLayer::isThereAForceApkVersion( std::string &strApkVersion )
{
	 return m_VersionJson->isThereAForceApkUpdateVersion(strApkVersion, m_sVersion);
}

void CUpdateLayer::changeRandPicture( CCSprite* pSprite )
{
	if(pSprite==nullptr)
	{
		return;
	}
	CCTexture2D* pTexture = pSprite->getTexture();
	CCTexture2D* pNewTexture = nullptr;

	int iRand = ((int)(CCRANDOM_0_1()*100))%9+1;//首次随机
	do
	{
		pNewTexture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("warScene/LoadImage/%d.png", iRand)->getCString());
		iRand ++;
		iRand = iRand%9+1;
		//第二次做按顺序移动操作，保证两次肯定找到新图片
	}
	while(pNewTexture==pTexture || pNewTexture==nullptr);

	pSprite->setTexture(pNewTexture);

	pSprite->setZOrder(-2);

	CCSprite* pTranstionSprite = CCSprite::createWithTexture(pTexture);
	pTranstionSprite->setPosition(pSprite->getPosition());
	pTranstionSprite->setAnchorPoint(pSprite->getAnchorPoint());
	pTranstionSprite->setScale(pSprite->getScale());
	pSprite->getParent()->addChild(pTranstionSprite, pSprite->getZOrder()+1);
	pTranstionSprite->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(0.6f), CCRemoveSelf::create()));
}
