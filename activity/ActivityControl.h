#pragma once

/******************************************************
*文件名称:	ActivityControl.h
*编写日期:	2016-6-14-11:35
*编写作者:	YPF
*功能描述:	activity网络控制
*******************************************************/

#include "AppUI.h"
#include "protos/protocol.h"

class CActivityControl : public CCNode
{
public:
	static CActivityControl* getInstance();

	CActivityControl();
	~CActivityControl();

	void askForActList(int iType);	//1.活动2.新品
	void askForBuyGift(int iId, int iExId=0);			//活动Id  收集兑换Id

	void recvActList(int type, google::protobuf::Message *msg);
	void recvBuyGiftMsg(int type, google::protobuf::Message *msg);
	
private:
	static CActivityControl* m_instance;
};