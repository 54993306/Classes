#ifndef __FRIEND_
#define __FRIEND_

#include "protos/protocol.h"

struct CFriend
{
	int friendId;	//好友ID
    string friendName;	//好友名称
	int level;	//好友等级
	bool online;	//是否在线
	int combat;	//战斗力
	time_t loginTime;	//上次登录时间
	int vip;		//VIP等级
	int thumb;	//头像
	int fcoin;	//友情点
	string fbId;
	CFriend();
	void read(const protos::Friend& frd);
};


#endif