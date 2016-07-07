#pragma once

/******************************************************
*文件名称:	MonsterInfo.h
*编写日期:	2016-6-14-11:45
*编写作者:	YPF
*功能描述:	英雄或敌人的信息悬浮框显示
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
#include "model/DataCenter.h"
#include "common/CursorTextField.h"

#include "model/RoleNameGM.h"


class CMonsterInfo: public BaseLayer
{
public:
	CREATE_LAYER(CMonsterInfo);
	CMonsterInfo();
	~CMonsterInfo();

    virtual bool init();
	void bindPos(CCNode* pAimNode);
	void setMonster(CMonster *monster);
	void setHero(CHero* pHero);
	void changeForSign(CHero* pHero);

	void show();
	void hide();
private:
	CLayout *m_ui;
	int m_iBaseDescFontSize;
};