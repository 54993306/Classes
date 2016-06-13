#ifndef _SCOPECOUNT_H_
#define _SCOPECOUNT_H_
#include "AppUI.h"

/************************************************************************/
/*                  
用于获取技能范围的区域得到范围内的对象		//  [9/9/2014 xc_lin]
*/
/************************************************************************/

namespace BattleSpace{

	class BaseRole;
	class AreaCountInfo;
	class WarManager;
	class SkillRange : public CCObject
	{
	public:
		SkillRange(WarManager* pManage);
		static SkillRange* create(WarManager* pManage);
		void getSelfArea(BaseRole* pAlive);
		void initValidGrids(BaseRole* pAlive,vector<int>& pValids);								//得到武将有效攻击格子
		void FixGrid(AreaCountInfo& pInfo);														//固定格子
		void FixAlive(AreaCountInfo& pInfo);													//固定武将
		void lowestAlive(AreaCountInfo& pInfo);													//血量最少单位处理
		void initSkillArea(BaseRole* pAlive,vector<int>& pVector);								//受击区域
		void initAreaTargets(BaseRole* pAlive);												//受击武将
		void initAttackInfo(BaseRole* pAlive);													//受击信息(战斗处提升效率核心方法)

		int CaptainGuard(BaseRole* pAlive);
		void initSkillEffectArea(BaseRole* pAlive,vector<int>& pVector);
	private:
		WarManager* mManage;
	};

};
#endif