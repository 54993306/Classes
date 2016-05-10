#include "CDownloadPackage.h"
#include "jni/CJniHelper.h"
#include "tools/ShowTexttip.h"
#include "scene/AnimationManager.h"
#include "UserDefaultData.h"
#include "CUpdateData.h"
#include "scene/loadWar.h"
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

		m_ui = LoadComponent("updateLayer.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);
		this->setVisible(true);

		//初始化下载路径
		initDownloadDir();

		m_iPackageIndex = CCUserDefault::sharedUserDefault()->getIntegerForKey(PACKAGE_DOWNLOAD_INDEX, 0);

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
}

void CDownloadPackage::onExit()
{
	BaseLayer::onExit();
}


void CDownloadPackage::initDownloadDir()
{
	CCLOG("initDownloadDir");
	m_pathToSave = SUB_PACKAGE_PATH;
	CCLOG("Path: %s", m_pathToSave.c_str());
	CCLOG("initDownloadDir end");
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
}

void CDownloadPackage::onProgress( int percent )
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
void CDownloadPackage::onSuccess()
{
	CCLOG("download success");

	//存储包下标
	CCUserDefault::sharedUserDefault()->setIntegerForKey(PACKAGE_DOWNLOAD_INDEX, m_iPackageIndex);
	CCUserDefault::sharedUserDefault()->flush();

	//是否已经下载完所有包
	if(m_iPackageIndex<PACKAGE_COUNT)
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

//判断当前版本是否需要更新
bool CDownloadPackage::checkIsNeedPackage( )
{
	bool bPackage = CCUserDefault::sharedUserDefault()->getBoolForKey(PACKAGE_DOWNLOAD_OPEN, true);
	if(bPackage)
	{
		int iPackageIndex = CCUserDefault::sharedUserDefault()->getIntegerForKey(PACKAGE_DOWNLOAD_INDEX, 0);
		if(iPackageIndex<PACKAGE_COUNT)
		{
			return true;
		}
	}
	return false;
}

void CDownloadPackage::downLoadPackage()
{
	downloadVersionByIndex();
	updateTextShow();
}


void CDownloadPackage::downloadVersionByIndex()
{
	if(m_pAssetManager)					//保证Asstmanage是最新的
	{
		CC_SAFE_DELETE(m_pAssetManager);
	}

	m_iPackageIndex++;

	CCString* strPackageName = CCString::createWithFormat(PACKAGE_NAME, m_iPackageIndex);

	std::string sUpdateServer = CCUserDefault::sharedUserDefault()->getStringForKey(SERVER_FOR_UPDATE);
	m_pAssetManager = new AssetsManager(CCString::createWithFormat("%s%s?r=%ld", sUpdateServer.c_str(), strPackageName->getCString(), getCurrentTime())->getCString() , m_pathToSave.c_str(), "http://www.baidu.com/version.php");
	m_pAssetManager->setZipFileName(CCString::createWithFormat("%s", strPackageName->getCString())->getCString());
	m_pAssetManager->setDelegate(this);
	//m_pAssetManager->setConnectionTimeout(3);			//3S内没有回调则报错调用 onError
	m_pAssetManager->update();
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
	m_pInfoLabel->setString(CCString::createWithFormat("resource updating - [%d/%d]", m_iPackageIndex, PACKAGE_COUNT)->getCString());
}
