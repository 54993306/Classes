/** 
 *
 *		Data : 2016.4.20
 *	
 *		Name : RoleConfig
 *
 *		Author : Lin_Xiancheng
 *
 *		Description :  解析处理武将配置数据
 *
 */
#ifndef __ROLECONFIG__
#define __ROLECONFIG__
#include "AppUI.h"

//定义的数据结构不用对外展示,将可能的用法在CRoleData中实现
class RoleConfig : public cocos2d::CCObject
{

public:
	RoleConfig();
	virtual bool init(){return true;}
	CREATE_FUNC(RoleConfig);

public:
	CC_SYNTHESIZE(int,		m_ModeID,					ModeID);				//根据模型来索引不同武将的配置数据
	CC_SYNTHESIZE(int,		m_MoveSound,				MoveSound);				//移动音效
	CC_SYNTHESIZE(int,		m_HitEffect,				HitEffect);				//
	CC_SYNTHESIZE(int,		m_HitEffectX,				HitEffectX);
	CC_SYNTHESIZE(int,		m_HitEffectY,				HitEffectY);
	CC_SYNTHESIZE(float,	m_HitEffectScale,			HitEffectScale);
	CC_SYNTHESIZE(bool,		m_Turn,						Turn);


//还没想好怎么去写的模式
	CC_SYNTHESIZE(int,		m_ActionEffectType,			ActionEffectType);		//动作特效类型
	CC_SYNTHESIZE(int,		m_ActionEffectID,			ActionEffectID);
	CC_SYNTHESIZE(int,		m_ActionEffectX,			ActionEffectX);
	CC_SYNTHESIZE(int,		m_ActionEffectY,			ActionEffectY);
};	


class CRroleData
{
public:
	CRroleData();
	~CRroleData();
	void AddConfigData(RoleConfig* Data);
	void ClearData();
	CCNode* getActionEffect(int ModeID);
	RoleConfig* getConfigData(int ModeId);
	int getMoveSound(int ModeID);
	bool isTurn(int id);
	void initData();
private:
	std::map<int,RoleConfig*> m_MapConfigData;
};


#endif
