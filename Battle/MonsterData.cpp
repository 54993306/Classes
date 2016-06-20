
#include <vector>
#include "Battle/MonsterData.h"
#include "Battle/ConstNum.h"
#include "Battle/RoleSkill.h"
#include "Battle/BattleDataCenter.h"
#include <protos/common/monster_common.pb.h>
namespace BattleSpace
{
	MonsterData::MonsterData(const google::protobuf::Message* pMessage/*=nullptr*/)
	:mMonsterID(0),mBatchNumber(0),mBossMonster(false),mMoveState(true)
	,mRoleDrop(0),mCritTime(0),mCallRole(false),mMaxHp(0)
	{
		if (pMessage)
			parseData(pMessage);
	}

	BattleSpace::sDataType MonsterData::getDataType()
	{
		return sDataType::eMonster;
	}

	MonsterData* MonsterData::create(const google::protobuf::Message* pMessage/*=nullptr*/)
	{
		MonsterData* tRole = new MonsterData(pMessage);
		if (tRole)
		{
			tRole->autorelease();
			return tRole;
		}else{
			delete tRole;
			tRole = NULL;
			return NULL;
		}
	}

	void MonsterData::parseData(const google::protobuf::Message* pMessage)
	{
		protos::common::Monster* tMonsterData = (protos::common::Monster*)pMessage;
		this->setServerID(tMonsterData->id());
		this->setMonsterID(tMonsterData->monsterid());
		this->setBatchNumber(tMonsterData->batch());
		this->setRoleModel(tMonsterData->thumb());
		this->setCallRole(tMonsterData->iscall());				//召唤类的武将都放在怪物列表中传输过来
		this->setAttribute((sAttribute)tMonsterData->monstertype());
		this->setRoleHp(tMonsterData->hp());
		this->setRoleAttack(tMonsterData->atk());
		this->setRoleDefense(tMonsterData->def());
		this->setRoleCrit(mRoleCrit);
		this->setRoleHit(tMonsterData->hit());
		this->setRoleDodge(tMonsterData->dodge());
		this->setRoleRegain(tMonsterData->renew());
		this->setRoleZoom(tMonsterData->zoom());
		this->setRoleDrop(tMonsterData->hasitem());
		this->setMoveSpeed(tMonsterData->movespeed());
		this->setAttackSpeed(tMonsterData->atkinterval());
		this->setBehavior((sBehavior)tMonsterData->buff());
		this->setCallRoleType((sCallType)tMonsterData->foe());
		this->setCallID(tMonsterData->monsterid());
		this->setCritTime(CCRANDOM_0_1()*(tMonsterData->maxround()- tMonsterData->minround()) + tMonsterData->minround());
		this->setDelayTime(tMonsterData->delay());
		this->setBossMonster(tMonsterData->isboss());
		if (!tMonsterData->has_move1() || !tMonsterData->move1())
			this->setMoveState(false);
		this->setMaxHp(tMonsterData->maxhp());
		if (tMonsterData->poslist().size()>0)
		{
			if (tMonsterData->poslist().size() > 1)					//根据传入的继续的格子,得到行列数
			{
				std::vector<int>VecCol;
				std::vector<int>VecRow;
				for (int i=0; i<tMonsterData->poslist().size();i++)
				{
					VecCol.push_back(tMonsterData->poslist(i).x());
					VecRow.push_back(tMonsterData->poslist(i).y());
				}
				sort(VecCol.begin(),VecCol.end());
				sort(VecRow.begin(),VecRow.end());
				VecCol.erase(unique(VecCol.begin(),VecCol.end()),VecCol.end());
				VecRow.erase(unique(VecRow.begin(),VecRow.end()),VecRow.end());
				this->setRoleRow(tMonsterData->posy());
				this->setRoleCol(tMonsterData->posx());
				this->setInitGrid(VecRow.at(0)+VecCol.at(0)*C_GRID_ROW);
			}else{
				this->setInitGrid(tMonsterData->poslist(0).x()*C_GRID_ROW+tMonsterData->poslist(0).y());
				if (tMonsterData->posy())
					this->setRoleRow(tMonsterData->posy());
				if (tMonsterData->posx())
					this->setRoleCol(tMonsterData->posx());
			}
		}
		if (tMonsterData->has_skill1())
			this->mNormalSkill->readData(&tMonsterData->skill1());
		if (tMonsterData->has_skill2())		//has判断是否为真
			this->mSpecialSkill->readData(&tMonsterData->skill2());	//skill2取得对象传入解析
		if (tMonsterData->has_skill3())
			this->mActiveSkill->readData(&tMonsterData->skill3());
		if (getCallRole())
			BattleData->addCallRole(this);
	}

	bool MonsterData::currBatchRole( int pBatch )
	{
		if (getCallRole() || getBatchNumber() != pBatch)
			return false;
		return true;
	}
}
