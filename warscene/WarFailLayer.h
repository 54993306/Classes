#ifndef _WARFAILLAYER_H_
#define _WARFAILLAYER_H_

#include "scene/layer/LayerManager.h"
#include "google/protobuf/message.h"
#include "protos/protocol.h"
class WarFailLayer :
	public BaseLayer
{
public:
	WarFailLayer();
	~WarFailLayer();
	virtual bool init();
	CREATE_LAYER(WarFailLayer);

	void onEnter();
	void onExit();
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

public:
	static const char WAR_FAIL_RESULT[];

	void afterTextEnd(CCNode *pSender);
	void showDropText(CCNode *pSender);

	void onHintClick(CCObject *pSender);
	void onExitWithMode(CCNode* pSender);

	void onExitClick(CCObject* pSender);
	void onExitWait(CCNode *pSender);

	void onRetryClick(CCObject *pSender);
	void onRetryWait(CCNode *pSender);
	void updateNewStep(const TMessage& tMsg);
private:
	CLayout* m_ui;

};

enum WAR_FAIL_TAG{
	TAG_FAIL_BASE = 50,
	TAG_F_TITLE_LEFT_1 = 100,
	TAG_F_TITLE_LEFT_2,
	TAG_F_TITLE_RIGHT
};
#endif
