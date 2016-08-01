#ifndef __FACEBOOKIMG_LAYER_
#define __FACEBOOKIMG_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "activity/HttpLoadImage.h"
#include "chapter.h"

class CFacebookImg: public BaseLayer,public HttpLoadImageDelegate
{
public:
	CREATE_LAYER(CFacebookImg);
	CFacebookImg();
	virtual bool init();
	void onEnter();
	void onExit();
	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void onClose(CCObject* pSender);
	void onHero(CCObject* pSender);
	void setStage(const CStage& stage);
	static int TableViewOffSet;
private:
	void showEffectStart();
	void callBackForShake();
	void showShakeCallBack();
	void showEffectCallBack();

	void onLeft(CCObject* pSender);
	void onRight(CCObject* pSender);
	void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
private:
	CLayout *m_ui;
	CLayout *m_cell;
	CPageView *m_pageView;
	int m_pos;
	int m_iCurrentChapterId;
	CCSprite* m_pBgEffect;
	CImageView *m_selectSpr;
	CStage m_stage;
};


#endif