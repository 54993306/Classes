


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
	RoleSkill skNormal;				//普通攻击
	RoleSkill skSpecial;			//特殊攻击
	RoleSkill skActive;				//必杀技能(必杀技有召唤的技能类型)
	RoleSkill skCaptain;			//主帅技能 			
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
public:
	const RoleSkill* getSkillByType(E_ROLESKILLTYPE pType)const;
protected:

};
//google::protobuf::Message			都是继承与
//怪物
class MonsterRoleData :public RoleBaseData
{ 
public:
	MonsterRoleData();
	static MonsterRoleData* create();
	int mId;					//关卡怪物ID
	int monsterId;				//怪物ID(标识ID)
	int batch;					//批次
	float delay;				//延迟时间
	bool isBoss;				//是否为boss
	int move1;					//多方向移动怪物				(应该要被删除掉的字段,怪物都是往前走,方向参数无意义,靠速度来控制怪物的移动即可,但是怪物还有一个相应的移动规则,不过也跟该字段无关了)
	int  quality;				//英雄品质 1白色,2绿色,3蓝色,4紫色,5红色
	void readData(const protos::common::Monster* monster);
};
//英雄
class HeroRoleData :public RoleBaseData 
{
public:
	static HeroRoleData* create();
	void readData(const protos::common::Hero* hero);
};
#endif