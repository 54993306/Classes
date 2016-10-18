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
	class BuffData;
	class RoleObject;
	class MoveObject;
	class BaseRoleData;
	class RoleSkill;
	class skEffectData;
	class SkillRange;
	class GuardArea;
	class HurtCount;
	class BattleRoleLayer;
	class BattleModel;
	class RoleConfigData;
	class RoleVariant;
	enum struct PlayHpType;
	class BaseRole:public CCObject			//�������ݵ�˳��ṹ��Ϊ�˷���鿴����
	{
	protected:
		BaseRole();
		GuardArea* mGuardArea;
		SkillRange* mSkillRange;
		HurtCount* mHurtCount;
		skEffectData* mSkillEffect;			//�ն�������ָ���ֹ�������
		RoleVariant* mVariant;
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
		void MonsterExcuteAI(); 
		bool monsterFlee();
		//monster move Logic
		bool MonstMoveExcute();
		int getMonsterMove();
		//hero touch move
		bool moveToGrid();
		bool moveToTouchEndGrid();
		bool unCommonAlive();
		bool movePrecondition();
		bool moveJudge();
		bool borderJudge(vector<int>& pVector);
		bool swappingRule(vector<int>& pDestinations);
		vector<BaseRole*> getAliveInArea(vector<int>& pAreas);
		bool vectorIntersection(vector<int>& pVector,vector<int>& ptVector);
		bool moveSwappingAlives(vector<BaseRole*>& pVector,int pOffs);
		bool callAliveJudge(vector<int>& pDestinations);
		PROPERTY_CONSTREAD(int,mCommandGrid,CommandGrid);				//��	��������
		bool skillAreaHasTarget();
		bool currGridHasTarget();
		bool inMoveObjectArea(int pGrid);
		bool intObstacleArea(int pGrid);
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
		void updateRole(float dt);
		vector<BaseRole*>& getCurrSkillTargets();
		void roleDie();
		bool hasOtherRole(int pGrid);
		void byOtherKill(BaseRole* pRole);
		void changeBoold(float pChangeNum);
		void playBooldNum(PlayHpType pType,int pNumber);
		void buffImpact();
		void AddBuff(const BuffData* pData);
		void clearDbuff();
		void changeModel(int pModel,float pScale = 1);
		void updateRage(float pRate);
		void VariantBegin(bool pInVariant);
		void VariantEnd(bool pInVariant);
		void colorBlink(int pNumber,const ccColor3B& color3);
		int getEndGrid();
		bool isVariant();
		//��װһЩ�õ��������������Եķ���,����ֱ�ӱ�¶���������ݶ���,������ϵĲ�β��ϵ�����
	public:
		//public
		CC_SYNTHESIZE(BaseRoleData*,mBaseData,BaseData);
		CC_SYNTHESIZE(BaseRoleData*,mLogicData,LogicData);
		vector<int> mMoveGrids;											//�佫�ƶ�·��
		vector<int> mObstacle;
		vector<int> mStandGrids;										//�����վλ
		vector<int> mSkillArea;											//�洢�佫��ǰ��������
		vector<BaseRole*> mAreaTargets;									//�洢������Ŀ��
		vector<BaseRole*> mHittingAlive;								//�ܻ�Ŀ���б����ж���				(������buff���ܻ�Ŀ���������㴦��)
		CC_SYNTHESIZE_READONLY(BuffManage*,mBuffManage,BuffManage);		//(�ǲ���Ӧ�ñ�¶��ȥ��)
		CC_SYNTHESIZE(RoleObject*,mRoleObject,RoleObject);
		CC_SYNTHESIZE(MoveObject*,mMoveObject,MoveObject);				//�����ƶ�����
		CC_SYNTHESIZE(RoleConfigData*,mConfigData,ConfigData);			//��ɫ��������(ģ�͸ı�ʱ��ı�)
		CC_SYNTHESIZE(BattleRoleLayer*,mRoleLayer,RoleLayer);			//��ʾ�����
		CC_SYNTHESIZE(CCProgressTimer*,mAngerVariant,AngerVariant);		//�佫����ŭ��ֵ
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
		CC_SYNTHESIZE(bool,mFirstInit,FirstInit);						//�佫���γ�ʼ��
		CC_SYNTHESIZE(bool,mClearState,ClearState);						//�Ƿ�������״̬
	public:
		//data
		CC_SYNTHESIZE(int,m_Model,Model);								//�佫ģ��
		PROPERTY_CONSTREAD(int,m_Hp,Hp);								//Ѫ��
		CC_SYNTHESIZE(int,m_MaxHp,MaxHp);								//Ѫ��Max
		CC_SYNTHESIZE(int,m_Atk,Atk);									//����
		PROPERTY_CONSTREAD(int,m_Def,Def);								//����
		CC_SYNTHESIZE(int,m_Hit,Hit);									//����
		CC_SYNTHESIZE(int,m_Doge,Doge);									//����
		CC_SYNTHESIZE(int,m_Crit,Crit);									//����
		CC_SYNTHESIZE(int,mRegain,Regain);								//�ظ�
		CC_SYNTHESIZE(bool,mOtherCamp,OtherCamp);
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
		CC_SYNTHESIZE(bool,m_TouchState,TouchState);					//�佫��ǰΪ�ƶ�״̬��   �����һ����ʱ�����ڻ��Ƶ�״̬
		CC_SYNTHESIZE(bool,mInTouchState,InTouchState);					//�佫�Ƿ��ڱ�������״̬��
		PROPERTY_CONSTREAD(int,m_TouchGrid,TouchGrid);					//�ƶ�״̬�µ�ǰλ��
		CC_SYNTHESIZE(int,m_UILayout,UiLayout);							//��Ӧ��UI�������ID
		CC_SYNTHESIZE(float,m_AtkDelay,AtkDelay);						//�佫�����ӳ�ʱ��(�ҷ��佫�ͷű�ɱ��ʱʹ��)
		CC_SYNTHESIZE(int,m_initCost,InitCost);							//��ʼ cost
		CC_SYNTHESIZE(int,mCostMax,CostMax);							//cost Max
		CC_SYNTHESIZE(float,m_AddCost,AddCost);							//cost ��仯��
		CC_SYNTHESIZE(int,m_CallAliveNum,CallAliveNum);					//record can call alive number use int alive is captain
		CC_SYNTHESIZE(bool,m_Captain,Captain);							//�Ƿ�Ϊ�ӳ�
		CC_SYNTHESIZE(bool,m_ExecuteCap,ExecuteCap);					//�ӳ���ִ�б��
		CC_SYNTHESIZE(bool,m_Opposite,Opposite);						//����Ƿ�ת�򹥻�
		CC_SYNTHESIZE(bool,mAutoState,AutoState);						//�Զ�ս��״̬
		CC_SYNTHESIZE(float,mEnterTime,EnterTime);
		CC_SYNTHESIZE(float,mSkillTime,SkillTime);
		CC_SYNTHESIZE(bool,mUpdateSkill,UpdateSkill);
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