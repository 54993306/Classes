
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
namespace google {
	namespace protobuf{
		class Message;
	}
}

namespace protos{
	namespace common{
		class Monster;
		class Hero;
	}
}
class MonsterData;
class HeroData;
using namespace std;
#include <vector>
class BattleDataCenter
{
public:
	static BattleDataCenter* ShareBattleDataCenter();
	void releaseRoleData();
public:
	void initBattleData( const google::protobuf::Message *pResponse,bool pWorldBoss = false );
	void initWordBossStage( const google::protobuf::Message *pResponse );
	void initNormalStage( const google::protobuf::Message *pResponse );
	const  vector<HeroData*>&  getHeroVector()const	;
	const vector<MonsterData*>& getMonsterVector()const;
	const vector<MonsterData*>& getCallRoleVector();
private:
	void initMonsterData(const protos::common::Monster* pData);
	void initHeroData(const protos::common::Hero* pData);
private:
	vector<HeroData*> mHeroVec;
	vector<MonsterData*>mMonsterVec;
	vector<MonsterData*>mCallRole;
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

#define BattleData BattleDataCenter::ShareBattleDataCenter()

#endif