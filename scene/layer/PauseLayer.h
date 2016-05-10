#ifndef _PAUSE_LAYER_H_
#include "AppUI.h"
class PauseLayer : public CCNode,public CCTargetedTouchDelegate
{
public:
	PauseLayer();
	virtual ~PauseLayer();
	CREATE_FUNC(PauseLayer);
	virtual bool init();
public:
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
protected:
	CLayout* m_ui;
};
#endif // !_PAUSE_LAYER_H_
