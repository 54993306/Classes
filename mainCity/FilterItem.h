
#ifndef __FILTERITEM_LAYER_
#define __FILTERITEM_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"


class CFilterItem: public BaseLayer
{
public:
	CFilterItem();
	~CFilterItem();

	CREATE_LAYER(CFilterItem);
	
    virtual bool init();
    void onEnter();
    void onExit();

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void setFilterType(const vector<int>& typeVec);

private:
	void onCheckFilter(CCObject *pSender, bool bChecked);
	void setFilterType(int type, bool isCheck);
	void requestUpdate();

private:
	CLayout *m_ui;
	vector<int> m_filterType;
	bool m_noEquip;  //未装备
	bool m_yesEquip;
};


#endif