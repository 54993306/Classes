#include "SkillRange.h"
#include "model/DataCenter.h"
//#include "model/BattleData.h"
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
using namespace cocos2d;
AreaCountInfo::AreaCountInfo(vector<int>& pVector)
	:mGrid(0),mEnemy(false),mAreaType(0),mRange(1),mDistance(0)
	,mAlive(nullptr),mBackAttack(false),mTargetType(eEnemyType),mVector(pVector)
{}

void AreaCountInfo::initData(WarAlive* pAlive)
{
	mAlive			= pAlive;
	mTargetType		= pAlive->getCurrEffect()->pTarget;
	mEnemy			= pAlive->getEnemy();
	mBackAttack		= pAlive->getOpposite();
	mAreaType		= pAlive->getCurrEffect()->mode;
	mRange			= pAlive->getCurrEffect()->range;					//有一个bool值来判断是否包含武将自身的站立范围
	mDistance = pAlive->getCurrEffect()->distance;
}
SkillRange::SkillRange():m_Manage(nullptr){}
//目标格子、范围类型、范围大小、攻击距离(默认为0)
bool SkillRange::init()
{
	m_Manage = DataCenter::sharedData()->getWar();
	return true;
}
//武将当前站位区域
void SkillRange::getSelfArea( WarAlive* pAlive )
{
	if (pAlive->getCurrEffect()->pTarget != eEnemyType)					//效果目标是敌方阵营
		return;
	pAlive->m_SkillArea.assign(
		pAlive->m_StandGrids.begin(),pAlive->m_StandGrids.end());		//最优先攻击相同格子怪物			
}
//得到武将有效攻击范围判断格子(多格武将),武将站位格子排序从大到小
void SkillRange::initValidGrids( WarAlive* alive,vector<int>& pValids )
{
	int aliveGrid = 0;
	vector<int> CountGrid;
	SkillEffect* effect = alive->getCurrEffect();
	if (alive->getTouchState())								//单格子武将触摸状态下处理(此处应进行移动状态下攻击格子特殊处理)
	{
		aliveGrid = alive->getTouchGrid();
		CountGrid = alive->TouchGrids;
	}else{
		CountGrid = alive->m_StandGrids;
		aliveGrid = alive->getGridIndex();
	}
	if (alive->role->row==1&&alive->role->col==1)
	{
		pValids.push_back(aliveGrid);
	}else if (effect->mode == eVertical || eNearby == effect->mode)
	{
		pValids = alive->m_StandGrids;
	}
	if (!CountGrid.size())
	{
		pValids.push_back(INVALID_GRID);
	}
	if (alive->getEnemy()|| (!alive->getEnemy()&&alive->getOpposite()))//武将反向攻击
	{
		for (int i=0;i<alive->role->row;i++)							//获取向后攻击的判断格子(我方武将反向)
			pValids.push_back(CountGrid.at(CountGrid.size()-i-1));
	}else{
		for (int i=0;i<alive->role->row;i++)							//获取向后攻击的判断格子(我方武将反向)
			pValids.push_back(CountGrid.at(i));
	}
}

