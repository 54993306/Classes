#include "Battle/RoleBuffData.h"

RoleBuffData::RoleBuffData()
:m_BuffID(0),m_BuffType(0),m_BuffDes(""),m_ChangeNum(0),m_Precent(0)
,m_TriggerRate(0),m_IsDBuff(false),m_TargetType(0),m_BuffDuration(0)
,m_BuffTarget(0),m_BuffLevel(0)
{}

void RoleBuffData::readData(const protos::common::Buff &buff)
{
	this->setBuffID(buff.buffid());		
	this->setBuffType(buff.type());
	this->setChangeNum(buff.damage());	
	this->setPrecent(buff.damage_rate());
	this->setTriggerRate(buff.userate());
	this->setIsDBuff(buff.debuf());
	this->setBuffDuration(buff.duration());
	this->setBuffTarget(buff.target());
	this->setBuffLevel(buff.level());
	this->setTargetType(buff.element());
	this->setBuffDes(buff.name());	
	//this->durative = buf.durative();	//����atbType ����Ӱ�����ͣ���ʬ/��ʿ/��
}

BuffInfo::BuffInfo()
	:m_Excute(false),m_Handle(false),m_ExTime(0),m_AddFirst(true)
{}
#define BUFFTIME   1.0f
void BuffInfo::setExTime(float var)
{
	m_ExTime += var;
	if (m_ExTime < BUFFTIME)				//����buff��Ĭ�ϴ���ʱ��
		return;
	m_ExTime = 0;
	m_Excute = true;
}
float BuffInfo::getExTime(){return m_ExTime;}

void BuffInfo::initBuffInfo( RoleBuffData& pBuffData )
{
	setBuffID(pBuffData.getBuffID());
	setBuffType(pBuffData.getBuffType());
	setChangeNum(pBuffData.getChangeNum());
	setPrecent(pBuffData.getPrecent());
	setBuffDuration(pBuffData.getBuffDuration());
	setTriggerRate(pBuffData.getTriggerRate());
	setIsDBuff(pBuffData.getIsDBuff());
	setBuffTarget(pBuffData.getBuffTarget());
	setBuffDes(pBuffData.getBuffDes());
	setBuffLevel(pBuffData.getBuffLevel());
}