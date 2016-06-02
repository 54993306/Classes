#include "SkillRange.h"
#include "model/DataCenter.h"
#include "Battle/TempData.h"
#include "common/CommonFunction.h"
#include "tools/commonDef.h"
#include "Battle/BattleRole.h"
#include "warscene/ConstNum.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include <functional>					//sort 的降序排序使用
#include "scene/alive/AliveDefine.h"
#include "warscene/BattleTools.h"
#include "Battle/SkillMacro.h"
#include "Battle/AreaCount.h"
using namespace cocos2d;
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
//武将当前站位区域
void SkillRange::getSelfArea( WarAlive* pAlive )
{
	if (pAlive->getCurrEffect()->pTarget != eEnemyType)					//效果目标是敌方阵营
		return;
	pAlive->mSkillArea.assign(
		pAlive->mStandGrids.begin(),pAlive->mStandGrids.end());		//最优先攻击相同格子怪物			
}
//得到武将有效攻击范围判断格子(多格武将),武将站位格子排序从大到小
void SkillRange::initValidGrids( WarAlive* pAlive,vector<int>& pValids )
{
	int aliveGrid = 0;
	vector<int> CountGrid;
	SkillEffect* effect = pAlive->getCurrEffect();
	if (pAlive->getTouchState())								//单格子武将触摸状态下处理(此处应进行移动状态下攻击格子特殊处理)
	{
		aliveGrid = pAlive->getTouchGrid();
		CountGrid = pAlive->mTouchGrids;
	}else{
		CountGrid = pAlive->mStandGrids;
		aliveGrid = pAlive->getGridIndex();
	}
	if (pAlive->role->row==1&&pAlive->role->col==1)
	{
		pValids.push_back(aliveGrid);
	}else if (effect->mode == eVertical || eNearby == effect->mode)
	{
		pValids = pAlive->mStandGrids;
	}
	if (!CountGrid.size())
	{
		pValids.push_back(INVALID_GRID);
	}
	if (pAlive->getEnemy()|| (!pAlive->getEnemy()&&pAlive->getOpposite()))//武将反向攻击
	{
		for (int i=0;i<pAlive->role->row;i++)							//获取向后攻击的判断格子(我方武将反向)
			pValids.push_back(CountGrid.at(CountGrid.size()-i-1));
	}else{
		for (int i=0;i<pAlive->role->row;i++)							//获取向后攻击的判断格子(我方武将反向)
			pValids.push_back(CountGrid.at(i));
	}
}

