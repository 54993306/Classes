/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : BattleRole
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __BattleRole__
#define __BattleRole__

//�ع���������໹��һ����Ϊ���ݴ洢�Ĵ��ڣ�����ʾ����aliveObject��Ϲ���һ���������佫��Actobject(�����佫�����߼��͹����߼��ĵط�)
//̫��ط������˵��������඼�����޸ģ�ʵ����Ӧ���Խӿڵķ�ʽ������ġ��ӿ��޸��٣����Ǽ̳нӿڵ�ʵ�־���˵��
/******************* ս���佫���� ********************/
#include "cocos2d.h"
#include "Battle/BattleMacro.h"
#include "Battle/SkillEffect.h"
#include "Battle/RoleBuffData.h"
  
using namespace cocos2d;
using namespace std;
class BuffManage;
class ActObject;
class MoveObject;
class TempRole;
class TempSkill;
class WarAlive:public cocos2d::CCObject					//�������ݵ�˳��ṹ��Ϊ�˷���鿴����
{
public:
	WarAlive();
	virtual ~WarAlive();
	CREATE_FUNC(WarAlive);						
	void ResetAttackState();					//�����佫��ǰ״̬��Ϣ
	void ExcuteNextEffect();					//�����佫��ִ����һ��Ч��״̬
	bool canSummonAlive();						//�Ƿ�����ٻ��佫
	void clearHitAlive();
	SkillEffect* getCurrEffect();				//�õ��佫��ǰЧ��
	TempSkill* getCurrSkill();					//�õ��佫��ǰЧ��
	bool NextEffect();							//�жϵڶ���Ч���Ƿ��ͷ�
	int getSkillType();
	int getEffectType();
	bool captainCallNumberJudge();
	bool hasAliveByTargets(WarAlive* pAlive);
	bool pierceJudge();
	void cloakingTarget();						//�ж��ܻ�Ŀ�����Ƿ�ȫΪ�������
public:
	TempRole* role;								//��������ɫ��Ϣ
	vector<int> m_StandGrids;					//�����վλ
	vector<int> moves;							//�������ƶ���Χ
	vector<int> TouchGrids;						//�ƶ�״̬���佫����Ӵ���
	vector<int> AtkArea;						//�����������
	vector<WarAlive*> AliveS;					//�������Ŀ��
	vector<int>m_SkillArea;						//�洢�佫��ǰ��������
	vector<WarAlive*> m_AreaTargets;			//�洢������Ŀ��
	vector<WarAlive*> HittingAlive;				//�ܻ�Ŀ���б����ж���				(������buff���ܻ�Ŀ���������㴦��)
	CC_PROPERTY(BuffManage*,m_BuffManage,BuffManage);
	CC_SYNTHESIZE(ActObject*,m_ActObject,ActObject);
	CC_SYNTHESIZE(MoveObject*,m_MoveObj,MoveObject);	//�����ƶ�����
	CC_SYNTHESIZE(unsigned int,m_AliveID,AliveID);
	CC_SYNTHESIZE(int,m_Model,Model);
	CC_SYNTHESIZE(bool,m_Enemy,Enemy);					//����
	CC_PROPERTY(bool,m_NorAtk,NorAtk);					//��ͨ����״̬
	CC_SYNTHESIZE(bool,m_SpecialAtk,SpeAtk);			//���⹥��
	CC_SYNTHESIZE(bool,m_CritSkill,CriAtk);				//��ɱ��
	CC_SYNTHESIZE(int,m_MstType,MstType);				//��������
	CC_PROPERTY(int,m_CallType,CallType);				//�佫�ٻ�����(�ҷ��佫��Ϊ�ٻ������佫)
	CC_SYNTHESIZE(bool,m_AIState,AIState);				//�佫�Ƿ���AI״̬
	CC_SYNTHESIZE(bool,m_DieState,DieState);			//�佫�Ƿ���AI״̬
	CC_PROPERTY(int,m_GridIndex,GridIndex);				//λ��
	CC_PROPERTY(int,m_MoveGrid,MoveGrid);				//�ƶ�Ŀ�����
	CC_SYNTHESIZE(bool,m_TouchState,TouchState);		//�佫��ǰΪ�ƶ�״̬��
	CC_PROPERTY(int,m_TouchGrid,TouchGrid);				//�ƶ�״̬�µ�ǰλ��
	CC_SYNTHESIZE(float,m_MoveSpeed,MoveSpeed);				//�ƶ��ٶ�(��/S)
	CC_SYNTHESIZE(int,m_UILayout,UiLayout);				//��Ӧ��UI�������ID
	CC_PROPERTY(float,m_Delaytime,Delaytime);			//�佫�����ӳ�ʱ��
	CC_PROPERTY(float,m_AtkDelay,AtkDelay);				//�佫�����ӳ�ʱ��
	CC_PROPERTY(float,m_StateDelay,StatDelay);			//�佫״̬�ӳ�ʱ��
	CC_PROPERTY(int,m_AliveState,AliveStat);			//�佫״̬
	CC_PROPERTY(float,m_Atktime,Atktime);				//�������ʱ��
	CC_PROPERTY(float,m_CritTime,CritTime);				//��ɱ��ʱ��
	CC_SYNTHESIZE(float,m_AtkInterval,AtkInterval);		//����(��/��)
	CC_SYNTHESIZE(float,m_TimePercent,TimePercent);		//���������ٷֱ�
	CC_SYNTHESIZE(int,m_initCost,InitCost);				//��ʼ cost
	CC_SYNTHESIZE(int,m_CostMax,Costmax);				//cost Max
	CC_SYNTHESIZE(float,m_AddCost,AddCost);				//cost ��仯��
	CC_PROPERTY(int,m_AtkNum,AtkNum);					//��¼��������(3���ͷ����⹥��)
	CC_SYNTHESIZE(int,m_Batch,Batch);					//����
	CC_SYNTHESIZE(int,m_GroupIndex,GroupIndex);			//Ч����
	CC_SYNTHESIZE(int,m_EffectIndex,EffIndex);			//Ч��Index
	CC_PROPERTY(int,m_Hp,Hp);							//Ѫ��
	CC_SYNTHESIZE(unsigned int,m_MaxHp,MaxHp);			//Ѫ��Max
	CC_SYNTHESIZE(int,m_Atk,Atk);						//����
	CC_PROPERTY(int,m_Def,Def);							//����
	CC_SYNTHESIZE(int,m_Hit,Hit);						//����
	CC_SYNTHESIZE(int,m_Doge,Doge);						//����
	CC_SYNTHESIZE(int,m_Crit,Crit);						//����
	CC_SYNTHESIZE(AliveType,m_AliveType,AliveType);		//��ɫƷ�ʵȼ�
	CC_SYNTHESIZE(unsigned int,m_Renew,Renew);			//�ظ�
	CC_SYNTHESIZE(bool,m_Battle,Battle);				//����״̬
	CC_SYNTHESIZE(int,m_SortieNum,SortieNum);			//��ǰ������˺�����
	CC_SYNTHESIZE(int,m_FatherID,FatherID);				//���ٻ��佫�洢���佫ID
	CC_SYNTHESIZE(int,m_CallAliveNum,CallAliveNum);		//record can call alive number use int alive is captain
	CC_SYNTHESIZE(bool,m_Captain,Captain);				//�Ƿ�Ϊ�ӳ�
	CC_SYNTHESIZE(bool,m_CritEffect,CritEffect);		//��ɱ�����Ź���Ч
	CC_SYNTHESIZE(bool,m_ExecuteCap,ExecuteCap);		//�ӳ���ִ�б��
	CC_SYNTHESIZE(bool,m_Opposite,Opposite);			//����Ƿ�ת�򹥻�
	CC_SYNTHESIZE(bool,m_Move,Move);					//�Ƿ�����ƶ�
	CC_SYNTHESIZE(bool,m_cloaking,Cloaking);			//����״̬
	CC_SYNTHESIZE(int,m_Hrt,Hrt);						//�˺�����Ч��(������,������)
	CC_SYNTHESIZE(int,m_HrtPe,Hrtpe);					//�˺��ٷֱ�(������,������)
	CC_SYNTHESIZE(float,m_Zoom,Zoom);					//���ű�
	CC_SYNTHESIZE(int,m_TerType,TerType);				//����Ӱ����������
	CC_SYNTHESIZE(int,m_TerTypeNum,TerTypeNum);			//����Ӱ��������ֵ
	CC_SYNTHESIZE(bool,m_LastAlive,LastAlive);			//����״̬
protected:
	inline bool init(){return true;}
};

#endif