/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : RoleBuff
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __RoleBuff__
#define __RoleBuff__
#include <protos/common/skill_common.pb.h>
#include "cocos2d.h"
using namespace cocos2d;
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
//用于对服务器数据做转换和接收
class RoleBuffData
{
public:
	RoleBuffData();				//详细参见datadefine Bufinfo		
	CC_SYNTHESIZE(int,m_BuffID,BuffID);					//buff ID
	CC_SYNTHESIZE(int,m_BuffType,BuffType);				//buff影响类型
	CC_SYNTHESIZE(int,m_ChangeNum,ChangeNum);			//影响数值
	CC_SYNTHESIZE(int,m_Precent,Precent);				//影响百分比
	CC_SYNTHESIZE(int,m_TriggerRate,TriggerRate);		//buff触发概率
	CC_SYNTHESIZE(bool,m_IsDBuff,IsDBuff);				//是否为减益buff
	CC_SYNTHESIZE(float,m_BuffDuration,BuffDuration);	//buff持续时间
	CC_SYNTHESIZE(int,m_BuffTarget,BuffTarget);			//目标，1：自己，2：受击目标，3：敌方目标
	CC_SYNTHESIZE(int,m_BuffLevel,BuffLevel);			//buff等级
	CC_SYNTHESIZE(int,m_TargetType,TargetType);			//种族限制(火1,木2,水3,0是不限制)
	CC_SYNTHESIZE(std::string,m_BuffDes,BuffDes);		//buff名称
	
	void readData(const protos::common::Buff &buff);
};
//战斗中实际使用到的buff结构,buff是可以具备很多逻辑的,但是现在buff的逻辑都放在buff管理器里面去写了。应该是接口的方式抽象出来写才对
class BuffInfo : public CCObject,public RoleBuffData
{
public:
	BuffInfo();
	CREATE_FUNC(BuffInfo);
	virtual bool init(){return true;}
public:
	CC_PROPERTY(float,m_ExTime,ExTime);					//执行时间
	CC_SYNTHESIZE(bool,m_Excute,Excute);				//是否为持续性Buf
	CC_SYNTHESIZE(bool,m_Handle,Handle);				//是否进行过处理（非持续性buf只处理一次，流血类每回合都需要处理）
	CC_SYNTHESIZE(bool,m_AddFirst,AddFirst);			//是否为第一次添加
	void initBuffInfo(RoleBuffData& pBuffData);
protected:
	
};
 
#endif