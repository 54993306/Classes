#ifndef _ACTOBJECT_H_
#define _ACTOBJECT_H_
#include "AliveObject.h"
#include "scene/state/StateDefine.h"

class WarAlive;
class SpecialEffectInfo;
class EffectData;
class WarControl;
class ArmatureEventData;
class WarMapData;

enum ActObjectTag
{
	Crit_Effect = 0,
	Boil_Effect,
	hit_tag,					//标记受击特效
	RousedState_Eff,			//buf效果id
};

enum struct EventType
{
	BeginEvevt,
	HitEvent,
	SharkEvent,
	Mask,
	Die,
};

class ActObject : public AliveObject,public StateMachine
{
public:
	ActObject();
	virtual ~ActObject();
	CREATE_FUNC(ActObject);
	virtual bool init();
	virtual void setModel(int model);						//重写父类的方法
	virtual void setActionKey(string actionKey);			//
	virtual void setAlive(WarAlive* var);
	virtual void setMoveState(int var = Walk_Index);
	void SpineActionEvent(int trackIndex,spEvent* Event);
	void SpineActionEnd(int trackIndex);
	void SpineComplete(int trackIndex,int loopCount);
	void AnimationEvent(CCArmature *armature, MovementEventType movementType, const char *movementID);
	void ActionEnd(const char* ActionName);
	void removeAct(CCNode* node);
	void updateFrameEvent(float dt);
	void updatePosition(float dt);							//刷新武将位置
	void MoveUpdate(float dt);
	void TurnToStand(float dt);
	CC_SYNTHESIZE(bool,m_Reset,Reset);						//武将是否置空过(引导重置用)
private:
	WarMapData* m_MapData;
	int m_lastFrame;
	ArmatureEventData* m_armatureEventData;
};


class MoveObj :public CCNode
{
public:
	MoveObj();
	virtual bool init();
	CREATE_FUNC(MoveObj);
	CC_PROPERTY(ActObject*,m_Actobject,ActObject);
	CC_PROPERTY(WarAlive*,m_Alive,Alive);
	CC_PROPERTY(int,m_grid,grid);
	vector<int> grids;
private:

};

#endif // !_ACTOBJECT_H_