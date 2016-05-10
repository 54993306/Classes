#ifndef __REWARD_LAYER_
#define __REWARD_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
struct  CBounty{
	int	id;	//悬赏任务ID
	time_t time;   //结束时间
	int   openLevel;
};

struct CBountyStage{
	int id;		//关卡ID
	int hardMode;	//难易情度(1,2,3...)
	int openLevel;	//开放等级
    bool isOpen;	//是否可进入
	int action;	//进入需要的体力值
};


class CRewardLayer: public BaseLayer
{
public:
	CREATE_LAYER(CRewardLayer);
	CRewardLayer();
	virtual bool init();
	void onEnter();
	void onExit();
	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void onStage(CCObject* pSender);
	void onClose(CCObject* pSender);
	void onHero(CCObject* pSender);
	static int TableViewOffSet;
private:
	void showEffectStart();
	void callBackForShake();
	void showShakeCallBack();
	void showEffectCallBack();
	void onSelect(CCObject* pSender);

	void updateOverTime(CBounty * bounty);
	void onLeft(CCObject* pSender);
	void onRight(CCObject* pSender);
private:
	CLayout *m_ui;
	CLayout *m_cell;
	CPageView *m_pageView;
	vector<CBounty> m_bountyList;
	int m_pos;
	int m_iCurrentChapterId;
	CCSprite* m_pBgEffect;
	CImageView *m_selectSpr;
};


#endif