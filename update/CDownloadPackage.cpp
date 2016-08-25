#include "CDownloadPackage.h"
#include "jni/CJniHelper.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "UserDefaultData.h"
#include "CUpdateData.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "ApplicationDefine.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <dirent.h>
#include <sys/stat.h>
#endif

CDownloadPackage::CDownloadPackage()
	:m_pAssetManager(nullptr)
	,m_pathToSave("")
	,m_pDownloadPacakgeDelegate(nullptr)
	,m_iPackageIndex(0)
	,m_iPackageMax(0)
	,m_iCurrentIPixelndex(0)
	,m_iMaxPixel(0)
	,m_iPercent(0)
	,m_iErrorTimes(0)
	,m_iStoryIndex(0)
	,m_iStoryMax(9)
{

}

CDownloadPackage::~CDownloadPackage()
{
	CC_SAFE_DELETE(m_pAssetManager);
}

bool CDownloadPackage::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("CDownloadPackageMask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		lay->setVisible(false);

		m_ui = LoadComponent("downloadPackage.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);
		this->setVisible(true);

		//初始化下载路径
		initDownloadDir();

		m_iPackageIndex = CCUserDefault::sharedUserDefault()->getIntegerForKey(PACKAGE_DOWNLOAD_INDEX, 0);

		m_VersionJson = new CPackageVersionJson;

		updateForChangePic(0);
		schedule(schedule_selector(CDownloadPackage::updateForChangePic), 3);
		scheduleUpdate();

		return true;
	}
	return false;

}


void CDownloadPackage::onEnter()
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

void CDownloadPackage::onExit()
{
	BaseLayer::onExit();

	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}


void CDownloadPackage::initDownloadDir()
{
	CCLOG("initDownloadDir");
	m_pathToSave = SUB_PACKAGE_PATH;
	CCLOG("Path: %s", m_pathToSave.c_str());
	CCLOG("initDownloadDir end");
}

//判断当前版本是否需要更新
bool CDownloadPackage::checkIsNeedPackage( )
{
	bool bPackage = CCUserDefault::sharedUserDefault()->getBoolForKey(PACKAGE_DOWNLOAD_OPEN, true);
	if(bPackage)
	{
		int iPackageIndex = CCUserDefault::sharedUserDefault()->getIntegerForKey(PACKAGE_DOWNLOAD_INDEX, 0);
		int iPackageNeedMax = CCUserDefault::sharedUserDefault()->getIntegerForKey(PACKAGE_DOWNLOAD_MAX, 0);
		//包没有下载够，或者还没开始下载
		if(iPackageIndex<iPackageNeedMax || iPackageIndex==0 || iPackageNeedMax==0)
		{
			return true;
		}
	}
	return false;
}


void CDownloadPackage::downLoadPackage()
{
	requestVersionInfo();
}

void CDownloadPackage::requestVersionInfo()
{
	std::string sUpdateServer = CCUserDefault::sharedUserDefault()->getStringForKey(SERVER_FOR_UPDATE);
	CCString* pUrl = CCString::createWithFormat("%s%s?r=%ld", sUpdateServer.c_str(), PACKAGE_VERSION_FILE, getCurrentTime());
	CCHttpRequest* request = new CCHttpRequest();
	request->setUrl(pUrl->getCString());
	request->setRequestType(CCHttpRequest::kHttpGet);
	request->setResponseCallback(this, httpresponse_selector(CDownloadPackage::requestVersionInfoCallBack));
	CCHttpClient::getInstance()->send(request);
	request->release();
}

void CDownloadPackage::requestVersionInfoCallBack( cocos2d::extension::CCHttpClient *sender, cocos2d::extension::CCHttpResponse *response )
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
	string path = m_pathToSave+"/" +PACKAGE_VERSION_FILE;
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


void CDownloadPackage::startDownload()
{
	//获取需要更新的目标版本
	std::string sApkVersion = CJniHelper::getInstance()->getVersionName();
	m_versionNeedData = m_VersionJson->getAimPackageVersion(sApkVersion);

	//如果没有拿到版本信息,重新拿几次，几次后还是拿不到，显示version-error, 2s钟后重启游戏
	if( m_versionNeedData.sVersion.compare("")==0 || m_versionNeedData.iZipNum==0 )
	{
		if (m_iErrorTimes > 0)
		{
			m_iErrorTimes--;
			//重新请求数据
			requestVersionInfo();
		}
		else
		{
			m_pInfoLabel->setString("Package version error , the game will restart !");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			showHelpTips("Download error, the game will restart !", true);
#endif
		}
		return;
	}

	//记录最大zip数量
	m_iPackageMax = m_versionNeedData.iZipNum;
	CCUserDefault::sharedUserDefault()->setIntegerForKey(PACKAGE_DOWNLOAD_MAX, m_iPackageMax);
	CCUserDefault::sharedUserDefault()->flush();

	//开始依次更新包
	downloadVersionByIndex();

	//更新Label显示
	updateTextShow();
}

