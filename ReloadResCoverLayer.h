#pragma once

/******************************************************
*文件名称:	ReloadResCoverLayer.h
*编写日期:	2016-4-13-10:38
*编写作者:	YPF
*功能描述:	锁屏或者切换后台再切回来时，需要reload资源时显示这个界面
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

class ReloadResCoverLayer: public BaseLayer
{
public:
	ReloadResCoverLayer();
	~ReloadResCoverLayer();

	static void CoverWhenReloadRes();

	CREATE_LAYER(ReloadResCoverLayer);

	virtual bool init();
	void onEnter();
	void onExit();
private:
	void callBackForReloadRes();
};