/************************************************************* 
 *
 *
 *		Data : 2016.8.21
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#ifndef __PLAYHPTYPE__H_
#define __PLAYHPTYPE__H_

namespace BattleSpace
{
	enum struct PlayHpType
	{
		nullType			= 0,		//����ʾ��Ѫ����
		typeface			= 1,		//��ʾ���������ݣ�1������
		gainType			= 2,		//��Ѫ
		angerType			= 3,		//ŭ������
		generalType			= 4,		//һ���͵�Ѫ
		cutType				= 5,		//�����͵�Ѫ
		addType				= 6,		//�����͵�Ѫ
		genralCritType		= 7,		//һ���ͱ���
		cutCritType			= 8,		//�����ͱ���
		addCritType			= 9,		//�����ͱ���
	};


}

#endif
