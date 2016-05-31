
#ifndef __SKILLMACRO_H__
#define __SKILLMACRO_H__

/************************************************************* 
 *
 *
 *		Data : 2016.5.30
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

//����ʩ����������
enum SkillAreaType{
	nullArea = 0,
	eFront,								//ǰ����������				1
	eNearby,							//�ܱ߷�Χ					2
	eVertical,							//�����������				3(���������佫վλ����)
	eCross,								//ʮ��						4
	eFrontTow,							//ǰ������					5
	eNearbyUnself,						//�ܱ߷�Χ����������			6
	eEnemy,								//�з�ȫ��					7	
	eOur,								//�ҷ�ȫ��					8
	eAllArea,							//ȫͼ						9
	ePuncture,							//ǰ���������Ͳ��ᴩ			10
	eTArea,								//T������					11
	eMapDoubleLine,						//��ͼ������					12(118)
	//���佫վλ�����޹��๥����Χ		(��100��ʼ�������⻯����)		
	RestArea = 100,
	eFrontOne,						//ǰ������					101
	eCenterOne,						//��	������					102
	eBackOne,						//��	������					103
	eFrontDisperse,					//ǰ����ɢ��Χ				104
	eCenterDisperse,				//�о���ɢ��Χ				105
	eBackArea,						//�����ɢ��Χ				106
	eFrontRow,						//ǰ��n��					107
	eCenterRow,						//�о�n��					108
	eBackRow,						//���n��					109
	eAnyFix,						//����̶�����				110
	eAnyFixDisperse,				//����̶����ӷ�ɢ����		111
	eAnyAlive,						//����̶��佫				112
	eAnyAliveDisperse,				//����̶��佫����			113
	eLowestHp,						//Ѫ����Ͷ���				114
	eMapTwoLine,					//��ͼ�м���������			115
	eMapFourLine,					//��ͼ��������				116		
	TypeLowestHp,					//����˧��Ѫ����Ͷ���		117
};
//��������
enum GuardType
{
	eFrontGuard =1,					//ǰ����Χ
	eBackGuard,						//�󷽷�Χ
	eFrontAndBackGuard,				//ǰ��Χ
	eUpDownGuard,					//��������Ϊ���·�Χ
};

enum AreaMark
{
	eUsType	= 1,			//�ҷ�Ŀ��
	eEnemyType,				//�з�Ŀ��
	eAllType,				//˫��Ŀ��

	eFrontDirection			= 11,		//ǰ
	eCentenDirection		= 12,		//��
	eBackDirection			= 13,		//��
};
#endif