void SkillRange::initSkillEffectArea( WarAlive* pAlive,vector<int>& pVector )
{
	AreaCountInfo CountInfo(pVector);
	CountInfo.initData(pAlive);
	initEffectTypeArea(CountInfo);	
	VectorRemoveRepeat(pVector);											//包括了正向遍历和除去重复
	if (!pAlive->getEnemy() && !pAlive->getOpposite())
		sort(pVector.begin(),pVector.end(),greater<int>());					//我方武将正向攻击的情况(对格子进行反向排序)
}
//返回受击区域(可以使用先获取敌方目标的方式然后判断敌方目标格子是否在攻击范围内)
void SkillRange::initSkillArea( WarAlive* pAlive,vector<int>& pVector )	
{
	if ( !pAlive->getCurrEffect() )				//在此处判断是因为该方法被多处调用
	{
		pAlive->m_SkillArea.clear();
		pVector.clear();
		return;
	}
	getSelfArea(pAlive);
	if (!pAlive->getCurrEffect()->range)
		return;
	initSkillEffectArea(pAlive,pVector);
}
//返回受击范围内受击武将
CCArray* SkillRange::initAreaTargets(WarAlive* pAlive)
{
	return CCArray::create();
	vector<WarAlive*>* VecAlive = m_Manage->getSkillTargets(pAlive);	//不直接使用格子去寻找是为了减少遍历的次数																
	for (auto tGrid : pAlive->m_SkillArea)								//格子是排序好的,要使用格子来进行遍历(必须,不同阵营获取目标顺序不同)
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
			pAlive->m_AreaTargets.push_back(tAlive);
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
		initSkillArea(pAlive,pAlive->m_SkillArea);
		if (!pAlive->m_SkillArea.size())
			break;
		initAreaTargets(pAlive);
		if (pAlive->m_AreaTargets.size())
			return;
		if (!pAlive->getEnemy()				&&	//敌方无法反向攻击
			!pAlive->getCaptain() )				
		{
			pAlive->setOpposite(true);
			initSkillArea(pAlive,pAlive->m_SkillArea);
			initAreaTargets(pAlive);
			if (pAlive->m_AreaTargets.size())
				return;
			pAlive->setOpposite(false);
		}
	}
	pAlive->setGroupIndex(0);					//都没有打中目标,默认显示第一个效果组
}

CCDictionary* SkillRange::PlaySkillInfo( WarAlive* alive )
{
	initAttackInfo(alive);
	return CCDictionary::create();
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
			initSkillArea(pAlive,pAlive->m_SkillArea);
			if ( !pAlive->m_SkillArea.size() )
				continue;
			initAreaTargets(pAlive);	
			if (pAlive->m_AreaTargets.size())
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
//武将警戒区域处理(多格子武将警戒区域处理)
int SkillRange::aliveGuard( WarAlive* pAlive )
{
	vector<int> Vecguard;
	initAliveGuard(pAlive,Vecguard);
	for (auto i:Vecguard)
	{
		for (auto tAlive:*m_Manage->getVecMonsters())
		{
			vector<int>::reverse_iterator iter= tAlive->m_StandGrids.rbegin();
			for (;iter != tAlive->m_StandGrids.rend();iter++)			//我方对怪物应该从最大的格子开始进行判断
			{
				if ( i != *iter )										//当前警戒区域范围内存在敌方目标
					continue;
				int tMoveGrid = i;
				int tRow = i%C_GRID_ROW;
				if ( tRow+pAlive->role->row > C_GRID_ROW )				//我方武将占3行,地方怪物在row=3的位置，超出地图范围了
					tMoveGrid = i-((pAlive->role->row+tRow)-C_GRID_ROW);	//把超出的部分减掉
				if ( tMoveGrid < C_BEGINGRID || tMoveGrid > C_ENDGRID )
					break;
				return tMoveGrid;
			}
		}
	}
	return INVALID_GRID;												//没有警戒区域自动不处理
}

void SkillRange::initAliveCurrGrids( WarAlive* pAlive,vector<int>& pVector )
{
	if (pAlive->getTouchState())
	{
		pVector.assign(pAlive->TouchGrids.begin(),pAlive->TouchGrids.end());
	}else{
		pVector.assign(pAlive->m_StandGrids.begin(),pAlive->m_StandGrids.end());
	}
}

void SkillRange::guradUpAndDown( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		int aliveCol = tGrid / C_GRID_ROW;
		int aliveRow = tGrid % C_GRID_ROW;
		for (int i=aliveRow-1;i<=aliveRow+1;i++)
		{
			if (i>=C_GRID_ROW||i<0)continue;
			pGuards.push_back(aliveCol*C_GRID_ROW+i);						
		}
	}
}

void SkillRange::guardFront( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		for (int i=1;i<=pAlive->role->alertRange;i++)		//从2开始的原因是因为1是外层的圈(现在外层的圈去掉了从1开始)
			pGuards.push_back(tGrid - i*C_GRID_ROW);
	}
}

void SkillRange::guradBack( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		for (int i=1;i<=pAlive->role->alertRange;i++)		//从2开始的原因是因为1是外层的圈(现在外层的圈去掉了从1开始)
			pGuards.push_back(tGrid + i*C_GRID_ROW);
	}
}

