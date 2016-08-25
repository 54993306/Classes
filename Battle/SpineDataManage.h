#ifndef __SPINEDATAMANAGE__
#define __SPINEDATAMANAGE__

/************************************************************* 
 *
 *
 *		Data : 2016.8.16
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 2016.4.12 处理spine动画的加载和释放
 *
 *
 *************************************************************/

#include "cocos2d.h"
#include <thread>
#include <functional>
#include <spine/spine-cocos2dx.h>

using namespace std;
using namespace spine;
using namespace cocos2d;
namespace BattleSpace
{
	typedef std::pair<spSkeletonData*,spAtlas*> SpineData;
	class WarManager;
	class SpineDataManage : public CCObject 
	{
	public: 
		SpineDataManage();
		virtual ~SpineDataManage();
		virtual bool init();
		CREATE_FUNC(SpineDataManage);
		CC_SYNTHESIZE(bool,mLoadSucceed,LoadSucceed);
		spSkeletonData* loadSpineImmediately(const char* pKey,const char* pPath);
	public:
		bool isSpineModel(int pModel);
		SpineData* getSpineData(string pKey);
		void ReleaseSpineData();
		void LoadSpineAnimation();
		void AddRoleSpineID(int pRoleID);
		void AddTrapID(int pTrapID);
	protected:
		void LoadStoryData(int pStoryRole);
		void LoadRoleData(int pRoleID);
		void LoadTrapData(int pTrap);
		void LoadVecRoleData();
		void LoadVecStoryData();
		void LoadVecTrap();
		void LoadRoleAction();
		void LoadStoryAction();
		void LoadTrapAction();
		void LoadTexture(const char* pPath);
		bool AsyncLoadTexture(const char* pPath);
		void LoadActionData(const char* pJson,const char* pAtlas,const char* pKey);
		void AsyncLoadCallBack(CCObject* ob);
	private:
		int mLoadNumber;
		int mTotalNumber;
		vector<int> mVecRole;
		vector<int> mVecStory;
		vector<int> mTrapVec;
		vector<int> mSpineModels;									//记录spine的ID
		map<string,SpineData> mMapData;
	};
};
#endif