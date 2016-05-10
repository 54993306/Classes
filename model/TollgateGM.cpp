#include "TollgateGM.h"
#include "Global.h"
#include "tools/ToolDefine.h"
TollgateGM::TollgateGM()
	:m_currID(0)
{
//	parse();
}
TollgateGM::~TollgateGM()
{
	for(vector<TollgateCfg *>::iterator iter = m_list.begin(); iter != m_list.end();++iter)
	{
		delete *iter;
	}
	m_list.clear();
}
TollgateCfg* TollgateGM::getCfg(int id)
{
	vector<TollgateCfg *>::iterator iter = m_list.begin();
	for (;iter != m_list.end();iter++)
	{
		if((*iter)->id == id)
		{
			return *iter;
		}
	}
	return nullptr;
}
void TollgateGM::parse()
{
	/*
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("tollgate.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));
		map<int,TollgateCfg*>::iterator iter = m_list.find(id);
		if(iter != m_list.end())
		{
			//配置文件ID重复，删除旧有ID
			delete iter->second;
			m_list.erase(iter);
		}
		TollgateCfg* cfg = new TollgateCfg();
		cfg->id = id;
		cfg->name = file->get(i,1);
		cfg->boss = atoi(file->get(i,2));
		CCArray* arr = strSplit(file->get(i,3),"|");
		CCObject* ob = nullptr;
		CCARRAY_FOREACH(arr,ob)
		{
			CCString* str = (CCString*)ob;
			cfg->wuJiang.push_back(atoi(str->getCString()));
		}
		cfg->mapID = atoi(file->get(i,4));
		cfg->star = atoi(file->get(i,5));
		cfg->level = atoi(file->get(i,6));
		cfg->openIcon = file->get(i,7);
		cfg->closeIcon = file->get(i,8);
		m_list[cfg->id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("tollgate.csv"));
	*/
}
const vector<TollgateCfg*>& TollgateGM::getCfgList()
{
	return m_list;
}

//添加关卡
void TollgateGM::addTollgate(TollgateCfg *tollgate)
{
	//m_list[tollgate->id] = tollgate;
	m_list.push_back(tollgate);
}

void TollgateGM::clearTollgate()
{
	for(vector<TollgateCfg *>::iterator iter = m_list.begin(); iter != m_list.end();++iter)
	{
		delete *iter;
	}
	m_list.clear();
}