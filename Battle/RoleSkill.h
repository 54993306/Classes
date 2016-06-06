/************************************************************* 
 *
 *
 *		Data : 2016.6.2
 *	
 *		Name : RoleSkill
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __RoleSkill_H_
#define __RoleSkill_H_
#include "cocos2d.h"
namespace protos{
	namespace common{
		class Skill;
		class Effect;
	}
}

enum E_EFFECTGROUP
{
	eFristGroup		=1,
	eSecondGroup	=2,
};
//技能类型(1 普通攻击，2 特殊攻击，3 必杀技，4 召唤技，5 队长技)
enum E_SKILLTYPE
{
	eNorAtk		=1,
	eSpeAtk		=2,
	eCriAtk		=3,
	eCallAtk	=4,
	eCapAtk		=5, 
};

class skEffectData;
class RoleSkill : public cocos2d::CCObject
{
public:
	RoleSkill();
	~RoleSkill();
	virtual bool init(){return true;}
	CREATE_FUNC(RoleSkill);
	CC_SYNTHESIZE(int,mSkillID,SkillID);				//技能ID
	CC_SYNTHESIZE(int,mSkillType,SkillType);			//技能类型( E_SKILLTYPE )
	CC_SYNTHESIZE(int,mUserRatio,UserRatio);			//成功施放概率(种族限制)
	CC_SYNTHESIZE(int,mExpendCost,ExpendCost);			//释放技能所需的cost
	CC_SYNTHESIZE(float,mCooldown,Cooldown);			//技能冷却时间
	CC_SYNTHESIZE(int,mTargetType,TargetType);			//目标类型(队长技种族限制)
	CC_SYNTHESIZE(int,mAffectType,AffectType);			//属性影响类型(队长技能)
	CC_SYNTHESIZE(float,mAffectRatio,AffectRatio);		//属性影响百分比(队长技)
	CC_SYNTHESIZE(int,mSkillLevel,SkillLevel);			//技能等级
	CC_SYNTHESIZE(bool,mMoveRole,MoveRole);				//角色移动
	void readData(const protos::common::Skill* pSkillData);
public:
	void clearEffectData();
	int getListSize()const;
	int getEffectSize(unsigned int pIndex) const;
	const skEffectData*getIndexEffect(unsigned int pListIndex,unsigned int pEffectIndex)const;

	const skEffectData* getFirstEffect() const;
	const skEffectData* getSecondEffect() const;
	const skEffectData* getSummonEffect() const;
	const std::vector<std::vector<skEffectData*>>& getEffectVector()const;
private:
	void initEffectData(const protos::common::Skill* pSkillData);
	std::vector<std::vector<skEffectData*>>mEffectList;
};

#endif