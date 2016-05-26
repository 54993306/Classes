
#ifndef  __CMONSTER_
#define __CMONSTER_
#include <string>
#include <protos/protocol.h>
#include "bag/bagData.h"
using namespace std;

//enum HeroType
//{
//	FireType = 1,	//火属性武将
//	WaterType,		//水属性武将
//	WoodType,		//木属性武将
//};
////技能类型(1 普通攻击，2 特殊攻击，3 必杀技，4 召唤技，5 队长技)没有必要分skill1-skill7了,不过已经是分了才加的这个类型
//enum ROLESKILLTYPE
//{
//	NorAtk = 1,
//	SpeAtk,
//	CriAtk,
//	CallAtk,
//	CapAtk, 
//};
//enum struct AliveType
//{
//	Common,					//普通型武将
//	Elite,					//精英型武将
//	Boss,					//boss(我方主帅类)
//	WorldBoss,				//世界boss
//};
//技能Buff
//影响类型(0:清除DeBuff，1:攻击，2:伤害，3:命中，4:防御，5:血量，6:暴击，7:迟缓，8:减速，9:沉默，10:定身，11:昏迷，12:沉睡，13:冻结，14:混乱，15:能量)

struct CBuff
{
	CBuff();				//详细参见datadefine Bufinfo
	int		buffId;			//buff ID
	int		buffType;		//buff影响类型
	int		damage;			//buff影响数值
	int		damage_rate;	//buff影响百分比
	int		useRate;		//buff触发概率
	bool	debuf;			//是否为减益buff
	int		duration;		//buff持续时间
	int		pTarget;		//目标，1：自己，2：受击目标，3：敌方目标
	int		level;			//buff等级
	int		typelimit;		//种族限制(火1,木2,水3,0是不限制)
	string	name;			//buff名称
	void readData(const protos::common::Buff &buff);
};

struct CEffect
{
	CEffect();				//详细参见datadefine Bufinfo
	vector<CBuff> buffList;	//buf列表
	int	effectId;			//效果ID
	int group;				//效果组
	int pos;				//效果组内位置
	int Efftype;			//效果类型
	int	userRate;			//触发概率
	int	cost;				//怒气值影响(可增减)
	int	damage;				//伤害百分比
	int	hurt;				//真实伤害
	int	pTarget;			//效果影响的对象(1:友方  2:敌方  3:敌我双方)
	int	batter;				//连击数(怪物配置一次召唤多少怪物时使用)
	int	repel;				//受击移动距离(受击对象移动距离，分敌我双方)
	int	erange;				//伤害浮动值(百分比)
	int	mode;				//攻击范围(1 直线群体...27 随机固定武将区域)
	int	distance;			//攻击距离 间隔多少行
	int	range;				//技能范围
	int pro_Type;			//属性影响类型(召唤数量限制)
	int pro_Rate;			//属性影响比率(加血吸血使用)
	string name;			//效果名称
	int	element;			//元素类型影响(1:冰 2:火 3:雷)
	int	element_hurt;		//元素伤害
	void readData(const protos::common::Effect &effect);
};
bool EffectSort(CEffect Effect1,CEffect Effect2);//{return Effect1.pos>Effect2.pos;}
struct CSkill
{
	CSkill();
	vector<vector<CEffect>>EffectList;
	int		id;					//英雄技能ID(唯一)
	int		skillId;			//技能ID
	int		skillType;			//技能类型( ROLESKILLTYPE )
	int		useRate;			//成功施放概率(种族限制)
	unsigned int cost;			//释放技能所需的cost
	int		star;				//技能开放星级
	int		pTarget;			//队长技种族限制(召唤类技能对应召唤类武将id,CallID)
	int		pro_type;			//限制类型
	int		pro_rate;			//限制百分比	
	int		coldDown;			//技能cd
	string	name;				//技能名称
	int		level;				//技能等级
	int     maxLevel;			//技能最大等级   
	bool	move;				//人物是否位移至目标前方
	void readData(const protos::common::Skill &skill);
};

struct ROLE
{
	ROLE();
	CSkill skill1;				//普通攻击     主帅特殊攻击 主帅被动技
	CSkill skill2;				//特殊攻击
	CSkill skill3;				//必杀技能(必杀技有召唤的技能类型)
	CSkill skill4;				//主帅普通攻击 
	CSkill skill5;				//主帅特殊攻击
	CSkill skill6;				
	CSkill skill7;				
	vector<CBuff> buffList;		//武将进入战场时带进来的buf
	int thumb;					//模型id
	bool enemy;					//是否为怪物
	int alert;					//警戒区域类型
	int alertRange;				//警戒区域范围
	float MoveSpeed;			//武将移动速度(格/S)
	int coldDown;				//死亡重新上阵CD
	int initCost;				//初始cost
	int useCost;				//上阵消耗cost
	int addCost;				//cost恢复速度(1/s)
	int maxCost;				//cost上限
	float atkInterval;			//攻击速度 (秒/次)
	int roletype;				//武将属性(火1,水2,木3)
	int row;					//武将所占行数
	int col;					//武将所占列数
	int maxhp;					//最大血量值(世界boss专用)
	int	hp;						//血量
	int	atk;					//攻击
	int	def;					//防御
	int	crit;					//暴击
	int hit;					//命中
	int dodge;					//闪避
	int dex;					//敏捷
	int renew;					//回复
	bool battle;				//武将是否为上阵状态(战斗中标记队长)
	int zoom;					//缩放比

