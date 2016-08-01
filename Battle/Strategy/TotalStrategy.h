/************************************************************* 
 *
 *
 *		Data : 2016.6.22
 *	
 *		Name : TotalStrategy
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __TotalStrategy_H_
#define __TotalStrategy_H_
#include "cocos2d.h"
#include "Battle/BattleMacro.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class WarManager;
	class BaseRole;
	class PvEStrategy;
	class AutoData : public CCObject
	{
	public:
		AutoData();
		UNINITCREATE(AutoData);
		CC_SYNTHESIZE(BaseRole*,mCallHero,CallHero);
		CC_SYNTHESIZE(BaseRole*,mInterceptRole,InterceptRole);
		CC_SYNTHESIZE(int,mInterceptGrid,InterceptGrid);
	};
	
	class TotalStrategy : public CCObject
	{
	protected:
		TotalStrategy();
	public:
		virtual ~TotalStrategy();
		CREATE_FUNC(TotalStrategy);
		CC_SYNTHESIZE(bool,mExcute,Excute);
		virtual bool init();
		void addEvent();
		void removeEvent();
		void initCrossArea();
		void excuteStrategy(float dt);
		void roleStateChange(CCObject* ob);
		void buttonUpdate(CCObject* ob);
		void changeAutoState(CCObject* ob);
		void roleGridChange(CCObject* ob);
		void upSkillState(CCObject* ob);
		void updateStrategy();
		void interceptInfo();
		void initCallHero();
		void excuteCallRole();
		void initCallGrid();
		void summonHero(BaseRole* pRole,int pGrid);
		void acceptButtonState(CCObject* ob);
		bool frontHasHero(BaseRole* pMonster);
		bool hasHeroLine(vector<int>& pLines,BaseRole* pRole);
		void moveRoleObject();
		void moveUnTargetRole();
		bool frontHasMonster(BaseRole* pHero);
		BaseRole* findFreeHero();
		int meetGrid(BaseRole* pMonster,BaseRole* pHero);
		bool hasHeroMove(int pGrid);
		bool sameRow(vector<int>& pGridsA,vector<int>& pGridsB);
		bool isMoveIntercept(BaseRole* pHero,BaseRole* pMonster);		
		void playSkillOrCallRole();
		BaseRole* hasUnBattleHero();
		BaseRole* unTargetHero();
		BaseRole* hasTargetHero();
		int getGridByCrossArea(BaseRole* pRole,int pGrid = 0);
		bool unCrossHeros(vector<int>& pGrids,BaseRole* pSelf = nullptr);
		int roleContainGrid(BaseRole* pRole,int grid);
		bool vectorHasRow(vector<int>& pGirds ,int pRow);
		bool standGrid(BaseRole* pRole,int pGrid);
		BaseRole* urgencyMove();
		bool roleInLastLine(BaseRole* pRole);
		bool urgencyJudge();
	protected:
		bool mSkillState;
		bool mAutoState;
		bool mButtonState;
		WarManager* mManage;
		AutoData* mAutoData;
		vector<int> mCrossArea;
		BaseRole* mSkillRole;
		PvEStrategy* mPveStrategy;
	};
};
#endif