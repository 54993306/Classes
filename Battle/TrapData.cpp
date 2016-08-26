
#include <protos/common/trap_common.pb.h>
#include "Battle/TrapData.h"
#include "Battle/BuffData.h"
#include "Battle/ConstNum.h"
#include "Battle/BattleMacro.h"
namespace BattleSpace
{
	TrapData::TrapData():mTrapID(0),mTrapModel(0),mTrapType(sTrapType::eNullType),mAttribute(0)
	,mRate(0),mDmage(0),mPosition(0),mTouch(0),mRound(0),mRotatione(0)
	{
		mTrapBuff = BuffData::create();
		mTrapBuff->retain();
	}

	TrapData::~TrapData()
	{
		CC_SAFE_RELEASE(mTrapBuff);
		mTrapBuff = nullptr;
	}

	const BuffData* TrapData::getTrapBuff() const
	{
		return mTrapBuff;
	}

	void TrapData::readData(const protos::common::Trap *pTrap)
	{
		if ( !pTrap ) return;
		this->setTrapID(pTrap->id());
		this->setTrapModel(pTrap->trapid());
		this->setTrapType((sTrapType)pTrap->type());
		this->setDmage(pTrap->damage());
		this->setPosition(pTrap->posx()*C_GRID_ROW + pTrap->posy());
		this->setAtbType(pTrap->pro_type());
		this->setRate(pTrap->pro_rate()*0.01f);
		this->setRotatione(pTrap->batch());
		mTrapBuff->readData(&pTrap->buff());
#if BATTLE_TEST
		//this->setTrapType(sTrapType::eObstacle);
		//if (this->getPosition() == 106)
			//this->setPosition(107);
		//this->setTrapModel(11000);
		//this->setRotatione(90);
#endif
	}

	TrapData* TrapData::createTrap(const protos::common::Trap *pTrapData)
	{
		TrapData* tTrap = new TrapData();
		if (tTrap)
		{
			tTrap->readData(pTrapData);
			tTrap->autorelease();
			return tTrap;
		}else{
			delete tTrap;
			tTrap = nullptr;
			return nullptr;
		}
	}
}