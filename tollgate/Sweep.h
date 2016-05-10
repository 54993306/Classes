
#ifndef _SWEEPLAYER_
#define _SWEEPLAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "common/CursorTextField.h"

class CSweepLayer: public BaseLayer
{
public:
	CREATE_LAYER(CSweepLayer);
	CSweepLayer();
    virtual bool init();
    void onEnter();
   
    void onExit();
	void setPosIndex(int index);
	
protected:
	void onBeginSweep(CCObject* pSender);
	void onSub(CCObject* pSender);
	void onAdd(CCObject* pSender);

	void updateSweepNum();

private:
	CLayout *m_ui;
	int  m_posIndex;                //阵位
	int  m_number;
	CursorTextField *m_numTextField;
};


#endif