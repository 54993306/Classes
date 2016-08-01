#pragma once

#include "scene/layer/LayerManager.h"
#include "google/protobuf/message.h"
#include "protos/protocol.h"
#include "bag/bagData.h"
#include "activity/HttpLoadImage.h"

using namespace protos;

class PvpEndLayer : public BaseLayer, public HttpLoadImageDelegate
{
public:
	PvpEndLayer();
	~PvpEndLayer();
	virtual bool init();
	CREATE_LAYER(PvpEndLayer);

	void onEnter();
	void onExit();

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer);

	void setBattleResult( bool bResult );

public:
	void processBattleFinish(int type, google::protobuf::Message *msg);

private:
	void onExitClick(CCObject *pSender);
	void onBackClick(CCNode* ob);
	void shareFb(CCObject *pSender);

private:
	CLayout* m_ui;
	int m_rank;
	bool m_bIsWin;
};
