#pragma once

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