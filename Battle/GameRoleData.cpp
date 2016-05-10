
#include "Battle/GameRoleData.h"
#include "model/BuffManage.h"


GameRoleData::GameRoleData():m_BuffManage(nullptr)
{
	m_BuffManage = BuffManage::create();
	m_BuffManage->retain();
}

GameRoleData::~GameRoleData()
{
	CC_SAFE_RELEASE(m_BuffManage);
	m_BuffManage = nullptr;
}