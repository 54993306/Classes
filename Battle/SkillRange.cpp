#include "SkillRange.h"
#include "model/DataCenter.h"
#include "Battle/BaseRoleData.h"
#include "common/CommonFunction.h"
#include "tools/commonDef.h"
#include "Battle/BaseRole.h"
#include "Battle/ConstNum.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include <functional>					//sort 的降序排序使用
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/BattleTools.h"
#include "Battle/AreaCount.h"
#include "Battle/RoleSkill.h"
#include "Battle/skEffectData.h"
using namespace cocos2d;
namespace BattleSpace{

	SkillRange::SkillRange(WarManager* pManage)
		:mManage(pManage){}
	//目标格子、范围类型、范围大小、攻击距离(默认为0)
	SkillRange* SkillRange::create( WarManager* pManage )
	{
		SkillRange* tSkillRange = new SkillRange(pManage);
		if (tSkillRange)
		{
			tSkillRange->autorelease();
			return tSkillRange;
		}else{
			delete tSkillRange;
			tSkillRange = NULL;
			return NULL;
		}
	}
	//返回受击区域(可以使用先获取敌方目标的方式然后判断敌方目标格子是否在攻击范围内)
	void SkillRange::initSkillArea( BaseRole* pRole,vector<int>& pVector )	
	{
		pVector.clear();
		if ( !pRole->getCurrEffect() )				//在此处判断是因为该方法被多处调用
			return;
		AreaCountInfo CountInfo(pVector,pRole);
		pRole->getCurrEffect()->initArea(CountInfo);
		CountInfo.excludeInvalid();
	}
	//返回受击范围内受击武将
	void SkillRange::initAreaTargets(BaseRole* pAlive)
	{
		vector<BaseRole*>* VecAlive = pAlive->getCurrSkillTargets();				//不直接使用格子去寻找是为了减少遍历的次数																
		for (auto tGrid : pAlive->mSkillArea)								//格子是排序好的,要使用格子来进行遍历(必须,不同阵营获取目标顺序不同)
		{
			for (BaseRole* tAlive: *VecAlive)
			{
				if ( pAlive->hasAliveByTargets(tAlive) || 
					!tAlive->inStandGrid(tGrid)		||
					!tAlive->getAliveState() )
					continue;												//受击数组已存在该武将
				if (tAlive->getLogicState() == sLogicState::eInvincible ||
					tAlive->getLogicState() == sLogicState::eFree		||
					tAlive->getLogicState() == sLogicState::eUnHit	)
					break;
				pAlive->mAreaTargets.push_back(tAlive);
				break;
			}
		}
		pAlive->cloakingTarget();
	}
	//返回攻击区域受击武将信息
	void SkillRange::initAttackInfo(BaseRole* pRole)
	{
		if (pRole->getLogicState() == sLogicState::eFree)
			return;
		int tGroupIndex = 0;
		pRole->setHasTarget(true);
		for (;tGroupIndex < pRole->getCurrSkill()->getListSize(); tGroupIndex++)
		{
			pRole->setGroupIndex(tGroupIndex);
			initSkillArea(pRole,pRole->mSkillArea);
			initAreaTargets(pRole);
			if (pRole->mAreaTargets.size())
				return;
			if (!pRole->getEnemy()	&&	//敌方无法反向攻击
				!pRole->getCaptain() )				
			{
				pRole->setOpposite(true);
				initSkillArea(pRole,pRole->mSkillArea);
				initAreaTargets(pRole);
				if (pRole->mAreaTargets.size())
					return;
				pRole->setOpposite(false);
				initSkillArea(pRole,pRole->mSkillArea);
			}
		}
		pRole->setHasTarget(false);
		pRole->setGroupIndex(0);					//都没有打中目标,默认显示第一个效果组
	}
	//主帅警戒区域处理,先判断所有位置效果组1，再判断所有位置效果组2
	int SkillRange::CaptainGuard( BaseRole* pRole )
	{
		int tCurrGrid = pRole->getGridIndex();
		int tCol = tCurrGrid/C_GRID_ROW;
		int tGroupIndex = 0;
		for (;tGroupIndex < pRole->getCurrSkill()->getListSize(); tGroupIndex++)	//效果优先而不是位置优先,先判断一个效果的所有位置再判断下一个效果
		{
			pRole->setGroupIndex(tGroupIndex);
			for ( int tRow = 0;tRow < C_GRID_ROW;tRow++ )
			{
				int tStandRow = tRow + pRole->getBaseData()->getRoleRow();		
				if (tStandRow > C_GRID_ROW)										//武将所占格子,不能超出地图外
					continue;														
				pRole->setGridIndex(tCol*C_GRID_ROW + tRow);
				initSkillArea(pRole,pRole->mSkillArea);
				if ( !pRole->mSkillArea.size() )
					continue;
				initAreaTargets(pRole);	
				if (pRole->mAreaTargets.size())
				{
					pRole->setGridIndex(tCurrGrid);
					return tCol * C_GRID_ROW + tRow;
				}
			}
		}
		pRole->setGroupIndex(0);
		pRole->setGridIndex(tCurrGrid);
		return INVALID_GRID;
	}
}