void CDownloadPackage::downloadVersionByIndex()
{
	if(m_pAssetManager)					//保证Asstmanage是最新的
	{
		CC_SAFE_DELETE(m_pAssetManager);
	}

	m_iPackageIndex++;

	onProgress(0);

	CCString* strPackageName = CCString::createWithFormat(PACKAGE_NAME, m_iPackageIndex);

	std::string sUpdateServer = CCUserDefault::sharedUserDefault()->getStringForKey(SERVER_FOR_UPDATE);
	m_pAssetManager = new AssetsManager(CCString::createWithFormat("%s%s/%s?r=%ld", sUpdateServer.c_str(), m_versionNeedData.sVersion.c_str(), strPackageName->getCString(), getCurrentTime())->getCString() , m_pathToSave.c_str());
	m_pAssetManager->setZipFileName(CCString::createWithFormat("%s", strPackageName->getCString())->getCString());
	m_pAssetManager->setDelegate(this);
	//m_pAssetManager->setConnectionTimeout(3);			//3S内没有回调则报错调用 onError
	m_pAssetManager->update();
}


void CDownloadPackage::onError( AssetsManagerErrorCode errorCode )
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
	showHelpTips("Download error, the game will restart !", true);
#endif
}

void CDownloadPackage::onProgress( int percent )
{
	//小于0大于100,数据异常
	if ( percent < 0 || percent > 100)
	{
		return;
	}

	CCLOG("%d", percent);	

	m_iPercent = percent;
	//显示百分比
	char progress[20];
	snprintf(progress, 20, "%d%%", m_iPercent);
	if(m_pLabel != nullptr)
	{
		m_pLabel->setString(progress);
	}

	//转换为像素百分比
	m_iCurrentIPixelndex = (m_iMaxPixel*m_iPercent)/100;
}

//下载成功
void CDownloadPackage::onSuccess()
{
	CCLOG("download success");

	//存储包下标
	CCUserDefault::sharedUserDefault()->setIntegerForKey(PACKAGE_DOWNLOAD_INDEX, m_iPackageIndex);
	CCUserDefault::sharedUserDefault()->flush();

	//是否已经下载完所有包
	if(m_iPackageIndex<m_iPackageMax)
	{
		downloadVersionByIndex();
		updateTextShow();
	}
	else
	{
		//成功下载了一个包， 重启游戏
		m_pInfoLabel->setString("Update Success!");
		runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFunc::create(this, callfunc_selector(CDownloadPackage::callBackForSuccess))));
	}
}

void CDownloadPackage::callBackForSuccess()
{
	if(m_pDownloadPacakgeDelegate)
	{
		m_pDownloadPacakgeDelegate->downloadPackageEnd(true);
		m_pDownloadPacakgeDelegate = nullptr;
	}
}

void CDownloadPackage::updateTextShow()
{
	m_pInfoLabel->setString(CCString::createWithFormat("resource updating - [%d/%d]", m_iPackageIndex, m_iPackageMax)->getCString());
}

void CDownloadPackage::updateForChangePic( float dt )
{
	CCSprite* pSprite = dynamic_cast<CCSprite*>(m_ui->findWidgetById("bg"));
	changeStoryPicture(pSprite);
}


void CDownloadPackage::update( float dt )
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

void CDownloadPackage::callbackForRestart()
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LogoScene));
#else
	CJniHelper::getInstance()->restartGame();
	CCDirector::sharedDirector()->end();
#endif
}

void CDownloadPackage::showHelpTips( const char *str , bool bShowOkOnly )
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
	pop->setButtonLisener(this, ccw_click_selector(CDownloadPackage::showHelpTipsClick));
	CCDirector::sharedDirector()->getRunningScene()->addChild(pop, 4562);
}

void CDownloadPackage::showHelpTipsClick( CCObject *pSender )
{
	//点击取消重启，点击确定下载新的apk
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==PopTipConfirm)
	{
		//确定
		runAction(CCCallFunc::create(this, callfunc_selector(CDownloadPackage::callbackForRestart)));
	}
}

void CDownloadPackage::changeStoryPicture( CCSprite* pSprite )
{
	if(pSprite==nullptr)
	{
		return;
	}

	CCTexture2D* pTexture = pSprite->getTexture();
	CCTexture2D* pNewTexture = nullptr;
	
	pNewTexture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("warScene/LoadImage/story/%d.jpg", m_iStoryIndex)->getCString());
	m_iStoryIndex++;

	if(m_iStoryIndex >= m_iStoryMax)
	{
		m_iStoryIndex = 0;
		unschedule(schedule_selector(CDownloadPackage::updateForChangePic));
		return;
	}

	//没有找到图片，返回不操作
	if(pNewTexture==nullptr)
	{
		return;
	}

	pSprite->setTexture(pNewTexture);

	pSprite->setZOrder(-2);

	CCSprite* pTranstionSprite = CCSprite::createWithTexture(pTexture);
	pTranstionSprite->setPosition(pSprite->getPosition());
	pTranstionSprite->setAnchorPoint(pSprite->getAnchorPoint());
	pTranstionSprite->setScale(pSprite->getScale());
	pSprite->getParent()->addChild(pTranstionSprite, pSprite->getZOrder()+1);
	pTranstionSprite->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(0.6f), CCRemoveSelf::create()));
}

void CDownloadPackage::cleanDownloadPackageInfo()
{
	CCUserDefault::sharedUserDefault()->setIntegerForKey(PACKAGE_DOWNLOAD_INDEX, 0);
	CCUserDefault::sharedUserDefault()->setIntegerForKey(PACKAGE_DOWNLOAD_MAX, 0);
	CCUserDefault::sharedUserDefault()->flush();
}

