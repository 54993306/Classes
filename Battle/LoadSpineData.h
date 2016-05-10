#ifndef __LOADSPINEDATA__
#define __LOADSPINEDATA__

/*** 
 *
 *		Data : 2016.4.12
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 处理spine动画的加载和释放
 *
 */

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>
#include <thread>
#include <functional>
using namespace spine;
class WarManager;

class LoadSpineData : public cocos2d::CCObject 
{

public: 
	LoadSpineData();
	~LoadSpineData();
	virtual bool init();
	CREATE_FUNC(LoadSpineData);

	void initStoryData();
	void AddRoleSpineID(int id);
	void AddStorySpineID(int id);
	void LoadStoryData(int id);
	void LoadRoleData(int id);
	void LoadVecRoleData();
	void LoadVecStoryData();
	void LoadSpineAnimation();
	void LoadRoleAction();
	void LoadStoryAction();
	void RealLoadTexture(const char* str);
	bool AsyncLoadTexture(const char* str);
	void LoadSpineAction(const char* json,const char* atlas,const char* key);
	void LoadTextureCallBack(CCObject* ob);
	spSkeletonData* loadSpineImmediately(const char* key,const char* path);
	void AddSpineDataInMap(const char* name,spSkeletonData* skeletonData,spAtlas*atlas);
	CC_SYNTHESIZE(bool,m_LoadSucceed,LoadSucceed)
private:
	std::vector<int> m_VecRole;
	std::vector<int> m_VecStory;
	int m_LoadSpineNum;
	int m_TotalSpineNum;
	WarManager* m_Manage;
	
};

#endif