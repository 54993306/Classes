#include "Battle/BattleDataCenter.h"

BattleDataCenter* BattleDataCenter::m_DataControl = nullptr;

BattleDataCenter::SingletonDestroy BattleDataCenter::m_Destroy;

BattleDataCenter::BattleDataCenter()
{

}

BattleDataCenter* BattleDataCenter::ShareBattleDataControl()
{
	if (m_DataControl == nullptr)
		m_DataControl = new BattleDataCenter;
	return m_DataControl;
}

void BattleDataCenter::BattleDataInit( BattleResponse *Response )
{

}



