
#ifndef __BATTLEDATACENTER__
#define __BATTLEDATACENTER__
/************************************************************* 
*
*
*		Data : 2016.6.2
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : Manage battle role data
*
*
*************************************************************/
#include <vector>

namespace google {
	namespace protobuf{
		class Message;
	}
}
using namespace std;
#define BattleData BattleSpace::BattleDataCenter::ShareBattleDataCenter()
namespace BattleSpace{
	class BaseRoleData;
	class BattleDataCenter
	{
	public:
		static BattleDataCenter* ShareBattleDataCenter();
		void releaseRoleData();
	public:
		void initBattleData( const google::protobuf::Message *pResponse,bool pWorldBoss = false );
		void initWordBossStage( const google::protobuf::Message *pResponse );
		void initNormalStage( const google::protobuf::Message *pResponse );
		const vector<BaseRoleData*>& getHeroVector()const;
		const vector<BaseRoleData*>& getMonsterVector()const;
		const vector<BaseRoleData*>& getCallRoleVector()const;
		void addCallRole(BaseRoleData* pBaseData);
	private:
		void initMonsterData(const google::protobuf::Message *pResponse);
		void initHeroData(const google::protobuf::Message *pResponse);
	private:
		vector<BaseRoleData*> mHeroVec;
		vector<BaseRoleData*>mMonsterVec;
		vector<BaseRoleData*>mCallRole;
		BattleDataCenter();
		static BattleDataCenter* mDataControl;
		class SingletonDestroy
		{
		public :
			~SingletonDestroy(){
				if (mDataControl != nullptr )
				{
					delete mDataControl;
					mDataControl = nullptr ;
				}
			}
		};
		static SingletonDestroy mDestroy;
	};
};
#endif