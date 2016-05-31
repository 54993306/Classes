#ifndef __BATTLEMACRO__
#define __BATTLEMACRO__

/** 
 *
 *		Data : 2016.4.12
 *	
 *		Name : BattleMacro
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : ���峣�ú�
 *
 */

#define CC_PROPERTY_CONSTREAD(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var);

enum MoveRange
{
	TypeOne,
	TypeTwo,
	TypeThree,
	TypeFour,
	TypeFive, 
};

enum AttributeType
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

enum CALL_Type			//�����佫�ٻ���������,��ͬ�����佫����ͬ
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
enum MST_TYPE				//��������
{
	MST_COMMON = 0,			//��ͨ���͹���
	MST_HIDE,				//Ǳ�������
	MST_TIME,				//ʱ���ٻ����佫
	MST_SKILL,				//�������ͷż�����
};
enum ALIVESTATE
{
	COMMONSTATE = 0,		//����״̬
	UNHIT,					//�����ܻ�״̬
	UNATK,					//���ɹ���״̬
	INVINCIBLE,				//�޵�״̬
};


enum HeroType
{
	FireType = 1,	//�������佫
	WaterType,		//ˮ�����佫
	WoodType,		//ľ�����佫
};
//��������(1 ��ͨ������2 ���⹥����3 ��ɱ����4 �ٻ�����5 �ӳ���)û�б�Ҫ��skill1-skill7��,�����Ѿ��Ƿ��˲żӵ��������
enum ROLESKILLTYPE
{
	NorAtk = 1,
	SpeAtk,
	CriAtk,
	CallAtk,
	CapAtk, 
};
enum struct AliveType
{
	Common,					//��ͨ���佫
	Elite,					//��Ӣ���佫
	Boss,					//boss(�ҷ���˧��)
	WorldBoss,				//����boss
};


#endif