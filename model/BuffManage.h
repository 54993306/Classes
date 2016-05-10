#ifndef _BUFF_MANAGER_H_
#define _BUFF_MANAGER_H_
#include "AppUI.h"
#include <map>
#include "BattleData.h"

using namespace std;
class WarAlive;
//  [12/10/2014 xc_lin]

/********************** BUFF 定义 ****************************/
class BuffInfo : public CCObject
{
public:
	BuffInfo();
	CREATE_FUNC(BuffInfo);
public:
	CC_SYNTHESIZE(int,m_Id,ID);							//bufID
	CC_SYNTHESIZE(int,m_Type,Type);						//buf类型
	CC_SYNTHESIZE(float,m_Duration,Duration);			//持续时间
	CC_PROPERTY(float,m_ExTime,ExTime);					//执行时间
	CC_SYNTHESIZE(int,m_ChangeNum,ChangeNum);			//影响数值
	CC_SYNTHESIZE(int,m_Precent,Precent);				//影响百分比
	CC_SYNTHESIZE(int,m_Rate,Rate);						//触发概率(必定添加但是不一定触发，不触发则不显示移除出buf管理器)
	CC_SYNTHESIZE(bool,m_Dbuf,Dbuf);					//buf是否为减益buf
	CC_SYNTHESIZE(int,m_Target,Target);					//目标，1：自己，2：我方目标，3：敌方目标
	CC_SYNTHESIZE(int,m_Level,Level);					//等级
	CC_SYNTHESIZE(bool,m_Excute,Excute);				//是否为持续性Buf
	CC_SYNTHESIZE(bool,m_Handle,Handle);				//是否进行过处理（非持续性buf只处理一次，流血类每回合都需要处理）
	CC_SYNTHESIZE(string,m_bfname,bfname);				//Buff名称
	CC_SYNTHESIZE(bool,m_AddFirst,AddFirst);			//是否为第一次添加
protected:
	virtual bool init(){return true;}
};

/******************************BUFF 管理	*******************************/
typedef map<unsigned int,BuffInfo*> BuffMap;//buf ID，buf
typedef map<int,vector<CCNode*>>	BuffEffectMapList;	//buf DI,  Effect Vector

class BuffManage : public CCObject
{
public:
	BuffManage();
	virtual ~BuffManage();
	CREATE_FUNC(BuffManage);
	CC_SYNTHESIZE(WarAlive*,m_alive,Alive);
	void Buffclear();
	void clearDbuf();											//清除减益buf
	BuffInfo* getbuff(int bufID);
	BuffInfo* getbuffbyType(int buftype);
	void removeBuf(int id);
	void AddBuff(CBuff& buf);									//将buff对象添加到武将身上
	bool AddBuffLogic(CBuff& buff);								//buf替换添加逻辑
	void upDateBuff(float dt);
	BuffMap* getBuffMap(){return &m_BuffMap;}
	vector<CCNode*>* getVecEffect(int buffid);					//判断是否已经存在效果数组
	void AddEffectVec(int buffid,vector<CCNode*>& Vec);
	BuffEffectMapList* getEffectMap(){return &m_EffectMap;}
	void ExcuteBuff(BuffInfo*bfinfo,bool handel = true);		//为false时清除buf对武将的影响/*buf处理类型分发函数*/

	int  bufCurrHpHandle(BuffInfo* bfinfo,bool handel);			//当前血量	/*可以存在当前的原因是buf移除不用恢复*/
	void bufAtkHandle(BuffInfo* bfinfo,bool handel);			//攻击		/*区分持续和非持续影响的属性*/
	void bufDefHandle(BuffInfo* bfinfo,bool handel);			//防御
	void bufCritHandle(BuffInfo* bfinfo,bool handel);			//暴击
	void bufDogeHandle(BuffInfo* bfinfo,bool handel);			//闪避
	void bufHitHandle(BuffInfo* bfinfo,bool handel);			//命中
	void bufMSpeedHandle(BuffInfo* bfinfo,bool handel);			//移动速度
	void bufAtkSpeedHandle(BuffInfo* bfinfo,bool handel);		//攻击速度

	int  bufMaxHpHandle(BuffInfo* bfinfo,bool handel);			//最大血量
	void bufHrtHandle(BuffInfo* bfinfo,bool handel);			//伤害
protected:
	BuffMap m_BuffMap; 
	BuffEffectMapList m_EffectMap;										//存储武将身上的表现效果。
	bool init(){return true;}			
private:
	int m_BufeffectID;		//人物身上表现出来的buf效果id
};

//遵循等级替换规则
enum BUFFTYPE
{
	CLEBF = 0,				//清除减益BF  ctrl+shift+u小写转大写，ctrl+u大写转小写
	CURRHP,					//持续型血量变化		1
	ATK,					//攻击				2
	DEF,					//防御				3
	CRI,					//暴击				4
	DOGE,					//闪避				5
	HIT,					//命中				6
	MOVESPEED,				//移动速度			7
	ATKSPEED,				//攻击速度			8
	FIRING,					//灼烧				9
	BLEED,					//流血

	CONTROL_ONE = 100,		//定身
	/*未使用*/
	HRT = 1000,				//伤害				2
	HPMAX,					//改变最大血量		5
};
#endif // _BUFFMANAGE_