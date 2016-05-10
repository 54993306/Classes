#include "EmBattleModel.h"
#include "GMessage.h"
EmBattleModel::EmBattleModel()
	:m_main(0)
	,m_selectID(0)
	,m_openNum(0)
{
	//测试数据
	setOpenNum(4);
	//主将位默认开启
	setMainWuJiang(1);

	addSubWuJiang(2);
	addSubWuJiang(3);
	addSubWuJiang(4);
	addSubWuJiang(5);
}
EmBattleModel::~EmBattleModel()
{
	m_sub.clear();
}
void EmBattleModel::setMainWuJiang(unsigned int uid)
{
	if(m_main != uid)
	{
		m_main = uid;
		CCNotificationCenter::sharedNotificationCenter()->postNotification(EMBATTLE_MAIN_CHANGE,nullptr);
	}
}
void EmBattleModel::addSubWuJiang(unsigned int uid)
{
	if(uid==0) return;
	if(m_sub.size()>=getOpenNum())
	{
		return;//未有可用的阵位
	}
	for(vector<unsigned int>::iterator iter = m_sub.begin(); iter != m_sub.end();++iter)
	{
		if(*iter == uid)
		{
			//已在阵中
			return;
		}
	}
	m_sub.push_back(uid);
	CCNotificationCenter::sharedNotificationCenter()->postNotification(EMBATTLE_SUB_CHANGE,nullptr);
}
void EmBattleModel::removeSubWuJiang(unsigned int id)
{
	for(vector<unsigned int>::iterator iter = m_sub.begin(); iter != m_sub.end();++iter)
	{
		if(*iter == id)
		{
			m_sub.erase(iter);
			CCNotificationCenter::sharedNotificationCenter()->postNotification(EMBATTLE_SUB_CHANGE,nullptr);
			break;
		}
	}
}
vector<unsigned int> EmBattleModel::getSubWuJiang()
{
	return m_sub;
}
void EmBattleModel::setSelectID(unsigned int id)
{
	if(id != m_selectID)
	{
		m_selectID = id;
		CCNotificationCenter::sharedNotificationCenter()->postNotification(EMBATTLE_SELECT_WUJIANG,nullptr);
	}
}
bool EmBattleModel::hasSubWuJiang(unsigned int id)
{
	for(vector<unsigned int>::iterator iter = m_sub.begin(); iter != m_sub.end();++iter)
	{
		if(*iter == id)
		{
			return true;
		}
	}
	return false;
}
void EmBattleModel::addWuJiang(unsigned int id)
{
	if(id <= 0) return;
	//如果当前没选择武将，就新增
	if(getSelectID() <=0)
	{
		addSubWuJiang(id);
		return;
	}
	if(id == getSelectID())
	{
		//新设置的武将跟以前一样，不做处理
		return;
	}
	if(getSelectID() == m_main)
	{
		//当前选择是主帅
		removeSubWuJiang(id);
		setMainWuJiang(id);
		unsigned int temp = getSelectID();
		setSelectID(id);
		addSubWuJiang(temp);
		CCNotificationCenter::sharedNotificationCenter()->postNotification(EMBATTLE_SUB_CHANGE,nullptr);
	}else if(id == m_main)
	{
		//当前选择是副帅
		unsigned int temp = getSelectID();
		removeSubWuJiang(temp);
		setMainWuJiang(temp);
		setSelectID(id);
		addSubWuJiang(id);	
	}else
	{
		//当前选择是副帅
		if( hasSubWuJiang(id) ) return;
		removeSubWuJiang(getSelectID());
		setSelectID(id);
		addSubWuJiang(id);
	}
}