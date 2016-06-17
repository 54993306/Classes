#pragma once

#include "scene/layer/LayerManager.h"
 
class RandomBusineseMan : public BaseLayer
{
public:
	RandomBusineseMan();
	~RandomBusineseMan();
	virtual bool init();
	CREATE_LAYER(RandomBusineseMan);

	void onEnter();
	void onExit();

	void show();

	void touchHide(CCObject* pSender);

	void scheduleCallBack(float dt);

	void callBackHead(CCNode* pSender);

private:
	CLayout* m_ui;
	float m_fTime;
	bool m_bIsTouchEnabled;
};