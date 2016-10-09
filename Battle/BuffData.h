

#ifndef __RoleBuff__
#define __RoleBuff__
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
#include "cocos2d.h"
using namespace cocos2d;
namespace protos{
	namespace common{
		class Buff;
	}
}

//���ڶԷ�����������ת���ͽ���
namespace BattleSpace
{
	enum struct sBuffType
	{
		eClearDebuff							=0,//�������Buff
		eCurrBlood								=1,//������Ѫ���仯		
		eAttack									=2,//����				
		eDefense								=3,//����				
		eCrit									=4,//����				
		eDodge									=5,//����				
		eHit									=6,//����
		eMoveSpeed								=7,//�ƶ��ٶ�			
		eAttackSpeed							=8,//�����ٶ�
		eFiring									=9,//����
		eBleed									=10,//��Ѫ
		eUnMove									=100,//����	
	};
	enum struct sRoleNature;
	class BuffData  : public CCObject
	{
	public:
		static BuffData* create();
		CC_SYNTHESIZE(int,m_BuffID,BuffID);					//buff ID
		CC_SYNTHESIZE(int,m_BuffType,BuffType);				//buffӰ������
		CC_SYNTHESIZE(int,m_ChangeNum,ChangeNum);			//Ӱ����ֵ
		CC_SYNTHESIZE(int,m_Precent,Precent);				//Ӱ��ٷֱ�
		CC_SYNTHESIZE(int,m_TriggerRate,TriggerRate);		//buff��������
		CC_SYNTHESIZE(bool,m_IsDBuff,IsDBuff);				//�Ƿ�Ϊ����buff
		CC_SYNTHESIZE(float,m_BuffDuration,BuffDuration);	//buff����ʱ��
		CC_SYNTHESIZE(int,m_BuffTarget,BuffTarget);			//Ŀ�꣬1���Լ���2���ܻ�Ŀ�꣬3���з�Ŀ��
		CC_SYNTHESIZE(int,m_BuffLevel,BuffLevel);			//buff�ȼ�
		CC_SYNTHESIZE(sRoleNature,mProperty,Property);		//��������(��1,ľ2,ˮ3,0�ǲ�����)
		CC_SYNTHESIZE(std::string,m_BuffDes,BuffDes);		//buff����
		void readData(const protos::common::Buff* buff);
		bool isProperty(sRoleNature pNature) const;
	protected:
		BuffData();												//��ϸ�μ�datadefine Bufinfo		
	};
};

#endif