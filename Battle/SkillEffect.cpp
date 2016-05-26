#include "Battle/SkillEffect.h"
#include "Battle/RoleBuff.h"

CEffect::CEffect()
	:effectId(0)			//效果ID
	,Efftype(0)				//效果类型
	,name("")				//效果名称
	,userRate(0)			//触发概率
	,cost(0)				//怒气值影响(可增减)
	,damage(0)				//效果伤害值
	,hurt(0)				//真实伤害
	,pTarget(0)				//效果影响的对象(1:友方  2:敌方  3:敌我双方)
	,element(0)				//元素类型影响(1:冰 2:火 3:风 4:雷)
	,element_hurt(0)		//元素伤害
	,batter(0)				//连击数
	,repel(0)				//击退距离(多少个格子)
	,erange(5)				//伤害浮动值(百分比)
	,mode(0)				//攻击方式(1 直线群体，2 可移动区域，3 纵向格子，4 十字，5 斜线, 6 敌方全体，7 前方向下 N*N，8 我方全体， 9 全图，10 直线单体， 11 前方区域, 12 敌方前排)
	,distance(0)			//攻击距离
	,range(0)				//技能范围
	,pro_Type(0)			//属性影响类型
	,pro_Rate(0)			//属性影响比率
{}

void CEffect::readData(const protos::common::Effect &effect)
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
		CBuff buff;
		buff.readData(effect.bufflist(i));
		this->buffList.push_back(buff);
	}
}

bool EffectSort(CEffect Effect1,CEffect Effect2){return Effect1.pos<Effect2.pos;}