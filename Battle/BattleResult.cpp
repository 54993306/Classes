#include "BattleResult.h"
#include "model/DataCenter.h"
#include "Battle/WarManager.h"
namespace BattleSpace{

	STR_LostHp::STR_LostHp():hitType(0),hitNum(0),anger(0)
	{}
	BattleResult::BattleResult():m_alive(nullptr),m_usType(0),m_usNum(0)
	{}

	BattleResult::~BattleResult()
	{
		m_HitTargets.clear();
		m_LostHp.clear();
		m_AddBuf.clear();
		m_DelBuf.clear();
		m_CurrHp.clear();
		m_Alive_s.clear();
	}
};