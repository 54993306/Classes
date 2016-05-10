#pragma once

#include "scene/layer/LayerManager.h"
#include "google/protobuf/message.h"
#include "protos/protocol.h"
#include "bag/bagData.h"

using namespace protos;

class WorldBossEndLayer : public BaseLayer
{
public:
	WorldBossEndLayer();
	~WorldBossEndLayer();
	virtual bool init();
	CREATE_LAYER(WorldBossEndLayer);

	void onEnter();
	void onExit();

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

public:
	void processBattleFinish(int type, google::protobuf::Message *msg);

private:
	void onExitClick(CCObject *pSender);
	void onBackClick(CCNode* ob);
	
private:
	CLayout* m_ui;
};
