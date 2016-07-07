

#ifndef __SkillEffect__
#define __SkillEffect__
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
namespace protos{
	namespace common{
		class Buff;
		class Effect;
	}
}
#include "cocos2d.h"
namespace BattleSpace{
	enum struct sAttribute
	{
		eNull						= 0,
		eAttack						= 1,//����
		eDefense					= 2,//����
		eBlood						= 3,//Ѫ������ 
		eHit						= 4,//����
		eCrit						= 5,//����
		DefRate						= 6,//���R=6*
		AtkRate						= 7,//����=7*
		NedHpRate					= 8,//Ѫ��=8*
		AgilityRate					= 9,//����=9*
		RenewRate					= 10,//�؏�=10*
		DogeRate					= 11,//����=11
		HitRate						= 12,//����=12
		CritRate					= 13,//����=13
		HpMaxRate					= 14,//Ѫ������=14*
	};
	enum eEffectCountType
	{
		eNull						= 0,
		eSuckblood					= 1,//��Ѫ��Ч��
		eBoom						= 2,//�Ա���Ч��(��pro_Rate)
		eRateBlood					= 3,//��Ѫ�ٷֱ�����ѪЧ��
		eNumberBoold				= 4,//����ֵ����ѪЧ��
		eCurrBooldRate				= 5,//�۵�ǰѪ���ٷֱ�
	};
	class BaseRole;
	class BuffData;
	class AffectArea;
	class AreaCountInfo;
	class skEffectData : public cocos2d::CCObject
	{
	public:
		virtual ~skEffectData();
		static skEffectData* create();
		CC_SYNTHESIZE(int,mEffectID,EffectID);
		CC_SYNTHESIZE(int,mGroup,Group);						//Ч������Ч����
		CC_SYNTHESIZE(int,mGroupPos,GroupPos);					//Ч������λ��
		CC_SYNTHESIZE(eEffectCountType,mCountType,CountType);	//Ч������
		CC_SYNTHESIZE(int,mUserRate,userRate);					//��������
		CC_SYNTHESIZE(int,mChangeCost,ChangeCost);				//ŭ��ֵӰ��(������)
		CC_SYNTHESIZE(float,mDamageRate,DamageRate);			//�˺��ٷֱ�
		CC_SYNTHESIZE(int,mRealHurt,RealHurt);					//��ʵ�˺�
		CC_SYNTHESIZE(int,mTargetType,TargetType);				//Ч��Ŀ������(1:�ѷ�  2:�з�  3:����˫��)
		CC_SYNTHESIZE(int,mCallRoleID,CallRoleID);				//���ٻ��佫ID
		CC_SYNTHESIZE(int,mBatter,Batter);						//������(��������һ���ٻ����ٹ���ʱʹ��)
		CC_SYNTHESIZE(int,mRepel,Repel);						//�ܻ��ƶ�����(�ܻ������ƶ����룬�ֵ���˫��)
		CC_SYNTHESIZE(float,mHurtRatio,HurtRatio);				//�˺�����ֵ(�ٷֱ�)
		CC_SYNTHESIZE(int,mAreaType,AreaType);					//��Χ����(1 ֱ��Ⱥ��...27 ����̶��佫����)
		CC_SYNTHESIZE(int,mSpaceRange,SpaceRange);				//��Χ��������
		CC_SYNTHESIZE(int,mAreaSize,AreaSize);					//��Χ��С
		CC_SYNTHESIZE(sAttribute,mImpactType,ImpactType);		//����Ӱ������(�ٻ���������)
		CC_SYNTHESIZE(int,mCallNumber,CallNumber);
		CC_SYNTHESIZE(int,mImpactNumber,ImpactNumber);
		CC_SYNTHESIZE(int,mImpactRate,ImpactRate);				//����Ӱ�����
	public:
		void clearBuffData();
		void initAffect(int pType);
		void readData(const protos::common::Effect* pEffect);
		void initArea(AreaCountInfo& pInfo) ;
		const std::vector<BuffData*>& getBuffVector()const;
	private:
		skEffectData();											//��ϸ�μ�datadefine Bufinfo
		void addBuffData(const protos::common::Buff* pBuffData);
		std::vector<BuffData*> mBuffVector;
		AffectArea* mAffectArea;
	};

	enum E_TargetType
	{
		eUsType	= 1,			//�ҷ�Ŀ��
		eEnemyType,				//�з�Ŀ��
		eAllType,				//˫��Ŀ��
	};
	bool EffectSort(const skEffectData* Effect1,const skEffectData* Effect2);//{return Effect1.pos>Effect2.pos;}
};
#endif