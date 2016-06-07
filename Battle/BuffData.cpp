#include "Battle/BuffData.h"
#include <protos/common/skill_common.pb.h>
namespace BattleSpace{
	BuffData::BuffData()
		:m_BuffID(0),m_BuffType(0),m_BuffDes(""),m_ChangeNum(0),m_Precent(0)
		,m_TriggerRate(0),m_IsDBuff(false),m_TargetType(0),m_BuffDuration(0)
		,m_BuffTarget(0),m_BuffLevel(0)
	{}

	void BuffData::readData(const protos::common::Buff* buff)
	{
		this->setBuffID(buff->buffid());		
		this->setBuffType(buff->type());
		this->setChangeNum(buff->damage());	
		this->setPrecent(buff->damage_rate());
		this->setTriggerRate(buff->userate());
		this->setIsDBuff(buff->debuf());
		this->setBuffDuration(buff->duration());
		this->setBuffTarget(buff->target());
		this->setBuffLevel(buff->level());
		this->setTargetType(buff->element());
		this->setBuffDes(buff->name());	
		//this->durative = buf.durative();	//来自atbType 属性影响类型，僵尸/道士/神将
	}
	//使用父类对象给子类对象赋值的标准写法
	void BuffData::initData( BuffData& pData )
	{
		*this = pData;
	}

	BuffInfo::BuffInfo()
		:m_Excute(false),m_Handle(false),m_ExTime(0),m_AddFirst(true)
	{}
#define BUFFTIME   1.0f
	void BuffInfo::setExTime(float var)
	{
		m_ExTime += var;
		if (m_ExTime < BUFFTIME)				//设置buff的默认触发时间
			return;
		m_ExTime = 0;
		m_Excute = true;
	}
	float BuffInfo::getExTime(){return m_ExTime;}
}