void SkillRange::initSkillEffectArea( WarAlive* pAlive,vector<int>& pVector )
{
	AreaCountInfo CountInfo(pVector,pAlive);
	initEffectTypeArea(CountInfo);	
	CountInfo.excludeInvalid();
	VectorRemoveRepeat(pVector);											//包括了正向遍历和除去重复
	if (!pAlive->getEnemy() && !pAlive->getOpposite())
		sort(pVector.begin(),pVector.end(),greater<int>());					//我方武将正向攻击的情况(对格子进行反向排序)
}
//返回受击区域(可以使用先获取敌方目标的方式然后判断敌方目标格子是否在攻击范围内)
void SkillRange::initSkillArea( WarAlive* pAlive,vector<int>& pVector )	
{
	if ( !pAlive->getCurrEffect() )				//在此处判断是因为该方法被多处调用
	{
		pAlive->mSkillArea.clear();
		pVector.clear();
		return;
	}
	getSelfArea(pAlive);
	if (!pAlive->getCurrEffect()->range)
		return;
	initSkillEffectArea(pAlive,pVector);
}
//返回受击范围内受击武将
void SkillRange::initAreaTargets(WarAlive* pAlive)
{
	vector<WarAlive*>* VecAlive = mManage->getSkillTargets(pAlive);	//不直接使用格子去寻找是为了减少遍历的次数																
	for (auto tGrid : pAlive->mSkillArea)								//格子是排序好的,要使用格子来进行遍历(必须,不同阵营获取目标顺序不同)
	{
		if ( pAlive->pierceJudge() )
			break;
		for (WarAlive* tAlive: *VecAlive)
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
void SkillRange::initAttackInfo(WarAlive* pAlive)
{
	int tGroupIndex = 0;
	for (;tGroupIndex < pAlive->getCurrSkill()->EffectList.size(); tGroupIndex++)
	{
		pAlive->setGroupIndex(tGroupIndex);
		initSkillArea(pAlive,pAlive->mSkillArea);
		if (!pAlive->mSkillArea.size())
			break;
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
int SkillRange::CaptainGuard( WarAlive* pAlive )
{
	int currgrid = pAlive->getGridIndex();
	int tGroupIndex = 0;
	for (;tGroupIndex < pAlive->getCurrSkill()->EffectList.size(); tGroupIndex++)	//效果优先而不是位置优先,先判断一个效果的所有位置再判断下一个效果
	{
		pAlive->setGroupIndex(tGroupIndex);
		for (int tGrid=C_CAPTAINGRID; tGrid<=C_ENDGRID; tGrid++)
		{
			int tStandRow = tGrid%C_GRID_ROW + pAlive->role->row;		
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
//血量最低单位
void SkillRange::lowestAlive(AreaCountInfo& pInfo)
{
	vector<WarAlive*>* tAlives = mManage->getSkillTargets(pInfo.getAlive());
	VectorSortAliveHp(*tAlives);
	if (pInfo.getAreaRange() >= tAlives->size())
	{
		for(auto i:*tAlives)
			pInfo.addGrid(i->getGridIndex());
	}else{
		for (auto i:*tAlives)
		{
			pInfo.addGrid(i->getGridIndex());
			if (pInfo.getVector().size() >= pInfo.getAreaRange())
				break;
		}
	}
}
//武将前方格子					1，前方不惯穿	  10
void SkillRange::FrontArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.getAlive(),tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = pInfo.getColByInfo(tGrid);
		int tRow = pInfo.getRowByGrid(tGrid);
		if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
			!pInfo.getEnemy() && pInfo.getBackAttack())
		{
			for(int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
				pInfo.addGrid(i*C_GRID_ROW+tRow);
		}else{
			for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
				pInfo.addGrid(i*C_GRID_ROW+tRow);
		}
	}
}
//周边范围						2
void SkillRange::NearbyArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.getAlive(),tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = pInfo.getColByInfo(tGrid);
		int tRow = pInfo.getRowByGrid(tGrid);
		int minRow = tRow - pInfo.getAreaRange();
		int maxRow = tRow + pInfo.getAreaRange();
		int minCol = tCol - pInfo.getAreaRange();
		int maxCol = tCol + pInfo.getAreaRange();
		minRow = max(0,minRow);
		maxRow = min(3,maxRow);
		minCol = max(16,minCol);
		maxCol = min(31,maxCol);
		for (int i=maxRow;i>=minRow;i--)
		{
			for (int j=maxCol;j>=minCol;j--)
				pInfo.addGrid(j*C_GRID_ROW+i);
		}
	}
}
//纵向格子类型					3
void SkillRange::VerticalArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.getAlive(),tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = pInfo.getColByInfo(tGrid);
		int tRow = pInfo.getRowByGrid(tGrid);
		if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
			!pInfo.getEnemy() && pInfo.getBackAttack())
		{
			for (int i=tRow-1; i<=tRow+1; i++)
			{
				if (i>=C_GRID_ROW || i<0)
					continue;
				for (int j=tCol+1; j<=tCol+pInfo.getAreaRange(); j++)
					pInfo.addGrid(j*C_GRID_ROW+i);						
			}
		}else{
			for (int i=tRow-1; i<=tRow+1; i++)
			{
				if (i>=C_GRID_ROW || i<0)
					continue;
				for (int j=tCol-1; j>=tCol-pInfo.getAreaRange(); j--)
					pInfo.addGrid(j*C_GRID_ROW+i);
			}
		}
	}
}
//交叉十字						4
void SkillRange::CrossArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.getAlive(),tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = pInfo.getColByInfo(tGrid);
		int tRow = pInfo.getRowByGrid(tGrid);
		int tCenterRow = tRow;
		int center_col = 0;
		if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
			!pInfo.getEnemy() && pInfo.getBackAttack())
		{
			center_col = tCol+(pInfo.getAreaRange()+1);
		}else{
			center_col = tCol-(pInfo.getAreaRange()+1);
		}
		for (int i=center_col - pInfo.getAreaRange();i<=center_col + pInfo.getAreaRange();i++)
			pInfo.addGrid(i*C_GRID_ROW+tCenterRow);
		for (int j=tCenterRow - pInfo.getAreaRange();j<=tCenterRow + pInfo.getAreaRange();j++)
		{
			if (j>=C_GRID_ROW || j<0)
				continue;
			pInfo.addGrid(center_col*C_GRID_ROW+j);
		}
	}
}
//前方两边						5
void SkillRange::FrontTowArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.getAlive(),tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = pInfo.getColByInfo(tGrid);
		int tRow = pInfo.getRowByGrid(tGrid);
		if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
			!pInfo.getEnemy() && pInfo.getBackAttack())
		{
			for (int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
			{
				if (tRow-1<0)continue;
				pInfo.addGrid(i*C_GRID_ROW+tRow-1);
			}
			for (int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
			{
				if (tRow+1>=C_GRID_ROW)continue;
				pInfo.addGrid(i*C_GRID_ROW+tRow+1);
			}
		}else{
			for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
			{
				if (tRow-1<0)continue;
				pInfo.addGrid(i*C_GRID_ROW+tRow-1);
			}					
			for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
			{
				if (tRow+1>=C_GRID_ROW)continue;
				pInfo.addGrid(i*C_GRID_ROW+tRow+1);
			}
		}
	}
}
//周边范围不包含自身				6
void SkillRange::NearbyUnselfArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.getAlive(),tAliveCountGrids);
	vector<int>& tStandVec = pInfo.getAlive()->mStandGrids;
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = pInfo.getColByInfo(tGrid);
		int tRow = pInfo.getRowByGrid(tGrid);
		int minRow = tRow - pInfo.getAreaRange();
		int maxRow = tRow + pInfo.getAreaRange();
		int minCol = tCol - pInfo.getAreaRange();
		int maxCol = tCol + pInfo.getAreaRange();
		minRow = max(0,minRow);
		maxRow = min(3,maxRow);
		minCol = max(16,minCol);
		maxCol = min(31,maxCol);
		for (int i=maxRow;i>=minRow;i--)
		{
			for (int j=maxCol;j>=minCol;j--)
			{
				if (find(tStandVec.begin(),tStandVec.end(),j*C_GRID_ROW+i) != tStandVec.end())
					continue;
				pInfo.addGrid(j*C_GRID_ROW+i);
			}
		}
	}
}
//阵营目标类型					8敌方，9我放，10全体
void SkillRange::CampArea( AreaCountInfo& pInfo )
{
	for ( auto tAlive:*mManage->getSkillTargets(pInfo.getAlive()) )
		pInfo.addGrid(tAlive->getGridIndex());
	pInfo.excludeCaptain();
}
//T型区域处理					11
void SkillRange::TTypeArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.getAlive(),tAliveCountGrids);
	vector<int>& tStandVec = pInfo.getAlive()->mStandGrids;
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = pInfo.getColByInfo(tGrid);
		int tRow = pInfo.getRowByGrid(tGrid);
		if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
			!pInfo.getEnemy() && pInfo.getBackAttack())
		{
			for (int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
				pInfo.addGrid(i*C_GRID_ROW+tRow);
			for (int j=tCol+2;j<tCol+2+pInfo.getAreaRange();j++)
			{
				for (int k=tRow-1;k<=tRow+1;k++)
				{
					if (k>=C_GRID_ROW||k<0)continue;
					pInfo.addGrid(j*C_GRID_ROW+k);
				}

			}
		}else{
			for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
				pInfo.addGrid(i*C_GRID_ROW+tRow);
			for (int j=tCol-2;j>tCol-2-pInfo.getAreaRange();j--)
			{
				for (int k=tRow-1;k<=tRow+1;k++)
				{
					if (k>=C_GRID_ROW||k<0)continue;
					pInfo.addGrid(j*C_GRID_ROW+k);
				}
			}
		}
	}
}
//前方单体						101，后方单体		103
void SkillRange::FrontOrBackOnesArea(AreaCountInfo& pInfo,bool pBack/*false*/)
{
	vector<WarAlive*>* tAlives = mManage->getSkillTargets(pInfo.getAlive());
	if (pBack)
	{
		vector<WarAlive*>::reverse_iterator iter = (*tAlives).rbegin();
		for (;iter != (*tAlives).rend();iter++)
		{
			if ( (*iter)->getCaptain() )		//非主帅的后方位置
				continue;
			pInfo.addGrid((*iter)->getGridIndex());
			return;
		}
	}else{
		pInfo.addGrid(tAlives->at(0)->getGridIndex());
		return ;
	}
}
//中间单体						102
void SkillRange::CenterAreaDispose(AreaCountInfo& pInfo)
{
	vector<WarAlive*>* tAlives = mManage->getSkillTargets(pInfo.getAlive());
	if (!tAlives->size())
		return;
	pInfo.addGrid(tAlives->at(tAlives->size()/2)->getGridIndex());
}
//随机固定格子					110
void SkillRange::FixGrid(AreaCountInfo& pInfo)
{
	for (int tLoopNum = 0;tLoopNum < eLoopNumberMax;tLoopNum++)
	{
		int tIndex = CCRANDOM_0_1() *(C_CAPTAINGRID-C_BEGINGRID)+C_BEGINGRID;	//随机数的范围 C_BEGINGRID ~ C_CAPTAINGRID
		if (pInfo.hasGrid(tIndex))
			continue;
		pInfo.addGrid(tIndex);
		if(pInfo.getVector().size() >= pInfo.getAreaRange()+pInfo.getAlive()->mStandGrids.size()) 
			return ;
	}
	CCLOG("[ *ERROR ]SkillRange::FixGrid");
}
//任意武将目标(range个)			112
void SkillRange::FixAlive(AreaCountInfo& pInfo)
{
	vector<WarAlive*>* tAlives = mManage->getSkillTargets(pInfo.getAlive());
	int tRange = min(tAlives->size(),pInfo.getAreaRange());		// num < val 表示武将不足	
	for (int tLoopNum = 0;tLoopNum < eLoopNumberMax;tLoopNum++)
	{
		int tIndex = CCRANDOM_0_1() *(tAlives->size() - 1);						//随机数的范围 num > j >= 0 
		WarAlive* tAlive = tAlives->at(tIndex);
		if (tAlive->getCaptain())												//随机得到几个有人的位置,不包含主帅
			if (tAlives->size() == 1)											//只有敌方主帅一人的情况
				return;
			else
				continue;
		if (pInfo.hasGrid(tAlive->getGridIndex()))
			continue;
		pInfo.addGrid(tAlive->getGridIndex());
		if(pInfo.getVector().size() >= pInfo.getAreaRange()+pInfo.getAlive()->mStandGrids.size())
			return;
	}
	CCLOG("[ *ERROR ]  SkillRange::FixAlive ");
}
//地图中心两格区域				115
void SkillRange::MapCenterTwoLineArea( AreaCountInfo& pInfo )
{
	if (pInfo.getEnemy())
	{
		int col = pInfo.getDistance()+C_BEGINGRID/C_GRID_ROW;
		for (int i=col;i<col+pInfo.getAreaRange();i++)
		{
			pInfo.addGrid(i*C_GRID_ROW+1);
			pInfo.addGrid(i*C_GRID_ROW+2);
		}
	}else{
		int col = C_ENDGRID/C_GRID_ROW - pInfo.getDistance();
		for (int i=col;i>col-pInfo.getAreaRange();i--)
		{
			pInfo.addGrid(i*C_GRID_ROW+1);
			pInfo.addGrid(i*C_GRID_ROW+2);
		}
	}
}
//前方四个区域					116
void SkillRange::FrontFourGridArea( AreaCountInfo& pInfo )
{
	if (pInfo.getEnemy())
	{
		int col = pInfo.getDistance()+C_BEGINGRID/C_GRID_ROW;
		for (int i=col;i<col+pInfo.getAreaRange();i++)
		{
			for (int j=0;j<4;j++)
				pInfo.addGrid(i*C_GRID_ROW+j);
		}
	}else{
		int col = C_ENDGRID/C_GRID_ROW - pInfo.getDistance();
		for (int i=col;i>col-pInfo.getAreaRange();i--)
		{
			for (int j=0;j<4;j++)
				pInfo.addGrid(i*C_GRID_ROW+j);
		}
	}
}
//地图任意两行   暂时废弃
void SkillRange::MapDoubleLine( AreaCountInfo& pInfo )
{
	map<int,pair<int , int>> LineMap;
	LineMap[1] = make_pair(0,1);
	LineMap[2] = make_pair(1,2);
	LineMap[3] = make_pair(2,3);
	LineMap[4] = make_pair(0,3);
	LineMap[5] = make_pair(0,2);
	LineMap[6] = make_pair(1,3);
	pair<int , int> cpair = LineMap.find(pInfo.getDistance())->second;
	int tCol = /*pInfo.getColByInfo(pInfo.getGrid())*/0;
	if (!pInfo.getEnemy()&&!pInfo.getBackAttack())
	{
		for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
		{
			pInfo.addGrid(i*C_GRID_ROW+cpair.first);
			pInfo.addGrid(i*C_GRID_ROW+cpair.second);
		}		
	}else{
		int col = C_ENDGRID/C_GRID_ROW;
		for (int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
		{
			pInfo.addGrid(i*C_GRID_ROW+cpair.first);
			pInfo.addGrid(i*C_GRID_ROW+cpair.second);
		}
	}
}
//技能的范围可以指定某个区域有很多种可能
void SkillRange::initEffectTypeArea(AreaCountInfo& pInfo)
{
	switch (pInfo.getAreaType())
	{
	case eFront:					//武将前方格子						1
	case ePuncture:					//前方不惯穿							10
		{
			FrontArea(pInfo);
		}break;
	case eNearby:					//周边范围							2
		{
			NearbyArea(pInfo);
		}break;
	case eVertical:					//纵向格子类型						3
		{
			VerticalArea(pInfo);
		}break;
	case eCross:					//交叉十字							4
		{	
			CrossArea(pInfo);
		}break;
	case eFrontTow:					//前方两边							5
		{		
			FrontTowArea(pInfo);
		}break;
	case eNearbyUnself:				//周边范围不包含自身					6
		{
			NearbyUnselfArea(pInfo);
		}break;								
	case eEnemy:					//全体敌方							7
	case eOur:						//全体我方							8
	case eAllArea: 
		{
			CampArea(pInfo);
		}break;
	case eTArea:					//T型区域处理						11
		{	
			TTypeArea(pInfo);
		}break;
	case eFrontOne:					//前方单体							101
		{
			FrontOrBackOnesArea(pInfo);
		}break;
	case eCenterOne:				//中间单体							102
		{
			CenterAreaDispose(pInfo);
		}break;
	case eBackOne:					//后方单体							103
		{
			FrontOrBackOnesArea(pInfo,true);
		}break;
	case eFrontDisperse:			//前方分散范围						104
		{
			FrontOrBackOnesArea(pInfo);
			pInfo.DisperseDispose();
		}break;
	case eCenterDisperse:			//中间分散范围						105
		{
			CenterAreaDispose(pInfo);
			pInfo.DisperseDispose();
		}break;
	case eBackArea:					//后方分散范围						106
		{
			FrontOrBackOnesArea(pInfo);
			pInfo.DisperseDispose();
		}break;
	case eFrontRow:					//前军n排							107
		{
			FrontOrBackOnesArea(pInfo);
			pInfo.RowType(eFrontDirection);
		}break;
	case eCenterRow:				//中军n排							108
		{
			CenterAreaDispose(pInfo);
			pInfo.RowType(eCentenDirection);
		}break;
	case eBackRow:					//后军n排							109
		{
			FrontOrBackOnesArea(pInfo,true);
			pInfo.RowType(eBackDirection);
		}break;
	case eAnyFix:					//随机固定格子						110
		{
			FixGrid(pInfo);
		}break;
	case eAnyFixDisperse:			//随机格子区域						111
		{
			FixGrid(pInfo);
			pInfo.DisperseDispose();
		}break;
	case eAnyAlive:					//任意武将目标(range个)				112
		{
			FixAlive(pInfo);
		}break;
	case eAnyAliveDisperse:			//任意武将区域						113
		{
			FixAlive(pInfo);
			pInfo.DisperseDispose();
		}break;
	case eLowestHp:					//血量最少单位						114
	case LowestHpUnCaptain:			//除主帅外血量最低对象				117
		{
			lowestAlive(pInfo);
			if (pInfo.getAreaType() == LowestHpUnCaptain)
				pInfo.excludeCaptain();
		}break;
	case eMapCenterTwoLine:			//地图中心两格区域					115
		{		
			MapCenterTwoLineArea(pInfo);
		}break;
	case eFrontFourGrid:			//前方四个区域						116
		{
			FrontFourGridArea(pInfo);
		}break;
	case eMapAnyDoubleLine:			//地图两行
		{
			MapDoubleLine(pInfo);
		}break;
	}
}