#ifndef __TASK_CONTROL_
#define __TASK_CONTROL_

#include "AppUI.h"
#include "protos/protocol.h"

struct CGameTips{
	bool mailTips;	//邮件提示
	bool taskTips;	//任务提示
	bool composeTips;	//调合塔提示
	bool lotteryTips;	//免费抽英雄提示
	bool heroTips;	//英雄提示
	bool farmTips;	//培植区提示
	bool strenTips;	//装备强化提示
	bool actTips;
	bool shopTips;
	bool bountyTips;
	bool risingTips;
	CGameTips();
	void read(const GameTips &gt);
};


class CTaskControl:public CCNode
{
public:
	
	static CTaskControl* getInstance();
	~CTaskControl();
	void recvTaskNotice(int type, google::protobuf::Message *msg);
	void recvTaskPrize(int type, google::protobuf::Message *msg);
	void recvMailNotice(int type, google::protobuf::Message *msg);
	void sendGetTaskPrize(int taskId);
	void sendTaskList(int type); //任务类型(1 常规任务，2 日常任务) 

	void recvGameTips(int type, google::protobuf::Message *msg);
	CGameTips *getGameTips();
	void resetGameTips();
private:
	static CTaskControl *instance;
	CTaskControl();
	CGameTips m_gameTips;
};
	



#endif