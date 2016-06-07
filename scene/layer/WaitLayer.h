#ifndef __WAITLAYER_
#define __WAITLAYER_

#include "AppUI.h"
namespace BattleSpace{
	class EffectObject;
}
class WaitLayer: public CPopupBox
{
public:
	DEFINE_POPUPBOX_CLASS(WaitLayer);
	virtual void onOpen(const PARAM param);
	virtual bool init();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent); 
 	void onEnter();
	CREATE_FUNC(WaitLayer);
protected:
	void showNode(CCNode *pSender);
	void hideNode(CCNode *pSender);
	void showSelf(float delt);
private:
	CLayout *m_ui;
	BattleSpace::EffectObject *m_loadEffect;
};
#endif // !__WAITLAYER_#
