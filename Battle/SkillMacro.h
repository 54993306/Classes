
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
#define eLoopNumberMax 200
namespace BattleSpace
{
	enum struct AffectType 
	{
		eNullArea						= 0,
		eFront							= 1,//ǰ����������				
		eNearby							= 2,//�ܱ߷�Χ					
		eVertical						= 3,//�����������				
		eCross							= 4,//ʮ��						
		eFrontTow						= 5,//ǰ������					
		eNearbyUnself					= 6,//�ܱ߷�Χ����������			
		eEnemy							= 7,//�з�ȫ��					
		eOur							= 8,//�ҷ�ȫ��					
		eAllArea						= 9,//ȫͼ					
		ePuncture						= 10,//ǰ���������Ͳ��ᴩ			
		eTArea							= 11,//T������				
		//���佫վλ�����޹��๥����Χ		(��100��ʼ�������⻯����)		
		RestArea						= 100,
		eFrontSingle					= 101,//ǰ������					
		eCenterSingle					= 102,//�о�����					
		eBackSingle						= 103,//�������					
		eFrontDisperse					= 104,//ǰ����ɢ��Χ				
		eCenterDisperse					= 105,//�о���ɢ��Χ				
		eBackDisperse					= 106,//�����ɢ��Χ				
		eFrontRow						= 107,//ǰ��n��					
		eCenterRow						= 108,//�о�n��					
		eBackRow						= 109,//���n��					
		eAnyFixGrid						= 110,//����̶�����				
		eAnyFixGridDisperse				= 111,//����̶����ӷ�ɢ����		
		eAnyFixAlive					= 112,//����̶��佫				
		eAnyFixAliveDisperse			= 113,//����̶��佫����			
		eLowestHp						= 114,//Ѫ����Ͷ���				
		eMapCenterTwoLine				= 115,//��ͼ�м���������			
		eFrontFourGrid					= 116,//��ͼ��������				
		eLowestHpUnCaptain				= 117,//����˧��Ѫ����Ͷ���		
		eMapAnyDoubleLine				= 118,//��ͼ����������			
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
}
#endif