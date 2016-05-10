#ifndef _ACTIVITY_CONTROL_
#define _ACTIVITY_CONTROL_

#include "AppUI.h"
#include "protos/protocol.h"

class CActivityControl : public CCNode
{
public:
	static CActivityControl* getInstance();

	CActivityControl();
	~CActivityControl();

	void askForActList(int iType);	//1.活动2.新品
	void askForBuyGift(int iId);			//活动Id

	void recvActList(int type, google::protobuf::Message *msg);
	void recvBuyGiftMsg(int type, google::protobuf::Message *msg);
	
private:
	static CActivityControl* m_instance;
};

#endif