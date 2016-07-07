#include "Battle/HeroData.h"
#include "Battle/RoleSkill.h"
#include <protos/common/hero_common.pb.h>
namespace BattleSpace{
	HeroData::HeroData(){}

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
		this->setProperty(hero->herotype());
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
}
