#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"

class CSkillInfo: public BaseLayer
{
public:
	CREATE_LAYER(CSkillInfo);
	CSkillInfo();
	~CSkillInfo();

    virtual bool init();
	void bindPos(CCNode* pAimNode);
	void setInfo(int iSkillId);
	void setPara(const char* str);
	void show();
	void hide();
private:
	CLayout *m_ui;
	int m_iBaseDescFontSize;
};