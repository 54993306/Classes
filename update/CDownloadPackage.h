#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "AssetsManager/AssetsManager.h"
#include "CDownloadPackageDelegate.h"

#define PACKAGE_NAME "%d.zip"
#define PACKAGE_COUNT 10 

class CDownloadPackage : public BaseLayer, public AssetsManagerDelegateProtocol
{
public:
	
	CREATE_LAYER(CDownloadPackage);

	CDownloadPackage();
	~CDownloadPackage();

    virtual bool init();
    void onEnter();
    void onExit();

	void onError(AssetsManagerErrorCode errorCode);	//错误信息
	void onProgress(int percent);	//更新下载进度
	void onSuccess();	//下载成功

	static bool checkIsNeedPackage();

	void downLoadPackage();

	void callBackForSuccess();

	CC_SYNTHESIZE(DownloadPackageDelegate*, m_pDownloadPacakgeDelegate, DownloadPacakgeDelegate);
private:
	void initDownloadDir();
	void downloadVersionByIndex();
	void updateTextShow();
private:
	CProgressBar*							m_progress;
	//CCSprite*									m_pZombieSprite;
	CLabel*										m_pLabel;
	CLabel*										m_pInfoLabel;
	std::string									m_pathToSave;
	std::string									m_sVersion;
	CLayout*									m_ui;
	AssetsManager*						m_pAssetManager;
	int												m_iPackageIndex;
};