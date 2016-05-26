/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : RoleBuff
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __RoleBuff__
#define __RoleBuff__

#include <protos/common/skill_common.pb.h>

struct CBuff
{
	CBuff();				//详细参见datadefine Bufinfo
	int		buffId;			//buff ID
	int		buffType;		//buff影响类型
	int		damage;			//buff影响数值
	int		damage_rate;	//buff影响百分比
	int		useRate;		//buff触发概率
	bool	debuf;			//是否为减益buff
	int		duration;		//buff持续时间
	int		pTarget;		//目标，1：自己，2：受击目标，3：敌方目标
	int		level;			//buff等级
	int		typelimit;		//种族限制(火1,木2,水3,0是不限制)
	std::string	name;			//buff名称
	void readData(const protos::common::Buff &buff);
};

#endif