#ifndef __QUICK_PRODUCT__
#define __QUICK_PRODUCT__
#include "AppUI.h"
class CQuickProduct:public CCNode
{
public:
	
	static CQuickProduct* create(CCNode *parent);
	bool init();
	
protected:
	void onUserStone(CCObject* pSender);
	void removeTip(CCNode * node);
private:
	CCNode *m_parent;

};

#endif