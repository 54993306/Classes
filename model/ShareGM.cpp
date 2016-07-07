#include "ShareGM.h"
#include "tools/ToolDefine.h"
#include "Global.h"


ShareGM::ShareGM()
{
	parseData();
}

ShareGM::~ShareGM()
{
	for(ShareMap::iterator iter = m_dataMap.begin();iter != m_dataMap.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_dataMap.clear();
}

const ShareData* ShareGM::getCfg(int id)
{
	ShareMap::iterator iter = m_dataMap.find(id);
	if(iter != m_dataMap.end())
	{
		return iter->second;
	}
	return nullptr;
}


void ShareGM::parseData()
{
	if (m_dataMap.size()>0)
	{
		return;
	}

	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("shareFb.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));		
		ShareMap::iterator iter = m_dataMap.find(id);
		if(iter != m_dataMap.end())
		{
			delete iter->second;
			m_dataMap.erase(iter);
		}
		ShareData* cfg = new ShareData();
		cfg->id = id;
		cfg->desc = file->get(i,1);
		cfg->url = file->get(i,2);
		m_dataMap[id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("shareFb.csv"));
}

