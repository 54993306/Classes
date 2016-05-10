
#ifndef __TASK_DATA_
#define __TASK_DATA_

#include "protos/protocol.h"
#include "bag/bagData.h"
struct CTask
{
	int taskId	;	//任务ID
    string name ;	//任务名称
	int level;	//任务等级
	int process;	//任务进度
	int target;	//任务目标
	int exp;	//奖励经验
	int coin;	//奖励金币
	int food;	//奖励粮食
    string note;	//任务描述
    bool isNew;	//新任务
    bool finish;	//任务是否完成
	vector<CPrize> prizeList;   //奖励道具列表
	int tindex;  //任务索引(读取任务描述)

	int action;	//奖励行动力
	int gold;	//奖励元宝
	time_t getTime;	//下次领取奖励的时间
	bool end;	//已结束(领完奖励后的状态)
	void read(const Task& task);
};


#endif