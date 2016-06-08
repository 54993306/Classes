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

//重构方向，这个类还是一样作为数据存储的存在，跟显示对象aliveObject组合构成一个完整的武将。Actobject(包含武将独立逻辑和公用逻辑的地方)
//太多地方引用了导致整个类都不敢修改，实际上应该以接口的方式来处理的。接口修改少，但是继承接口的实现就难说了
/******************* 战场武将数据 ********************/
#include "cocos2d.h"
#include "Battle/BattleMacro.h"
#include "Battle/BattleRoleMacro.h"

using namespace std;
namespace BattleSpace{
	class BuffManage;
	class ActObject;
	class MoveObject;
	class BaseRoleData;
	class RoleSkill;
	class skEffectData;
	class BaseRole:public cocos2d::CCObject			//定义数据的顺序结构是为了方便查看数据
	{
	protected:
		BaseRole();
	public:
		virtual ~BaseRole();
		static BaseRole* create();
		void ResetAttackState();					//重置武将当前状态信息
		void ExcuteNextEffect();					//重置武将下执行下一个效果状态
		bool canSummonAlive();						//是否可以召唤武将
		void clearHitAlive();
		const skEffectData* getCurrEffect();		//得到武将当前效果
		const RoleSkill* getCurrSkill();			//得到武将当前效果
		bool NextEffect();							//判断第二个效果是否释放
		int getSkillType();
		bool captainCallNumberJudge();
		bool hasAliveByTargets(BaseRole* pAlive);
		bool pierceJudge();
		void cloakingTarget();						//判断受击目标内是否全为隐身对象
		bool standInGrid(int pGrid);
		bool critJudge();
		void initAliveData();
		void initAliveByFather(BaseRole*pFather);
	public:
		//public
		CC_SYNTHESIZE(BaseRoleData*,mBaseData,BaseData);
		CC_SYNTHESIZE(BaseRoleData*,mLogicData,LogicData);
		std::vector<int> mStandGrids;					//多格子站位
		std::vector<int> mSkillArea;						//存储武将当前技能区域
		std::vector<BaseRole*> mAreaTargets;				//存储区域内目标
		std::vector<BaseRole*> HittingAlive;				//受击目标中被击中对象				(用于做buff和受击目标死亡结算处理)
		CC_PROPERTY_CONSTREAD(BuffManage*,m_BuffManage,BuffManage);								//(是不是应该暴露出去呢)
		CC_SYNTHESIZE(ActObject*,m_ActObject,ActObject);
		CC_SYNTHESIZE(MoveObject*,m_MoveObj,MoveObject);	//设置移动对象
		CC_SYNTHESIZE(unsigned int,m_AliveID,AliveID);		//武将ID
		CC_PROPERTY_CONSTREAD(bool,m_NorAtk,NorAtk);		//普通攻击状态
		CC_SYNTHESIZE(bool,m_SpecialAtk,SpeAtk);			//特殊攻击
		CC_SYNTHESIZE(bool,m_CritSkill,CriAtk);				//必杀技
		CC_SYNTHESIZE(bool,m_Enemy,Enemy);					//怪物(武将类型)应该在数据基类中存在，或是逻辑中根据情况来赋值也可以
		CC_PROPERTY_CONSTREAD(int,m_CallType,CallType);		//武将召唤类型(我方武将均为召唤类型武将)
		CC_SYNTHESIZE(bool,m_DieState,DieState);			//武将阵亡
		CC_PROPERTY_CONSTREAD(int,m_GridIndex,GridIndex);	//位置
		CC_PROPERTY_CONSTREAD(int,m_MoveGrid,MoveGrid);		//移动目标格子
		CC_SYNTHESIZE(float,m_MoveSpeed,MoveSpeed);			//移动速度(格/S)
		CC_PROPERTY_CONSTREAD(float,m_Delaytime,Delaytime);	//武将出现延迟时间
		CC_PROPERTY_CONSTREAD(int,m_AliveState,AliveStat);	//武将逻辑状态				(关于它的逻辑可以全放在武将的内部进行处理)
		CC_PROPERTY_CONSTREAD(float,m_Atktime,Atktime);		//攻击间隔时间
		CC_SYNTHESIZE(float,m_AtkInterval,AtkInterval);		//攻速(次/秒)
		CC_PROPERTY_CONSTREAD(int,m_AtkNum,AtkNum);			//记录攻击次数(3次释放特殊攻击)
		CC_SYNTHESIZE(int,m_GroupIndex,GroupIndex);			//效果组
		CC_SYNTHESIZE(int,m_EffectIndex,EffIndex);			//效果Index
		CC_SYNTHESIZE(bool,m_Battle,Battle);				//上阵状态
		CC_SYNTHESIZE(int,m_SortieNum,SortieNum);			//当前回造成伤害次数
		CC_SYNTHESIZE(int,m_FatherID,FatherID);				//被召唤武将存储父武将ID
		CC_SYNTHESIZE(bool,m_CritEffect,CritEffect);		//必杀技播放过特效
		CC_SYNTHESIZE(bool,m_Move,Move);					//是否可以移动
		CC_SYNTHESIZE(float,m_Zoom,Zoom);					//缩放比
	public:
		//data
		CC_SYNTHESIZE(int,m_Model,Model);					//武将模型
		CC_PROPERTY_CONSTREAD(int,m_Hp,Hp);					//血量
		CC_SYNTHESIZE(unsigned int,m_MaxHp,MaxHp);			//血量Max
		CC_SYNTHESIZE(int,m_Atk,Atk);						//攻击
		CC_PROPERTY_CONSTREAD(int,m_Def,Def);				//防御
		CC_SYNTHESIZE(int,m_Hit,Hit);						//命中
		CC_SYNTHESIZE(int,m_Doge,Doge);						//闪避
		CC_SYNTHESIZE(int,m_Crit,Crit);						//暴击
		CC_SYNTHESIZE(unsigned int,m_Renew,Renew);			//回复
	public:
		//unused
		CC_SYNTHESIZE(int,m_Hrt,Hrt);						//伤害减免效果(正加深,负减少)
		CC_SYNTHESIZE(int,m_HrtPe,Hrtpe);					//伤害百分比(正加深,负减少)
		CC_SYNTHESIZE(int,m_TerType,TerType);				//地形影响属性类型
		CC_SYNTHESIZE(int,m_TerTypeNum,TerTypeNum);			//地形影响属性数值
	public:
		//hero
		vector<int> mTouchGrids;							//移动状态下武将多格子处理
		CC_SYNTHESIZE(bool,m_AIState,AIState);				//武将是否在AI状态
		CC_SYNTHESIZE(bool,m_TouchState,TouchState);		//武将当前为移动状态下
		CC_PROPERTY_CONSTREAD(int,m_TouchGrid,TouchGrid);	//移动状态下当前位置
		CC_SYNTHESIZE(int,m_UILayout,UiLayout);				//对应的UI控制面板ID
		CC_PROPERTY_CONSTREAD(float,m_AtkDelay,AtkDelay);	//武将攻击延迟时间(我方武将释放必杀技时使用)
		CC_SYNTHESIZE(int,m_initCost,InitCost);				//初始 cost
		CC_SYNTHESIZE(int,m_CostMax,Costmax);				//cost Max
		CC_SYNTHESIZE(float,m_AddCost,AddCost);				//cost 秒变化率
		CC_SYNTHESIZE(int,m_CallAliveNum,CallAliveNum);		//record can call alive number use int alive is captain
		CC_SYNTHESIZE(bool,m_Captain,Captain);				//是否为队长
		CC_SYNTHESIZE(bool,m_ExecuteCap,ExecuteCap);		//队长技执行标记
		CC_SYNTHESIZE(bool,m_Opposite,Opposite);			//标记是否转向攻击
	public:
		//monster
		CC_SYNTHESIZE(int,m_MstType,MstType);				//怪物类型
		CC_PROPERTY_CONSTREAD(float,m_CritTime,CritTime);	//必杀技时间
		CC_SYNTHESIZE(int,m_Batch,Batch);					//批次
		CC_SYNTHESIZE(E_ALIVETYPE,m_AliveType,AliveType);	//角色品质等级				目前用法是世界boss，但是有多种拓展的可能
		CC_SYNTHESIZE(bool,m_cloaking,Cloaking);			//隐身状态
		CC_SYNTHESIZE(bool,m_LastAlive,LastAlive);			//场上最后武将
	protected:
	};
};
#endif