#include "SkillGM.h"
#include "tools/ToolDefine.h"
#include "Global.h"
SkillGM::SkillGM()
{
	parse();
}
SkillGM::~SkillGM()
{
	for(SkillCSV::iterator iter = m_cfg.begin();iter != m_cfg.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_cfg.clear();
	for(EffectCsv::iterator iter = m_effect.begin();iter != m_effect.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_effect.clear();
}
const SkillCfg* SkillGM::getCfg(int id)
{
	SkillCSV::iterator iter = m_cfg.find(id);
	if(iter != m_cfg.end())
	{
		return iter->second;
	}

	//没有找到，默认返回首个
	CCLOG("ERROR——const SkillCfg* SkillGM::getCfg(int id)");
	return m_cfg.begin()->second;
}
void SkillGM::parse()
{
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("skill.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));				//配置表里存的都是string转化成int
		SkillCSV::iterator iter = m_cfg.find(id);
		if(iter != m_cfg.end())
		{
			//配置文件ID重复，删除旧有ID武将
			delete iter->second;
			m_cfg.erase(iter);
		}
		SkillCfg* cfg = new SkillCfg();
		cfg->id = id;
		cfg->name = file->get(i,1);
		cfg->icon = atoi(file->get(i,2));
		cfg->desc = file->get(i,3);
		cfg->type = atoi(file->get(i,4));
		cfg->model = file->get(i,5);
		strRemoveSpace(cfg->name);
	//	strRemoveSpace(cfg->icon);
		strRemoveSpace(cfg->desc);
		strRemoveSpace(cfg->model);

		strReplace(cfg->name,"\t","");
	//	strReplace(cfg->icon,"\t","");
		strReplace(cfg->desc,"\t","");
		strReplace(cfg->model,"\t","");
		m_cfg[id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("skill.csv"));
	//解析技能效果配置 
	file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("skillEffect1.csv"));
	if(!file)
	{
		return;
	}
	row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));
		EffectCsv::iterator iter = m_effect.find(id);
		if(iter != m_effect.end())
		{
			//配置文件ID重复，删除旧有ID武将
			delete iter->second;
			m_effect.erase(iter);
		}
		SkillEffectCfg* cfg = new SkillEffectCfg();
		cfg->id = id;
		cfg->type = atoi(file->get(i,1));
		CCArray* arr = strSplit(file->get(i,2),"|");
		CCObject* ob = nullptr;
		CCARRAY_FOREACH(arr,ob)
		{
			CCString* str = (CCString*)ob;
			cfg->names.push_back(str->getCString());
		}
		m_effect[id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("skillEffect1.csv"));
}
const SkillEffectCfg* SkillGM::getEffect(int id)
{
	EffectCsv::iterator iter = m_effect.find(id);
	if(iter != m_effect.end())
	{
		return iter->second;
	}
	//没有找到，默认返回首个
	CCLOG("ERROR——const SkillEffectCfg* SkillGM::getEffect(int id)");
	return m_effect.begin()->second;
}