void SkillRange::guradFrontAndBack( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		for (int i=1;i<=pAlive->role->alertRange;i++)		//从2开始的原因是因为1是外层的圈(现在外层的圈去掉了从1开始)
		{
			pGuards.push_back(tGrid - i*C_GRID_ROW);
			pGuards.push_back(tGrid + i*C_GRID_ROW);
		}
	}
}
//警戒区域可以参照之前5方向移动方式进行处理
void SkillRange::initAliveGuard( WarAlive* pAlive,vector<int>& pGuards )
{
	switch (pAlive->role->alert)
	{
	case eFrontGuard:
		{
			guardFront(pAlive,pGuards);
		}break;
	case eBackGuard:
		{
			guradBack(pAlive,pGuards);
		}break;
	case eFrontAndBackGuard:
		{
			guradFrontAndBack(pAlive,pGuards);
		}break;
	case eUpDownGuard:
		{
			guradUpAndDown(pAlive,pGuards);							//没有长度的限制
		}break;
	default:
		{
			if (pAlive->role->alert)
				CCLOG("[ *ERROR ] SkillRange::initAliveGuard Type = %d",pAlive->role->alert);
			return;
		}
	}
	VectorRemoveRepeat(pGuards);
	sort(pGuards.begin(),pGuards.end(),greater<int>());				//警戒区域优先判断后方与上方区域是否有人
}
//前后方区域处理,找出最前或最后的位置
int SkillRange::FrontOrBackAreaDispose(WarAlive*pAlive,bool pBack/*false*/)
{
	vector<WarAlive*>* tAlives = m_Manage->getSkillTargets(pAlive);
	if (pBack)
	{
		vector<WarAlive*>::reverse_iterator iter = (*tAlives).rbegin();
		for (;iter != (*tAlives).rend();iter++)
		{
			if ( !(*iter)->getCaptain() )		//非主帅的后方位置
				return (*iter)->getGridIndex();
		}
	}else{
		return tAlives->at(0)->getGridIndex();
	}
	return INVALID_GRID;
}
//中心区域处理
int SkillRange::CenterAreaDispose(WarAlive*pAlive)
{
	vector<WarAlive*>* tAlives = m_Manage->getSkillTargets(pAlive);
	if (!tAlives->size())
		return INVALID_GRID;
	return tAlives->at(tAlives->size()/2)->getGridIndex();
}
//分散处理,分散的计算方法都是一样的
void SkillRange::DisperseDispose(vector<int>& tVector,int pAnchorGrid,int pDisperseValue/*=1*/)
{
	int tRow = pAnchorGrid % C_GRID_ROW;
	int tCol = pAnchorGrid / C_GRID_ROW;
	for (int i = C_BEGINGRID; i<C_GRID_ROW*C_GRID_COL ; i++ )
	{
		int tTargetRow = i % C_GRID_ROW;
		int tTargetCol = i / C_GRID_ROW;
		if (pDisperseValue == 1)
		{
			if ((tRow == tTargetRow && tCol+1 >= tTargetCol && tTargetCol >= tCol-1)
				|| (tCol == tTargetCol && tRow+1 >= tTargetRow && tTargetRow >= tRow-1))
			{
				tVector.push_back(i);
			}
		}else{	//		pDisperseValue == 2
			if ((tRow == tTargetRow+1 || tRow == tTargetRow-1 || tRow==tTargetRow)
				&&	(tCol == tTargetCol+1 || tCol == tTargetCol-1 || tCol==tTargetCol)
				||	(tRow == tTargetRow && tCol+2 >= tTargetCol && tTargetCol >= tCol-2)
				||	(tCol == tTargetCol && tRow+2 >= tTargetRow && tTargetRow >= tRow-2))
			{
				tVector.push_back(i);
			}
		}
	}
}
//行数
vector<int> SkillRange::RowType(bool pEnemy,int pTarget,int col,int Type,int val)
{
	vector<int> Vec;
	for (int i = C_BEGINGRID;i<C_GRID_COL*C_GRID_ROW;i++)
	{
		int tarCol = i/C_GRID_ROW;
		switch (Type)
		{
		case eFrontDirection:
		case eCentenDirection:
			{
				if ((pTarget==eUsType && pEnemy)||(pTarget==eEnemyType && !pEnemy)||(pTarget==eAllType && !pEnemy))
				{
					if (col >= tarCol && tarCol > col - val)		//偏向敌方
						Vec.push_back(i);
				}else if ((pTarget==eEnemyType && pEnemy) || (pTarget==eUsType && !pEnemy) || (pTarget==eAllType && pEnemy))
				{
					if (col <= tarCol && tarCol < col + val)		//偏向我方
						Vec.push_back(i);
				}else{
					CCLOG("[ERROR] SkillRange::RowType");
				}
			}
			break;
		case eBackDirection:
			{
				if ((pTarget==eUsType && pEnemy) || (pTarget==eEnemyType && !pEnemy) || (pTarget==eAllType && !pEnemy))
				{
					if (col <= tarCol && tarCol < col + val)		//偏向我方
						Vec.push_back(i);
				}else if ((pTarget==eEnemyType && pEnemy)||(pTarget==eUsType && !pEnemy) || (pTarget==eAllType && pEnemy))
				{
					if (col >= tarCol && tarCol > col - val)		//偏向敌方
						Vec.push_back(i);
				}else{
					CCLOG("[ERROR] SkillRange::RowType");
				}
			}
			break;
		default:
			break;
		}
	}
	return Vec;
}
//这样的攻击范围是不在武将的攻击判定格子遍历的情况下处理的
void SkillRange::FixGrid(AreaCountInfo& pInfo,bool pDisprese /*= false*/)
{
	WarAlive* tAlive = pInfo.mAlive;	
	vector<int>&tVector = pInfo.mVector;
	do{
		int tIndex = CCRANDOM_0_1() *(C_CAPTAINGRID-C_BEGINGRID)+C_BEGINGRID;	//随机数的范围 C_BEGINGRID ~ C_CAPTAINGRID
		if (find(tVector.begin(),tVector.end(),tIndex) != tVector.end())
		{
			tVector.push_back(tIndex);
			if(tVector.size() >= pInfo.mRange+tAlive->m_StandGrids.size()) 
				break;
		}

	} while (true);
	if (pDisprese && tVector.size() >= pInfo.mRange+tAlive->m_StandGrids.size())
	{
		for (auto tGrid:tVector)
		{
			if (tAlive->standInGrid(tGrid))
				continue;														//武将站立区域不做分散处理
			DisperseDispose(tVector,tGrid);
		}
	}
}
//这样的攻击范围是不在武将的攻击判定格子遍历的情况下处理的，也可做溅射型伤害处理
void SkillRange::FixAlive(AreaCountInfo& pInfo)
{
	vector<int>&tVector = pInfo.mVector;
	vector<WarAlive*>* tAlives = m_Manage->getSkillTargets(pInfo.mAlive);
	int tRange = min(tAlives->size(),pInfo.mRange);		// num < val 表示武将不足	
	do{
		int tIndex = CCRANDOM_0_1() *(tAlives->size() - 1);						//随机数的范围 num > j >= 0 
		WarAlive* tAlive = tAlives->at(tIndex);
		if (tAlive->getCaptain())												//随机得到几个有人的位置,不包含主帅
			if (tAlives->size() == 1)											//只有敌方主帅一人的情况
				break;
			else
				continue;
		if (find(tVector.begin(),tVector.end(),tAlive->getGridIndex()) != tVector.end())
		{
			tVector.push_back(tAlive->getGridIndex());
			if(tVector.size() >= pInfo.mRange+pInfo.mAlive->m_StandGrids.size()) 
				break;
		}
	} while (true);
}
//血量最低单位
void SkillRange::lowestAlive(AreaCountInfo& pInfo)
{
	vector<WarAlive*>* tAlives = m_Manage->getSkillTargets(pInfo.mAlive);
	VectorSortAliveHp(*tAlives);
	if (pInfo.mRange >= tAlives->size())
	{
		for(auto i:*tAlives)
			pInfo.mVector.push_back(i->getGridIndex());
	}else{
		for (auto i:*tAlives)
		{
			pInfo.mVector.push_back(i->getGridIndex());
			if (pInfo.mVector.size() >= pInfo.mRange)
				break;
		}
	}
}

