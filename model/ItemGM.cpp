#include "ItemGM.h"
#include "tools/ToolDefine.h"
#include "Global.h"


ItemGM::ItemGM()
{
	parseItem();
}

ItemGM::~ItemGM()
{
	for(ItemCSV::iterator iter = m_cfg.begin();iter != m_cfg.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_cfg.clear();

}

const ItemData* ItemGM::getCfg(int id)
{
	ItemCSV::iterator iter = m_cfg.find(id);
	if(iter != m_cfg.end())
	{
		return iter->second;
	}
	return nullptr;
}


void ItemGM::parseItem()
{
	if (m_cfg.size()>0)
	{
		return;
	}

	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("itemDesc.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));
		ItemCSV::iterator iter = m_cfg.find(id);
		if(iter != m_cfg.end())
		{
			delete iter->second;
			m_cfg.erase(iter);
		}
		ItemData* cfg = new ItemData();
		cfg->itemId = id;
		cfg->itemName = file->get(i,1);
		cfg->itemDesc = file->get(i,2);
		cfg->itemDesc1 = file->get(i,3);
		cfg->skillName = file->get(i,4);
		cfg->skillDesc = file->get(i,5);
		m_cfg[id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("itemDesc.csv"));
}

