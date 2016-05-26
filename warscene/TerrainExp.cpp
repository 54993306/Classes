#include "TerrainExp.h"
#include "model/DataCenter.h"
#include "CombatTask.h"
#include "model/terrainLayer.h"
#include "scene/alive/AliveDefine.h"
#include "Global.h"
#include "Battle/BattleRole.h"
#include "model/TerrainManager.h"
#include "scene/effect/EffectDefine.h"

TerrainExp::TerrainExp()
{}

TerrainExp::~TerrainExp()
{}

bool TerrainExp::init() { return true; }

float TerrainExp::DataHandle(WarAlive* alive, float time , terRound* round,CCArray* m_task)
{
	float ptime = 0;
	do{
		if(!round)break;
		CCLOG("[%d terrainID] ",round->m_TerID);		//m_target攻击对象
		int lostHp = round->m_LostHp[round->m_AliveID];
		int currHp = round->m_CurrHp[round->m_AliveID];
		unsigned int currMaxHp = round->m_CurrMaxHp[round->m_AliveID];
		CCLOG("[ %d currHp:%d currMaxHp:%d lostHp:%d ]",round->m_AliveID,currHp,currMaxHp,lostHp);	//targets受击数组,currHp受击后血量
		ActObject* aliveOb = alive->getActObject();
		ptime = StateHandle(aliveOb,alive,time,lostHp,currHp,m_task);
		CCArray* arr = CCArray::create();
		arr->addObject(aliveOb);
		arr->addObject(CCInteger::create(round->m_effect));
		arr->addObject(CCInteger::create(round->m_music));
		//m_task->addObject(CombatTask::create(callfuncO_selector(TerrainExp::playerTerrainEffect),time,arr));
		//m_task->addObject(CombatTask::create(callfuncO_selector(TerrainLayer::RemoveNullityTerrain),ptime,CCInteger::create(alive->getGridIndex())));
		CCLOG("================terrain hurt===================");		
		return ptime;
	} while (0);
	return ptime;
}

float TerrainExp::StateHandle(ActObject* aliveOb,WarAlive* alive,float sysTime ,float lostHp ,float currHp,CCArray* m_task)
{
	float ptime = 0;
	do{
		if (!aliveOb || lostHp==0 )break;
		//写一个方法屏幕移动到目标位置，根据grid来进行移动
		//float skillTime1 = targetSkill(startTarget,usEffect,sysTime);			//播放地形伤害特效
		//ptime = aliveOb->targetChangePosture(Hit_Index,sysTime+0.2f,m_task);
		//掉血
		//aliveOb->setTerCurrHp(currHp);
		//aliveOb->setTerLostHp(lostHp);	
	} while (0);
	return ptime+0.3;
}

void TerrainExp::playerTerrainEffect(CCObject* ob)
{
	CCArray* arr = (CCArray*)ob;
	ActObject* act = (ActObject*)arr->objectAtIndex(0);
	CCInteger* effectID = (CCInteger*)arr->objectAtIndex(1);
	CCInteger* musicID = (CCInteger*)arr->objectAtIndex(2);
	EffectObject* _Effect = EffectObject::create(ToString(effectID->getValue()));
	act->addChild(_Effect);
}