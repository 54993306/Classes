/************************************************************* 
 *
 *
 *		Data : 2016.6.22
 *	
 *		Name : Strategy
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __Strategy_H_
#define __Strategy_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class WarManager;
	class BattleModel;
	class BaseRole;
	class AutoData;
	class TotalStrategy;
	class PvEStrategy : public CCObject
	{
	public:
		PvEStrategy();
		virtual ~PvEStrategy();
		virtual bool init();
		void initCostInfo();
		void setTotalStrategy(TotalStrategy* pStrategy);
		CREATE_FUNC(PvEStrategy);
		void excuteStrategy();
		void updatePvEStrategy();
		void loopPvEBattle(float dt);
		void interceptInfo();
		void initCallHero();
		bool frontHasOther(BaseRole* pHero);
		void excuteCallRole();
		void summonHero(BaseRole* pRole,int pGrid);
		void setSummonGrid(BaseRole* pRole,int pGrid);
		void moveRoleObject();
		BaseRole* findFreeHero();
		bool frontHasHero(BaseRole* pOther);
		bool urgencyJudge();
		BaseRole* urgencyMove();
		bool hasOtherMove(int pGrid);
		void callOtherRole();
		BaseRole* hasUnBattleOther();
		BaseRole* hasTargetOther();
		void playSkill();
		void moveUnTargetRole(BaseRole* tOther);
		void freeOtherMove();
		bool hasSameRowByHero(BaseRole* pOther);
		bool hasSameRowByOther(int pGrid);
		void otherEnterMaxGrid(BaseRole* pOther);
	protected:
		WarManager *mManage;
		BattleModel *mBattleModel;
		bool mPvEBattle;
		bool mExcuteStrategy;
		vector<BaseRole*>mOtherRoles;
		vector<BaseRole*>mHeros;
		vector<BaseRole*>mAliveOthers;
		float mMaxCost;							//维护两个cost值
		float mCurrCost;
		float mCostSpeed;
		AutoData *mAutoData;
		TotalStrategy* mTotal;
		BaseRole* mSkillRole;
	};
};
#endif