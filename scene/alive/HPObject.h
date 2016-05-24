#ifndef _HPOBJECT_H_
#define _HPOBJECT_H_
#include "cocos2d.h"
#include "AppUI.h"
class AliveObject;
class WarAlive;
USING_NS_CC;
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
	void playChangeNumber(int pChangeNumber,int pType);
	void showHp(CCObject* ob);
	void hideHp(float dt);
protected:
	void updateShow();
	CCSequence* normalAction(int pDistance);
	CCSequence* critAction(int pDistance);
	CCSequence* critBackgroundAction(int pDistance);
	void missEffect();
	void critBackgroundEffect(int pDistance);
	void runActionByType(int pType,CCNode* pLabel);
	void offsByEnemy(CCNode* pLabel);
	void gainNumberPlay(int pChangeNumber);
	void lostNumberPlay(int pChangeNumber,int pType);
protected:
	CCProgressTimer* m_Skin;
	CCSprite* m_Background;
	float m_Percent;
	WarAlive* m_Alive;
	AliveObject* m_ActObject;
};
enum LostHpType
{
	nullType			= 0,		//不显示掉血内容
	typeface			= 1,		//显示字体类内容；1：闪避
	gainType			= 2,		//加血
	angerType			= 3,		//怒气类型
	generalType			= 4,		//一般型掉血
	cutType				= 5,		//减少型掉血
	addType				= 6,		//增加型掉血
	genralCritType		= 7,		//一般型暴击
	cutCritType			= 8,		//减少型暴击
	addCritType			= 9,		//增加型暴击
};
#endif // !_HPOBJECT_H_
