#include "Battle/SkillEffect.h"
#include "Battle/RoleBuffData.h"

//������ʽ(1 ֱ��Ⱥ�壬2 ���ƶ�����3 ������ӣ�4 ʮ�֣�5 б��, 6 �з�ȫ�壬7 ǰ������ N*N��8 �ҷ�ȫ�壬 9 ȫͼ��10 ֱ�ߵ��壬 11 ǰ������, 12 �з�ǰ��)
SkillEffect::SkillEffect()
:effectId(0),Efftype(0),name(""),userRate(0),pro_Type(0),pro_Rate(0)
,cost(0),damage(0),hurt(0),pTarget(0),element(0),element_hurt(0)
,batter(0),repel(0),erange(5),mode(0),distance(0),range(0)
{}

void SkillEffect::readData(const protos::common::Effect &effect)
{
	this->effectId = effect.effectid();
	this->Efftype = effect.type();
	this->name = effect.name();
	this->userRate = effect.userrate();
	this->cost = effect.cost();
	this->damage = effect.damage();
	this->hurt = effect.hurt();
	this->pTarget = effect.target();
	this->batter = effect.batter();
	this->repel = effect.repel();
	this->erange = effect.erange();
	this->mode = effect.mode();
	this->distance = effect.distance();
	this->range = effect.range();
	this->pro_Type = effect.pro_type();
	this->pro_Rate = effect.pro_rate();
	this->pos = effect.pos();
	this->group = effect.group();
	for (int i=0; i<effect.bufflist_size();i++)
	{
		RoleBuffData buff;
		buff.readData(effect.bufflist(i));
		this->buffList.push_back(buff);
	}
}

bool EffectSort(SkillEffect Effect1,SkillEffect Effect2){return Effect1.pos<Effect2.pos;}