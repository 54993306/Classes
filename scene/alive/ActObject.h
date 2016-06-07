#ifndef _ACTOBJECT_H_
#define _ACTOBJECT_H_

/************************************************************* 
 *
 *
 *		Data : 2016.5.12
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 在战场上的一个完整的武将，由数据对象waralive和现实对象aliveobject构成(大的方向)
 *
 *
 *************************************************************/
#include "scene/alive/AliveObject.h"
#include "scene/state/StateDefine.h"
class ArmatureEventData;

namespace BattleSpace{
	class WarAlive;
	class SpecialEffectInfo;
	class EffectData;
	class WarControl;
	class WarMapData;
	class WarManager;
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
		void initStateManage();
		void SpineActionEvent(int trackIndex,spEvent* Event);
		void SpineActionEnd(int trackIndex);
		void SpineComplete(int trackIndex,int loopCount);
		void AnimationEvent(CCArmature *armature, MovementEventType movementType, const char *movementID);
		void ActionEnd(const char* ActionName);
		void removeAct(CCNode* node);
		void updateFrameEvent(float dt);
		void updatePosition(float dt);							//刷新武将位置
		void MoveUpdate(float dt);
		CC_SYNTHESIZE(bool,m_Reset,Reset);						//武将是否置空过(引导重置用)
		bool isDistination(float pDt);
		void moveEnd();
		bool firstBattle(CCPoint& p);
		void walkDirection(CCPoint& p);
		void roleMoveSpeed();
		float getMoveTime();
		void initMoveObject(CCNode* pMoveParent);
		void setActMoveGrid(int pGird);
	private:
		void actionEvent(int pFremIndex);
		void dieActionEnd();
		void attackActionEnd();
		void initSpineModel();
		void initSpineEffect();
		void initCocosModel();
		void toWlkActionDispose();
		void setCocosAction();
		void lostAction();
		void hideSpineEffect();
		void setSpineAction();
		void setSpineEffectAction();
	private:
		WarMapData* m_MapData;
		WarManager* m_Manage;
		int m_lastFrame;
		ArmatureEventData* m_armatureEventData;
	};
};
#endif // !_ACTOBJECT_H_