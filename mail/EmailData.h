#ifndef __EMAIL_DATA_
#define __EMAIL_DATA_

#include "protos/protocol.h"
#include "bag/bagData.h"
#include "tollgate/Chapter.h"
struct CMail
{
	int mailId;	//邮件ID
	string title;	//邮件标题
	string content;	//邮件内容
	string addTime;	//邮件接收时间
	int senderId;	//发件人ID
	string sender;	//发件人名称
    bool readed;	//是否已读取
	int icon;		//头像ID(系统头像 0)
	CGetPrizeRes prize;
	void read(const Mail& mail);
};

#endif