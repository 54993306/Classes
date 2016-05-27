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
#include "cocos2d.h"
using namespace cocos2d;
//��ѭ�ȼ��滻����
enum BUFFTYPE
{
	CLEBF = 0,				//�������BF  ctrl+shift+uСдת��д��ctrl+u��дתСд
	CURRHP,					//������Ѫ���仯		1
	ATK,					//����				2
	DEF,					//����				3
	CRI,					//����				4
	DOGE,					//����				5
	HIT,					//����				6
	MOVESPEED,				//�ƶ��ٶ�			7
	ATKSPEED,				//�����ٶ�			8
	FIRING,					//����				9
	BLEED,					//��Ѫ

	CONTROL_ONE = 100,		//����
	/*δʹ��*/
	HRT = 1000,				//�˺�				2
	HPMAX,					//�ı����Ѫ��		5
};
//���ڶԷ�����������ת���ͽ���
class RoleBuffData
{
public:
	RoleBuffData();				//��ϸ�μ�datadefine Bufinfo		
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
	
	void readData(const protos::common::Buff &buff);
};
//ս����ʵ��ʹ�õ���buff�ṹ,buff�ǿ��Ծ߱��ܶ��߼���,��������buff���߼�������buff����������ȥд�ˡ�Ӧ���ǽӿڵķ�ʽ�������д�Ŷ�
class BuffInfo : public CCObject,public RoleBuffData
{
public:
	BuffInfo();
	CREATE_FUNC(BuffInfo);
	virtual bool init(){return true;}
public:
	CC_PROPERTY(float,m_ExTime,ExTime);					//ִ��ʱ��
	CC_SYNTHESIZE(bool,m_Excute,Excute);				//�Ƿ�Ϊ������Buf
	CC_SYNTHESIZE(bool,m_Handle,Handle);				//�Ƿ���й������ǳ�����bufֻ����һ�Σ���Ѫ��ÿ�غ϶���Ҫ����
	CC_SYNTHESIZE(bool,m_AddFirst,AddFirst);			//�Ƿ�Ϊ��һ�����
	void initBuffInfo(RoleBuffData& pBuffData);
protected:
	
};
 
#endif