#ifndef __FRIEND_LAYER_
#define __FRIEND_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "friend/Friend.h"
#include "common/CursorTextField.h"
#include "army/union.h"
class CSelectFirend:public BaseLayer
{
public:
	CSelectFirend():m_selectType(0),m_selectFriend(nullptr),m_stagId(0){}
	~CSelectFirend();
	CREATE_LAYER(CSelectFirend);
	virtual bool init();
	void onEnter();
	void onExit();
	void setStagId(int stageId);
	void friendListRes(int type, google::protobuf::Message *msg);
protected:
	void onClose(CCObject* pSender);
	void onAddFriend(CCObject* pSender);
	CCObject* friendDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	void processNetMsg(int type, google::protobuf::Message *msg);
	int getCombatLevel(int combat);
private:
	CLayout *m_ui;
	int  m_selectHero;
	CLayout *m_cell;
	CTableView *m_friendTable;

	int  m_selectType;
	vector<CFriend> m_friendList;
	CFriend *m_selectFriend;
	vector<CUnion> m_unionList;
	int m_stagId;
};



#endif // !__GOLDMINE_LAYER_
