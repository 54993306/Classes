#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "Friend.h"
#include "common/CursorTextField.h"
#include "activity/HttpLoadImage.h"

class CPanelTips;

class CFriendLayer:public BaseLayer, public HttpLoadImageDelegate
{
public:
	CFriendLayer():m_selectType(0),m_selectFriend(nullptr),m_isSearch(false),m_pPanelTips(nullptr){}
	~CFriendLayer();
	CREATE_LAYER(CFriendLayer);
	virtual bool init();
	void onEnter();
	void onDeleteFriend(CCObject* pSender);
	void onExit();

	void updateHero(const TMessage& tMsg);
	void friendListRes(int type, google::protobuf::Message *msg);

protected:
	void onClose(CCObject* pSender);
	void onAddFriend(CCObject* pSender);
	CCObject* friendDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	void showSelectRadioImg(int selIndex);
	void onSwitchBtn(CCObject *pSender, bool bChecked);
	void addFriendCell(CLayout* layCell, unsigned int uIdx, CTableViewCell * pCell);
	void addAcceptCell(CLayout* layCell, unsigned int uIdx, CTableViewCell * pCell);
	void processNetMsg(int type, google::protobuf::Message *msg);
	void removeCell(int type);

	void onAcceptFriend(CCObject* pSender);
	CCObject* recomFriendDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void onComfirmDeleteFriend(CCObject* pSender);
	void onSearch(CCObject* pSender);
	void onUpdateSearch(float dt);
	void onInviteFriend(CCObject* pSender);
	void onFaceBookUserInfo(CCObject *object);
	void accountBinRes(int type, google::protobuf::Message *msg);
	void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer);
	std::string getStoragePath(const char* downDir, const char* fileName);

private:
	void showFriendTip(bool bShow, const char* sInfo);

private:
	CLayout *m_ui;
	int  m_selectHero;
	CLayout *m_cell;
	CLayout *m_cell1;
	CLayout *m_cell2;

	CTableView *m_friendTable;
	CTableView *m_recommendTable;
	CTableView *m_acceptTable;

	CLayout *m_friendView;
	CLayout *m_recommendView;
	CLayout *m_acceptView;
	int  m_selectType;
	typedef map<int,vector<CFriend>> FriendMap;

	FriendMap m_friendListMap;
	CFriend *m_selectFriend;
	CursorTextField *m_textFieldName;
	bool m_isSearch;
	string m_textStr;
	CPanelTips* m_pPanelTips;
};