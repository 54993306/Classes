#ifndef __GUIDE_LAYER_
#define __GUIDE_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "GuideData.h"

#define ClickNodeShadowTag 121

class CGuideLayer:public BaseLayer
{
public:
	CGuideLayer();
	CREATE_LAYER(CGuideLayer);
	virtual bool init();
	void onEnter();
	void onExit();	
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
	void setTipMoveToPosition(CCNode *node, GuideTask task, const CCPoint& pos, CCRect rect = CCRectZero);	
	void addStencil(GuideTask &task, CCRect &rect, const CCPoint& pos);
	void setTipPosition(const CCPoint& pos);
	void runDelayStep();	
	void addStoryGuide(const char* text1, const char* text2, int tag=0);
	void addOpenNewFunction(CCNode *node, GuideTask task, const CCPoint& pos, CCRect rect = CCRectZero);
	void rollAnimationCall(CCNode *node);
	void update(float dt);
	CCNode* getClickBoundNode();
protected:
	void onClose(CCObject* pSender);
	void recoverAction(CCNode *node);
	void delayNextStep(float dt);
	void recoverFingerAction(CCNode *node);
	void runSelectAnim(CCNode *node);
	void callNextGuide(CCNode *node);
	CCSprite *m_selectSpr;
	CCSprite *m_finger;
	CCNode *m_stencil;
	CCRect m_rect;
	CCNode *m_clickNode;
	int m_type;
	CLayout *m_ui;
	CLayout *m_openLay;
	CCPoint m_destPos;
	CCClippingNode* m_pClipNode;
	CCArmature* m_Armature;
};



#endif // !__GOLDMINE_LAYER_
