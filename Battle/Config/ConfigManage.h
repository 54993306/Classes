/************************************************************* 
*
*
*		Data : 2016.8.17
*	
*		Name : ConfigManage
*
*		Author : Lin_Xiancheng
*
*		Description : ���ù���
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
		vector<int> mEnterArea;									//�佫����ʱ���ٻ�����
		vector<int>	mOtherEnter;								//�Է���Ӫ�佫����ս������
		vector<int> mUndefinedArea;								//δ��������,������󶼲��ɽ���
		vector<int> mMoveArea;									//���ƶ����Ӵ洢
		vector<int> mObstacleArea;								//�ϰ�����(�����ϵ�)
		vector<int> mCostArea;									//������cost���Ӵ洢

		EffectData* mEffectData;
		BuffConfig* mBuffData;
		StoryData* mStoryData;
		RoleConfig* mRoleConfig;
		ArmatureEventDataMgr* mModelEventData;
	};
};
#endif