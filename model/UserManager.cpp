#include "UserManager.h"
#include "AppUI.h"
#include "Global.h"
UserManager::UserManager()
	:m_userData(nullptr)
	,m_oldLevel(0)
{

}
UserManager::~UserManager()
{
	clear();
	CC_SAFE_RELEASE(m_userData);
	m_userData = nullptr;
}
bool UserManager::init()
{
	m_userData = UserData::create();
	m_userData->retain();
	return true;
}
void UserManager::test()
{
	//测试数据
	CityAlive* alive = CityAlive::create();
	alive->setModel(m_userData->getModel());
	alive->setAliveID(m_userData->getAliveID());
	//alive->setName(m_userData->getName().c_str());
	addAlive(alive);
	//const char* model[6] = {"hero3","hero2","hero1","enemy1","enemy2","enemy3"};
	/*
	const char* race1[2] = {"enemy1","hero2"};//僵尸
	const char* race2[2] = {"enemy2","hero1"};//神将
	const char* race3[2] = {"enemy3","hero3"};//道士
	const char* name[12] = {"洗髓☆海秋","高☆亦云","缠身☆碧蓉","雷火☆飞烟","血虹☆涵菱","天道☆忆之","风魔☆亦玉","擎天☆梦颜","阳炎☆语兰","诸葛☆碧春","金翼☆冰菱","洛书☆心逸"};
	int index = 0;
	for(int i= 100; i <112;++i)
	{
		CityAlive* alive = CityAlive::create();
		if(index>=2)
		{
			index = 0;
		}
		if(getUserData()->getRaceType() == RaceType_1)
		{
			alive->setModel(race3[index]);
		}else if(getUserData()->getRaceType() == RaceType_2)
		{
			alive->setModel(race1[index]);
		}else
		{
			alive->setModel(race2[index]);
		}
		++index;
		alive->setAliveID(i);
		//char str[100];
		//sprintf(str,"%s%d",U8("玩家"),i);
		alive->setName(U8(name[i-100]));
		addAlive(alive);
	}*/
}

void UserManager::addRoleAlive(Role *role)
{
	CityAlive* alive = CityAlive::create();
	//alive->setAliveModel("enemy1");
	switch (role->rolenation())
	{
	case 1:
		//alive->setAliveModel("hero6");
		break;
	case 2:
		//alive->setAliveModel("hero2");
		break;
	case 3:
		//alive->setAliveModel("hero5");
		break;
	default:
		break;
	}
	alive->setAliveID(role->roleid());
	alive->setName(role->rolename().c_str());
// 	if (role->has_posx()&&role->has_posy())
// 	{
// 		alive->setX(role->posx());
// 		alive->setY(role->posy());
// 	}
	addAlive(alive);
}
//删除role
void UserManager::removeRoleAlive(int roleId)
{
	map<unsigned int,CityAlive*>::iterator iter = m_list.find(roleId);
	if(iter != m_list.end())
	{
		delete iter->second;
		m_list.erase(iter);
	}
}

void UserManager::addAlive(CityAlive* alive)
{
	do 
	{
		if(!alive) break;
		if(getAlive(alive->getAliveID())) break;
		alive->retain();
		m_list[alive->getAliveID()] = alive;
	} while (0);
}
CCArray* UserManager::getAlives()
{
	CCArray* arr = CCArray::create();
	do 
	{
		if(m_list.empty()) break;
		for(map<unsigned int,CityAlive*>::iterator iter = m_list.begin();iter != m_list.end();++iter)
		{
			arr->addObject(iter->second);
		}
	} while (0);
	return arr;
}
CityAlive* UserManager::getAlive(unsigned int aliveID)
{
	map<unsigned int,CityAlive*>::iterator iter = m_list.find(aliveID);
	if(iter != m_list.end())
	{
		return iter->second;
	}
	return nullptr;
}
void UserManager::clear()
{
	for(map<unsigned int,CityAlive*>::iterator iter = m_list.begin();iter != m_list.end();++iter)
	{
		iter->second->release();
	}
	m_list.clear();
}

void UserManager::setoldLevel()
{
	m_oldLevel = m_userData->getLevel();
}

bool UserManager::upLevel()
{
	if (m_oldLevel < m_userData->getLevel())return true;
	return false;
}
