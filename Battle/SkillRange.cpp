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
	void SkillRange::initSkillArea( BaseRole* pAlive,vector<int>& pVector )	
	{
		if ( !pAlive->getCurrEffect() )				//在此处判断是因为该方法被多处调用
		{
			pAlive->mSkillArea.clear();
			pVector.clear();
			return;
		}
		AreaCountInfo CountInfo(pVector,pAlive);
		pAlive->getCurrEffect()->initArea(CountInfo);
		CountInfo.excludeInvalid();
	}
	//返回受击范围内受击武将
	void SkillRange::initAreaTargets(BaseRole* pAlive)
	{
		vector<BaseRole*>* VecAlive = pAlive->getSkillTargets();	//不直接使用格子去寻找是为了减少遍历的次数																
		for (auto tGrid : pAlive->mSkillArea)								//格子是排序好的,要使用格子来进行遍历(必须,不同阵营获取目标顺序不同)
		{
			for (BaseRole* tAlive: *VecAlive)
			{
				if ( pAlive->hasAliveByTargets(tAlive) || 
					!tAlive->standInGrid(tGrid) )
					continue;												//受击数组已存在该武将
				if (tAlive->getAliveStat() == INVINCIBLE || 
					tAlive->getAliveStat() == UNHIT )
					break;
				pAlive->mAreaTargets.push_back(tAlive);
				break;
			}
		}
		pAlive->cloakingTarget();
	}
	//返回攻击区域受击武将信息
	void SkillRange::initAttackInfo(BaseRole* pAlive)
	{
		int tGroupIndex = 0;
		for (;tGroupIndex < pAlive->getCurrSkill()->getListSize(); tGroupIndex++)
		{
			pAlive->setGroupIndex(tGroupIndex);
			initSkillArea(pAlive,pAlive->mSkillArea);
			initAreaTargets(pAlive);
			if (pAlive->mAreaTargets.size())
				return;
			if (!pAlive->getEnemy()				&&	//敌方无法反向攻击
				!pAlive->getCaptain() )				
			{
				pAlive->setOpposite(true);
				initSkillArea(pAlive,pAlive->mSkillArea);
				initAreaTargets(pAlive);
				if (pAlive->mAreaTargets.size())
					return;
				pAlive->setOpposite(false);
			}
		}
		pAlive->setGroupIndex(0);					//都没有打中目标,默认显示第一个效果组
	}
	//主帅警戒区域处理,先判断所有位置效果组1，再判断所有位置效果组2
	int SkillRange::CaptainGuard( BaseRole* pAlive )
	{
		int currgrid = pAlive->getGridIndex();
		int tGroupIndex = 0;
		for (;tGroupIndex < pAlive->getCurrSkill()->getListSize(); tGroupIndex++)	//效果优先而不是位置优先,先判断一个效果的所有位置再判断下一个效果
		{
			pAlive->setGroupIndex(tGroupIndex);
			for (int tGrid=C_CAPTAINGRID; tGrid<=C_ENDGRID; tGrid++)
			{
				int tStandRow = tGrid%C_GRID_ROW + pAlive->getBaseData()->getRoleRow();		
				if (tStandRow > C_GRID_ROW)												//武将所占格子,不能超出地图外
					continue;														
				pAlive->setGridIndex(tGrid);
				initSkillArea(pAlive,pAlive->mSkillArea);
				if ( !pAlive->mSkillArea.size() )
					continue;
				initAreaTargets(pAlive);	
				if (pAlive->mAreaTargets.size())
				{
					pAlive->setGridIndex(currgrid);
					return tGrid;
				}
			}
		}
		pAlive->setGroupIndex(0);
		pAlive->setGridIndex(currgrid);
		return INVALID_GRID;
	}
}