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
	class BattleScene;
	class BaseRole;
	class WarManager;
	class CombatLogic;
	class CombatEffect : public CCObject
	{
	public:
		CombatEffect();
		~CombatEffect();
		virtual bool init();
		void addEvent();
		CREATE_FUNC(CombatEffect)
		void AttackNull(CCObject* ob);
		void PlayerSkill(BaseRole* alive);
		void SkillEfHandle(CCObject* ob);
		void SkillEfHandleForEnemy(CCObject* ob);
		void skillShowCallBack(CCObject* ob);
		void BattleEffect(CCObject* ob);
		void setScene(BattleScene* scene);
		void armatureMovementEventCallFunc(CCArmature * pArmature, MovementEventType type, const char *data);
		void SpineActionEnd(int trackIndex);
		void SpineActionEvent(int trackIndex, spEvent* pEvent);
		void BatterSpine(int index);
		void continuousHurt();													//计算连击伤害
		float CountAliveAtk();
		void showVsAnimate( CombatLogic *pCombatLogic );	//pvp的开场vs动画
		void showVsAnimateCallBack(CCNode *pSender);
		CC_SYNTHESIZE(int, m_PlayerNum, PlayerNum);
	private:
		BattleScene* m_Scene;
		CCArmature* _armaturePlayerSkill;
		SkeletonAnimation* m_skeletonNode;
		CPlayerSkillData* _playerSkillData;
		WarManager* mManage;
	};
};
#endif