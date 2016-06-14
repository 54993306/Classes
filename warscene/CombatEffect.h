#ifndef _ATKEXP_H_
#define _ATKEXP_H_
#include "AppUI.h"
#include <spine/spine-cocos2dx.h>
/************************************************************************/
/*               
					战斗效果处理//  [11/5/2014 xc_lin]
*/
/************************************************************************/
using namespace spine;
class CPlayerSkillData;

namespace BattleSpace{
	class EffectInfo;
	class ActObject;
	class WarScene;
	class BaseRole;
	class BattleResult; 
	class CombatEffect : public CCObject
	{
	public:
		CombatEffect();
		~CombatEffect();
		virtual bool init();
		CREATE_FUNC(CombatEffect)
		void AttackNull(BattleResult* Result);
		void PlayerSkill(BaseRole* alive);
		void SkillEfHandle(CCObject* ob);
		void SkillEfHandleForEnemy(CCObject* ob);
		void skillShowCallBack(CCObject* ob);
		void BattleEffect(BattleResult* Result);
		void setScene(WarScene* scene);
		void armatureMovementEventCallFunc(CCArmature * pArmature, MovementEventType type, const char *data);
		void SpineActionEnd(int trackIndex);
		void SpineActionEvent(int trackIndex, spEvent* pEvent);
		void BatterSpine(int index);
		void continuousHurt();													//计算连击伤害
		float CountAliveAtk();
		CC_SYNTHESIZE(int, m_PlayerNum, PlayerNum);
	private:
		WarScene* m_Scene;
		CCArmature* _armaturePlayerSkill;
		SkeletonAnimation* m_skeletonNode;
		CPlayerSkillData* _playerSkillData;
	};
};
#endif