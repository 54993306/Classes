#pragma once

/******************************************************
*文件名称:	CUpdateLayer.h
*编写日期:	2016-6-14-11:51
*编写作者:	YPF
*功能描述:	游戏版本更新
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "AssetsManager/AssetsManager.h"
#include "CUpdateData.h"
#include "CVersionJson.h"
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <dirent.h>
#include <sys/stat.h>
#endif

class CUpdateLayer : public BaseLayer, public AssetsManagerDelegateProtocol
{
public:
	
	CREATE_LAYER(CUpdateLayer);

	CUpdateLayer();
	~CUpdateLayer();

    virtual bool init();
    void onEnter();
    void onExit();

	void onError(AssetsManagerErrorCode errorCode);	//错误信息
	void onProgress(int percent);	//更新下载进度
	void onSuccess();	//下载成功

	static bool checkUpdate(const char* sVersion);

	void setVersion(const char* sVersion);

	//请求所有版本的文件信息
	void requestVersionInfo();
	//数据回调
	void requestVersionInfoCallBack(cocos2d::extension::CCHttpClient *sender, cocos2d::extension::CCHttpResponse *response);

	void callBackForSuccess();

public:
	void initDownloadDir();
	void startDownload();
	void downloadVersion();
	void updateForChangePicture(float dt);
	void update(float dt);

	//更新中断，弹窗口提示重启还是下载新的apk
	void  showHelpTips( const char *str , bool bShowOkOnly);
	void showHelpTipsClick( CCObject *pSender );

	//检查是否低于一个强制更新版本（从最大版本向下找）( 注意: 比较的是apk写入版本 )
	bool isThereAForceApkVersion( std::string &strApkVersion );

	//随机换图片
	void changeRandPicture( CCSprite* pSprite );

private:
	CProgressBar*							m_progress;
	//CCSprite*									m_pZombieSprite;
	CLabel*										m_pLabel;
	CLabel*										m_pInfoLabel;
	std::string									m_pathToSave;
	std::string									m_sVersion;
	CLayout*									m_ui;
	AssetsManager*						m_pAssetManager;
	CVersionJson*							m_VersionJson;
	vector<CVersionData>			m_versionNeedData;
	int												m_iIndexVersion;
	int												m_iCurrentIPixelndex;
	int												m_iMaxPixel;
	int												m_iPercent;
	int												m_iErrorTimes;
};


inline void CheckPathDir(std::string& path)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	DIR *pDir = NULL;
	pDir = opendir(path.c_str());
	if (!pDir)
	{
		mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	}
#else
	if ((GetFileAttributesA(path.c_str())) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectoryA(path.c_str(), 0);
	}
#endif
}

inline void AddPathToSearchPath(std::string& path)
{
	vector<string> searchPaths = CCFileUtils::sharedFileUtils()->getSearchPaths();
	vector<string>::iterator iter = searchPaths.begin();
	searchPaths.insert(iter, path);
	CCFileUtils::sharedFileUtils()->setSearchPaths(searchPaths);

	CheckPathDir(path);
}
