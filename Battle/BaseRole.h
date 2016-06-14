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
#include "cocos2d.h"
#include "Battle/BattleMacro.h"
#include "Battle/RoleMacro.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace{
	class BuffManage;
	class ActObject;
	class MoveObject;
	class BaseRoleData;
	class RoleSkill;
	class skEffectData;
	class WarManager;
	class SkillRange;
	class GuardArea;
	class BaseRole:public CCObject			//�������ݵ�˳��ṹ��Ϊ�˷���鿴����
	{
	protected:
		BaseRole();
		WarManager* mManage;
		GuardArea* mGuardArea;
		SkillRange* mSkillRange;
	public:
		virtual void excuteLogic(float pTime);
		bool stateDispose(float pTime);
		bool delayEntrance(float pTime);
		bool autoSkillAlive();
		void attackTime(float pTime);
		bool AttackJudge();
		bool walkState();
		bool aliveAttackState();
		void AliveExcuteAI();
		void excuteCritEffect();
		void attackDirection();
		void SkillActionAndEffect(int pActionIndex,int pEffectID);
		void AliveCritEnd();
		/****************** Hero *********************/
		void heroCritEffect();
		void HeroExcuteAI();
		bool IsAutoMoveType();
		/******************* Monster ********************/
		void monsterCritEffect();
		void MonsterExcuteAI(float dt); 
		bool monsterFlee();
		//move Logic
		bool MonstMoveExcute();
		int monsterMove();
		int getMonsterMoveGrid();
		int MoveJudge(int grid);				//�����ж��Ƿ�����ƶ������վλ����
		int CountMoveGrid(int grid);
	public:
		virtual ~BaseRole();
		virtual bool init();
		CREATE_FUNC(BaseRole);
		void ResetAttackState();					//�����佫��ǰ״̬��Ϣ
		void ExcuteNextEffect();					//�����佫��ִ����һ��Ч��״̬
		bool canSummonAlive();						//�Ƿ�����ٻ��佫
		void clearHitAlive();
		skEffectData* getCurrEffect();		//�õ��佫��ǰЧ��
		RoleSkill* getCurrSkill();			//�õ��佫��ǰЧ��
		bool NextEffect();							//�жϵڶ���Ч���Ƿ��ͷ�
		int getSkillType();
		bool captainCallNumberJudge();
		bool hasAliveByTargets(BaseRole* pAlive);
		void cloakingTarget();						//�ж��ܻ�Ŀ�����Ƿ�ȫΪ�������
		bool standInGrid(int pGrid);
		bool critJudge();
		void initAliveData();
		void initAliveByFather(BaseRole*pFather);
		vector<BaseRole*>* getSkillTargets();
	public:
		//public
		CC_SYNTHESIZE(BaseRoleData*,mBaseData,BaseData);
		CC_SYNTHESIZE(BaseRoleData*,mLogicData,LogicData);
		std::vector<int> mStandGrids;					//�����վλ
		std::vector<int> mSkillArea;						//�洢�佫��ǰ��������
		std::vector<BaseRole*> mAreaTargets;				//�洢������Ŀ��
		std::vector<BaseRole*> HittingAlive;				//�ܻ�Ŀ���б����ж���				(������buff���ܻ�Ŀ���������㴦��)
		CC_SYNTHESIZE_READONLY(BuffManage*,mBuffManage,BuffManage);//(�ǲ���Ӧ�ñ�¶��ȥ��)
		CC_SYNTHESIZE(ActObject*,m_ActObject,ActObject);
		CC_SYNTHESIZE(MoveObject*,m_MoveObj,MoveObject);	//�����ƶ�����
		CC_SYNTHESIZE(unsigned int,m_AliveID,AliveID);		//�佫ID
		CC_PROPERTY_CONSTREAD(bool,m_NorAtk,NorAtk);		//��ͨ����״̬
		CC_SYNTHESIZE(bool,m_SpecialAtk,SpeAtk);			//���⹥��
		CC_SYNTHESIZE(bool,m_CritSkill,CriAtk);				//��ɱ��
		CC_SYNTHESIZE(bool,m_Enemy,Enemy);					//����(�佫����)Ӧ�������ݻ����д��ڣ������߼��и����������ֵҲ����
		CC_PROPERTY_CONSTREAD(int,m_CallType,CallType);		//�佫�ٻ�����(�ҷ��佫��Ϊ�ٻ������佫)
		CC_SYNTHESIZE(bool,m_DieState,DieState);			//�佫����
		CC_PROPERTY_CONSTREAD(int,m_GridIndex,GridIndex);	//λ��
		CC_PROPERTY_CONSTREAD(int,m_MoveGrid,MoveGrid);		//�ƶ�Ŀ�����
		CC_SYNTHESIZE(float,m_MoveSpeed,MoveSpeed);			//�ƶ��ٶ�(��/S)
		CC_PROPERTY_CONSTREAD(float,m_Delaytime,Delaytime);	//�佫�����ӳ�ʱ��
		CC_PROPERTY_CONSTREAD(int,m_AliveState,AliveStat);	//�佫�߼�״̬				(���������߼�����ȫ�����佫���ڲ����д���)
		CC_PROPERTY_CONSTREAD(float,m_Atktime,Atktime);		//�������ʱ��
		CC_SYNTHESIZE(float,m_AtkInterval,AtkInterval);		//����(��/��)
		CC_PROPERTY_CONSTREAD(int,m_AtkNum,AtkNum);			//��¼��������(3���ͷ����⹥��)
		CC_SYNTHESIZE(int,m_GroupIndex,GroupIndex);			//Ч����
		CC_SYNTHESIZE(int,m_EffectIndex,EffIndex);			//Ч��Index
		CC_SYNTHESIZE(bool,m_Battle,Battle);				//����״̬
		CC_SYNTHESIZE(int,m_SortieNum,SortieNum);			//��ǰ������˺�����
		CC_SYNTHESIZE(int,m_FatherID,FatherID);				//���ٻ��佫�洢���佫ID
		CC_SYNTHESIZE(bool,m_CritEffect,CritEffect);		//��ɱ�����Ź���Ч
		CC_SYNTHESIZE(bool,m_Move,Move);					//�Ƿ�����ƶ�
		CC_SYNTHESIZE(float,m_Zoom,Zoom);					//���ű�
	public:
		//data
		CC_SYNTHESIZE(int,m_Model,Model);					//�佫ģ��
		CC_PROPERTY_CONSTREAD(int,m_Hp,Hp);					//Ѫ��
		CC_SYNTHESIZE(unsigned int,m_MaxHp,MaxHp);			//Ѫ��Max
		CC_SYNTHESIZE(int,m_Atk,Atk);						//����
		CC_PROPERTY_CONSTREAD(int,m_Def,Def);				//����
		CC_SYNTHESIZE(int,m_Hit,Hit);						//����
		CC_SYNTHESIZE(int,m_Doge,Doge);						//����
		CC_SYNTHESIZE(int,m_Crit,Crit);						//����
		CC_SYNTHESIZE(unsigned int,m_Renew,Renew);			//�ظ�
	public:
		//unused
		CC_SYNTHESIZE(int,m_Hrt,Hrt);						//�˺�����Ч��(������,������)
		CC_SYNTHESIZE(int,m_HrtPe,Hrtpe);					//�˺��ٷֱ�(������,������)
		CC_SYNTHESIZE(int,m_TerType,TerType);				//����Ӱ����������
		CC_SYNTHESIZE(int,m_TerTypeNum,TerTypeNum);			//����Ӱ��������ֵ
	public:
		//hero
		vector<int> mTouchGrids;							//�ƶ�״̬���佫����Ӵ���
		CC_SYNTHESIZE(bool,m_AIState,AIState);				//�佫�Ƿ���AI״̬
		CC_SYNTHESIZE(bool,m_TouchState,TouchState);		//�佫��ǰΪ�ƶ�״̬��
		CC_PROPERTY_CONSTREAD(int,m_TouchGrid,TouchGrid);	//�ƶ�״̬�µ�ǰλ��
		CC_SYNTHESIZE(int,m_UILayout,UiLayout);				//��Ӧ��UI�������ID
		CC_PROPERTY_CONSTREAD(float,m_AtkDelay,AtkDelay);	//�佫�����ӳ�ʱ��(�ҷ��佫�ͷű�ɱ��ʱʹ��)
		CC_SYNTHESIZE(int,m_initCost,InitCost);				//��ʼ cost
		CC_SYNTHESIZE(int,m_CostMax,Costmax);				//cost Max
		CC_SYNTHESIZE(float,m_AddCost,AddCost);				//cost ��仯��
		CC_SYNTHESIZE(int,m_CallAliveNum,CallAliveNum);		//record can call alive number use int alive is captain
		CC_SYNTHESIZE(bool,m_Captain,Captain);				//�Ƿ�Ϊ�ӳ�
		CC_SYNTHESIZE(bool,m_ExecuteCap,ExecuteCap);		//�ӳ���ִ�б��
		CC_SYNTHESIZE(bool,m_Opposite,Opposite);			//����Ƿ�ת�򹥻�
	public:
		//monster
		CC_SYNTHESIZE(int,m_MstType,MstType);				//��������
		CC_PROPERTY_CONSTREAD(float,m_CritTime,CritTime);	//��ɱ��ʱ��
		CC_SYNTHESIZE(int,m_Batch,Batch);					//����
		CC_SYNTHESIZE(E_ALIVETYPE,m_AliveType,AliveType);	//��ɫƷ�ʵȼ�				Ŀǰ�÷�������boss�������ж�����չ�Ŀ���
		CC_SYNTHESIZE(bool,m_cloaking,Cloaking);			//����״̬
		CC_SYNTHESIZE(bool,m_LastAlive,LastAlive);			//��������佫
	};
};
#endif