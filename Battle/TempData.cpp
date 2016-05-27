#include "Battle/TempData.h"
#include "warscene/ConstNum.h"
TempSkill::TempSkill()
	:id(0)			//Ӣ�ۼ���ID(Ψһ)
	,skillId(0)		//����ID
	,name("")		//��������
	,skillType(0)		//��������(1 ��ͨ������2 ��ͨ���ܣ�3 �������ܣ�4 ��ɱ����5 ��˧��)
	,useRate(0)		//�ɹ�ʩ�Ÿ���
	,cost(0)		//�ͷż��������ŭ��ֵ
	,level(0)		//���ܵȼ�
	,move(false)
	,pTarget(0)
	,pro_rate(0)
	,pro_type(0)
	,maxLevel(0)
	,coldDown(0)
	,star(0)
{}

TempRole::TempRole()
	:name("")				//����
	,thumb(0)				//ģ��ID
	,roletype(0)			//�佫����(��ʬ�񽫵�ʿ)
	,level(1)				//�佫�ȼ�
	,hp(0)					//Ѫ��
	,atk(0)					//������
	,def(0)					//����
	,crit(0)				//��������
	,hit(0)					//����
	,dodge(0)				//����
	,renew(0)				//�ظ�
	,dex(0)
	,initCost(0)			//ŭ��ֵ
	,addCost(0)				//ŭ��ֵ�غϱ仯
	,maxCost(0)				//���ŭ��ֵ
	,grid(0)				//�佫���վλ����
	,row(1)
	,col(1)
	,zoom(0)				//���ű�
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
	,CallType(0)			//�佫����
	,MstType(0)				//��������
	,maxhp(0)				//����bossר������
{}

TempMonster::TempMonster()
	:mId(0)				//�ؿ�����ID
	,monsterId(0)		//����ID(��ʶID)
	,batch(0)			//����
	,delay(0)		//�ӳ�ʱ��
	,quality(0)
	,move1(0)
	,move2(0)
	,move3(0)
	,move4(0)
	,move5(0)
	,isBoss(false)
{}

void TempSkill::readData(const protos::common::Skill &skill)
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
	vector<SkillEffect>	EffectList1;//���ܵ�Ч���б�1
	vector<SkillEffect>	EffectList2;//���ܵ�Ч���б�2
	for (int i=0;i<skill.effect_size();i++)
	{
		SkillEffect effect;
		effect.readData(skill.effect(i));
		if (effect.group == 1)
		{
			EffectList1.push_back(effect);
		}else if(effect.group == 2){
			EffectList2.push_back(effect);
		}else{
			CCLOG("[*ERROR] BattleData CSkill::readData");
		}
	}
	std::sort(EffectList1.begin(),EffectList1.end(),EffectSort);
	EffectList.push_back(EffectList1);
	if (EffectList2.size())
	{
		std::sort(EffectList2.begin(),EffectList2.end(),EffectSort);
		EffectList.push_back(EffectList2);
	}
	// 	this->star = skill.star();
	this->maxLevel = skill.maxlevel();
}

void TempMonster::readData(const protos::common::Monster &monster)
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
	this->CallType = monster.foe();
	this->hasitem = monster.hasitem();
	this->dodge = monster.dodge();
	this->enemy = true;
	this->atkInterval = monster.atkinterval();
	this->mCritTime = CCRANDOM_0_1()*(monster.maxround()- monster.minround()) +  monster.minround();
	this->MoveSpeed = monster.movespeed();
	this->isCall = monster.iscall();				//�ٻ�����佫�����ڹ����б��д������
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
		if (monster.poslist().size() > 1)					//���ݴ���ļ����ĸ���,�õ�������
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
	if (monster.has_skill2())		//has�ж��Ƿ�Ϊ��
		this->skill2.readData(monster.skill2());	//skill2ȡ�ö��������
	if (monster.has_skill3())
		this->skill3.readData(monster.skill3());
	this->quality = monster.quality();
	this->level = monster.level();
}

void TempHero::readData(const protos::common::Hero &hero)
{
	this->crit = hero.crit();
	this->atk = hero.atk();
	this->def = hero.def();
	this->dex = hero.dex();
	this->hp = hero.hp();
	this->renew = hero.renew();
	this->name = hero.name();
	this->level = hero.level();
	this->zoom = hero.zoom();
	this->initCost = hero.initcost();
	this->addCost = hero.addcost();
	this->maxCost = hero.maxcost();
	this->hit = hero.hit();
	this->thumb = hero.thumb();
	this->roletype = hero.herotype();
	this->enemy = false;
	this->coldDown = hero.colddown();
	this->useCost = hero.usecost();
	this->MoveSpeed = hero.movespeed();
	this->battle = hero.battle();
	this->alert = hero.alert();
	this->alertRange = hero.alertrange();
	if (hero.posy())
		this->row = hero.posy();
	if (hero.posx())
		this->col = hero.posx();
	if (hero.has_skill1())
		this->skill1.readData(hero.skill1());
	if (hero.has_skill2())		//has�ж��Ƿ�Ϊ��
		this->skill2.readData(hero.skill2());	//skill2ȡ�ö��������
	if (hero.has_skill3())
		this->skill3.readData(hero.skill3());
	if (hero.has_skill4())
		this->skill4.readData(hero.skill4()); 
	if (hero.has_skill5())
		this->skill5.readData(hero.skill5());
	this->dodge = hero.dodge();
}