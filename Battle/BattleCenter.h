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
#define BattleGuide BattleSpace::BattleCenter::shareManageCenter()->getCombatGuideMg()
#define BattleHurtCount BattleSpace::BattleCenter::shareManageCenter()->getHurtCount()

namespace BattleSpace
{
	class WarManager;
	class CoordsManage;
	class TrapManage;
	class CombatGuideManage;
	class SpineDataManage;
	class ConfigManage;
	class AreaManage;
	class BattleModel;
	class HurtCount;
	class BattleCenter
	{
	public:
		void clearBattleData();
		CoordsManage* getCoordsManage();
		WarManager* getWarManage();
		TrapManage* getTrapManage();
		CombatGuideManage* getCombatGuideMg();				//3
		SpineDataManage* getSpineManage();
		ConfigManage* getConfigManage();
		AreaManage* getAreaManage();
		BattleModel* getBattleModel();
		HurtCount* getHurtCount();
	protected:
		WarManager *mWarManage;
		CoordsManage *mMapManage;
		TrapManage *mTrapManage;
		CombatGuideManage* mGuideManage;
		SpineDataManage* mSpineDataManage;
		ConfigManage* mConfigManage;
		AreaManage* mAreaManage;
		BattleModel* mBattleModel;
		HurtCount* mHurtcount;
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