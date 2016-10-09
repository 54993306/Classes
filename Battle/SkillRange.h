#ifndef _SCOPECOUNT_H_
#define _SCOPECOUNT_H_
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
using namespace cocos2d;
namespace BattleSpace
{
	class BaseRole;
	class AreaCountInfo;
	class SkillRange : public CCObject
	{
	public:
		SkillRange();
		static SkillRange* create();
		void initValidGrids(BaseRole* pAlive,vector<int>& pValids);								//得到武将有效攻击格子
		void FixGrid(AreaCountInfo& pInfo);														//固定格子
		void FixAlive(AreaCountInfo& pInfo);													//固定武将
		void lowestAlive(AreaCountInfo& pInfo);													//血量最少单位处理
		void initSkillArea(BaseRole* pAlive,vector<int>& pVector);								//受击区域
		void initAreaTargets(BaseRole* pAlive);													//受击武将
		void initAttackInfo(BaseRole* pAlive);													//受击信息(战斗处提升效率核心方法)
		int CaptainGuard(BaseRole* pAlive);
	};

};
#endif