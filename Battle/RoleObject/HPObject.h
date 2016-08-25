#ifndef _HPOBJECT_H_
#define _HPOBJECT_H_
#include "cocos2d.h"
#include "AppUI.h"
namespace BattleSpace
{
	class AliveObject;
	class BaseRole;
	enum struct PlayHpType;
	class HPObject : public CCNode
	{
	public:
		HPObject();
		virtual ~HPObject();
		CREATE_FUNC(HPObject);
		void initHp(AliveObject* pAlive);
		void initBackground(const char* pPath);
		void initSkinByEnemy();
		void initSkin(const char* pPath);
		void setSkinDirection();
		CC_PROPERTY(float,m_HpNumber,HpNumber);
		CC_PROPERTY(float,m_HpNumberMax,HpNumberMax);
		void playChangeNumber(int pChangeNumber,PlayHpType pType);
		void showHp(CCObject* ob);
		void hideHp(float dt);
	protected:
		void updateShow();
		CCSequence* normalAction(int pDistance);
		CCSequence* critAction(int pDistance);
		CCSequence* critBackgroundAction(int pDistance);
		void missEffect();
		void critBackgroundEffect(int pDistance);
		void runActionByType(PlayHpType pType,CCNode* pLabel);
		void offsByEnemy(CCNode* pLabel);
		void gainNumberPlay(int pChangeNumber);
		void lostNumberPlay(int pChangeNumber,PlayHpType pType);
		int maxMoveDistance(CCNode* pLabel);
	protected:
		CCProgressTimer* m_Skin;
		CCSprite* m_Background;
		float m_Percent;
		BaseRole* m_Alive;
		AliveObject* m_ActObject;
	};
}
#endif // !_HPOBJECT_H_
