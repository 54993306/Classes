
#include "CRoleData.h"
#include "bag/bagData.h"
#include "Battle/ConstNum.h"

CSkill::CSkill()
	:id(0)			//英雄技能ID(唯一)
	,skillId(0)		//技能ID
	,name("")		//技能名称
	,skillType(0)		//技能类型(1 普通攻击，2 普通技能，3 被动技能，4 必杀技，5 主帅技)
	,useRate(0)		//成功施放概率
	,cost(0)		//释放技能所需的怒气值
	,level(0)		//技能等级
	,move(false)
	,pTarget(0)
	,pro_rate(0)
	,pro_type(0)
	,maxLevel(0)
	,coldDown(0)
	,star(0)
{}

ROLE::ROLE()
	:name("")				//名称
	,thumb(0)				//模型ID
	,roletype(0)			//武将类型(僵尸神将道士)
	,level(1)				//武将等级
	,hp(0)					//血量
	,atk(0)					//攻击力
	,def(0)					//防御
	,crit(0)				//暴击几率
	,hit(0)					//命中
	,dodge(0)				//闪避
	,renew(0)				//回复
	,dex(0)
	,initCost(0)			//怒气值
	,addCost(0)				//怒气值回合变化
	,maxCost(0)				//最大怒气值
	,grid(0)				//武将标记站位格子
	,row(1)
	,col(1)
	,zoom(0)				//缩放比
	,hasitem(0)				//
	,enemy(false)
	,coldDown(0)
	,useCost(0)
	,mCritTime(0)
	,MoveSpeed(0)
	,isCall(false)
	,CallID(0)
	,alert(0)
	,alertRange(0)
	,MstType(0)				//怪物类型
	,maxhp(0)				//世界boss专有属性
{}

CMonster::CMonster()
	:mId(0)				//关卡怪物ID
	,monsterId(0)		//怪物ID(标识ID)
	,batch(0)			//批次
	,delay(0)		//延迟时间
	,quality(0)
	,move1(0)
	,move2(0)
	,move3(0)
	,move4(0)
	,move5(0)
	,isBoss(false)
{}

CHero::CHero() 
	:id(0)				//玩家英雄ID(全服唯一)
	,recLevel(0)		//可招募等级
	,coin(0)			//招募价格
	,addHp(0)			//血量成长系数
	,addAtk(0)			//攻击成长系数
	,addDef(0)			//防御成长系数
	,index(0)			//布阵的顺序
	,recruit(false)
	,star(0)
	,heroid(0)
	,own(false)
	,iColor(0)
	,evol(false)
	,remove(true)
{}

void CSkill::readData(const protos::common::Skill &skill)
{
	this->id = skill.skillid();
	this->skillId = skill.skillid();
	this->name = skill.name();
	this->skillType = skill.type();
	this->useRate = skill.userate();
	this->cost = abs(skill.cost());
	this->level = skill.level();
	this->move = skill.move();
	this->pTarget = skill.target();
	this->pro_rate = skill.pro_rate();
	this->pro_type = skill.pro_type();
	this->coldDown = skill.colddown();
	this->maxLevel = skill.maxlevel();
	this->star = skill.activequality();
}

