/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : BattleTrap
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __BattleTrap__
#define __BattleTrap__

#include <protos/common/trap_common.pb.h>

struct BattleTrap
{
	BattleTrap();
	int  id;					//�ؿ�����ID
	int  terrainId;				//���α�ʶID
	std::string name;				//��������
	int  terrainType;			//��������
	int  pro_type;				//Ӱ������
	int  pro_rate;				//Ӱ������ֵ
	int  damage;				//������ɵ��˺�
	int  posX;					//�ڵ�ͼ�ϵ�λ��X
	int  posY;					//�ڵ�ͼ�ϵ�λ��Y
	int  triggerNum;			//����������
	int	 existNum;				//����ʱ��
	int  batch;					//����
	//CBuff buff;					//���θ���buf 
	void readData(const protos::common::Trap &terr);
};

#endif