
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
	front,							//ǰ����������				1
	roundArea,						//�ܱ߷�Χ					2
	lengthwaysArea,					//�����������				3(���������佫վλ����)
	crossArea,						//ʮ��						4
	slantArea,						//ǰ������					5
	roundExclude,					//�ܱ߷�Χ����������			6
	EnemyArea,						//�з�ȫ��					7	
	usArea,							//�ҷ�ȫ��					8
	allArea,						//ȫͼ						9
	frontAreaVia,					//ǰ���������Ͳ��ᴩ			10
	tTypeArea,						//T������					11
	//���佫վλ�����޹��๥����Χ		(��100��ʼ�������⻯����)		
	RestArea = 100,
	frontOne,						//ǰ������					101
	centerOne,						//��	������					102
	backOne,						//��	������					103
	frontArea,						//ǰ����ɢ��Χ				104
	centerArea,						//�о���ɢ��Χ				105
	backArea,						//�����ɢ��Χ				106
	frontRowArea,					//ǰ��n��					107
	centerRowArea,					//�о�n��					108
	backRowArea,					//���n��					109
	anyFixGrid,						//����̶�����				110
	anyFixGridArea,					//����̶����ӷ�ɢ����		111
	anyFixAlive,					//����̶��佫				112
	anyFixAliveArea,				//����̶��佫����			113
	allTypeLowestHp,				//Ѫ����Ͷ���				114
	frontTwoType,					//��ͼ�м���������			115
	frontFourType,					//��ͼ��������				116		
	TypeLowestHp,					//����˧��Ѫ����Ͷ���		117
	DoubleLine,						//��ͼ������					118
};
//��������
enum GuardType
{
	GuardFront =1,					//ǰ����Χ
	GuardBack,						//�󷽷�Χ
	GuardFrontAndBack,				//ǰ��Χ
	GuardUpDown,					//��������Ϊ���·�Χ
};

enum AreaMark
{
	ustarget	= 1,		//�ҷ�Ŀ��
	enemyTyep,				//�з�Ŀ��
	allType,				//˫��Ŀ��

	FrontArea		= 11,		//ǰ
	CentenArea		= 12,		//��
	BackArea		= 13,		//��

	RowArea			= 20,		//��
	DisperseArea	= 30,		//��ɢ

	Hit_Alive	= 100,
	Atk_Area	= 200,
};
#endif