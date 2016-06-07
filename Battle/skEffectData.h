

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
	class BuffData;
	class skEffectData : public cocos2d::CCObject
	{
	public:
		skEffectData();									//��ϸ�μ�datadefine Bufinfo
		~skEffectData();
		virtual bool init(){return true;}
		CREATE_FUNC(skEffectData);
		CC_SYNTHESIZE(int,mEffectID,EffectID);
		CC_SYNTHESIZE(int,mGroup,Group);				//Ч������Ч����
		CC_SYNTHESIZE(int,mGroupPos,GroupPos);			//Ч������λ��
		CC_SYNTHESIZE(int,mEffectType,EffectType);		//Ч������
		CC_SYNTHESIZE(int,mUserRate,userRate);			//��������
		CC_SYNTHESIZE(int,mChangeCost,ChangeCost);		//ŭ��ֵӰ��(������)
		CC_SYNTHESIZE(float,mDamageRate,DamageRate);	//�˺��ٷֱ�
		CC_SYNTHESIZE(int,mRealHurt,RealHurt);			//��ʵ�˺�
		CC_SYNTHESIZE(int,mTargetType,TargetType);		//Ч��Ŀ������(1:�ѷ�  2:�з�  3:����˫��)
		CC_SYNTHESIZE(int,mBatter,Batter);				//������(��������һ���ٻ����ٹ���ʱʹ��)
		CC_SYNTHESIZE(int,mRepel,Repel);				//�ܻ��ƶ�����(�ܻ������ƶ����룬�ֵ���˫��)
		CC_SYNTHESIZE(float,mHurtRatio,HurtRatio);		//�˺�����ֵ(�ٷֱ�)
		CC_SYNTHESIZE(int,mAreaType,AreaType);			//��Χ����(1 ֱ��Ⱥ��...27 ����̶��佫����)
		CC_SYNTHESIZE(int,mSpaceRange,SpaceRange);		//��Χ��������
		CC_SYNTHESIZE(int,mAreaSize,AreaSize);			//��Χ��С
		CC_SYNTHESIZE(int,mImpactType,ImpactType);		//����Ӱ������(�ٻ���������)
		CC_SYNTHESIZE(int,mImpactRate,ImpactRate);		//����Ӱ�����(��Ѫ��Ѫʹ��)
	public:
		void readData(const protos::common::Effect* pEffect);
		void clearBuffData();
		const std::vector<BuffData*>& getBuffVector()const;
	private:
		void addBuffData(const protos::common::Buff* pBuffData);
		std::vector<BuffData*> mBuffVector;
	};

	bool EffectSort(const skEffectData* Effect1,const skEffectData* Effect2);//{return Effect1.pos>Effect2.pos;}
};
#endif