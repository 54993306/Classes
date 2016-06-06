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
//��������(1 ��ͨ������2 ���⹥����3 ��ɱ����4 �ٻ�����5 �ӳ���)
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
	CC_SYNTHESIZE(int,mSkillID,SkillID);				//����ID
	CC_SYNTHESIZE(int,mSkillType,SkillType);			//��������( E_SKILLTYPE )
	CC_SYNTHESIZE(int,mUserRatio,UserRatio);			//�ɹ�ʩ�Ÿ���(��������)
	CC_SYNTHESIZE(int,mExpendCost,ExpendCost);			//�ͷż��������cost
	CC_SYNTHESIZE(float,mCooldown,Cooldown);			//������ȴʱ��
	CC_SYNTHESIZE(int,mTargetType,TargetType);			//Ŀ������(�ӳ�����������)
	CC_SYNTHESIZE(int,mAffectType,AffectType);			//����Ӱ������(�ӳ�����)
	CC_SYNTHESIZE(float,mAffectRatio,AffectRatio);		//����Ӱ��ٷֱ�(�ӳ���)
	CC_SYNTHESIZE(int,mSkillLevel,SkillLevel);			//���ܵȼ�
	CC_SYNTHESIZE(bool,mMoveRole,MoveRole);				//��ɫ�ƶ�
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