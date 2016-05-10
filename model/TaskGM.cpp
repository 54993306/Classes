#include "TaskGM.h"
#include "tools/ToolDefine.h"
#include "Global.h"


TaskGM::TaskGM()
{
}

TaskGM::~TaskGM()
{
	for(TaskCSV::iterator iter = m_cfg.begin();iter != m_cfg.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_cfg.clear();

	for(TaskCSV::iterator iter = m_taskCfg.begin();iter != m_taskCfg.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_taskCfg.clear();
}

const TaskData* TaskGM::getCfg(int id)
{
	TaskCSV::iterator iter = m_cfg.find(id);
	if(iter != m_cfg.end())
	{
		return iter->second;
	}
	return nullptr;
}

void TaskGM::parseEvolve()
{
	if (m_cfg.size()>0)
	{
		return;
	}
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("evolQuest.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));				//配置表里存的都是string转化成int
		TaskCSV::iterator iter = m_cfg.find(id);
		if(iter != m_cfg.end())
		{
			//配置文件ID重复，删除旧有ID武将
			delete iter->second;
			m_cfg.erase(iter);
		}
		TaskData* cfg = new TaskData();
		cfg->taskId = id;
		cfg->taskName = file->get(i,1);
		cfg->taskDesc = file->get(i,2);
		cfg->icon = atoi(file->get(i, 3));
		strRemoveSpace(cfg->taskName);
		strRemoveSpace(cfg->taskDesc);

		strReplace(cfg->taskName,"\t","");
		strReplace(cfg->taskDesc,"\t","");
		m_cfg[id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("evolQuest.csv"));
}


void TaskGM::parseTask()
{
	if (m_taskCfg.size()>0)
	{
		return;
	}

	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("task.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));				//配置表里存的都是string转化成int
		TaskCSV::iterator iter = m_taskCfg.find(id);
		if(iter != m_taskCfg.end())
		{
			delete iter->second;
			m_taskCfg.erase(iter);
		}
		TaskData* cfg = new TaskData();
		cfg->taskId = id;
		cfg->taskName = file->get(i,1);
		cfg->taskDesc = file->get(i,2);
		cfg->icon = atoi(file->get(i,3));
		cfg->type = atoi(file->get(i,4));
		cfg->mask = atoi(file->get(i,5));
		m_taskCfg[id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("task.csv"));
}

const TaskData* TaskGM::getTaskCfg(int id)
{
	TaskCSV::iterator iter = m_taskCfg.find(id);
	if(iter != m_taskCfg.end())
	{
		return iter->second;
	}
	return nullptr;
}
