

#ifndef __RoleBuff__
#define __RoleBuff__
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
#include "cocos2d.h"
using namespace cocos2d;
namespace protos{
	namespace common{
		class Buff;
	}
}

//用于对服务器数据做转换和接收
namespace BattleSpace
{
	enum struct sBuffType
	{
		eClearDebuff							=0,//清除减益Buff
		eCurrBlood								=1,//持续型血量变化		
		eAttack									=2,//攻击				
		eDefense								=3,//防御				
		eCrit									=4,//暴击				
		eDodge									=5,//闪避				
		eHit									=6,//命中
		eMoveSpeed								=7,//移动速度			
		eAttackSpeed							=8,//攻击速度
		eFiring									=9,//灼烧
		eBleed									=10,//流血
		eUnMove									=100,//定身	
	};
	enum struct sRoleNature;
	class BuffData  : public CCObject
	{
	public:
		static BuffData* create();
		CC_SYNTHESIZE(int,m_BuffID,BuffID);					//buff ID
		CC_SYNTHESIZE(int,m_BuffType,BuffType);				//buff影响类型
		CC_SYNTHESIZE(int,m_ChangeNum,ChangeNum);			//影响数值
		CC_SYNTHESIZE(int,m_Precent,Precent);				//影响百分比
		CC_SYNTHESIZE(int,m_TriggerRate,TriggerRate);		//buff触发概率
		CC_SYNTHESIZE(bool,m_IsDBuff,IsDBuff);				//是否为减益buff
		CC_SYNTHESIZE(float,m_BuffDuration,BuffDuration);	//buff持续时间
		CC_SYNTHESIZE(int,m_BuffTarget,BuffTarget);			//目标，1：自己，2：受击目标，3：敌方目标
		CC_SYNTHESIZE(int,m_BuffLevel,BuffLevel);			//buff等级
		CC_SYNTHESIZE(sRoleNature,mProperty,Property);		//种族限制(火1,木2,水3,0是不限制)
		CC_SYNTHESIZE(std::string,m_BuffDes,BuffDes);		//buff名称
		void readData(const protos::common::Buff* buff);
		bool isProperty(sRoleNature pNature) const;
	protected:
		BuffData();												//详细参见datadefine Bufinfo		
	};
};

#endif