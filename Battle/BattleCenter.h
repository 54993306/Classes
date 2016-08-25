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
#define BattleCoords BattleSpace::BattleCenter::shareManageCenter()->getCoordsManage()
#define BattleManage BattleSpace::BattleCenter::shareManageCenter()->getWarManage()
#define SpineManage BattleSpace::BattleCenter::shareManageCenter()->getSpineManage()
#define BattleTrapManage BattleSpace::BattleCenter::shareManageCenter()->getTrapManage()
#define BattleAreaManage BattleSpace::BattleCenter::shareManageCenter()->getAreaManage()
#define BattleConfig BattleSpace::BattleCenter::shareManageCenter()->getConfigManage()
#define BattleModelManage BattleSpace::BattleCenter::shareManageCenter()->getBattleModel()

namespace BattleSpace
{
	class WarManager;
	class CoordsManage;
	class CRroleData;
	class TrapManage;
	class CombatGuideManage;
	class SpineDataManage;
	class ConfigManage;
	class AreaManage;
	class BattleModel;
	class BattleCenter
	{
	public:
		void clearBattleData();
		CoordsManage* getCoordsManage();					//1 没有定义宏
		WarManager* getWarManage();
		TrapManage* getTrapManage();
		CRroleData* getRoleConfig();				//2属于配置的部分
		CombatGuideManage* getCombatGuideMg();		//3
		SpineDataManage* getSpineManage();
		ConfigManage* getConfigManage();
		AreaManage* getAreaManage();
		BattleModel* getBattleModel();
	protected:
		CRroleData *mRoleConfigData;
		WarManager *mWarManage;
		CoordsManage *mMapManage;
		TrapManage *mTrapManage;
		CombatGuideManage* mGuideManage;
		SpineDataManage* mSpineDataManage;
		ConfigManage* mConfigManage;
		AreaManage* mAreaManage;
		BattleModel* mBattleModel;
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