#ifndef __BAG_DATA__
#define __BAG_DATA__

#include "protos/protocol.h"
// #include "model/BattleData.h"
#include "cocos2d.h"
#include "CCTK/ImageView.h"
USING_NS_CC;
USING_NS_CC_WIDGET;


struct CPrize{
	CPrize();
	int type;			//奖励类型(1 道具，2 英雄，3 铜钱，4 元宝，5 粮食(世界BOSS积分), 6体力)
	int id ;				//奖励物品ID
	int num ;			//数量
	int quality;		//品质(星星)
	int thumb;		//关联图标
	int color ;		//颜色
	void read(const Prize& prize);
};


struct CExtAttr{
	int attrType;		//属性类型
	int attrVal;		//属性值
	int attrMax;		//属性最大值
// 	void read(const ExtAttr& attr);
};

struct CArmor
{

	CArmor();

	int armorType;		//装备类型(1 头盔，2 盔甲，3 鞋子，4 武器，5 首饰，6 宝物)
	int baseHp;		//基础生命值
	int baseDef;		//基础攻击速度
	int baseAtk;		//基础攻击力
	int baseDex ;		//敏捷
	int baseHit ;		//命中
	int baseCrit ;		//暴击
	int baseRenew ;		//回复
	int baseDodge ;		//闪避

	int addHp;		//每级血量成长值
	int addDef;		//每级防御成长值
	int addAtk;		//每级攻击力成长值
	int addHit;		//当前强化命中加成
	int addDex;		//当前强化敏捷加成
	int addCrit;		//当前强化暴击加成
	int addRenew;		//当前强化回复加成
	int addDodge;		//当前强化闪避加成

	int strenLv;		//强化等级
	int hero;			//英雄头像(装备在英雄身上)
	int baptize;		//装备可洗炼属性的数量


	string atkRange;		//攻击洗炼范围
	string hpRange;		//生命洗炼范围
	string defRange;
	string IntRange;		//敏捷变动范围
	string hitRange;		//命中变动范围
	string critRange;		//暴击变动范围
	string renewRange;	//回复变动范围
	string dodgeRange;	//闪避变动范围

	int nextAtk;		//下一强化等级攻击加成
	int nextHp;			//下一强化等级血量加成
	int nextDef;		//下一强化等级防御加成
	int nextHit;		//下一强化等级命中加成
	int nextDex;		//下一强化等级敏捷加成
	int nextCrit;		//下一强化等级暴击加成
	int nextRenew;		//下一强化等级回复加成
	int nextDodge;		//下一强化等级闪避加成

	int combat;			//战力

	std::vector<CExtAttr> attrList;		//附加属性
	int armor_skill;
	void read(const Armor& armor);

};

struct CProp{
	int AddType;	  //使用后获得的资源类型 (1强化成功率加成，2英雄增加经验值)
	int AddVal;	    //使用后获得的资源数量
	int param;	     //备用

	void read(const Prop& prop);
};

// struct CSoul
// {
// 	int  refId ;		//参照英雄、怪物ID
//     string name;		//英雄、怪物名称
// 	int	hp ;			//血量
// 	int	atk ;		//攻击力
// 	int	def ;		//防御
// 	int	crit;		//暴击几率
// 	int  dex ;		//敏捷
// 	int	addHp ;		//血量成长系数
// 	int	addAtk ;		//攻击成长系数
// 	int	addDef;		//防御成长系数
// 	int  hit ;		//命中
// 	int  soulType;		//类型(1 僵尸， 2 道士， 3 神将)
// 	int  thumb ;		//模型ID
// 	int  quality;		//品质 1 白色， 2 绿色， 3 蓝色，4 紫色， 5 红色
// 	int  move1 ;		//方向1移动范围
// 	int  move2 ;		//方向2移动范围
// 	int  move3;		//方向3移动范围
// 	int  move4;		//方向4移动范围
// 	int  move5;		//方向5移动范围
// 	CSkill skill1 ;	//普通攻击
// 	CSkill skill2;	//特殊攻击
// 	CSkill skill3;	//必杀技能
// 	CSkill skill4;	//主帅普通攻击
// 	CSkill skill5;	//主帅特殊攻击
// 	CSkill skill6;	//主帅必杀技
// 	CSkill skill7;	//主帅被动技
// 	void read(const Soul& sou);
// };

struct CDropStage{
	int stageId;	//关卡ID
	bool open;		//是否开启
	CDropStage();
	void read(const DropStage& drpSt);
};

struct CItem
{
	CItem();
	int id ;		//玩家道具ID
	int itemId ;	//道具标识ID
	string itemName;	//道具名称
	int itemType ;	//道具类型(1 常规道具，只显示名称和描述说明，不具有特殊属性， 2 装备，3 消耗类道具 比如药品 ……)
	int useLevel ;	//使用等级
	int moneyType  ;	//购买道具花费的货币类型
	int buyPrice  ;	//购买价格
	int sellPrice  ;	//出售价格
	int quality ;	//装备品阶(1 白色， 2 绿色， 3 蓝色，4 紫色， 5 橙色)

	int itemLevel;//道具等级

	time_t useTime ;	//道具使用时间
	time_t loseTime ;	//道具过期时间
	int itemNum ;	//拥有道具的数量
	int gridNum ;	//物品背包位置

	CArmor armor;	//装备数据 (itemType 为2时，使用该项)
	CProp prop;		//消耗类道具(itemType 为3时，使用该项)
// 	CMonster monst;
	int itparam;
	int iconId;		//道具图标ID
	bool canUse;

	int iStar;		//星级

	vector<CDropStage> stageList;
//	CSoul soul;
	bool stren;		//是否可强化
	void read(const Item& item);
};

struct CBagData
{
	int maxNum;
	std::vector<CItem> itemList;
	void read(const WareHouseResponse& res);
};

struct  CStrengthenResponse
{
	 int result ;				//强化结果（1 成功, 2 金币不足，3 铁匠铺等级限制，4 强化到最大等级, 5数据错误）
	 int maxLevel;		//最大等级
	 CItem armor ;		//要强化的装备数据
	 int coin;					//强化费用
	 int gold;				//金币强化

	 void read(const StrengthenResponse& res);
};

CCTexture2D* setItemQualityTexture(int quality);
CCTexture2D* setQualityTexture(int quality);

CImageView *getPrizeSprite(const CPrize *prize);

CImageView *getItemSprite(const CItem *item);

CCSprite * addItemLight(int quality, CCNode *parent, CCPoint pos= CCPointZero);

typedef void (CCObject::*SEL_SelectItem)(CItem*); 
#define ccw_select_Item(_SELECTOR_) (SEL_SelectItem)(&_SELECTOR_)

#endif