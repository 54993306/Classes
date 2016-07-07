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
namespace BattleSpace
{
	class BuffManage;
	class RoleObject;
	class MoveObject;
	class BaseRoleData;
	class RoleSkill;
	class skEffectData;
	class WarManager;
	class SkillRange;
	class GuardArea;
	class HurtCount;
	class BattleRoleLayer;
	class CombatGuideManage;
	class BaseRole:public CCObject			//�������ݵ�˳��ṹ��Ϊ�˷���鿴����
	{
	protected:
		BaseRole();
		WarManager* mManage;
		GuardArea* mGuardArea;
		SkillRange* mSkillRange;
		HurtCount* mHurtCount;
		CombatGuideManage* mGuideManage;
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
		BaseRole* getAbsentCallRole();
		BaseRole* getCallRole( RoleSkill* skill );
		BaseRole* createCallRole(int CallId);
		void attackEventLogic();
		bool soriteNumberEnd();
		void initContainGrid(int pGrid,vector<int>& pVector);
		bool singleGrid();
		/****************** Hero *********************/
		void heroCritEffect();
		void HeroExcuteAI();
		void heroGuard();
		bool IsAutoMoveType();
		/******************* Monster ********************/
		void monsterCritEffect();
		void MonsterExcuteAI(float dt); 
		bool monsterFlee();
		//monster move Logic
		bool MonstMoveExcute();
		int getMonsterMove();
		//hero touch move
		bool moveToTouchEndGrid();
		bool unCommonAlive();
		bool movePrecondition();
		bool moveJudge();
		bool borderJudge(vector<int>& pVector);
		bool swappingRule(vector<int>& pDestinations);
		vector<BaseRole*> getAliveInArea(vector<int>& pAreas);
		bool vectorIntersection(vector<int>& pVector,vector<int>& ptVector);
		void moveSwappingAlives(vector<BaseRole*>& pVector,int pOffs);
		bool callAliveJudge(vector<int>& pDestinations);
		PROPERTY_CONSTREAD(int,mCommandGrid,CommandGrid);				//��	��������
	public:
		virtual ~BaseRole();
		virtual bool init();
		CREATE_FUNC(BaseRole);
		void ResetAttackState();										//�����佫��ǰ״̬��Ϣ
		void ExcuteNextEffect();										//�����佫��ִ����һ��Ч��״̬
		bool canSummonAlive();											//�Ƿ�����ٻ��佫
		void clearHitAlive();
		skEffectData* getCurrEffect();									//�õ��佫��ǰЧ��
		RoleSkill* getCurrSkill();										//�õ��佫��ǰЧ��
		bool NextEffect();												//�жϵڶ���Ч���Ƿ��ͷ�
		int getSkillType();
		bool captainCallNumberJudge();
		bool hasAliveByTargets(BaseRole* pAlive);
		void cloakingTarget();											//�ж��ܻ�Ŀ�����Ƿ�ȫΪ�������
		bool inStandGrid(int pGrid);
		bool inStandVector(vector<int>& pGrids);
		void createRoleObject();
		bool critJudge();
		void initAliveData();
		void initAliveByFather(BaseRole*pFather);
		vector<BaseRole*>* getSkillTargets();
	public:
		//public
		CC_SYNTHESIZE(BaseRoleData*,mBaseData,BaseData);
		CC_SYNTHESIZE(BaseRoleData*,mLogicData,LogicData);
		std::vector<int> mStandGrids;									//�����վλ
		std::vector<int> mSkillArea;									//�洢�佫��ǰ��������
		std::vector<BaseRole*> mAreaTargets;							//�洢������Ŀ��
		std::vector<BaseRole*> HittingAlive;							//�ܻ�Ŀ���б����ж���				(������buff���ܻ�Ŀ���������㴦��)
		CC_SYNTHESIZE_READONLY(BuffManage*,mBuffManage,BuffManage);		//(�ǲ���Ӧ�ñ�¶��ȥ��)
		CC_SYNTHESIZE(RoleObject*,mRoleObject,RoleObject);
		CC_SYNTHESIZE(MoveObject*,mMoveObject,MoveObject);				//�����ƶ�����
		CC_SYNTHESIZE(BattleRoleLayer*,mRoleLayer,RoleLayer);			//��ʾ�����
		CC_SYNTHESIZE(unsigned int,m_AliveID,AliveID);					//�佫ID
		PROPERTY_CONSTREAD(bool,m_NorAtk,NorAtk);						//��ͨ����״̬
		CC_SYNTHESIZE(bool,m_SpecialAtk,SpeAtk);						//���⹥��
		CC_SYNTHESIZE(bool,m_CritSkill,CriAtk);							//��ɱ��
		CC_SYNTHESIZE(bool,mHasTarget,HasTarget);						//�����ܻ�Ŀ��
		CC_SYNTHESIZE(bool,m_Enemy,Enemy);								//����(�佫����)Ӧ�������ݻ����д��ڣ������߼��и����������ֵҲ����
		PROPERTY_CONSTREAD(sCallType,mCallType,CallType);				//�佫�ٻ�����(�ҷ��佫��Ϊ�ٻ������佫)
		CC_SYNTHESIZE(bool,mAliveState,AliveState);						//���״̬
		PROPERTY_CONSTREAD(int,mGridIndex,GridIndex);					//λ��
		ROLE_READONLY(int,mMaxGrid,MaxGrid);							//���λ��
		CC_SYNTHESIZE(int,m_MoveGrid,MoveGrid);							//�ƶ�Ŀ�����
		CC_SYNTHESIZE(float,m_MoveSpeed,MoveSpeed);						//�ƶ��ٶ�(��/S)
		CC_SYNTHESIZE(float,mDelaytime,Delaytime);						//�佫�����ӳ�ʱ��
		CC_SYNTHESIZE(sLogicState,mLogicState,LogicState);				//�佫�߼�״̬				(���������߼�����ȫ�����佫���ڲ����д���)
		PROPERTY_CONSTREAD(float,m_Atktime,Atktime);					//�������ʱ��
		CC_SYNTHESIZE(float,m_AtkInterval,AtkInterval);					//����(��/��)
		PROPERTY_CONSTREAD(int,m_AtkNum,AtkNum);						//��¼��������(3���ͷ����⹥��)
		CC_SYNTHESIZE(int,m_GroupIndex,GroupIndex);						//Ч����
		CC_SYNTHESIZE(int,m_EffectIndex,EffIndex);						//Ч��Index
		CC_PROPERTY(bool,mBattle,Battle);								//����״̬
		CC_SYNTHESIZE(int,m_SortieNum,SortieNum);						//��ǰ������˺�����
		CC_SYNTHESIZE(int,m_FatherID,FatherID);							//���ٻ��佫�洢���佫ID
		CC_SYNTHESIZE(bool,m_CritEffect,CritEffect);					//��ɱ�����Ź���Ч
		CC_SYNTHESIZE(bool,m_Move,Move);								//�Ƿ�����ƶ�
		CC_SYNTHESIZE(float,m_Zoom,Zoom);								//���ű�
	public:
		//data
		CC_SYNTHESIZE(int,m_Model,Model);								//�佫ģ��
		PROPERTY_CONSTREAD(int,m_Hp,Hp);								//Ѫ��
		CC_SYNTHESIZE(unsigned int,m_MaxHp,MaxHp);						//Ѫ��Max
		CC_SYNTHESIZE(int,m_Atk,Atk);									//����
		PROPERTY_CONSTREAD(int,m_Def,Def);								//����
		CC_SYNTHESIZE(int,m_Hit,Hit);									//����
		CC_SYNTHESIZE(int,m_Doge,Doge);									//����
		CC_SYNTHESIZE(int,m_Crit,Crit);									//����
		CC_SYNTHESIZE(unsigned int,m_Renew,Renew);						//�ظ�
	public:
		//unused
		CC_SYNTHESIZE(int,m_Hrt,Hrt);									//�˺�����Ч��(������,������)
		CC_SYNTHESIZE(int,m_HrtPe,Hrtpe);								//�˺��ٷֱ�(������,������)
		CC_SYNTHESIZE(int,m_TerType,TerType);							//����Ӱ����������
		CC_SYNTHESIZE(int,m_TerTypeNum,TerTypeNum);						//����Ӱ��������ֵ
	public:
		//hero
		vector<int> mTouchGrids;										//�ƶ�״̬���佫����Ӵ���
		CC_SYNTHESIZE(bool,m_AIState,AIState);							//�佫�Ƿ���AI״̬
		CC_SYNTHESIZE(bool,m_TouchState,TouchState);					//�佫��ǰΪ�ƶ�״̬��
		PROPERTY_CONSTREAD(int,m_TouchGrid,TouchGrid);					//�ƶ�״̬�µ�ǰλ��
		CC_SYNTHESIZE(int,m_UILayout,UiLayout);							//��Ӧ��UI�������ID
		CC_SYNTHESIZE(float,m_AtkDelay,AtkDelay);						//�佫�����ӳ�ʱ��(�ҷ��佫�ͷű�ɱ��ʱʹ��)
		CC_SYNTHESIZE(int,m_initCost,InitCost);							//��ʼ cost
		CC_SYNTHESIZE(int,m_CostMax,Costmax);							//cost Max
		CC_SYNTHESIZE(float,m_AddCost,AddCost);							//cost ��仯��
		CC_SYNTHESIZE(int,m_CallAliveNum,CallAliveNum);					//record can call alive number use int alive is captain
		CC_SYNTHESIZE(bool,m_Captain,Captain);							//�Ƿ�Ϊ�ӳ�
		CC_SYNTHESIZE(bool,m_ExecuteCap,ExecuteCap);					//�ӳ���ִ�б��
		CC_SYNTHESIZE(bool,m_Opposite,Opposite);						//����Ƿ�ת�򹥻�
		CC_SYNTHESIZE(bool,mAutoState,AutoState);						//�Զ�ս��״̬
	public:
		//monster
		CC_SYNTHESIZE(sBehavior,mBehavior,Behavior);					//��������
		PROPERTY_CONSTREAD(float,m_CritTime,CritTime);					//��ɱ��ʱ��
		CC_SYNTHESIZE(int,m_Batch,Batch);								//����
		CC_SYNTHESIZE(sMonsterSpecies,mMonsterSpecies,MonsterSpecies);	//��ɫƷ�ʵȼ�				Ŀǰ�÷�������boss�������ж�����չ�Ŀ���
		CC_SYNTHESIZE(bool,m_cloaking,Cloaking);						//����״̬
		CC_SYNTHESIZE(bool,m_LastAlive,LastAlive);						//��������佫
	};
};
#endif