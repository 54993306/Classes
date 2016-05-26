#include "Battle/BattleField.h"

BattleField::BattleField()
	:id(0)
	,trapType(0)			//Ч������ (1 �籩��2 ��ʯ��3 ���ߣ�4 �ڰ���5 ��أ�6 ����)
	,name("")			//Ч������
	,interval(0)		//�������(��)
	,limit(0)			//��󷢶�����
	,param1(0)			//����1
	,param2(0)			//����2
	,batch(0)
{}

void BattleField::readData(const protos::FieldEff &fieldeff)
{
	this->id = fieldeff.id();
	this->trapType = fieldeff.type();
	this->name = fieldeff.name();
	this->interval = fieldeff.interval();
	this->limit = fieldeff.limit();
	this->param1 = fieldeff.param1();
	this->param2 = fieldeff.param2();
	this->batch = fieldeff.batch();
	//this->monster.readData(fieldeff.monster());
}