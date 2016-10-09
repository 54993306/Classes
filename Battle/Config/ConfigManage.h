/************************************************************* 
*
*
*		Data : 2016.8.17
*	
*		Name : ConfigManage
*
*		Author : Lin_Xiancheng
*
*		Description : 配置管理
*
*
*************************************************************/

#ifndef __ConfigManage_H_
#define __ConfigManage_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class EffectData;
	class BaseRole;
	class BuffConfig;
	class EffectInfo;
	class StoryData;
	class RoleConfig;
	class RoleConfigData;
	class ArmatureEventDataMgr;
	
	class ConfigManage : public CCObject
	{
	public:
		ConfigManage();
		virtual ~ConfigManage();
		virtual bool init();
		CREATE_FUNC(ConfigManage);
		void clearBattleConfig();
		void initBattleData();
	public:
		const vector<int>& getMoveVec() const;
		const vector<int>& getEnterVec() const;
		const vector<int>& getAddcostVec() const;
		const vector<int>& getOtherEnter() const;
		const vector<int>& getNndefinedArea() const;
		const vector<int>& getObstacleArea() const;
		bool inMoveArea(int pGrid);
		bool inEnterArea(int pGrid);
		bool inOtherEnter(int pGrid);
		bool inAddCostArea(int pGrid);
		bool inUnDefineArea(int pGrid);
		bool inObstacleArea(int pGrid);

		const EffectData* getEffData() const;
		const BuffConfig* getBuffData() const;
		const StoryData* getStoryData() const;
		const EffectInfo* getEffectInfo(int pInfoID) const;
		const RoleConfigData* getConfigData(int pModel) const;
		const ArmatureEventDataMgr* getArmatureDataMgr() const;
	public:
		void initObstacle(vector<int>& pVector);
	public:
		void initRoleSkillInfo(int pEffectID,BaseRole* pRole);
		void initRoleConfigData(BaseRole* pRole);
	private:
		vector<int> mEnterArea;									//武将创建时的召唤区域
		vector<int>	mOtherEnter;								//对方阵营武将进入战场区域
		vector<int> mUndefinedArea;								//未定义区域,任意对象都不可进入
		vector<int> mMoveArea;									//可移动格子存储
		vector<int> mObstacleArea;								//障碍区域(配置上的)
		vector<int> mCostArea;									//可增加cost格子存储

		EffectData* mEffectData;
		BuffConfig* mBuffData;
		StoryData* mStoryData;
		RoleConfig* mRoleConfig;
		ArmatureEventDataMgr* mModelEventData;
	};
};
#endif