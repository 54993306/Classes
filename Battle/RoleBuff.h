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

struct CBuff
{
	CBuff();				//��ϸ�μ�datadefine Bufinfo
	int		buffId;			//buff ID
	int		buffType;		//buffӰ������
	int		damage;			//buffӰ����ֵ
	int		damage_rate;	//buffӰ��ٷֱ�
	int		useRate;		//buff��������
	bool	debuf;			//�Ƿ�Ϊ����buff
	int		duration;		//buff����ʱ��
	int		pTarget;		//Ŀ�꣬1���Լ���2���ܻ�Ŀ�꣬3���з�Ŀ��
	int		level;			//buff�ȼ�
	int		typelimit;		//��������(��1,ľ2,ˮ3,0�ǲ�����)
	std::string	name;			//buff����
	void readData(const protos::common::Buff &buff);
};

#endif