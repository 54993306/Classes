


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

class TempRole
{
public:
	TempRole();
	TempSkill skill1;				//普通攻击     主帅特殊攻击 主帅被动技
	TempSkill skill2;				//特殊攻击
	TempSkill skill3;				//必杀技能(必杀技有召唤的技能类型)
	TempSkill skill4;				//主帅普通攻击 			
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
class TempMonster :public TempRole
{ 
public:
	TempMonster();
	int mId;					//关卡怪物ID
	int monsterId;				//怪物ID(标识ID)
	int batch;					//批次
	float delay;				//延迟时间
	bool isBoss;				//是否为boss
	int move1;					//多方向移动怪物
	int  quality;				//英雄品质 1白色,2绿色,3蓝色,4紫色,5红色
	void readData(const protos::common::Monster &monster);
};
//英雄
class TempHero :public TempRole
{
public:
	void readData(const protos::common::Hero &hero);
};
#endif