void CMonster::readData(const protos::common::Monster &monster)
{
	this->mId = monster.id();
	this->monsterId = monster.monsterid();
	this->name = monster.name();
	this->atk = monster.atk();
	this->def = monster.def();
	this->hp = monster.hp();
	this->crit = monster.crit();
	this->hit = monster.hit();
	this->renew = monster.renew();
	this->roletype = monster.monstertype();
	this->thumb = monster.thumb();
	this->zoom = monster.zoom();
	this->hasitem = monster.hasitem();
	this->dodge = monster.dodge();
	this->enemy = true;
	this->atkInterval = monster.atkinterval();
	this->mCritTime = CCRANDOM_0_1()*(monster.maxround()- monster.minround()) +  monster.minround();
	this->MoveSpeed = monster.movespeed();
	this->isCall = monster.iscall();				//召唤类的武将都放在怪物列表中传输过来
	this->CallID = monster.monsterid();
	this->MstType = monster.buff();
	this->batch = monster.batch();
	this->delay = monster.delay();
	this->initCost = monster.initcost();
	this->move1 = monster.move1();
	this->move2 = monster.move2();
	this->move3 = monster.move3();
	this->move4 = monster.move4();
	this->move5 = monster.move5();
	this->isBoss = monster.isboss();
	this->maxhp = monster.maxhp();
	if (monster.poslist().size()>0)
	{
		if (monster.poslist().size() > 1)					//根据传入的继续的格子,得到行列数
		{
			vector<int>VecCol;
			vector<int>VecRow;
			for (int i=0; i<monster.poslist().size();i++)
			{
				VecCol.push_back(monster.poslist(i).x());
				VecRow.push_back(monster.poslist(i).y());
			}
			sort(VecCol.begin(),VecCol.end());
			sort(VecRow.begin(),VecRow.end());
			VecCol.erase(unique(VecCol.begin(),VecCol.end()),VecCol.end());
			VecRow.erase(unique(VecRow.begin(),VecRow.end()),VecRow.end());
			this->row = monster.posy();
			this->col = monster.posx();
			this->grid = VecRow.at(0)+VecCol.at(0)*C_GRID_ROW;
		}else{
			this->grid = monster.poslist(0).x()*C_GRID_ROW+monster.poslist(0).y();
			if (monster.posy())
				this->row = monster.posy();
			if (monster.posx())
				this->col = monster.posx();
		}
	}
 	if (monster.has_skill1())
		this->skill1.readData(monster.skill1());
 	if (monster.has_skill2())		//has判断是否为真
		this->skill2.readData(monster.skill2());	//skill2取得对象传入解析
 	if (monster.has_skill3())
		this->skill3.readData(monster.skill3());
	this->quality = monster.quality();
	this->level = monster.level();
}

void CHero::readData(const protos::common::Hero &hero)
{
	this->id = hero.id();
	this->addAtk = hero.addatk();
	this->addDef = hero.adddef();
	this->addHp = hero.addhp();
	this->coin = hero.coin();
	this->crit = hero.crit();
	this->atk = hero.atk();
	this->def = hero.def();
	this->dex = hero.dex();
	this->heroid = hero.heroid();
	this->hp = hero.hp();
	this->renew = hero.renew();
	this->name = hero.name();
	this->level = hero.level();
	this->zoom = hero.zoom();
	this->combat = hero.combat();
	this->initCost = hero.initcost();
	this->addCost = hero.addcost();
	this->maxCost = hero.maxcost();
	this->hit = hero.hit();
	this->thumb = hero.thumb();
	this->roletype = hero.herotype();
	this->prate = hero.prate();
	this->exp = hero.exp();
	this->nextExp = hero.nextexp();
	this->enemy = false;
	this->addDex = hero.adddex();
	this->addHit = hero.addhit();
	this->addCrit = hero.addcrit();
	this->addDodge = hero.adddodge();
	this->addRenew = hero.addrenew();
	this->maxLevel = hero.maxlevel();
	this->atkInterval = hero.atkinterval();
	this->maxQuality = hero.maxquality();
	this->coldDown = hero.colddown();
	this->useCost = hero.usecost();
	this->MoveSpeed = hero.movespeed();
	this->battle = hero.battle();
	this->alert = hero.alert();
	this->alertRange = hero.alertrange();
	this->remove = hero.remove();
	if (hero.posy())
		this->row = hero.posy();
	if (hero.posx())
		this->col = hero.posx();
	//if (hero.has_skill1())
		this->skill1.readData(hero.skill1());
	//if (hero.has_skill2())		//has判断是否为真
		this->skill2.readData(hero.skill2());	//skill2取得对象传入解析
	//if (hero.has_skill3())
		this->skill3.readData(hero.skill3());
	//if (hero.has_skill4())
		this->skill4.readData(hero.skill4()); 
	//if (hero.has_skill5())
		this->skill5.readData(hero.skill5());
	this->quality = hero.quality();
	this->coin = hero.coin();
	//this->armorNum = hero.armornum();
	if (hero.has_armor1())
		this->armor1.read(hero.armor1());
	if (hero.has_armor2())
		this->armor2.read(hero.armor2());
	if (hero.has_armor3())
	{
		this->armor3.read(hero.armor3());
	}
	if (hero.has_armor4())
	{
		this->armor4.read(hero.armor4());
	}
	if (hero.has_armor5())
	{
		this->armor5.read(hero.armor5());
	}
	this->dodge = hero.dodge();
	this->itemId = hero.itemid();
	this->itemNum1 = hero.itemnum1();
	this->itemNum2 = hero.itemnum2();
	if (hero.has_own())
	{
		this->own = hero.own();
	}else{
		this->own = true;
	}
	this->iColor = hero.color();
	this->evol = hero.evol();
}
