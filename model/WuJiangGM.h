#ifndef _WUJIANG_GM_H_
#define _WUJIANG_GM_H_
#include "DataDefine.h"
#include <map>
#include <vector>
using namespace std;
class WuJiang
{
public:
	WuJiang()
		:uid(0)
		,cfgID(0)
		,level(0)
		,exp(0)
		,currHp(0)
		,maxHp(0)
		,attack(0)
		,maxAttack(0)
		,defense(0)
		,maxDef(0)
		,hit(0)
		,crit(0)
		,speed(0)
		,hurtDef(0)
		,hurtExt(0)
		,attackSpeed(0)
		,critSkill(0)
		,mainSkill(0)
	{

	}
	unsigned int uid;//唯一ID
	int cfgID; //配置静态ID
	int level;
	unsigned int exp;
	int currHp;
	int maxHp;
	int attack;
	int maxAttack;//攻击力培养上限
	int defense;	//防御
	int maxDef;//防御力培养上限
	int hit;//命中
	int crit;//暴击
	int speed;//移动速度
	float hurtDef;//伤害减免
	float hurtExt;//伤害加成
	int attackSpeed;//先攻值
	vector<int> subSkill;//普通技
	int critSkill;//必杀技
	int mainSkill;//主将技
};

//武将管理类
class WuJiangGM
{
public:
	WuJiangGM();
	~WuJiangGM();
	typedef map<int,WuJiangCfg*> CfgData;
	typedef map<unsigned int,WuJiang*> WuJiangData;
public:
	const WuJiangCfg* getCfg(int id);//由武将ID得到武将CSV数据
	WuJiang* getWuJiang(unsigned int uid);
	void addWuJiang(WuJiang* data);
	vector<const WuJiang*> getAll();//得到拥有的所有武将
protected:
	void parse();//解析csv/wujiang.csv文件
protected:
	CfgData m_cfg;
	WuJiangData m_list;
};
#endif // !_WUJIANG_GM_H_
