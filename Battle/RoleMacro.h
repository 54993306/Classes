/************************************************************* 
 *
 *
 *		Data : 2016.6.2
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#ifndef __BATTLEROLEMACRO__
#define __BATTLEROLEMACRO__
namespace BattleSpace{
	enum E_ATTRIBUTETYPE
	{
		atb_normal = 0,
		atb_attak ,			//1		����
		atb_def,			//2		����
		atb_hp,				//3		Ѫ������ 
		atb_hit,			//4		����
		atb_crit,			//5		����
		atb_agile,			//6		�ȹ�
		atb_move,			//7		�ƶ�����
		atb_addhrt,			//8		��������
		atb_cuther,			//9		��������
		atb_ice_hrt,		//10	���˺�
		atb_mine_hrt,		//11	���˺�
		atb_fire_hrt,		//12	���˺�
		atb_ice_def	 = 20,	//20	������
		atb_mine_def,		//21	�׿���
		atb_fire_def,		//22	����
		atb_anger	 = 30,	//30	�����ظ�
		atb_hp_recover,		//31	Ѫ���ظ�
	};

	enum E_CALLTYPE				//�����佫�ٻ���������,��ͬ�����佫����ͬ
	{
		CommonType = 0,			//��ͨ�����佫
		AutoMoveType,			//�Զ��ƶ������佫
		FixedlyCantFlyType,		//ԭ�ز����ɱ���������
		FixedlyType,			//ԭ�ز������ɱ���������
		NotAttack,				//ʯͷ�಻�����佫
		AutoSkill,				//��ʯ�༴���ͷű�ɱ���佫
		UNAddHP,				//���ɱ���Ѫ�����佫
		OnlyOnce,				//ֻ����һ������
	};

	enum E_MONSTERTYPE			//��������
	{
		MST_COMMON = 0,			//��ͨ���͹���
		MST_HIDE,				//Ǳ�������
		MST_TIME,				//ʱ���ٻ����佫
		MST_SKILL,				//�������ͷż�����
	};

	enum E_ALIVESTATE
	{
		COMMONSTATE = 0,		//����״̬
		UNHIT,					//�����ܻ�״̬
		UNATK,					//���ɹ���״̬
		INVINCIBLE,				//�޵�״̬
	};

	enum E_HEROTYPE
	{
		FireType = 1,			//�������佫
		WaterType,				//ˮ�����佫
		WoodType,				//ľ�����佫
	};

	enum struct E_ALIVETYPE
	{
		eCommon,				//��ͨ���佫
		eElite,					//��Ӣ���佫
		eBoss,					//boss(�ҷ���˧��)
		eWorldBoss,				//����boss
	};
};
#endif	//__BATTLEROLEMACRO__
