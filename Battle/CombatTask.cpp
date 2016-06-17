#include "CombatTask.h"
#include "model/DataCenter.h"
#include "Battle/AnimationManager.h"
#include "Global.h"
using namespace cocos2d;

CombatTask::CombatTask()
	:m_pfnSelector(nullptr)
	,m_param(nullptr)
	,m_id(0)
	,m_fInterval(0)
	,m_target(nullptr)
{}
CombatTask::~CombatTask()
{
	m_pfnSelector = nullptr;
	CC_SAFE_RELEASE(m_param);
	m_param = nullptr;
	CC_SAFE_RELEASE(m_target);
	m_target = nullptr;
}

CombatTask* CombatTask::create(CCObject* pTraget,SEL_CallFuncO m_pfnSelector,float fInterval,CCObject* arr/*=nullptr*/)
{
	CombatTask* task = new CombatTask();
	task->autorelease();
	if (pTraget)
	{
		pTraget->retain();
		task->m_target = pTraget;
	}else{
		CCLOG("[ *ERROR ]CombatTask::create");
		task->m_target = CSceneManager::sharedSceneManager()->getRunningScene();
	}
	task->m_pfnSelector = m_pfnSelector;
	static int m_taskID = 0;
	++m_taskID;
	task->m_id = m_taskID;
	task->m_fInterval = fInterval;
	if(arr)
	{
		arr->retain();
		task->m_param = arr;
	}
	return task;
}
