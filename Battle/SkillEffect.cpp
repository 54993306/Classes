#include "Battle/SkillEffect.h"
#include "Battle/RoleBuff.h"

CEffect::CEffect()
	:effectId(0)			//Ч��ID
	,Efftype(0)				//Ч������
	,name("")				//Ч������
	,userRate(0)			//��������
	,cost(0)				//ŭ��ֵӰ��(������)
	,damage(0)				//Ч���˺�ֵ
	,hurt(0)				//��ʵ�˺�
	,pTarget(0)				//Ч��Ӱ��Ķ���(1:�ѷ�  2:�з�  3:����˫��)
	,element(0)				//Ԫ������Ӱ��(1:�� 2:�� 3:�� 4:��)
	,element_hurt(0)		//Ԫ���˺�
	,batter(0)				//������
	,repel(0)				//���˾���(���ٸ�����)
	,erange(5)				//�˺�����ֵ(�ٷֱ�)
	,mode(0)				//������ʽ(1 ֱ��Ⱥ�壬2 ���ƶ�����3 ������ӣ�4 ʮ�֣�5 б��, 6 �з�ȫ�壬7 ǰ������ N*N��8 �ҷ�ȫ�壬 9 ȫͼ��10 ֱ�ߵ��壬 11 ǰ������, 12 �з�ǰ��)
	,distance(0)			//��������
	,range(0)				//���ܷ�Χ
	,pro_Type(0)			//����Ӱ������
	,pro_Rate(0)			//����Ӱ�����
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