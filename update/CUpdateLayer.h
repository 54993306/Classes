#pragma once

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
private:
	void initDownloadDir();
	void startDownload();
	void downloadVersion();
	
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
};

inline void AddPathToSearchPath(std::string& path)
{
	vector<string> searchPaths = CCFileUtils::sharedFileUtils()->getSearchPaths();
	vector<string>::iterator iter = searchPaths.begin();
	searchPaths.insert(iter, path);
	CCFileUtils::sharedFileUtils()->setSearchPaths(searchPaths);

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