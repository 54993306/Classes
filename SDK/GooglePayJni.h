#pragma once

extern "C"
{
	//�Խӵ���Ϣ����
	void c_handlMsg( int iMsg, const char *str , int iTag1, int iTag2 );

	//����֧������android��
	//1.str-��ƷID 2.���tag
	void c_pay(int iMsg, const char *str, int iTag, int iPrice);

	//�������ĵ�����
	void c_concume_purchase(int iMsg, const char *str);

	//��ʼ�������ⶩ��
	void c_start_consume_trouble_purchaes(int iMsg);
}