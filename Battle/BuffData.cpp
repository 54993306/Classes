#include "Battle/BuffData.h"
#include "Battle/RoleMacro.h"
#include <protos/common/skill_common.pb.h>
namespace BattleSpace
{
	BuffData::BuffData():m_BuffID(0),m_BuffType(0),m_BuffDes(""),m_ChangeNum(0),m_Precent(0)
		,m_TriggerRate(-1),m_IsDBuff(false),mProperty(sRoleNature::eNull),m_BuffDuration(1)
		,m_BuffTarget(0),m_BuffLevel(0)
	{}
	BuffData* BuffData::create()
	{
		BuffData* tBuffData = new BuffData();
		if (tBuffData)
		{
			tBuffData->autorelease();
			return tBuffData;
		}else{
			delete tBuffData;
			tBuffData = nullptr;
			return nullptr;
		}
	}

	void BuffData::readData(const protos::common::Buff* buff)
	{
		if ( !buff)return ;
		this->setBuffID(buff->buffid());		
		this->setBuffType(buff->type());
		this->setChangeNum(buff->damage());	
		this->setPrecent(buff->damage_rate());
		this->setTriggerRate(buff->userate());
		this->setIsDBuff(buff->debuf());
		this->setBuffDuration(buff->duration());
		this->setBuffTarget(buff->target());
		this->setBuffLevel(buff->level());
		this->setProperty((sRoleNature)buff->element());
		this->setBuffDes(buff->name());	
	}

	bool BuffData::isProperty( sRoleNature pNature ) const
	{
		if (mProperty == sRoleNature::eNull || pNature == mProperty )
			return true;
		return false;
	}
}