int SkillRange::getRowByGrid( int pGrid )
{
	return pGrid % C_GRID_ROW;
}

int SkillRange::getColByInfo( AreaCountInfo& pInfo )
{
	int tCol = pInfo.mGrid / C_GRID_ROW;
	if (pInfo.mAreaType == eMapDoubleLine)
		return tCol;
	if (pInfo.mEnemy && !pInfo.mBackAttack || 
		!pInfo.mEnemy && pInfo.mBackAttack)			//让技能区域整体移动的情况就类似于武将位置变动了的情况
	{
		tCol = tCol + pInfo.mDistance;
	}else{
		tCol = tCol - pInfo.mDistance;
	}
}
//武将前方格子			1，前方不惯穿	  10
void SkillRange::FrontArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.mAlive,tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = getColByInfo(pInfo);
		int tRow = getRowByGrid(tGrid);
		if (pInfo.mEnemy && !pInfo.mBackAttack || 
			!pInfo.mEnemy && pInfo.mBackAttack)
		{
			for(int i=tCol+1;i<=tCol+pInfo.mRange;i++)
				pInfo.mVector.push_back(i*C_GRID_ROW+tRow);
		}else{
			for (int i=tCol-1;i>=tCol-pInfo.mRange;i--)
				pInfo.mVector.push_back(i*C_GRID_ROW+tRow);
		}
	}
}
//周边范围				2
void SkillRange::NearbyArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.mAlive,tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = getColByInfo(pInfo);
		int tRow = getRowByGrid(tGrid);
		int minRow = tRow - pInfo.mRange;
		int maxRow = tRow + pInfo.mRange;
		int minCol = tCol - pInfo.mRange;
		int maxCol = tCol + pInfo.mRange;
		minRow = max(0,minRow);
		maxRow = min(3,maxRow);
		minCol = max(16,minCol);
		maxCol = min(31,maxCol);
		for (int i=maxRow;i>=minRow;i--)
		{
			for (int j=maxCol;j>=minCol;j--)
				pInfo.mVector.push_back(j*C_GRID_ROW+i);
		}
	}
}
//纵向格子类型			3
void SkillRange::VerticalArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.mAlive,tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = getColByInfo(pInfo);
		int tRow = getRowByGrid(tGrid);
		if (pInfo.mEnemy && !pInfo.mBackAttack || 
			!pInfo.mEnemy && pInfo.mBackAttack)
		{
			for (int i=tRow-1; i<=tRow+1; i++)
			{
				if (i>=C_GRID_ROW || i<0)
					continue;
				for (int j=tCol+1; j<=tCol+pInfo.mRange; j++)
					pInfo.mVector.push_back(j*C_GRID_ROW+i);						
			}
		}else{
			for (int i=tRow-1; i<=tRow+1; i++)
			{
				if (i>=C_GRID_ROW || i<0)
					continue;
				for (int j=tCol-1; j>=tCol-pInfo.mRange; j--)
					pInfo.mVector.push_back(j*C_GRID_ROW+i);
			}
		}
	}
}
//交叉十字				4
void SkillRange::CrossArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.mAlive,tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = getColByInfo(pInfo);
		int tRow = getRowByGrid(tGrid);
		int tCenterRow = tRow;
		int center_col = 0;
		if (pInfo.mEnemy && !pInfo.mBackAttack || 
			!pInfo.mEnemy && pInfo.mBackAttack)
		{
			center_col = tCol+(pInfo.mRange+1);
		}else{
			center_col = tCol-(pInfo.mRange+1);
		}
		for (int i=center_col - pInfo.mRange;i<=center_col + pInfo.mRange;i++)
			pInfo.mVector.push_back(i*C_GRID_ROW+tCenterRow);
		for (int j=tCenterRow - pInfo.mRange;j<=tCenterRow + pInfo.mRange;j++)
		{
			if (j>=C_GRID_ROW || j<0)
				continue;
			pInfo.mVector.push_back(center_col*C_GRID_ROW+j);
		}
	}
}
//前方两边				5
void SkillRange::FrontTowArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.mAlive,tAliveCountGrids);
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = getColByInfo(pInfo);
		int tRow = getRowByGrid(tGrid);
		if (pInfo.mEnemy && !pInfo.mBackAttack || 
			!pInfo.mEnemy && pInfo.mBackAttack)
		{
			for (int i=tCol+1;i<=tCol+pInfo.mRange;i++)
			{
				if (tRow-1<0)continue;
				pInfo.mVector.push_back(i*C_GRID_ROW+tRow-1);
			}
			for (int i=tCol+1;i<=tCol+pInfo.mRange;i++)
			{
				if (tRow+1>=C_GRID_ROW)continue;
				pInfo.mVector.push_back(i*C_GRID_ROW+tRow+1);
			}
		}else{
			for (int i=tCol-1;i>=tCol-pInfo.mRange;i--)
			{
				if (tRow-1<0)continue;
				pInfo.mVector.push_back(i*C_GRID_ROW+tRow-1);
			}					
			for (int i=tCol-1;i>=tCol-pInfo.mRange;i--)
			{
				if (tRow+1>=C_GRID_ROW)continue;
				pInfo.mVector.push_back(i*C_GRID_ROW+tRow+1);
			}
		}
	}
}
//周边范围不包含自身		6
void SkillRange::NearbyUnselfArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.mAlive,tAliveCountGrids);
	vector<int>& tStandVec = pInfo.mAlive->m_StandGrids;
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = getColByInfo(pInfo);
		int tRow = getRowByGrid(tGrid);
		int minRow = tRow - pInfo.mRange;
		int maxRow = tRow + pInfo.mRange;
		int minCol = tCol - pInfo.mRange;
		int maxCol = tCol + pInfo.mRange;
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
				pInfo.mVector.push_back(j*C_GRID_ROW+i);
			}
		}
	}
}
//阵营目标类型			8敌方，9我放，10全体
void SkillRange::CampArea( AreaCountInfo& pInfo )
{
	vector<WarAlive*>* tAlives = m_Manage->getSkillTargets(pInfo.mAlive);
	for ( auto tAlive:*tAlives )
	{
		if (tAlive->getCaptain())
			continue;
		pInfo.mVector.push_back(tAlive->getGridIndex());
	}
}
//T型区域处理			11
void SkillRange::TTypeArea( AreaCountInfo& pInfo )
{
	vector<int> tAliveCountGrids;
	initValidGrids(pInfo.mAlive,tAliveCountGrids);
	vector<int>& tStandVec = pInfo.mAlive->m_StandGrids;
	for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
	{
		int tCol = getColByInfo(pInfo);
		int tRow = getRowByGrid(tGrid);
		if (pInfo.mEnemy && !pInfo.mBackAttack || 
			!pInfo.mEnemy && pInfo.mBackAttack)
		{
			for (int i=tCol+1;i<=tCol+pInfo.mRange;i++)
				pInfo.mVector.push_back(i*C_GRID_ROW+tRow);
			for (int j=tCol+2;j<tCol+2+pInfo.mRange;j++)
			{
				for (int k=tRow-1;k<=tRow+1;k++)
				{
					if (k>=C_GRID_ROW||k<0)continue;
					pInfo.mVector.push_back(j*C_GRID_ROW+k);
				}

			}
		}else{
			for (int i=tCol-1;i>=tCol-pInfo.mRange;i--)
				pInfo.mVector.push_back(i*C_GRID_ROW+tRow);
			for (int j=tCol-2;j>tCol-2-pInfo.mRange;j--)
			{
				for (int k=tRow-1;k<=tRow+1;k++)
				{
					if (k>=C_GRID_ROW||k<0)continue;
					pInfo.mVector.push_back(j*C_GRID_ROW+k);
				}
			}
		}
	}
}
//地图任意两行
void SkillRange::MapDoubleLine( AreaCountInfo& pInfo )
{
	map<int,pair<int , int>> LineMap;
	LineMap[1] = make_pair(0,1);
	LineMap[2] = make_pair(1,2);
	LineMap[3] = make_pair(2,3);
	LineMap[4] = make_pair(0,3);
	LineMap[5] = make_pair(0,2);
	LineMap[6] = make_pair(1,3);
	pair<int , int> cpair = LineMap.find(pInfo.mDistance)->second;
	int tCol = getColByInfo(pInfo);
	if (!pInfo.mEnemy&&!pInfo.mBackAttack)
	{
		for (int i=tCol-1;i>=tCol-pInfo.mRange;i--)
		{
			pInfo.mVector.push_back(i*C_GRID_ROW+cpair.first);
			pInfo.mVector.push_back(i*C_GRID_ROW+cpair.second);
		}		
	}else{
		int col = C_ENDGRID/C_GRID_ROW;
		for (int i=tCol+1;i<=tCol+pInfo.mRange;i++)
		{
			pInfo.mVector.push_back(i*C_GRID_ROW+cpair.first);
			pInfo.mVector.push_back(i*C_GRID_ROW+cpair.second);
		}
	}
}

