#ifndef ARMATURE_EVENT_DATA_H_
#define ARMATURE_EVENT_DATA_H_

#include "AppUI.h"

/************************************************************************/
/*         
骨骼帧事件管理
//  [11/23/2015 mr.yan]
*/
/************************************************************************/
namespace BattleSpace
{
	//帧事件数据
	struct ArmatureEvent
	{
		string		name;				//帧事件名称
		int			index;				//帧事件位置
		string		extraInfo;			//额外信息
		int			sound;				//音效
	};

	//动作-帧事件数组
	typedef map<string, vector<ArmatureEvent>> ArmatureActionMap;	

	//单个骨骼的帧事件数据集合
	class ArmatureEventData
	{
	public:
		ArmatureEventData();
		CC_SYNTHESIZE(int, m_armatureId, ArmatureId);
		ArmatureActionMap m_armatureActionMap;
		vector<ArmatureEvent*>& getEventVector(const char* sAction, int iIndex);
	private:
		vector<ArmatureEvent*> _vectorTemp;
	};

	//角色ID-角色帧事件信息
	typedef map<int, ArmatureEventData*> ArmatureEventDataMap;	

	//骨骼帧事件管理
	class ArmatureEventDataMgr : public CCObject
	{
	public:
		ArmatureEventDataMgr();
		~ArmatureEventDataMgr();
		virtual bool init();
		CREATE_FUNC(ArmatureEventDataMgr);
		ArmatureEventData* getArmatureEventData( int iArmatureId ) const;
		void reloadData();
	protected:
		ArmatureEventDataMap armatureEventDataMap;
		ArmatureEventData* mNullData;
	};
}
#endif