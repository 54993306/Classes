#pragma once

/******************************************************
*文件名称:	ItemInfo.h
*编写日期:	2016-6-14-11:49
*编写作者:	YPF
*功能描述:	物品信息的悬浮框显示
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
#include "model/DataCenter.h"

class CItemInfo: public BaseLayer
{
public:
	CREATE_LAYER(CItemInfo);
	CItemInfo();
	~CItemInfo();

    virtual bool init();
	void bindPos(CCNode* pAimNode);
	void setInfo(CItem *info);
	void setInfo(CPrize *pPrize);
	void changeForSign(CItem *info);
	void changeForSign(CPrize *pPrize);

	void show();
	void hide();
private:
	CLayout *m_ui;
	int m_iBaseDescFontSize;
};