

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
namespace BattleSpace{
	//���ڶԷ�����������ת���ͽ���
	class BuffData  : public cocos2d::CCObject
	{
	public:
		virtual ~BuffData(){};
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
		CC_SYNTHESIZE(int,m_TargetType,TargetType);			//��������(��1,ľ2,ˮ3,0�ǲ�����)
		CC_SYNTHESIZE(std::string,m_BuffDes,BuffDes);		//buff����
		void readData(const protos::common::Buff* buff);
	protected:
		BuffData();										//��ϸ�μ�datadefine Bufinfo		
		void initData(BuffData& pData);
	};
	//ս����ʵ��ʹ�õ���buff�ṹ,buff�ǿ��Ծ߱��ܶ��߼���,��������buff���߼�������buff����������ȥд�ˡ�Ӧ���ǽӿڵķ�ʽ�������д�Ŷ�
	class BuffInfo : public BuffData
	{
	private:
		BuffInfo(BuffData& pData);
	public:
		virtual ~BuffInfo(){};
		static BuffInfo* create(BuffData& pData);
	public:
		CC_PROPERTY(float,m_ExTime,ExTime);					//ִ��ʱ��
		CC_SYNTHESIZE(bool,m_Excute,Excute);				//�Ƿ�Ϊ������Buf
		CC_SYNTHESIZE(bool,m_Handle,Handle);				//�Ƿ���й������ǳ�����bufֻ����һ�Σ���Ѫ��ÿ�غ϶���Ҫ����
		CC_SYNTHESIZE(bool,m_AddFirst,AddFirst);			//�Ƿ�Ϊ��һ�����
	};
	//��ѭ�ȼ��滻����
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
		eHurt									=1000,//�˺�				
		eMaxBlood								=1001,//�ı����Ѫ��		
	};
};

#endif