void SkillRange::initEffectTypeArea(AreaCountInfo& pInfo)
{
	int tCol = pInfo.mGrid / C_GRID_ROW;
	int tRow = pInfo.mGrid % C_GRID_ROW;
	switch (pInfo.mAreaType)
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
	case eEnemy:			//全体敌方									7
	case eOur:				//全体我方									8
	case eAllArea: 
		{
			CampArea(pInfo);
		}break;
	case eTArea:			//T型区域处理								11
		{	
			TTypeArea(pInfo);
		}break;
	case eMapDoubleLine:
		{
			MapDoubleLine(pInfo);
		}break;
	case eFrontOne://前方单体								101
		{
			pInfo.mVector.push_back( FrontOrBackAreaDispose(pInfo.mAlive) );
		}break;
	case eCenterOne://中间单体							102
		{
			pInfo.mVector.push_back( CenterAreaDispose(pInfo.mAlive) );
		}break;
	case eBackOne:	//后方单体							103
		{
			pInfo.mVector.push_back( FrontOrBackAreaDispose(pInfo.mAlive,true) );
		}break;
	case eFrontDisperse://前方分散范围						104
		{
			int grid = FrontOrBackAreaDispose(pInfo.mAlive);
			pInfo.mVector = DisperseDispose(grid,pInfo.mRange);
		}break;
	case eCenterDisperse://中间分散范围						105
		{
			int grid = CenterAreaDispose(pInfo.mAlive);
			pInfo.mVector = DisperseDispose(grid,pInfo.mRange);
		}break;
	case eBackArea://后方分散范围							106
		{
			int grid = FrontOrBackAreaDispose(pInfo.mAlive,true);
			pInfo.mVector = DisperseDispose(grid,pInfo.mRange);
		}break;
	case eFrontRow:		//前军n排					107
		{
			int grid = FrontOrBackAreaDispose(pInfo.mAlive);
			pInfo.mVector =  RowType(pInfo.mEnemy,pInfo.mTargetType,grid/C_GRID_ROW,eFrontDirection,pInfo.mRange);
		}break;
	case eCenterRow:		//中军n排					108
		{
			int grid = CenterAreaDispose(pInfo.mAlive);
			pInfo.mVector = RowType(pInfo.mEnemy,pInfo.mTargetType,grid/C_GRID_ROW,eCentenDirection,pInfo.mRange);
		}break;
	case eBackRow:		//后军n排					109
		{
			int grid = FrontOrBackAreaDispose(pInfo.mAlive,true);
			pInfo.mVector = RowType(pInfo.mEnemy,pInfo.mTargetType,grid/C_GRID_ROW,eBackDirection,pInfo.mRange);
		}break;
	case eAnyFix:		//随机固定格子				110
		{
			if (!pInfo.mVector.size())						//这种攻击范围计算方式和武将站位格子无关,不接受站位格子循环处理,只处理一次,但是每次过来至少都添加站立格子了,因此这个判断有问题
			{
				if (!pInfo.mAlive->AtkArea.size())
					FixGrid(pInfo);
				pInfo.mVector = pInfo.mAlive->AtkArea;				//为什么这里还需要复制传出去？
			}
		}
	case eAnyFixDisperse:	//随机格子区域				111
		{
			if (!pInfo.mAlive->AtkArea.size())
				FixGrid(pInfo,true);
			pInfo.mVector = pInfo.mAlive->AtkArea;
		}break;
	case eAnyAlive:		//任意武将目标(range个)		112
		{
			if (!pInfo.mAlive->AliveS.size())
				FixAlive(pInfo);
			if (pInfo.mAlive->AliveS.size())			//仍然是把存储固定武将的格子取出来传了出去，为什么？
			{
				for (int i=0;i<pInfo.mAlive->AliveS.size();i++)
					pInfo.mVector.push_back(pInfo.mAlive->AliveS.at(i)->getGridIndex());
			}
		}break;
	case eAnyAliveDisperse:	//任意武将区域				113
		{
			if (!pInfo.mAlive->AliveS.size())
				FixAlive(pInfo);
			if (!pInfo.mAlive->AliveS.size())
				break;
			for (int i=0;i<pInfo.mAlive->AliveS.size();i++)
			{
				vector<int>Vec = DisperseDispose(pInfo.mAlive->AliveS.at(i)->getGridIndex());
				vector<int>::iterator iter = Vec.begin();
				for (;iter != Vec.end();iter ++)
					pInfo.mVector.push_back(*iter);
			}
		}break;
	case eLowestHp://血量最少单位					114
		{
			if ((pInfo.mEnemy&&pInfo.mTargetType==eUsType) || (!pInfo.mEnemy&&pInfo.mTargetType==eEnemyType))
			{
				pInfo.mVector = lowestAlive(eEnemyType,pInfo.mRange);
			}else if ((!pInfo.mEnemy&&pInfo.mTargetType==eUsType) || (pInfo.mEnemy&&pInfo.mTargetType==eEnemyType))
			{
				pInfo.mVector = lowestAlive(eUsType,pInfo.mRange);
			}else{
				CCLOG("[ ERROR ] SkillRange::getAtkGrid Type allTypeLowestHp");
			}
		}break;
	case eMapTwoLine:		//地图中心两格区域			115
		{		
			if (pInfo.mEnemy)
			{
				int col = pInfo.mDistance+C_BEGINGRID/C_GRID_ROW;
				for (int i=col;i<col+pInfo.mRange;i++)
				{
					pInfo.mVector.push_back(i*C_GRID_ROW+1);
					pInfo.mVector.push_back(i*C_GRID_ROW+2);
				}
			}else{
				int col = C_ENDGRID/C_GRID_ROW - pInfo.mDistance;
				for (int i=col;i>col-pInfo.mRange;i--)
				{
					pInfo.mVector.push_back(i*C_GRID_ROW+1);
					pInfo.mVector.push_back(i*C_GRID_ROW+2);
				}
			}
		}break;
	case eMapFourLine:		//前方四个区域				116
		{
			if (pInfo.mEnemy)
			{
				int col = pInfo.mDistance+C_BEGINGRID/C_GRID_ROW;
				for (int i=col;i<col+pInfo.mRange;i++)
				{
					for (int j=0;j<4;j++)
						pInfo.mVector.push_back(i*C_GRID_ROW+j);
				}
			}else{
				int col = C_ENDGRID/C_GRID_ROW - pInfo.mDistance;
				for (int i=col;i>col-pInfo.mRange;i--)
				{
					for (int j=0;j<4;j++)
						pInfo.mVector.push_back(i*C_GRID_ROW+j);
				}
			}
		}break;
	case TypeLowestHp:	//除主帅外血量最低对象		117
		{
			if ((pInfo.mEnemy&&pInfo.mTargetType==eUsType) || (!pInfo.mEnemy&&pInfo.mTargetType==eEnemyType))
			{
				pInfo.mVector = lowestAlive(eEnemyType,pInfo.mRange,true);
			}else if ((!pInfo.mEnemy&&pInfo.mTargetType==eUsType) || (pInfo.mEnemy&&pInfo.mTargetType==eEnemyType))
			{
				pInfo.mVector = lowestAlive(eUsType,pInfo.mRange,true);
			}else{
				CCLOG("[ ERROR ] SkillRange::getAtkGrid Type allTypeLowestHp");
			}
			for (vector<int>::iterator iter = pInfo.mVector.begin();iter != pInfo.mVector.end();)
			{
				if (*iter >= C_CAPTAINGRID)
					iter = pInfo.mVector.erase(iter);
				else
					iter++;
			}
		}break;
	default:break;
	}
}