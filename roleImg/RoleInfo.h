
#ifndef _ROLE_INFO_H
#define _ROLE_INFO_H

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
#include "model/DataCenter.h"

class CRoleInfo: public BaseLayer
{
public:
	CREATE_LAYER(CRoleInfo);
	CRoleInfo();
	~CRoleInfo();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();

	void setRoleInfoVal(UserData *data);

	void recoverTime(UserData * data, CLabel * acttime);
	

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
protected:
	void onChangeHeadImg(CCObject* pSender);
	void onChangName(CCObject* pSender);
	void updateActionTime(float dt);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void processError(int errCode);
	void updateRoleProperty();
	void onSet(CCObject* pSender);
	void hide(CCObject* pObj);
	void show(CCObject* pObj);
	CCSprite* maskedSprite(CCSprite *textureSprite);
private:
	CLayout *m_ui;
	UserData *m_data;
	
};

#endif