#include "RewardGM.h"
#include "tools/ToolDefine.h"
#include "Global.h"


RewardDataGM::RewardDataGM()
{
	parse();
	parseStory();
}

RewardDataGM::~RewardDataGM()
{
	for(RewardCSV::iterator iter = m_cfg.begin();iter != m_cfg.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_cfg.clear();

}

void RewardDataGM::parse()
{
	if (m_cfg.size()>0)
	{
		return;
	}

	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("rewardStage.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));				//配置表里存的都是string转化成int
		RewardCSV::iterator iter = m_cfg.find(id);
		if(iter != m_cfg.end())
		{
			delete iter->second;
			m_cfg.erase(iter);
		}
		RewardData* cfg = new RewardData();
		cfg->id = id;
		cfg->name = file->get(i,1);
		cfg->icon = file->get(i,2);
		cfg->icon1 = file->get(i,3);
		cfg->level = file->get(i,4);
		m_cfg[id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("rewardStage.csv"));
}

void RewardDataGM::parseStory()
{
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("rewardStory.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		const char* str = file->get(i,1);			
		m_storyVec.push_back(str);
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("rewardStory.csv"));
}

const RewardData* RewardDataGM::getRewardCfg(int id)
{
	RewardCSV::iterator iter = m_cfg.find(id);
	if(iter != m_cfg.end())
	{
		return iter->second;
	}
	return nullptr;
}

const char* RewardDataGM::getStoryStr(int pos)
{
	if (pos<m_storyVec.size())
	{
		return m_storyVec.at(pos).c_str();
	}
	return nullptr;
}

int RewardDataGM::getStoryLength()
{
	return m_storyVec.size();
}
