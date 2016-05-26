/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : BattleField
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __BattleField__
#define __BattleField__

#include "protos/stage_protocol.pb.h"

//����Ч�����������˵��
//�籩������1��ʾ����(1��£��2��ɢ��3����4���ң�5���ϣ�6����)������2��ʾ�ƶ�������
//��ʯ������1��ʾ�˺�������2Ϊ0
//���ߣ�����1��2Ϊ0������3Ϊ���ֵĹ���
//�ڰ�����
//��أ�����1��ʾ������Χ�仯����(�ĵ��е�5������)������2Ϊ0
//�����
//����Ч��
struct BattleField
{
	BattleField();
	//CMonster monster;	//����3(����Ч������)
	int id;				//����id 
	int trapType;		//Ч������ (1 �籩��2 ��ʯ��3 ���ߣ�4 �ڰ���5 ��أ�6 ����)
	std::string name;		//Ч������
	int interval;		//�������(��)
	int limit;			//��󷢶�����
	int param1;			//����1
	int param2;			//����2
	int batch;			//����
	void readData(const protos::FieldEff &fieldeff);
};

#endif