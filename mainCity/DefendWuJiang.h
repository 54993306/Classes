#ifndef __DEFEND_WUJIANG__
#define __DEFEND_WUJIANG__
#include "AppUI.h"
#include "scene/layer/LayerManager.h"

#include "CityData.h"
#include "model/CRoleData.h"

class CDefendWuJiang:public BaseLayer
{
public:
	CDefendWuJiang():m_replaceId(0),m_selectIndex(0),m_cityId(0),m_id(0),m_iStartIndex(0){}
	~CDefendWuJiang(){}
	CREATE_LAYER(CDefendWuJiang);
	virtual bool init();
	void onEnter();
	void onExit();
	void onClickButton(CCObject* pSender);
	CCObject* gridViewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void setCityId(int id, int cityId);
	void setReplaceId(int id);
	void onClose(CCObject* pSender);
	void addTableCell( unsigned int uIdx, CGridViewCell * pCell );
	void movementCallBack(CCArmature* pAramture, MovementEventType type, const char* sData);
protected:
	void showHeroList();
	void onSelectHero(CCObject* pSender);
	bool longSelectHero(CCObject* pSender, CCTouch* pTouch);
	CCNode* getItemByIndex(int iIndex);

	void SpineComplete( int trackIndex,int loopCount );
private:
	CLayout *m_ui;
	CGridView *m_grid;
	std::vector<CHero> m_heroList;  //英雄列表
	int m_selectIndex;
	int m_cityId;
	int m_id;
	int m_replaceId;
	int m_iStartIndex;
	CCSprite *m_selectImg;
	vector<int> m_VecMode;
	CCNode* m_pAnimationNode;
};


#endif