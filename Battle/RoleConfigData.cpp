
/************************************************************* 
*
*
*		Data : 2016.8.29
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#include "Battle/RoleConfigData.h"

namespace BattleSpace
{
	RoleConfigData::RoleConfigData():m_ModeID(0),m_MoveSound(542),m_Turn(false),m_HitEffect(0)
		,m_HitEffectX(0),m_HitEffectY(0),m_HitEffectScale(1),mVariant(false),mVariantCooldown(0)
		,mVariantDuration(0),mVariantCooling(0),mVariantRate(0),mVariantEffect(0),mVariantModel(0)
		,mVariantScale(1),mHPPosition(0)
	{}

	CCNode* RoleConfigData::getActionEffect()
	{
		if (getHitEffect())				//当前的需求只有这个,先把这个写出来后面再拓展
		{
			char str[60] = {0};
			sprintf(str,"ActionEffect/%d.png",getHitEffect());
			CCSprite* sp = CCSprite::create(str);
			if (sp)
			{
				sp->setPosition(ccp(getHitEffectX(),getHitEffectY()));
				sp->setScale(getHitEffectScale());
				sp->runAction(CCSequence::create(CCFadeOut::create(0.3f),CCRemoveSelf::create(),nullptr));
				return sp;
			}else{
				CCLOG("[ *ERROR ] RoleConfig::getActionEffect %s",str);
			}
		}
		CCSprite* sp = CCSprite::create();
		sp->runAction(CCSequence::create(CCFadeOut::create(0.3f),CCRemoveSelf::create(),nullptr));
		return sp;
	}
}
