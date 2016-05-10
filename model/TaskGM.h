#ifndef TASK_GM_H_
#define TASK_GM_H_
#include "DataDefine.h"

struct TaskData
{
	int taskId;
	string taskName;
	string taskDesc;
	int icon;
	int type;
	int mask;
};


class TaskGM
{
public:
	typedef map<int,TaskData*> TaskCSV;
	TaskGM();
	~TaskGM();
	const TaskData* getCfg(int id);
	const TaskData* getTaskCfg(int id);	
	void parseEvolve();
	void parseTask();
protected:
	TaskCSV m_cfg;     //进阶任务
	TaskCSV m_taskCfg; //接收任务
};
#endif // !SKILL_GM_H_
