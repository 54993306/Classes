

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
#include "Battle/RoleMacro.h"
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
		CC_SYNTHESIZE(sAttribute,mAttribute,TargetType);			//��������(��1,ľ2,ˮ3,0�ǲ�����)
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
	enum struct BUFFTYPE
	{
		CLEBF			=0,					//�������BF  ctrl+shift+uСдת��д��ctrl+u��дתСд
		CURRHP			=1,					//������Ѫ���仯		1
		ATK				=2,					//����				2
		DEF				=3,					//����				3
		CRI				=4,					//����				4
		DOGE			=5,					//����				5
		HIT				=6,					//����				6
		MOVESPEED		=7,					//�ƶ��ٶ�			7
		ATKSPEED		=8,					//�����ٶ�			8
		FIRING			=9,					//����				9
		BLEED			=10,				//��Ѫ

		CONTROL_ONE		=100,				//����
		/*δʹ��*/
		HRT				=1000,				//�˺�				
		HPMAX			=1001,				//�ı����Ѫ��		
	};
};

#endif