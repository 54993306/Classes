#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "activity/HttpLoadImage.h"

//请求规则奖励
void inline AskForPvpRecord()
{
	GetTcpNet->sendDataType(PvpRecordResMsg);
}

class CPvpRecord: public BaseLayer, public  HttpLoadImageDelegate
{
public:
	CREATE_LAYER(CPvpRecord);
	CPvpRecord();
	~CPvpRecord();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);	
	void onToggle(CCObject* pSender);
	void setOptionType(int type);

	void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer);

protected:
	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void onSelect(CCObject* pSender);

	void updateUI();

	void updateTeamInfo(const RecordData& recordData, int iTag, CLayout *pParent);

	void onReven(CCObject *pSender);

private:
	CLayout								*m_ui;
	CTableView							*m_tableView;
	CLayout								*m_cell;
	CLayout								*m_heroLay;
	CCSize									m_tableSize;
	PvpRecordRes						m_recordRes;
	std::string								m_sRoleName;
};