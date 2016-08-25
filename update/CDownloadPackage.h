#pragma once

/******************************************************
*文件名称:	CDownloadPackage.h
*编写日期:	2016-6-14-11:49
*编写作者:	YPF
*功能描述:	拆分资源包下载
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "AssetsManager/AssetsManager.h"
#include "CDownloadPackageDelegate.h"
#include "CPackageVersionJson.h"

#define PACKAGE_NAME "%d.zip"

class CDownloadPackage : public BaseLayer, public AssetsManagerDelegateProtocol
{
public:
	
	CREATE_LAYER(CDownloadPackage);

	CDownloadPackage();
	~CDownloadPackage();

    virtual bool init();
    void onEnter();
    void onExit();

	//请求所有版本的文件信息
	void requestVersionInfo();
	//数据回调
	void requestVersionInfoCallBack(cocos2d::extension::CCHttpClient *sender, cocos2d::extension::CCHttpResponse *response);

	void onError(AssetsManagerErrorCode errorCode);	//错误信息
	void onProgress(int percent);	//更新下载进度
	void onSuccess();	//下载成功

	static bool checkIsNeedPackage();

	void downLoadPackage();

	void startDownload();

	void callBackForSuccess();

	CC_SYNTHESIZE(DownloadPackageDelegate*, m_pDownloadPacakgeDelegate, DownloadPacakgeDelegate);

	void updateForChangePic(float dt);

	void update(float dt);

	void callbackForRestart();

	//更新中断，弹窗口提示重启还是下载新的apk
	void  showHelpTips( const char *str , bool bShowOkOnly);
	void showHelpTipsClick( CCObject *pSender );

	//清理存档
	static void cleanDownloadPackageInfo();

private:
	void initDownloadDir();
	void downloadVersionByIndex();
	void updateTextShow();

	//随机换图片
	void changeStoryPicture( CCSprite* pSprite );

private:
	CProgressBar*								m_progress;
	//CCSprite*									m_pZombieSprite;
	CLabel*											m_pLabel;
	CLabel*											m_pInfoLabel;
	std::string										m_pathToSave;
	CLayout*										m_ui;
	AssetsManager*							m_pAssetManager;
	int												m_iPackageIndex;
	int												m_iPackageMax;
	CPackageVersionJson*					m_VersionJson;
	CPackageVersionData					m_versionNeedData;
	int												m_iCurrentIPixelndex;
	int												m_iMaxPixel;
	int												m_iPercent;
	int												m_iErrorTimes;
	int												m_iStoryIndex;			//故事标签
	int												m_iStoryMax;			//故事数量
};