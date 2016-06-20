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
	
	enum struct sRoleType
	{
		eBaseRole				= 0,
		eHeroRole				= 1,
		eMonsterRole			= 2,
	};

	enum struct sProperty
	{
		eNull						= 0,
		eAttack						= 1,	//����
		eDefense					= 2,	//����
		eBlood						= 3,	//Ѫ������ 
		eHit						= 4,	//����
		eCrit						= 5,	//����
	};

	enum struct sCallType					//�����佫�ٻ���������,��ͬ�����佫����ͬ
	{
		eCommon						= 0,	//��ͨ�����佫
		eAutoMove					= 1,	//�Զ��ƶ������佫
		eFixedlyCantFly				= 2,	//ԭ�ز����ɱ���������
		eNotAttack					= 4,	//ʯͷ�಻�����佫
		eAutoSkill					= 5,	//��ʯ�༴���ͷű�ɱ���佫
		eUNAddHP					= 6,	//���ɱ���Ѫ�����佫
		eOnlyOnce					= 7,	//ֻ����һ������
	};

	enum struct sBehavior					//��Ϊ����
	{
		eNormal						= 0,	//��ͨ���͹���
		eHide						= 1,	//Ǳ�������
		eAutoSkill					= 3,	//�������ͷż�����
	};

	enum struct sLogicState					//��ɫ�߼�״̬
	{
		eNormal						= 0,	//����״̬
		eUnHit						= 1,	//�����ܻ�״̬
		eUnAttack					= 2,	//���ɹ���״̬
		eInvincible					= 3,	//�޵�״̬
	};

	enum struct sMonsterSpecies				//��������
	{
		eCommon						= 0,	//��ͨ���佫
		eElite						= 1,	//��Ӣ���佫
		eBoss						= 2,	//boss(�ҷ���˧��)
		eWorldBoss					= 3,	//����boss
	};
	//��1>ľ3; ľ3>ˮ2; ˮ2>��1
	enum struct sAttribute					//��ɫ����
	{
		eNull						= 0,
		eFire						= 1,	//�������佫
		eWater						= 2,	//ˮ�����佫
		eWood						= 3,	//ľ�����佫
	};
};
#endif	//__BATTLEROLEMACRO__