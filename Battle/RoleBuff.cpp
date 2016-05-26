#include "Battle/RoleBuff.h"

CBuff::CBuff()
	:buffId(0)		//buff ID
	,buffType(0)		//buffӰ������
	,name("")		//buff����
	,damage(0)		//buffӰ����ֵ
	,damage_rate(0) //buffӰ��ٷֱ�
	,useRate(0)		//buff��������
	,debuf(false)	//�Ƿ�Ϊ����buff
	,duration(0)	//buff�����غ���
	,pTarget(0)		//Ŀ������
	,level(0)		//�ȼ�
	,typelimit(0)	//��������,1��ʬ��2��ʿ��3��
{}

void CBuff::readData(const protos::common::Buff &buff)
{
	this->buffId = buff.buffid();		
	this->buffType = buff.type();
	this->name = buff.name();	
	this->damage = buff.damage();	
	this->damage_rate = buff.damage_rate();
	this->useRate = buff.userate();
	this->debuf = buff.debuf();
	this->duration = buff.duration();
	this->pTarget = buff.target();
	this->level = buff.level();
	//this->durative = buf.durative();	//����atbType ����Ӱ�����ͣ���ʬ/��ʿ/��
	this->typelimit = buff.element();
}