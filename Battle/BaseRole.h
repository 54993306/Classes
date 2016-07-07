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
	class BaseRole:public CCObject			//定义数据的顺序结构是为了方便查看数据
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
		PROPERTY_CONSTREAD(int,mCommandGrid,CommandGrid);				//触	摸结束点
	public:
		virtual ~BaseRole();
		virtual bool init();
		CREATE_FUNC(BaseRole);
		void ResetAttackState();										//重置武将当前状态信息
		void ExcuteNextEffect();										//重置武将下执行下一个效果状态
		bool canSummonAlive();											//是否可以召唤武将
		void clearHitAlive();
		skEffectData* getCurrEffect();									//得到武将当前效果
		RoleSkill* getCurrSkill();										//得到武将当前效果
		bool NextEffect();												//判断第二个效果是否释放
		int getSkillType();
		bool captainCallNumberJudge();
		bool hasAliveByTargets(BaseRole* pAlive);
		void cloakingTarget();											//判断受击目标内是否全为隐身对象
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
		std::vector<int> mStandGrids;									//多格子站位
		std::vector<int> mSkillArea;									//存储武将当前技能区域
		std::vector<BaseRole*> mAreaTargets;							//存储区域内目标
		std::vector<BaseRole*> HittingAlive;							//受击目标中被击中对象				(用于做buff和受击目标死亡结算处理)
		CC_SYNTHESIZE_READONLY(BuffManage*,mBuffManage,BuffManage);		//(是不是应该暴露出去呢)
		CC_SYNTHESIZE(RoleObject*,mRoleObject,RoleObject);
		CC_SYNTHESIZE(MoveObject*,mMoveObject,MoveObject);				//设置移动对象
		CC_SYNTHESIZE(BattleRoleLayer*,mRoleLayer,RoleLayer);			//显示对象层
		CC_SYNTHESIZE(unsigned int,m_AliveID,AliveID);					//武将ID
		PROPERTY_CONSTREAD(bool,m_NorAtk,NorAtk);						//普通攻击状态
		CC_SYNTHESIZE(bool,m_SpecialAtk,SpeAtk);						//特殊攻击
		CC_SYNTHESIZE(bool,m_CritSkill,CriAtk);							//必杀技
		CC_SYNTHESIZE(bool,mHasTarget,HasTarget);						//存在受击目标
		CC_SYNTHESIZE(bool,m_Enemy,Enemy);								//怪物(武将类型)应该在数据基类中存在，或是逻辑中根据情况来赋值也可以
		PROPERTY_CONSTREAD(sCallType,mCallType,CallType);				//武将召唤类型(我方武将均为召唤类型武将)
		CC_SYNTHESIZE(bool,mAliveState,AliveState);						//存活状态
		PROPERTY_CONSTREAD(int,mGridIndex,GridIndex);					//位置
		ROLE_READONLY(int,mMaxGrid,MaxGrid);							//最大位置
		CC_SYNTHESIZE(int,m_MoveGrid,MoveGrid);							//移动目标格子
		CC_SYNTHESIZE(float,m_MoveSpeed,MoveSpeed);						//移动速度(格/S)
		CC_SYNTHESIZE(float,mDelaytime,Delaytime);						//武将出现延迟时间
		CC_SYNTHESIZE(sLogicState,mLogicState,LogicState);				//武将逻辑状态				(关于它的逻辑可以全放在武将的内部进行处理)
		PROPERTY_CONSTREAD(float,m_Atktime,Atktime);					//攻击间隔时间
		CC_SYNTHESIZE(float,m_AtkInterval,AtkInterval);					//攻速(次/秒)
		PROPERTY_CONSTREAD(int,m_AtkNum,AtkNum);						//记录攻击次数(3次释放特殊攻击)
		CC_SYNTHESIZE(int,m_GroupIndex,GroupIndex);						//效果组
		CC_SYNTHESIZE(int,m_EffectIndex,EffIndex);						//效果Index
		CC_PROPERTY(bool,mBattle,Battle);								//上阵状态
		CC_SYNTHESIZE(int,m_SortieNum,SortieNum);						//当前回造成伤害次数
		CC_SYNTHESIZE(int,m_FatherID,FatherID);							//被召唤武将存储父武将ID
		CC_SYNTHESIZE(bool,m_CritEffect,CritEffect);					//必杀技播放过特效
		CC_SYNTHESIZE(bool,m_Move,Move);								//是否可以移动
		CC_SYNTHESIZE(float,m_Zoom,Zoom);								//缩放比
	public:
		//data
		CC_SYNTHESIZE(int,m_Model,Model);								//武将模型
		PROPERTY_CONSTREAD(int,m_Hp,Hp);								//血量
		CC_SYNTHESIZE(unsigned int,m_MaxHp,MaxHp);						//血量Max
		CC_SYNTHESIZE(int,m_Atk,Atk);									//攻击
		PROPERTY_CONSTREAD(int,m_Def,Def);								//防御
		CC_SYNTHESIZE(int,m_Hit,Hit);									//命中
		CC_SYNTHESIZE(int,m_Doge,Doge);									//闪避
		CC_SYNTHESIZE(int,m_Crit,Crit);									//暴击
		CC_SYNTHESIZE(unsigned int,m_Renew,Renew);						//回复
	public:
		//unused
		CC_SYNTHESIZE(int,m_Hrt,Hrt);									//伤害减免效果(正加深,负减少)
		CC_SYNTHESIZE(int,m_HrtPe,Hrtpe);								//伤害百分比(正加深,负减少)
		CC_SYNTHESIZE(int,m_TerType,TerType);							//地形影响属性类型
		CC_SYNTHESIZE(int,m_TerTypeNum,TerTypeNum);						//地形影响属性数值
	public:
		//hero
		vector<int> mTouchGrids;										//移动状态下武将多格子处理
		CC_SYNTHESIZE(bool,m_AIState,AIState);							//武将是否在AI状态
		CC_SYNTHESIZE(bool,m_TouchState,TouchState);					//武将当前为移动状态下
		PROPERTY_CONSTREAD(int,m_TouchGrid,TouchGrid);					//移动状态下当前位置
		CC_SYNTHESIZE(int,m_UILayout,UiLayout);							//对应的UI控制面板ID
		CC_SYNTHESIZE(float,m_AtkDelay,AtkDelay);						//武将攻击延迟时间(我方武将释放必杀技时使用)
		CC_SYNTHESIZE(int,m_initCost,InitCost);							//初始 cost
		CC_SYNTHESIZE(int,m_CostMax,Costmax);							//cost Max
		CC_SYNTHESIZE(float,m_AddCost,AddCost);							//cost 秒变化率
		CC_SYNTHESIZE(int,m_CallAliveNum,CallAliveNum);					//record can call alive number use int alive is captain
		CC_SYNTHESIZE(bool,m_Captain,Captain);							//是否为队长
		CC_SYNTHESIZE(bool,m_ExecuteCap,ExecuteCap);					//队长技执行标记
		CC_SYNTHESIZE(bool,m_Opposite,Opposite);						//标记是否转向攻击
		CC_SYNTHESIZE(bool,mAutoState,AutoState);						//自动战斗状态
	public:
		//monster
		CC_SYNTHESIZE(sBehavior,mBehavior,Behavior);					//怪物类型
		PROPERTY_CONSTREAD(float,m_CritTime,CritTime);					//必杀技时间
		CC_SYNTHESIZE(int,m_Batch,Batch);								//批次
		CC_SYNTHESIZE(sMonsterSpecies,mMonsterSpecies,MonsterSpecies);	//角色品质等级				目前用法是世界boss，但是有多种拓展的可能
		CC_SYNTHESIZE(bool,m_cloaking,Cloaking);						//隐身状态
		CC_SYNTHESIZE(bool,m_LastAlive,LastAlive);						//场上最后武将
	};
};
#endif