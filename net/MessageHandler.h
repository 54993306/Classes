#ifndef _MESSAGEHANDLER_H_
#define _MESSAGEHANDLER_H_
#include "google/protobuf/message.h"
class MessageHandler
{
public:
	virtual bool ProcessMsg(int type, google::protobuf::Message *msg) = 0;
};

#endif