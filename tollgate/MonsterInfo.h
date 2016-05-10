
#ifndef _MONSTERINFO_H
#define _MONSTERINFO_H

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

#endif