#include "Battle/HeroData.h"
#include "Battle/RoleSkill.h"
#include <protos/common/hero_common.pb.h>
namespace BattleSpace
{
	HeroData::HeroData(const google::protobuf::Message* pMessage/*=nullptr*/)
	:mCostSpeed(0),mMaxCost(0),mColdDown(0),mExpendCost(0),mCaptain(false)
	,mAlertRange(0),mAlertType(0),mInitCost(0)
	{
		if (pMessage)
			parseData(pMessage);
	}

	HeroData* HeroData::create(const google::protobuf::Message* pMessage/*=nullptr*/)
	{
		HeroData* tRole = new HeroData(pMessage);
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

	void HeroData::parseData(const google::protobuf::Message* pMessage)
	{
		protos::common::Hero* tHeroData = (protos::common::Hero*)pMessage;
		this->setServerID(tHeroData->id());
		this->setRoleModel(tHeroData->thumb());
		this->setAttribute((sAttribute)tHeroData->herotype());
		if (tHeroData->posy())
			this->setRoleRow(tHeroData->posy());
		if (tHeroData->posx())
			this->setRoleCol(tHeroData->posx());
		this->setRoleHp(tHeroData->hp());
		this->setRoleAttack(tHeroData->atk());
		this->setRoleDefense(tHeroData->def());
		this->setRoleCrit(tHeroData->crit());
		this->setRoleHit(tHeroData->hit());
		this->setRoleDodge(tHeroData->dodge());
		this->setRoleAgility(tHeroData->dex());
		this->setRoleRegain(tHeroData->renew());
		this->setRoleZoom(tHeroData->zoom());
		this->setMoveSpeed(tHeroData->movespeed());
		this->setAttackSpeed(tHeroData->atkinterval());
		this->setAlertType(tHeroData->alert());
		this->setAlertRange(tHeroData->alertrange());
		this->setColdDown(tHeroData->colddown());
		this->setInitCost(tHeroData->initcost());
		this->setExpendCost(tHeroData->usecost());
		this->setCostSpeed(tHeroData->addcost());
		this->setMaxCost(tHeroData->maxcost());
		this->setCaptain(tHeroData->battle());
		if (tHeroData->has_skill1())
			this->mNormalSkill->readData(&tHeroData->skill1());
		if (tHeroData->has_skill2())	
			this->mSpecialSkill->readData(&tHeroData->skill2());	//skill2取得对象传入解析
		if (tHeroData->has_skill3())
			this->mActiveSkill->readData(&tHeroData->skill3());
		if (tHeroData->has_skill4())
			this->mCaptainSkill->readData(&tHeroData->skill4()); 
	}

}
