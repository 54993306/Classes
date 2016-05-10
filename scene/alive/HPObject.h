#ifndef _HPOBJECT_H_
#define _HPOBJECT_H_
#include "cocos2d.h"
#include "AppUI.h"
class AliveObject;

USING_NS_CC;
class HPObject : public CCNode
{
public:
	HPObject();
	virtual ~HPObject();
	CREATE_FUNC(HPObject);
	void setSkin(const char* bgSkin,const char* hpSkin,bool isEnemy = false);
	void setHp(float hp);
	void setMaxHp(float maxHp);
	inline int getHp(){return m_hpVal;}
	inline int getMaxHp(){return m_maxHpVal;}
	void playerNum(AliveObject* target,int num,int type);
	void showHp(CCObject* ob);
	void hideHp(float dt);
/***
 *@Data : 2016.4.20
 *@MethodName : bool hitEffect(ActObject* target);
 *@Author : Lin_Xiancheng
 *@Description : 武将收集叠加特效,添加一张图片做淡出操作,若存在淡出特效则不播放变红效果
 *
 *@brief : 武将可能在其他动作时播放特效，后续可能添加的是特效而不是图片,spine骨骼动画,帧动画等拓展,涉及到动作和资源加载
 */ 
	bool hitEffect(AliveObject* target);
	inline float getPercent(){return m_percent;}
	CCProgressTimer* getPro();
protected:
	void updateShow();
protected:
	CCProgressTimer* m_hp;
	CCSprite* m_bg;
	float m_hpVal;
	float m_maxHpVal;
	float m_percent;
};
enum LostHpType
{
	nullType = 0,		//不显示掉血内容
	typeface ,			//显示字体类内容；1：闪避
	gainType,			//加血
	angerType,			//怒气类型
	generalType,		//一般型掉血
	cutType,			//减少型掉血
	addType,			//增加型掉血
	genralCritType,		//一般型暴击
	cutCritType,		//减少型暴击
	addCritType,		//增加型暴击
};
#endif // !_HPOBJECT_H_
