
/************************************************************* 
 *
 *
 *		Data : 2016.8.29
 *	
 *		Name : RoleConfigData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __RoleConfigData_H_
#define __RoleConfigData_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class RoleConfigData : public cocos2d::CCObject
	{
	protected:
		RoleConfigData();
		virtual bool init(){return true;}
	public:
		CREATE_FUNC(RoleConfigData);
	public:
		CC_SYNTHESIZE(int,		m_ModeID,					ModeID);				//����ģ����������ͬ�佫����������
		CC_SYNTHESIZE(bool,		m_Turn,						Turn);
		CC_SYNTHESIZE(int,		m_MoveSound,				MoveSound);				//�ƶ���Ч
		CC_SYNTHESIZE(int,		m_HitEffect,				HitEffect);				//
		CC_SYNTHESIZE(int,		m_HitEffectX,				HitEffectX);
		CC_SYNTHESIZE(int,		m_HitEffectY,				HitEffectY);
		CC_SYNTHESIZE(float,	m_HitEffectScale,			HitEffectScale);		//�ܻ�Ч������
		CC_SYNTHESIZE(bool,		mVariant,					Variant);				//�Ƿ�ɱ���
		CC_SYNTHESIZE(float,	mVariantScale,				VariantScale);			//��������
		CC_SYNTHESIZE(float,	mVariantCooldown,			VariantCooldown);		//����CDʱ��
		CC_SYNTHESIZE(float,	mVariantDuration,			VariantDuration);		//�������ʱ��
		CC_SYNTHESIZE(float,	mVariantCooling,			VariantCooling);		//������ȴʱ��
		CC_SYNTHESIZE(float,	mVariantRate,				VariantRate);			//�������Ա仯����
		CC_SYNTHESIZE(int,		mVariantEffect,				VariantEffect);			//������Ч(��Ӱ��ģ�Ͷ��ǰ���һ��ġ����б���ģ��,�����ÿ��Բ���Ҫ����)
		CC_SYNTHESIZE(int,		mVariantModel,				VariantModel);			//����ģ��(������Ч�ͷ�����Ч���ǹ̶���)
	public:
		CCNode* getActionEffect();
	};
};
#endif