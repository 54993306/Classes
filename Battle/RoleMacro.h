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
namespace BattleSpace
{
#define bNotification CCNotificationCenter::sharedNotificationCenter()

	enum struct sMoveType				//�佫��������
	{
		eNull						= 0,
		eWalk						= 1,//��ͨ����
		eBurrow						= 2,//����ƶ�
		eFight						= 3,//�����ƶ�
	};

	enum struct sStandType
	{
		eNull						= 0,
		eGround						= 1,//��ǰλ���ڵ���
		eUnderground				= 2,//��ǰλ���ڵ���
		eSky						= 3,//��ǰλ�������
		eWater						= 4,//��ǰλ����ˮ��
	};

	enum struct sRoleCamp
	{
		eNULL						= 0,//��ʼ��ֵ
		eHeroRole					= 1,//Ӣ�����ɫ
		eMonsterRole				= 2,//�������ɫ
		eRivalRole					= 3,//�������ɫ
		eChildRole					= 4,//�ٻ����ɫ
		eNeutrality					= 5,//�������ɫ
	};

	enum struct sCallType				//�����佫�ٻ���������,��ͬ�����佫����ͬ
	{
		eCommon						= 0,//��ͨ�����佫
		eAutoMove					= 1,//�Զ��ƶ������佫
		eFixedlyCantFly				= 2,//ԭ�ز����ɱ���������
		eNotAttack					= 4,//ʯͷ�಻�����佫
		eAutoSkill					= 5,//��ʯ�༴���ͷű�ɱ���佫
		eUNAddHP					= 6,//���ɱ���Ѫ�����佫
		eOnlyOnce					= 7,//ֻ����һ������
		eBoxHaveRole				= 8,//���������,����ʱ��ɫ���ٻ�����
	};

	enum struct sBehavior				//��������
	{
		eNormal						= 0,//��ͨ���͹���
		eHide						= 1,//Ǳ�������
		eAutoSkill					= 3,//�������ͷż�����
	};

	enum struct sLogicState
	{
		eNormal						= 0,//����״̬
		eUnHit						= 1,//�����ܻ�״̬
		eUnAttack					= 2,//���ɹ���״̬
		eInvincible					= 3,//�޵�״̬
		eFree						= 4,//����״̬(���ƶ�,���ɹ��������ܻ�)
	};

	enum struct sRoleNature
	{
		eNull						= 0,//Ĭ�ϳ�ֵ
		eFire						= 1,//�������佫
		eWater						= 2,//ˮ�����佫
		eWood						= 3,//ľ�����佫
	};

	enum struct sMonsterSpecies
	{
		eCommon							= 0,//��ͨ���佫
		eElite							= 1,//��Ӣ���佫
		eBoss							= 2,//boss(�ҷ���˧��)
		eWorldBoss						= 3,//����boss
	};
};
#endif	//__BATTLEROLEMACRO__
