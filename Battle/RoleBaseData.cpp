

#include <vector>
#include "Battle/RoleBaseData.h"
#include "warscene/ConstNum.h"
#include "Battle/RoleSkill.h"
#include <protos/common/hero_common.pb.h>
#include <protos/common/monster_common.pb.h>
using namespace std;

RoleBaseData::RoleBaseData()
:mServerID(0),mRoleModel(0),mRoleType(0),mRoleHp(0),mRoleAttack(0),mRoleDefense(0),mRoleCrit(0),mRoleHit(0),mRoleDodge(0),mRoleRegain(0),mRoleAgility(0),mInitCost(0)
,mCostSpeed(0),mMaxCost(0),mInitGrid(0),mRoleRow(1),mRoleCol(1),mRoleZoom(0),mRoleDrop(0),mColdDown(0),mExpendCost(0)
,mCritTime(0),mMoveSpeed(0),mCallRole(false),mCallID(0),mAlertType(0),mAlertRange(0),mCallType(0),mMonsterType(0)
,mMaxHp(0),mAttackSpeed(0),mCaptain(false),mDelayTime(0)
{
	mNormalSkill = RoleSkill::create();
	mNormalSkill->retain();
	mSpecialSkill = RoleSkill::create();
	mSpecialSkill->retain();
	mActiveSkill = RoleSkill::create();
	mActiveSkill->retain();
	mCaptainSkill = RoleSkill::create();
	mCaptainSkill->retain();
}

RoleBaseData::~RoleBaseData()
{
	CC_SAFE_RELEASE(mNormalSkill);
	mNormalSkill = nullptr;
	CC_SAFE_RELEASE(mSpecialSkill);
	mSpecialSkill = nullptr;
	CC_SAFE_RELEASE(mActiveSkill);
	mActiveSkill = nullptr;
	CC_SAFE_RELEASE(mCaptainSkill);
	mCaptainSkill = nullptr;
}

bool RoleBaseData::hasSpecialSkill() const
{
	if (mSpecialSkill->getSkillID())
		return true;
	return false;
}

bool RoleBaseData::hasActiveSkill() const
{
	if (mActiveSkill->getSkillID())
		return true;
	return false;
}

int RoleBaseData::getActiveSkillType() const
{
	if (mActiveSkill->getSkillID())
		return mActiveSkill->getSkillType();
	return 0;
}

bool RoleBaseData::hasCaptainSkill() const
{
	if (mCaptainSkill->getSkillID())
		return true;
	return false;
}

const RoleSkill* RoleBaseData::getNormalSkill() const
{
	return mNormalSkill;
}

const RoleSkill* RoleBaseData::getSpecialSkill() const
{
	return mSpecialSkill;
}

const RoleSkill* RoleBaseData::getActiveSkill() const
{
	return mActiveSkill;
}

const RoleSkill* RoleBaseData::getCaptainSkill() const
{
	return mCaptainSkill;
}


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
	this->setCallRole(monster->iscall());				//召唤类的武将都放在怪物列表中传输过来
	this->setRoleType(monster->monstertype());
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
	this->setMonsterType(monster->buff());
	this->setCallType(monster->foe());
	this->setCallID(monster->monsterid());
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
			vector<int>VecCol;
			vector<int>VecRow;
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

HeroData* HeroData::create()
{
	HeroData* tRole = new HeroData();
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

void HeroData::readData(const protos::common::Hero* hero)
{
	this->setServerID(hero->id());
	this->setRoleModel(hero->thumb());
	this->setRoleType(hero->herotype());
	if (hero->posy())
		this->setRoleRow(hero->posy());
	if (hero->posx())
		this->setRoleCol(hero->posx());
	this->setRoleHp(hero->hp());
	this->setRoleAttack(hero->atk());
	this->setRoleDefense(hero->def());
	this->setRoleCrit(hero->crit());
	this->setRoleHit(hero->hit());
	this->setRoleDodge(hero->dodge());
	this->setRoleAgility(hero->dex());
	this->setRoleRegain(hero->renew());
	this->setRoleZoom(hero->zoom());
	this->setMoveSpeed(hero->movespeed());
	this->setAttackSpeed(hero->atkinterval());
	this->setAlertType(hero->alert());
	this->setAlertRange(hero->alertrange());
	this->setColdDown(hero->colddown());
	this->setInitCost(hero->initcost());
	this->setExpendCost(hero->usecost());
	this->setCostSpeed(hero->addcost());
	this->setMaxCost(hero->maxcost());
	this->setCaptain(hero->battle());
	if (hero->has_skill1())
		this->mNormalSkill->readData(&hero->skill1());
	if (hero->has_skill2())		//has判断是否为真
		this->mSpecialSkill->readData(&hero->skill2());	//skill2取得对象传入解析
	if (hero->has_skill3())
		this->mActiveSkill->readData(&hero->skill3());
	if (hero->has_skill4())
		this->mCaptainSkill->readData(&hero->skill4()); 
}