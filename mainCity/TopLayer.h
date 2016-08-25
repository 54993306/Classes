#ifndef __TOP_LAYER__
#define __TOP_LAYER__

#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"


class CTopLayer: public BaseLayer
{
public:
	CTopLayer():m_iIndexForShow(0){};
	virtual bool init();
	CREATE_LAYER(CTopLayer);

	void onEnter();
	void onExit();

	void updateRoleProperty(const TMessage& tMsg);
	void updateTaskNotice(const TMessage& tMsg);
	void changeLabel(CLabel * label, int coin);

	void show(CCObject* pObj);
	void hide(CCObject* pObj);

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

protected: 
	void hideBtn(CCNode *pSender);
	void hideLabel(CCNode *node);
	void onAddResourceBtn(CCObject* pSender);
	void updateMailNotice(const TMessage& tMsg);
	void onHeadImgBtn(CCObject *pSender);
	void updateActionTime(float dt);
private:
	CLayout *m_ui;
	CCPoint m_btnPos[8];  
	CCPoint m_preMovePos; //地图移动点击坐标
	float  m_moveX[9];

	CLabel *m_coinLabel;
	CLabel *m_moneyLabel;
	CLabel *m_foodLabel;
	int m_iIndexForShow;
};


#endif