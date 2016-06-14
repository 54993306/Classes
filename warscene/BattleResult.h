#ifndef __BATTLERESULT__
#define __BATTLERESULT__

#include "AppUI.h"
/************************************************************* 
 *
 *
 *		Data : 2016.6.14
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
//每一个掉血数据，参数为掉血类型和掉血数值
namespace BattleSpace{

	struct STR_LostHp
	{
		STR_LostHp();
		int hitType;
		int hitNum;			//计算可以为浮点,最后转化为整型伤害
		int anger;
	};
	class BaseRole;
	class ActObject;
	class BattleResult : public CCObject
	{
	public:
		BattleResult();
		virtual ~BattleResult();
		virtual bool init(){return true;}
		CREATE_FUNC(BattleResult);			
		vector<unsigned int> m_HitTargets;								//受击武将ID
		map<unsigned int,BaseRole*> m_Alive_s;							//存储受击数组的武将信息
		map<unsigned int,STR_LostHp> m_LostHp;
		map<unsigned int,int> m_CurrHp;									//原先用于做血量条变化,现用于做
		map<unsigned int,int> m_Repel;									//记录击退格子
		map<unsigned int,unsigned int> m_AddBuf;
		map<unsigned int,unsigned int> m_DelBuf;
		CC_SYNTHESIZE(BaseRole*,m_alive,Alive);
		CC_SYNTHESIZE(int,m_usType,usType);								//攻击目标血量变化类型
		CC_SYNTHESIZE(int,m_usNum,usNum);								//攻击目标血量变化
	protected:
	};
};
#endif