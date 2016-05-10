
#ifndef _ROLE_UPDATE_H
#define _ROLE_UPDATE_H

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
#include "model/DataCenter.h"

class CRoleUpdate: public BaseLayer
{
public:
	CREATE_LAYER(CRoleUpdate);
	CRoleUpdate();
	~CRoleUpdate();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();

	void setRoleInfoVal(UserData *data);

    void updateRole(UserData * data, const Role *role);	

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);

protected:
	void onChangName(CCObject* pSender);
	void updateActionTime(float dt);
	void runAnimation();

	void runMoveAnim(CLayout * node, float delay);
	void showRoleData(UserData * data, const Role * role);
private:
	CLayout *m_ui;
	UserData *m_data;
	
};

#endif