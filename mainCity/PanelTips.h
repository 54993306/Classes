#pragma once

/******************************************************
*文件名称:	PanelTips.h
*编写日期:	2016-4-26-15:26
*编写作者:	YPF
*功能描述:	界面内无元素时给予提示
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

class CPanelTips:public BaseLayer
{
public:
	~CPanelTips();

	CPanelTips();

	CREATE_LAYER(CPanelTips);
	virtual bool init();
	void onEnter();
	void onExit();
	void setString(const char* sInfo);
private:
	CLayout *m_ui;

};