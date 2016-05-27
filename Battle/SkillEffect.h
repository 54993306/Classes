/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : SkillEffect
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __SkillEffect__
#define __SkillEffect__

#include <protos/common/skill_common.pb.h>
#include "Battle/RoleBuffData.h" 

class SkillEffect
{
public:
	SkillEffect();						//详细参见datadefine Bufinfo
	std::vector<RoleBuffData> buffList;	//buf列表
	int	effectId;						//效果ID
	int group;							//效果组
	int pos;							//效果组内位置
	int Efftype;						//效果类型
	int	userRate;						//触发概率
	int	cost;							//怒气值影响(可增减)
	int	damage;							//伤害百分比
	int	hurt;							//真实伤害
	int	pTarget;						//效果影响的对象(1:友方  2:敌方  3:敌我双方)
	int	batter;							//连击数(怪物配置一次召唤多少怪物时使用)
	int	repel;							//受击移动距离(受击对象移动距离，分敌我双方)
	int	erange;							//伤害浮动值(百分比)
	int	mode;							//攻击范围(1 直线群体...27 随机固定武将区域)
	int	distance;						//攻击距离 间隔多少行
	int	range;							//技能范围
	int pro_Type;						//属性影响类型(召唤数量限制)
	int pro_Rate;						//属性影响比率(加血吸血使用)
	std::string name;					//效果名称
	int	element;						//元素类型影响(1:冰 2:火 3:雷)
	int	element_hurt;					//元素伤害
	void readData(const protos::common::Effect &effect);
};

bool EffectSort(SkillEffect Effect1,SkillEffect Effect2);//{return Effect1.pos>Effect2.pos;}
#endif