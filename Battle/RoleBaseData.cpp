

#include <vector>
#include "Battle/RoleBaseData.h"
#include "warscene/ConstNum.h"
#include <protos/common/hero_common.pb.h>
#include <protos/common/monster_common.pb.h>
using namespace std;

RoleBaseData::RoleBaseData()
:thumb(0),roletype(0),hp(0),atk(0),def(0),crit(0),hit(0),dodge(0),renew(0),dex(0),initCost(0)
,addCost(0),maxCost(0),grid(0),row(1),col(1),zoom(0),hasitem(0),enemy(false),coldDown(0),useCost(0)
,mCritTime(0),MoveSpeed(0),isCall(false),CallID(0),alert(0),alertRange(0),CallType(0),MstType(0)
,maxhp(0),atkInterval(0),battle(false)
{}

RoleBaseData::~RoleBaseData()
{

}

const RoleSkill* RoleBaseData::getSkillByType( E_SKILLTYPE pType ) const
{

}

MonsterRoleData::MonsterRoleData()
:mId(0),monsterId(0),batch(0),delay(0),quality(0),move1(0),isBoss(false)
{}

MonsterRoleData* MonsterRoleData::create()
{
	MonsterRoleData* tRole = new MonsterRoleData();
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

void MonsterRoleData::readData(const protos::common::Monster* monster)
{
	this->mId = monster->id();
	this->monsterId = monster->monsterid();
	this->atk = monster->atk();
	this->def = monster->def();
	this->hp = monster->hp();
	this->crit = monster->crit();
	this->hit = monster->hit();
	this->renew = monster->renew();
	this->roletype = monster->monstertype();
	this->thumb = monster->thumb();
	this->zoom = monster->zoom();
	this->CallType = monster->foe();
	this->hasitem = monster->hasitem();
	this->dodge = monster->dodge();
	this->enemy = true;
	this->atkInterval = monster->atkinterval();
	this->mCritTime = CCRANDOM_0_1()*(monster->maxround()- monster->minround()) +  monster->minround();
	this->MoveSpeed = monster->movespeed();
	this->isCall = monster->iscall();				//召唤类的武将都放在怪物列表中传输过来
	this->CallID = monster->monsterid();
	this->MstType = monster->buff();
	this->batch = monster->batch();
	this->delay = monster->delay();
	this->initCost = monster->initcost();
	this->move1 = monster->move1();
	this->isBoss = monster->isboss();
	this->maxhp = monster->maxhp();
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
			this->row = monster->posy();
			this->col = monster->posx();
			this->grid = VecRow.at(0)+VecCol.at(0)*C_GRID_ROW;
		}else{
			this->grid = monster->poslist(0).x()*C_GRID_ROW+monster->poslist(0).y();
			if (monster->posy())
				this->row = monster->posy();
			if (monster->posx())
				this->col = monster->posx();
		}
	}
	if (monster->has_skill1())
		this->skNormal.readData(&monster->skill1());
	if (monster->has_skill2())		//has判断是否为真
		this->skSpecial.readData(&monster->skill2());	//skill2取得对象传入解析
	if (monster->has_skill3())
		this->skActive.readData(&monster->skill3());
	this->quality = monster->quality();
}

HeroRoleData* HeroRoleData::create()
{
	HeroRoleData* tRole = new HeroRoleData();
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

void HeroRoleData::readData(const protos::common::Hero* hero)
{
	this->crit = hero->crit();
	this->atk = hero->atk();
	this->def = hero->def();
	this->dex = hero->dex();
	this->hp = hero->hp();
	this->renew = hero->renew();
	this->zoom = hero->zoom();
	this->initCost = hero->initcost();
	this->addCost = hero->addcost();
	this->maxCost = hero->maxcost();
	this->hit = hero->hit();
	this->thumb = hero->thumb();
	this->roletype = hero->herotype();
	this->enemy = false;
	this->coldDown = hero->colddown();
	this->useCost = hero->usecost();
	this->MoveSpeed = hero->movespeed();
	this->battle = hero->battle();
	this->alert = hero->alert();
	this->alertRange = hero->alertrange();
	this->atkInterval = hero->atkinterval();
	if (hero->posy())
		this->row = hero->posy();
	if (hero->posx())
		this->col = hero->posx();
	if (hero->has_skill1())
		this->skNormal.readData(&hero->skill1());
	if (hero->has_skill2())		//has判断是否为真
		this->skSpecial.readData(&hero->skill2());	//skill2取得对象传入解析
	if (hero->has_skill3())
		this->skActive.readData(&hero->skill3());
	if (hero->has_skill4())
		this->skCaptain.readData(&hero->skill4()); 
	this->dodge = hero->dodge();
}