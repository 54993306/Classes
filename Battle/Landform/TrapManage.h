#ifndef _TEERAIN_MANAGER_H_
#define _TEERAIN_MANAGER_H_
#include "cocos2d.h"
#include "Battle/TrapData.h"
#include "Battle/BuffData.h"
using namespace cocos2d;
using namespace std;
/************************************************************* 
 *
 *
 *		Data : 2016.8.16
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : [12/10/2014 xc_lin]
 *
 *
 *************************************************************/

namespace BattleSpace
{
	class TrapData;
	class BattleTrap;
	class BaseRole;
	class TrapManage : public CCObject
	{
	public:
		TrapManage();
		virtual ~TrapManage();
		CREATE_FUNC(TrapManage);	
		virtual bool init();
	public:
		void clear();
		void updateTrap(float dt);
		void updateActivateTrap();
		void AddTrap(BattleTrap* terrain);
		const BattleTrap* getTrap(int terrainID) const;
		const BattleTrap* getTrapByGrid(int grid) const;
		const map<int,BattleTrap*>& getBattleTraps() const;
		void initTrapData(const vector<TrapData*>& pTraps);
		void initObstacle(BaseRole* pRole,vector<int>& pVector);
	protected:
		TrapData* mTrapData;
		BattleTrap* mDefault;
		map<int,BattleTrap*> mTrapMap;											//战场上的地形
	};
};
#endif // !_TEERAIN_MANAGER_H_
