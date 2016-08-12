/************************************************************* 
*
*
*		Data : 2016.8.11
*	
*		Name : ManageCenter
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __ManageCenter_H_
#define __ManageCenter_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
#define ManageCenter BattleSpace::BattleCenter::shareManageCenter()
#define BattleManage BattleSpace::BattleCenter::shareManageCenter()->getWar()

namespace BattleSpace
{
	class WarManager;
	class MapManager;
	class CRroleData;
	class TerrainManager;
	class CombatGuideManage;

	class BattleCenter
	{
	public:
		MapManager* getMap();
		WarManager* getWar();
		TerrainManager* getTer();
		CRroleData* getRoleConfig();
		CombatGuideManage* getCombatGuideMg();
	protected:
		CRroleData *mRoleConfigData;
		WarManager *mWarManage;
		MapManager *mMapManage;
		TerrainManager *mTerrainManage;
		CombatGuideManage* mGuideManage;
	public:
		virtual ~BattleCenter();
		static BattleCenter* shareManageCenter();
		void rleaseData();
	private:
		static BattleCenter* mManageCenter;
		BattleCenter();	
		class ManageSingletonDestory
		{
		public:
			~ManageSingletonDestory()
			{
				if (mManageCenter != nullptr)
				{
					delete mManageCenter;
					mManageCenter = nullptr;
				}
			}
		};
		static ManageSingletonDestory mDestory;
	};
};
#endif