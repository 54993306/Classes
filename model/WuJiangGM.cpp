#include "WuJiangGM.h"
#include "tools/ToolDefine.h"
#include "Global.h"
WuJiangGM::WuJiangGM()
{
	parse();
	//测试数据
	for(int i = 1; i <=6; ++i)
	{
		WuJiang* pWuJiang = new WuJiang();
		pWuJiang->uid = i;
		pWuJiang->attack = 100+i;
		pWuJiang->attackSpeed = 10+i;
		pWuJiang->cfgID = 99+i;
		pWuJiang->crit = 50+i;
		pWuJiang->currHp = 150+i;
		pWuJiang->defense = 100+i;
		pWuJiang->exp = 1099+i;
		pWuJiang->hit = 80+i;
		pWuJiang->hurtDef = 0.2f+i*0.1;
		pWuJiang->hurtExt = 0.1f+i*0.1;
		pWuJiang->level = 5+i;
		pWuJiang->maxAttack = 200+i;
		pWuJiang->maxDef = 500+i;
		pWuJiang->maxHp = 400+i;
		pWuJiang->speed = 100+i;
		const WuJiangCfg* cfg = getCfg(pWuJiang->cfgID);
		if(cfg)
		{
			if(cfg->mainSkill.size())
			{
				pWuJiang->mainSkill = cfg->mainSkill[0];
			}
			if(cfg->critSkill.size())
			{
				pWuJiang->critSkill = cfg->critSkill[0];
			}
			for(int j = 0; j < 3; ++j)
			{
				if(j < cfg->subSkill.size())
				{
					pWuJiang->subSkill.push_back(cfg->subSkill[j]);
				}
			}
		}
		addWuJiang(pWuJiang);
	}
}
WuJiangGM::~WuJiangGM()
{
	for(CfgData::iterator iter = m_cfg.begin();iter != m_cfg.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_cfg.clear();
	for(WuJiangData::iterator iter = m_list.begin();iter != m_list.end();++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_list.clear();
}
const WuJiangCfg* WuJiangGM::getCfg(int id)
{
	CfgData::iterator iter = m_cfg.find(id);
	if(iter == m_cfg.end())
	{
		return nullptr;
	}
	return iter->second;
}	
void WuJiangGM::parse()
{
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("wujiang.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		int id = atoi(file->get(i,0));
		CfgData::iterator iter = m_cfg.find(id);
		if(iter != m_cfg.end())
		{
			//配置文件ID重复，删除旧有ID武将
			delete iter->second;
			m_cfg.erase(iter);
		}
		WuJiangCfg* cfg = new WuJiangCfg();
		cfg->id = id;
		cfg->name = file->get(i,1);
		cfg->quality = atoi(file->get(i,2));
		cfg->capacity = atoi(file->get(i,3));
		cfg->type = atoi(file->get(i,4));
		cfg->model = file->get(i,5);
		cfg->icon = file->get(i,6);
		cfg->cardIcon = file->get(i,7);
		cfg->desc = file->get(i,8);
		//普通技
		CCArray* arr = strSplit(file->get(i,9),"|");
		CCObject* ob = nullptr;
		CCARRAY_FOREACH(arr,ob)
		{
			CCString* str = (CCString*)ob;
			cfg->subSkill.push_back(atoi(str->getCString()));
		}
		//主将技
		arr = strSplit(file->get(i,10),"|");
		CCARRAY_FOREACH(arr,ob)
		{
			CCString* str = (CCString*)ob;
			cfg->mainSkill.push_back(atoi(str->getCString()));
		}
		//必杀技
		arr = strSplit(file->get(i,11),"|");
		CCARRAY_FOREACH(arr,ob)
		{
			CCString* str = (CCString*)ob;
			cfg->critSkill.push_back(atoi(str->getCString()));
		}
		cfg->dragVal = atoi(file->get(i,12));
		strRemoveSpace(cfg->cardIcon);
		strRemoveSpace(cfg->icon);
		strRemoveSpace(cfg->model);
		strRemoveSpace(cfg->name);

		strReplace(cfg->cardIcon,"\t","");
		strReplace(cfg->icon,"\t","");
		strReplace(cfg->model,"\t","");
		strReplace(cfg->name,"\t","");
		m_cfg[id] = cfg;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("wujiang.csv"));
}

WuJiang* WuJiangGM::getWuJiang(unsigned int uid)
{
	for(WuJiangData::iterator iter = m_list.begin();iter != m_list.end();++iter)
	{
		if(iter->second->uid == uid)
		{
			return iter->second;
		}
	}
	return nullptr;
}
void WuJiangGM::addWuJiang(WuJiang* data)
{
	if(!data) return;
	WuJiangData::iterator iter = m_list.find(data->uid);
	if(iter != m_list.end())
	{
		return;//已岑在应有武将
	}
	m_list[data->uid] = data;
}
vector<const WuJiang*> WuJiangGM::getAll()
{
	vector<const WuJiang*> list;
	for(WuJiangData::iterator iter = m_list.begin();iter != m_list.end();++iter)
	{
		list.push_back(iter->second);
	}
	return list;
}