#include "HeroInfoGM.h"
#include "tools/ToolDefine.h"
#include "Global.h"

HeroInfoGM::HeroInfoGM()
{
	parse();
}

HeroInfoGM::~HeroInfoGM()
{
	for(HeroInfoCSV::iterator iter = m_cfg.begin();iter != m_cfg.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_cfg.clear();

}

const HeroInfoData* HeroInfoGM::getCfg(int id)
{
	HeroInfoCSV::iterator iter = m_cfg.find(id);
	if(iter != m_cfg.end())
	{
		return iter->second;
	}
	return nullptr;
}


void HeroInfoGM::parse()
{
	if (m_cfg.size()>0)
	{
		return;
	}

	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("heroDesc.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));
		HeroInfoCSV::iterator iter = m_cfg.find(id);
		if(iter != m_cfg.end())
		{
			delete iter->second;
			m_cfg.erase(iter);
		}
		HeroInfoData* cfg = new HeroInfoData();
		cfg->heroId = id;
		cfg->heroName = file->get(i,1);
		cfg->heroDesc = file->get(i,2);
		cfg->iType1 = atoi(file->get(i, 3));
		cfg->iType2 = atoi(file->get(i, 4));
		cfg->heroStory = file->get(i, 5);
		m_cfg[id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("heroDesc.csv"));
}