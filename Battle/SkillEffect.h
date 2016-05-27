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

#ifndef __SkillEffect__
#define __SkillEffect__

#include <protos/common/skill_common.pb.h>
#include "Battle/RoleBuffData.h" 

class SkillEffect
{
public:
	SkillEffect();						//��ϸ�μ�datadefine Bufinfo
	std::vector<RoleBuffData> buffList;	//buf�б�
	int	effectId;						//Ч��ID
	int group;							//Ч����
	int pos;							//Ч������λ��
	int Efftype;						//Ч������
	int	userRate;						//��������
	int	cost;							//ŭ��ֵӰ��(������)
	int	damage;							//�˺��ٷֱ�
	int	hurt;							//��ʵ�˺�
	int	pTarget;						//Ч��Ӱ��Ķ���(1:�ѷ�  2:�з�  3:����˫��)
	int	batter;							//������(��������һ���ٻ����ٹ���ʱʹ��)
	int	repel;							//�ܻ��ƶ�����(�ܻ������ƶ����룬�ֵ���˫��)
	int	erange;							//�˺�����ֵ(�ٷֱ�)
	int	mode;							//������Χ(1 ֱ��Ⱥ��...27 ����̶��佫����)
	int	distance;						//�������� ���������
	int	range;							//���ܷ�Χ
	int pro_Type;						//����Ӱ������(�ٻ���������)
	int pro_Rate;						//����Ӱ�����(��Ѫ��Ѫʹ��)
	std::string name;					//Ч������
	int	element;						//Ԫ������Ӱ��(1:�� 2:�� 3:��)
	int	element_hurt;					//Ԫ���˺�
	void readData(const protos::common::Effect &effect);
};

bool EffectSort(SkillEffect Effect1,SkillEffect Effect2);//{return Effect1.pos>Effect2.pos;}
#endif