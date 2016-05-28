


/************************************************************* 
 *
 *
 *		Data : 2016.5.27
 *	
 *		Name : TempData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __TempData__
#define __TempData__

#include <string>
#include <protos/protocol.h>
#include "bag/bagData.h"
#include "Battle/SkillEffect.h"

using namespace std;

class TempSkill
{
public:
	TempSkill();
	vector<vector<SkillEffect>>EffectList;
	int		id;					//Ӣ�ۼ���ID(Ψһ)
	int		skillId;			//����ID
	int		skillType;			//��������( ROLESKILLTYPE )
	int		useRate;			//�ɹ�ʩ�Ÿ���(��������)
	unsigned int cost;			//�ͷż��������cost
	int		star;				//���ܿ����Ǽ�
	int		pTarget;			//�ӳ�����������(�ٻ��༼�ܶ�Ӧ�ٻ����佫id,CallID)
	int		pro_type;			//��������
	int		pro_rate;			//���ưٷֱ�	
	int		coldDown;			//����cd
	string	name;				//��������
	int		level;				//���ܵȼ�
	int     maxLevel;			//�������ȼ�   
	bool	move;				//�����Ƿ�λ����Ŀ��ǰ��
	void readData(const protos::common::Skill &skill);
};

class TempRole
{
public:
	TempRole();
	TempSkill skill1;				//��ͨ����     ��˧���⹥�� ��˧������
	TempSkill skill2;				//���⹥��
	TempSkill skill3;				//��ɱ����(��ɱ�����ٻ��ļ�������)
	TempSkill skill4;				//��˧��ͨ���� 			
	int thumb;					//ģ��id
	bool enemy;					//�Ƿ�Ϊ����
	int alert;					//������������
	int alertRange;				//��������Χ
	float MoveSpeed;			//�佫�ƶ��ٶ�(��/S)
	int coldDown;				//������������CD
	int initCost;				//��ʼcost
	int useCost;				//��������cost
	int addCost;				//cost�ָ��ٶ�(1/s)
	int maxCost;				//cost����
	float atkInterval;			//�����ٶ� (��/��)
	int roletype;				//�佫����(��1,ˮ2,ľ3)
	int row;					//�佫��ռ����
	int col;					//�佫��ռ����
	int maxhp;					//���Ѫ��ֵ(����bossר��)
	int	hp;						//Ѫ��
	int	atk;					//����
	int	def;					//����
	int	crit;					//����
	int hit;					//����
	int dodge;					//����
	int dex;					//����
	int renew;					//�ظ�
	bool battle;				//�佫�Ƿ�Ϊ����״̬(ս���б�Ƕӳ�)
	int zoom;					//���ű�

	int grid;					//�佫վλ
	bool isCall;				//�Ƿ�Ϊ�ٻ����佫(�з����ﶼ����,�ڹ����б��ж�ȡ)
	int	CallID;					//�ٻ��佫ID
	float mCritTime;			//�����ͷű�ɱ��ʱ��
	int MstType;				//��������
	int CallType;				//�ٻ�����(Control)
	int hasitem;				//������Ʒ����1����2����3����4������5�����
	int	level;					//�佫�ȼ�
	string name;				//����
};
//����
class TempMonster :public TempRole
{ 
public:
	TempMonster();
	int mId;					//�ؿ�����ID
	int monsterId;				//����ID(��ʶID)
	int batch;					//����
	float delay;				//�ӳ�ʱ��
	bool isBoss;				//�Ƿ�Ϊboss
	int move1;					//�෽���ƶ�����
	int  quality;				//Ӣ��Ʒ�� 1��ɫ,2��ɫ,3��ɫ,4��ɫ,5��ɫ
	void readData(const protos::common::Monster &monster);
};
//Ӣ��
class TempHero :public TempRole
{
public:
	void readData(const protos::common::Hero &hero);
};
#endif