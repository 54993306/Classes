
#ifndef  __CMONSTER_
#define __CMONSTER_
#include <string>
#include <protos/protocol.h>
#include "bag/bagData.h"

using namespace std;

struct CSkill
{
	CSkill();
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
	//vector<CBuff> buffList;	//武将进入战场时带进来的buf
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
#endif // ! __CMONSTER_