
#ifndef _ROLE_HEADIMG_H
#define _ROLE_HEADIMG_H

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"

class CChangeHeadImg: public BaseLayer
{
public:
	CREATE_LAYER(CChangeHeadImg);
	CChangeHeadImg();
	~CChangeHeadImg();

    virtual bool init();
    void onEnter();
    void onExit();

	void onChangHeadImg(CCObject* pSender);
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addGridCell(unsigned int uIdx, CGridViewCell* pCell);
	void onClose(CCObject* pSender);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
private:
	CLayout *m_ui;
	CGridView *m_gridView;
	CLayout *m_cell;
	vector<int> m_headIconList;
};

#endif