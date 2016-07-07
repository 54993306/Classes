
#ifndef __RANK_LAYER_
#define __RANK_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "RankData.h"
#include "activity/HttpLoadImage.h"

class CRankLayer: public BaseLayer,public HttpLoadImageDelegate
{
public:
	CREATE_LAYER(CRankLayer);
	CRankLayer();
	~CRankLayer();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);	
	void onToggle(CCObject* pSender);
	void setOptionType(int type);
protected:
	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);

	void addBossRank(CLayout* pCell, CRankData * data);

	void ProcessMsg(int type, google::protobuf::Message *msg);
	void updateBuyInfo(const TMessage& tMsg);
	void onSwitchBtn(CCObject *pSender, bool bChecked);
	void onSelect(CCObject* pSender);
	void addCombatRank(CLayout* pCell, CRankData * data);
	void addMyRank(CRankData *data);
	void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer);
	void addPvpRank(CLayout* pCell, CRankData * data);
private:
	CLayout *m_ui;
	CTableView *m_tableView;
	CLayout *m_cell;
	CLayout *m_cell1;
	CLayout *m_cell2;
	CLayout *m_heroLay;
	CCSize m_tableSize;
	bool m_bArchive;
	int m_optionType;
	map<int,CRankResponse> m_rankMap;
};

#endif