	int grid;					//武将站位
	bool isCall;				//是否为召唤类武将(敌方怪物都可能,在怪物列表中读取)
	int	CallID;					//召唤武将ID
	float mCritTime;			//怪物释放必杀技时间
	int MstType;				//怪物类型
	int CallType;				//召唤类型(Control)
	int hasitem;				//掉落物品类型1、蓝2、金3、彩4、道具5、金币
	int	level;					//武将等级
	string name;				//名称
};
//怪物
struct CMonster :public ROLE
{
	CMonster();
	int mId;					//关卡怪物ID
	int monsterId;				//怪物ID(标识ID)
	int batch;					//批次
	float delay;				//延迟时间
	bool isBoss;				//是否为boss
	int move1;					//多方向移动怪物
	int move2;
	int move3;
	int move4;
	int move5;
	int  quality;				//英雄品质 1白色,2绿色,3蓝色,4紫色,5红色
	void readData(const protos::common::Monster &monster);
};
//英雄
struct CHero :public ROLE
{
	CHero();
	int	id;						//玩家英雄ID(全服唯一)
	int heroid;					//武将id
	int	recLevel;				//可招募等级
	int exp;
	int nextExp;
	int	coin ;					//招募价格
	int	addHp;					//血量成长系数
	int	addAtk;					//攻击成长系数
	int	addDef;					//防御成长系数
	int	addDex;					//敏捷加成
	int addHit;					//命中加成
	int addCrit;				//暴击加成
	int addDodge;				//闪避加成
	int	addRenew;				//回复加成
	int maxLevel;				//最大等级
	int maxQuality;				//最大品阶
	int starExp;				//当前突破经验
	int nextStarExp;			//突破升级经验
	int prate;					//英雄生产效率
	int combat;					//战斗力
	int index;					//位置(布阵)
	int quality;				//英雄品质 
	int food ;					//招募花费的粮食数量
	bool recruit;				//是否已满足招募条件
	CItem armor1;				//装备1
	CItem armor2;				//装备2
	CItem armor3;				//装备3
	CItem armor4;				//装备4
	int armorNum;				//解锁的装备位置数量
	vector<CItem> recItemList;	//招募英雄需要的道具列表
	bool  remove;               //若英雄已驻守，是否可撤离
	int  star;					//英雄星级
	int itemId;					//进阶或招募所用到的道具ID
	int itemNum1;				//需求碎片数量1
	int itemNum2;				//需求碎片数量2
	bool own;					//是否已拥有该英雄
	int iColor;					//1 白色， 2 绿色， 3 蓝色，4 紫色， 5 红色
	bool evol;					//英雄是否可进阶
	void readData(const protos::common::Hero &hero);
};

struct CTerrain
{
	CTerrain();
	int  id;					//关卡地形ID
	int  terrainId;				//地形标识ID
	string name;				//地形名称
	int  terrainType;			//地形类型
	int  pro_type;				//影响属性
	int  pro_rate;				//影响属性值
	int  damage;				//地形造成的伤害
	int  posX;					//在地图上的位置X
	int  posY;					//在地图上的位置Y
	int  triggerNum;			//被触发次数
	int	 existNum;				//存在时间
	int  batch;					//批次
	CBuff buff;					//地形附带buf 
	void readData(const protos::common::Trap &terr);
};
//场地效果类型与参数说明
//风暴：参数1表示风向(1聚拢，2扩散，3向左，4向右，5向上，6向下)，参数2表示移动格子数
//陨石：参数1表示伤害，参数2为0
//死者：参数1、2为0，参数3为出现的怪物
//黑暗：无
//泥地：参数1表示基本范围变化类型(文档中的5种类型)，参数2为0
//复活：无
//场地效果
struct CSceneTrap
{
	CSceneTrap();
	CMonster monster;	//参数3(死者效果参数)
	int id;				//陷阱id 
	int trapType;		//效果类型 (1 风暴，2 陨石，3 死者，4 黑暗，5 泥地，6 复活)
	string name;		//效果名称
	int interval;		//发动间隔(秒)
	int limit;			//最大发动次数
	int param1;			//参数1
	int param2;			//参数2
	int batch;			//批次
	void readData(const protos::FieldEff &fieldeff);
};
//战斗数据初始化
struct BattleDataInit 
{
	vector<CMonster> monsterList;
	vector<CHero> heroList;
	vector<CTerrain> terrList;
};
#endif // ! __CMONSTER_