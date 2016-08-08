
#include <vector>
#include "Battle/MonsterData.h"
#include "Battle/ConstNum.h"
#include "Battle/RoleSkill.h"
#include <protos/common/monster_common.pb.h>
namespace BattleSpace{
	MonsterData::MonsterData()
		:mMonsterID(0),mBatchNumber(0),mBossMonster(false),mMoveState(true)
	{}

	MonsterData* MonsterData::create()
	{
		MonsterData* tRole = new MonsterData();
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

	void MonsterData::readData(const protos::common::Monster* monster)
	{
		this->setServerID(monster->id());
		this->setMonsterID(monster->monsterid());
		this->setBatchNumber(monster->batch());
		this->setRoleModel(monster->thumb());
		this->setProperty(monster->monstertype());
		this->setRoleHp(monster->hp());
		this->setRoleAttack(monster->atk());
		this->setRoleDefense(monster->def());
		this->setRoleCrit(mRoleCrit);
		this->setRoleHit(monster->hit());
		this->setRoleDodge(monster->dodge());
		this->setRoleRegain(monster->renew());
		this->setRoleZoom(monster->zoom());
		this->setRoleDrop(monster->hasitem());
		this->setMoveSpeed(monster->movespeed());
		this->setAttackSpeed(monster->atkinterval());
		this->setBehavior((sBehavior)monster->buff());
		this->setCallType((sCallType)monster->foe());
		if (monster->iscall())
		{
			if (this->getCallType() == sCallType::eBoxHaveRole)				//中立型的怪物
			{
				this->setRoleNature(sRoleType::eNeutrality);
			}else{
				this->setRoleNature(sRoleType::eChildRole);
			}
		}else{
			this->setRoleNature(sRoleType::eMonsterRole);
		}
		this->setCritTime(CCRANDOM_0_1()*(monster->maxround()- monster->minround()) +  monster->minround());
		this->setDelayTime(monster->delay());
		this->setBossMonster(monster->isboss());
		if (!monster->has_move1() || !monster->move1())
			this->setMoveState(false);
		this->setMaxHp(monster->maxhp());
		if (monster->poslist().size()>0)
		{
			if (monster->poslist().size() > 1)					//根据传入的继续的格子,得到行列数
			{
				std::vector<int>VecCol;
				std::vector<int>VecRow;
				for (int i=0; i<monster->poslist().size();i++)
				{
					VecCol.push_back(monster->poslist(i).x());
					VecRow.push_back(monster->poslist(i).y());
				}
				sort(VecCol.begin(),VecCol.end());
				sort(VecRow.begin(),VecRow.end());
				VecCol.erase(unique(VecCol.begin(),VecCol.end()),VecCol.end());
				VecRow.erase(unique(VecRow.begin(),VecRow.end()),VecRow.end());
				this->setRoleRow(monster->posy());
				this->setRoleCol(monster->posx());
				this->setInitGrid(VecRow.at(0)+VecCol.at(0)*C_GRID_ROW);
			}else{
				this->setInitGrid(monster->poslist(0).x()*C_GRID_ROW+monster->poslist(0).y());
				if (monster->posy())
					this->setRoleRow(monster->posy());
				if (monster->posx())
					this->setRoleCol(monster->posx());
			}
		}
		if (monster->has_skill1())
			this->mNormalSkill->readData(&monster->skill1());
		if (monster->has_skill2())		//has判断是否为真
			this->mSpecialSkill->readData(&monster->skill2());	//skill2取得对象传入解析
		if (monster->has_skill3())
			this->mActiveSkill->readData(&monster->skill3());
	}
}
