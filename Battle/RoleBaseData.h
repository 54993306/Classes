


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

#include "cocos2d.h"
#include "Battle/RoleSkill.h"

namespace protos{
	namespace common{
		class Hero;
		class Monster;
	}
}

enum struct E_ROLESKILLTYPE
{
	eSkillNormal		=1,
	eSKillSpecial		=2,
	eSkillActive		=3,
	eSkillCaptain		=4,
};
class RoleBaseData : public cocos2d::CCObject
{
public:
	RoleBaseData();
	~RoleBaseData();
	RoleSkill skNormal;				//��ͨ����
	RoleSkill skSpecial;			//���⹥��
	RoleSkill skActive;				//��ɱ����(��ɱ�����ٻ��ļ�������)
	RoleSkill skCaptain;			//��˧���� 			
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
public:
	const RoleSkill* getSkillByType(E_ROLESKILLTYPE pType)const;
protected:

};
//google::protobuf::Message			���Ǽ̳���
//����
class MonsterRoleData :public RoleBaseData
{ 
public:
	MonsterRoleData();
	static MonsterRoleData* create();
	int mId;					//�ؿ�����ID
	int monsterId;				//����ID(��ʶID)
	int batch;					//����
	float delay;				//�ӳ�ʱ��
	bool isBoss;				//�Ƿ�Ϊboss
	int move1;					//�෽���ƶ�����				(Ӧ��Ҫ��ɾ�������ֶ�,���ﶼ����ǰ��,�������������,���ٶ������ƹ�����ƶ�����,���ǹ��ﻹ��һ����Ӧ���ƶ�����,����Ҳ�����ֶ��޹���)
	int  quality;				//Ӣ��Ʒ�� 1��ɫ,2��ɫ,3��ɫ,4��ɫ,5��ɫ
	void readData(const protos::common::Monster* monster);
};
//Ӣ��
class HeroRoleData :public RoleBaseData 
{
public:
	static HeroRoleData* create();
	void readData(const protos::common::Hero* hero);
};
#endif