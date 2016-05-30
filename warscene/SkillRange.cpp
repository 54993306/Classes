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
SkillRange::SkillRange():m_Manage(nullptr){}
AreaCountInfo::AreaCountInfo()
	:CountGrid(0),Enemy(false),type(0),range(1),attackDistance(0)
	,alive(nullptr),BackAtk(false),pTarget(enemyTyep){}

void AreaCountInfo::initData(WarAlive* pAlive)
{
	alive = pAlive;
	pTarget = pAlive->getCurrEffect()->pTarget;
	Enemy = pAlive->getEnemy();
	BackAtk = pAlive->getOpposite();
	type = pAlive->getCurrEffect()->mode;
	range = pAlive->getCurrEffect()->range;
	attackDistance = pAlive->getCurrEffect()->distance;
}
//目标格子、范围类型、范围大小、攻击距离(默认为0)
bool SkillRange::init()
{
	m_Manage = DataCenter::sharedData()->getWar();
	return true;
}
//武将当前站位区域
void SkillRange::getSelfArea( WarAlive* pAlive )
{
	if (pAlive->getCurrEffect()->pTarget != enemyTyep)					//效果目标是敌方阵营
		return;
	pAlive->m_SkillArea.assign(
		pAlive->m_StandGrids.begin(),pAlive->m_StandGrids.end());		//最优先攻击相同格子怪物			
}

void SkillRange::initSkillEffectArea( WarAlive* pAlive,vector<int>& pVector )
{
	AreaCountInfo CountInfo;
	CountInfo.initData(pAlive);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32								
	if (!pAlive->getEnemy())
	{
		//CountInfo.type = EnemyArea;
	}
#endif
	vector<int> tAliveAttackJudgeGrid;
	initValidGrids(pAlive,tAliveAttackJudgeGrid);
	for (auto tGrid : tAliveAttackJudgeGrid)											
	{
		CountInfo.CountGrid = tGrid;
		initEffectTypeArea(CountInfo,pVector);	
	}
	RemoveVectorRepeat(pVector);											//包括了正向遍历和除去重复
	if (!pAlive->getEnemy() && !pAlive->getOpposite())
		sort(pVector.begin(),pVector.end(),greater<int>());					//我方武将正向攻击的情况(对格子进行反向排序)
}
//返回受击区域(可以使用先获取敌方目标的方式然后判断敌方目标格子是否在攻击范围内)
void SkillRange::initSkillArea( WarAlive* pAlive,vector<int>& pVector )	
{
	getSelfArea(pAlive);
	if ( !pAlive->getCurrEffect() )
	{
		pAlive->m_SkillArea.clear();
		pVector.clear();
		return;
	}
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
			if ( pAlive->hasAliveByTargets(tAlive) )
				continue;												//受击数组已存在该武将
			if (tGrid < C_CAPTAINGRID)									//我方不能对主帅加血和造成影响
			{
				for (int i=0;i<tAlive->m_StandGrids.size();i++)
				{
					if (tAlive->m_StandGrids.at(i) == tGrid)
					{
						if (tAlive->getAliveStat() == INVINCIBLE || tAlive->getAliveStat() == UNHIT)
							break;
						pAlive->m_AreaTargets.push_back(tAlive);
						break;
					}
				}
			}else{
				if (pAlive->getEnemy())
					pAlive->m_AreaTargets.push_back( m_Manage->getAliveByGrid(C_CAPTAINGRID) );
				break;
			}
		}
	}
	pAlive->cloakingTarget();
}

CCDictionary* SkillRange::PlaySkillInfo( WarAlive* alive )
{
	initAttackInfo(alive);
	return CCDictionary::create();
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
			!pAlive->getCaptain() )				//主帅的AI方式就是警戒区域为最后一排
		{
			pAlive->setOpposite(true);
			initSkillArea(pAlive,pAlive->m_SkillArea);
			initAreaTargets(pAlive);
			if (pAlive->m_AreaTargets.size())
			{
				return;
			}else{
				pAlive->setOpposite(false);
			}
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
int SkillRange::aliveGuard( WarAlive* alive )
{
	vector<int> Vecguard;
	initAliveGuard(alive,Vecguard);
	for (auto i:Vecguard)
	{
		for (WarAlive* pAlive:*m_Manage->getVecMonsters())
		{
			vector<int>::reverse_iterator iter= pAlive->m_StandGrids.rbegin();
			for (;iter != pAlive->m_StandGrids.rend();iter++)			//我方对怪物应该从最大的格子开始进行判断
			{
				if ( i != *iter )										//当前警戒区域范围内存在敌方目标
					continue;
				int tMoveGrid = i;
				int tRow = i%C_GRID_ROW;
				if ( tRow+alive->role->row > C_GRID_ROW )				//我方武将占3行,地方怪物在row=3的位置，超出地图范围了
					tMoveGrid = i-((alive->role->row+tRow)-C_GRID_ROW);	//把超出的部分减掉
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
	case GuardFront:
		{
			guardFront(pAlive,pGuards);
		}break;
	case GuardBack:
		{
			guradBack(pAlive,pGuards);
		}break;
	case GuardFrontAndBack:
		{
			guradFrontAndBack(pAlive,pGuards);
		}break;
	case GuardUpDown:
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
	RemoveVectorRepeat(pGuards);
	sort(pGuards.begin(),pGuards.end(),greater<int>());				//警戒区域优先判断后方与上方区域是否有人
}
//前后方区域处理,找出最前或最后的位置
int SkillRange::FrontOrBackAreaDispose(bool Enemy,int pTarget,bool back/*false*/)
{
	int pgrid = INVALID_GRID;
	if ((Enemy&&pTarget == ustarget)||( !Enemy&&pTarget == enemyTyep))			/*敌方自己，我方敌人*/
	{
		if ( back )pgrid = C_ENDGRID;
		for (auto i:* m_Manage->getVecMonsters())
		{
			if (back)
			{
				pgrid = min(pgrid,i->getGridIndex());//后方区域,找出最小的位置
			}else{
				pgrid = max(pgrid,i->getGridIndex());//前方区域,找出最大的位置
			}
		}
	} 
	if ((Enemy&&pTarget == enemyTyep) || ( !Enemy&&pTarget == ustarget))		/*敌方敌人，我方自己*/
	{
		if( !back)pgrid = C_ENDGRID;
		for (auto i : *m_Manage->getVecHeros())
		{
			if (i->getCaptain())continue;			//boss不算在内
			if (back)
			{
				pgrid = max(pgrid,i->getGridIndex());//后方区域
			}else{
				pgrid = min(pgrid,i->getGridIndex());//前方区域
			}

		}
	}
	return pgrid;
}
//中心区域处理
int SkillRange::CenterAreaDispose(bool Enemy,int pTarget)
{
	int grid = INVALID_GRID;
	CCObject* obj = nullptr;
	if ((Enemy&&ustarget == pTarget) || ( !Enemy&&enemyTyep == pTarget))
	{
		vector<WarAlive*>* tMonsts = m_Manage->getVecMonsters();
		return tMonsts->at(tMonsts->size()/2)->getGridIndex();
	}else if ((Enemy&&enemyTyep == pTarget) || (!Enemy&&ustarget == pTarget))
	{
		vector<WarAlive*>* tHeros = m_Manage->getVecHeros();
		return tHeros->at(tHeros->size()/2)->getGridIndex();
	}else{
		CCArray*arr = m_Manage->getAlives(true);
		WarAlive* alive = (WarAlive*)arr->objectAtIndex(arr->count()/2);
		return alive->getGridIndex();
	}
	return grid;
}
//分散处理,分散的计算方法都是一样的
vector<int> SkillRange::DisperseDispose(int grid,int Val/*=1*/)
{
	vector<int> grids;
	int row = grid % C_GRID_ROW;
	int col = grid / C_GRID_ROW;
	for (int i = C_BEGINGRID; i<C_GRID_ROW*C_GRID_COL ; i++ )
	{
		int targetRow = i % C_GRID_ROW;
		int targetCol = i / C_GRID_ROW;
		if (Val == 1)
		{
			if ((row == targetRow && col+1 >= targetCol && targetCol >= col-1)
				|| (col == targetCol && row+1 >= targetRow && targetRow >= row-1))
			{
				grids.push_back(i);
			}
		}else if (Val ==2)
		{
			if ((row == targetRow+1 || row == targetRow-1 || row==targetRow)
				&&	(col == targetCol+1 || col == targetCol-1 || col==targetCol)
				||	(row == targetRow && col+2 >= targetCol && targetCol >= col-2)
				||	(col == targetCol && row+2 >= targetRow && targetRow >= row-2))
			{
				grids.push_back(i);
			}
		}else{
			outPutERRORMsg("SkillRange::DisperseDispose",false);
		}
	}
	return grids;
}
//行数
vector<int> SkillRange::RowType(bool Enemy,int pTarget,int col,int Type,int val)
{
	vector<int> Vec;
	for (int i = C_BEGINGRID;i<C_GRID_COL*C_GRID_ROW;i++)
	{
		int tarCol = i/C_GRID_ROW;
		switch (Type)
		{
		case FrontArea:
		case CentenArea:
			{
				if ((pTarget==ustarget && Enemy)||(pTarget==enemyTyep && !Enemy)||(pTarget==allType && !Enemy))
				{
					if (col >= tarCol && tarCol > col - val)		//偏向敌方
						Vec.push_back(i);
				}else if ((pTarget==enemyTyep && Enemy) || (pTarget==ustarget && !Enemy) || (pTarget==allType && Enemy))
				{
					if (col <= tarCol && tarCol < col + val)		//偏向我方
						Vec.push_back(i);
				}else{
					CCLOG("[ERROR] SkillRange::RowType");
				}
			}
			break;
		case BackArea:
			{
				if ((pTarget==ustarget && Enemy) || (pTarget==enemyTyep && !Enemy) || (pTarget==allType && !Enemy))
				{
					if (col <= tarCol && tarCol < col + val)		//偏向我方
						Vec.push_back(i);
				}else if ((pTarget==enemyTyep && Enemy)||(pTarget==ustarget && !Enemy) || (pTarget==allType && Enemy))
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
//根据效果伤害类型得到受击数组
CCArray* SkillRange::RandomGrid(bool Enemy,int pTarget)
{
	CCArray* arr = nullptr;
	if ((Enemy&&pTarget == ustarget)||( !Enemy&&pTarget == enemyTyep))			/*敌方自己，我方敌人*/
	{
		arr = m_Manage->getMonsts(true);
	}else if ((Enemy&&pTarget == enemyTyep) || ( !Enemy&&pTarget == ustarget))	/*敌方敌人，我方自己*/
	{
		arr = DataCenter::sharedData()->getWar()->getHeros(true);
	}else if (pTarget == allType)
	{
		arr = m_Manage->getHeros(true);
		CCArray* _brr = m_Manage->getMonsts(true);
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(_brr,obj)
			arr->addObject(obj);
	}else{
		CCLOG("[ ERROR* ] SkillRange::RandomGrid");
	}
	return arr;
}

int sortAliveID(const WarAlive*a,const WarAlive*b)	{ return a->getAliveID() > b->getAliveID();  }
int JudgeAliveID(const WarAlive*a,const WarAlive*b) { return a->getAliveID() == b->getAliveID(); }

void SkillRange::FixGrid(AreaCountInfo& CountInfo,bool disprese /*= false*/)
{
	WarAlive* alive = CountInfo.alive;
	int var = CountInfo.range;
	CCArray* arr = RandomGrid(CountInfo.Enemy,CountInfo.range);
	int num = 0;
	if (arr&&arr->count())
	{
		num = arr->count();
	}else{
		CCLOG("[*ERROR ] SkillRange::FixGrid line(649)");
		return;
	}
	var = min(num,var);		// num < val 表示武将不足						//随机得到几个有人的位置,不包含主帅
	do{
		int j = CCRANDOM_0_1() * (num-1);									//随机数的范围 num > j >= 0 
		WarAlive* pAlive = (WarAlive*)arr->objectAtIndex(j);
		if (pAlive->getCaptain())
			if (arr->count()== 1)											//只有敌方主帅一人的情况
				break;
			else
				continue;
		alive->AtkArea.push_back(pAlive->getGridIndex());
		if(alive->AtkArea.size() >= var) break;
	} while (true);
	if (disprese && alive->AtkArea.size())
	{
		vector<int> vec;
		for (vector<int>::iterator iter = alive->AtkArea.begin();iter != alive->AtkArea.end();iter++)
			vec = DisperseDispose(*iter);
		for (vector<int>::iterator iter = vec.begin();iter != vec.end();iter++)
			alive->AtkArea.push_back(*iter);
		RemoveVectorRepeat(alive->AtkArea);
	}
}

void SkillRange::FixAlive(AreaCountInfo& CountInfo)
{
	WarAlive* alive = CountInfo.alive;
	int var = CountInfo.range;
	CCArray* arr = RandomGrid(CountInfo.Enemy,CountInfo.pTarget);
	int num = 0;
	if (arr&&arr->count())
	{
		num = arr->count();
	}else{
		CCLOG("[*ERROR ] SkillRange::FixAlive line(928)");
		return;
	}
	var = min(num,var);		// num < val 表示武将不足
	do{
		int j = CCRANDOM_0_1() * (num-1);												//随机数的范围 num > j >= 0 
		WarAlive* pAlive = (WarAlive*)arr->objectAtIndex(j);
		if (pAlive->getCaptain())
			if (arr->count()== 1)
				break;
			else
				continue;
		alive->AliveS.push_back(pAlive);
		sort(alive->AliveS.begin(),alive->AliveS.end(),sortAliveID);
		vector<WarAlive*>::iterator pos;
		pos = unique(alive->AliveS.begin(),alive->AliveS.end(),JudgeAliveID);		//得到重复元素开始的位置
		alive->AliveS.erase(pos,alive->AliveS.end());								//去除重复武将
		if(alive->AliveS.size() >= var) break;
	} while (true);
}
//对武将血量百分比进行排序
void SortAliveHp(vector<WarAlive*>* VecAlive,int left,int right)
{
	if ( left < right )
	{
		int i = left;
		int j = right;
		WarAlive* alive = VecAlive->at(i);
		float pe = (float)alive->getHp()/alive->getMaxHp();
		while ( i < j ) 
		{
			while (	i < j  && 
				(float)VecAlive->at(j)->getHp()/VecAlive->at(j)->getMaxHp() > pe )
				j--;
			if ( i < j )
			{
				VecAlive->at(i) = VecAlive->at(j);
				i++;
			}
			while ( i < j  &&
				(float)VecAlive->at(j)->getHp()/VecAlive->at(j)->getMaxHp() <= pe )
				i++;
			if( i < j )
			{
				VecAlive->at(j) = VecAlive->at(i);
				j--;
			}
		}
		VecAlive->at(i) = alive;
		SortAliveHp(VecAlive,left,i-1);
		SortAliveHp(VecAlive,i+1,right);
	}
}
//血量最低单位
vector<int> SkillRange::lowestAlive(int type /*=allType*/,int num /*=1*/,bool unCaptain/* = false*/)
{
	vector<int>Vec;
	vector<WarAlive*>* VecAlive;
	CCArray*arr = nullptr;
	switch (type)
	{
	case ustarget:	{ VecAlive = m_Manage->getVecHeros();	}
					break;
	case enemyTyep:	{ VecAlive = m_Manage->getVecMonsters();}
					break;
	case allType:
		{
			VecAlive = m_Manage->getVecHeros();
			vector<WarAlive*>* VecAlive2 = m_Manage->getVecMonsters();
			VecAlive->insert(VecAlive->end(),VecAlive2->begin(),VecAlive2->end());//vector相加
		}break;
	default:break;
	}
	SortAliveHp(VecAlive,0,VecAlive->size()-1);			//快速排序得出血量百分比由小到大的数组
	if (num >= VecAlive->size())
	{
		for(auto i:*VecAlive)
			Vec.push_back(i->getGridIndex());
	}else{
		for (auto i:*VecAlive)
		{
			if (unCaptain && i->getCaptain())
				continue;
			Vec.push_back(i->getGridIndex());
			if (Vec.size()>=num)
				break;
		}
	}
	return Vec;
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
	}else if (effect->mode == lengthwaysArea || roundArea == effect->mode)
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

void SkillRange::initEffectTypeArea(AreaCountInfo& CountInfo,vector<int>&pVector)
{
	WarAlive* tAlive = CountInfo.alive;
	int aliveCol = CountInfo.CountGrid / C_GRID_ROW;
	int aliveRow = CountInfo.CountGrid % C_GRID_ROW;
	int skilltype = CountInfo.type;							//技能类型
	int range = CountInfo.range;							//得到当前对象的伤害范围，该值由服务器发送	
	if (CountInfo.type != DoubleLine)
	{
		if (CountInfo.Enemy && !CountInfo.BackAtk || 
			!CountInfo.Enemy && CountInfo.BackAtk)			//让技能区域整体移动的情况就类似于武将位置变动了的情况
		{
			aliveCol = aliveCol + CountInfo.attackDistance;
		}else{
			aliveCol = aliveCol - CountInfo.attackDistance;
		}
	}
	switch (CountInfo.type)
	{
	case front:					//武将前方格子			1
	case frontAreaVia:			//前方不惯穿				10
		{
			if (CountInfo.Enemy && !CountInfo.BackAtk || 
				!CountInfo.Enemy && CountInfo.BackAtk)
			{
				for(int i=aliveCol+1;i<=aliveCol+range;i++)
					pVector.push_back(i*C_GRID_ROW+aliveRow);
			}else{
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
					pVector.push_back(i*C_GRID_ROW+aliveRow);
			}
		}break;
	case roundArea:	//周边范围							2
		{
			int minRow = aliveRow - range;
			int maxRow = aliveRow + range;
			int minCol = aliveCol - range;
			int maxCol = aliveCol + range;
			minRow = max(0,minRow);
			maxRow = min(3,maxRow);
			minCol = max(16,minCol);
			maxCol = min(31,maxCol);
			for (int i=maxRow;i>=minRow;i--)
			{
				for (int j=maxCol;j>=minCol;j--)
					pVector.push_back(j*C_GRID_ROW+i);
			}
		}break;
	case lengthwaysArea:			//纵向三格子			3
		{
			if (CountInfo.Enemy && !CountInfo.BackAtk || 
				!CountInfo.Enemy && CountInfo.BackAtk)
			{
				for (int i=aliveRow-1;i<=aliveRow+1;i++)
				{
					if (i>=C_GRID_ROW||i<0)continue;
					for (int j=aliveCol+1;j<=aliveCol+range;j++)
						pVector.push_back(j*C_GRID_ROW+i);						
				}
			}else{
				for (int i=aliveRow-1;i<=aliveRow+1;i++)
				{
					if (i>=C_GRID_ROW||i<0)continue;
					for (int j=aliveCol-1;j>=aliveCol-range;j--)
						pVector.push_back(j*C_GRID_ROW+i);
				}
			}
		}break;
	case crossArea:					//交叉十字			4
		{	
			int center_row = aliveRow;
			int center_col = 0;
			if (CountInfo.Enemy && !CountInfo.BackAtk || 
				!CountInfo.Enemy && CountInfo.BackAtk)
			{
				center_col = aliveCol+(range+1);
			}else{
				center_col = aliveCol-(range+1);
			}
			for (int i=center_col - range;i<=center_col + range;i++)
				pVector.push_back(i*C_GRID_ROW+center_row);
			for (int j=center_row - range;j<=center_row + range;j++)
			{
				if (j>=C_GRID_ROW||j<0)continue;
				pVector.push_back(center_col*C_GRID_ROW+j);
			}
		}break;
	case slantArea:				//前方两边				5
		{		
			if (CountInfo.Enemy && !CountInfo.BackAtk || 
				!CountInfo.Enemy && CountInfo.BackAtk)
			{
				for (int i=aliveCol+1;i<=aliveCol+range;i++)
				{
					if (aliveRow-1<0)continue;
					pVector.push_back(i*C_GRID_ROW+aliveRow-1);
				}
				for (int i=aliveCol+1;i<=aliveCol+range;i++)
				{
					if (aliveRow+1>=C_GRID_ROW)continue;
					pVector.push_back(i*C_GRID_ROW+aliveRow+1);
				}
			}else{
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
				{
					if (aliveRow-1<0)continue;
					pVector.push_back(i*C_GRID_ROW+aliveRow-1);
				}					
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
				{
					if (aliveRow+1>=C_GRID_ROW)continue;
					pVector.push_back(i*C_GRID_ROW+aliveRow+1);
				}
			}
		}break;
	case roundExclude:	//警戒区域类型					6
		{
			int minRow = aliveRow - range;
			int maxRow = aliveRow + range;
			int minCol = aliveCol - range;
			int maxCol = aliveCol + range;
			minRow = max(0,minRow);
			maxRow = min(3,maxRow);
			minCol = max(16,minCol);
			maxCol = min(31,maxCol);
			for (int i=maxRow;i>=minRow;i--)
			{
				for (int j=maxCol;j>=minCol;j--)
				{
					if (j*C_GRID_ROW+i != CountInfo.CountGrid)
						pVector.push_back(j*C_GRID_ROW+i);
				}
			}
		}break;								
	case EnemyArea:			//全体敌方					7
	case usArea:			//全体我方					8
		{
			if (CountInfo.Enemy && CountInfo.type==EnemyArea || !CountInfo.Enemy&&CountInfo.type==usArea)
			{
				vector<WarAlive*>* vec = m_Manage->getVecHeros();
				for(auto i:*vec)
				{
					if (vec->size() > 1 && i->getCaptain())
					{
						continue;
						pVector.push_back(i->getGridIndex());
					}
				}
			}else{
				vector<WarAlive*>* vec = m_Manage->getVecMonsters();
				for(auto i:*vec)
					pVector.push_back(i->getGridIndex());				
			}
		}break;
	case allArea: 
		{
			for (int i = C_BEGINGRID; i<C_GRID_ROW*C_GRID_COL ; i++ )
				pVector.push_back(i);
		} break;
	case tTypeArea:			//T型区域处理				11
		{	
			if (CountInfo.Enemy && !CountInfo.BackAtk || 
				!CountInfo.Enemy && CountInfo.BackAtk)
			{
				for (int i=aliveCol+1;i<=aliveCol+range;i++)
					pVector.push_back(i*C_GRID_ROW+aliveRow);
				for (int j=aliveCol+2;j<aliveCol+2+range;j++)
				{
					for (int k=aliveRow-1;k<=aliveRow+1;k++)
					{
						if (k>=C_GRID_ROW||k<0)continue;
						pVector.push_back(j*C_GRID_ROW+k);
					}

				}
			}else{
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
					pVector.push_back(i*C_GRID_ROW+aliveRow);
				for (int j=aliveCol-2;j>aliveCol-2-range;j--)
				{
					for (int k=aliveRow-1;k<=aliveRow+1;k++)
					{
						if (k>=C_GRID_ROW||k<0)continue;
						pVector.push_back(j*C_GRID_ROW+k);
					}
				}
			}
		}break;
	case frontOne://前方单体								101
		{
			pVector.push_back( FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget) );
		}break;
	case centerOne://中间单体							102
		{
			pVector.push_back( CenterAreaDispose(CountInfo.Enemy,CountInfo.pTarget) );
		}break;
	case backOne:	//后方单体							103
		{
			pVector.push_back( FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget,true) );
		}break;
	case frontArea://前方分散范围						104
		{
			int grid = FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget);
			pVector = DisperseDispose(grid,range);
		}break;
	case centerArea://中间分散范围						105
		{
			int grid = CenterAreaDispose(CountInfo.Enemy,CountInfo.pTarget);
			pVector = DisperseDispose(grid,range);
		}break;
	case backArea://后方分散范围							106
		{
			int grid = FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget,true);
			pVector = DisperseDispose(grid,range);
		}break;
	case frontRowArea:		//前军n排					107
		{
			int grid = FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget);
			pVector =  RowType(CountInfo.Enemy,CountInfo.pTarget,grid/C_GRID_ROW,FrontArea,range);
		}break;
	case centerRowArea:		//中军n排					108
		{
			int grid = CenterAreaDispose(CountInfo.Enemy,CountInfo.pTarget);
			pVector = RowType(CountInfo.Enemy,CountInfo.pTarget,grid/C_GRID_ROW,CentenArea,range);
		}break;
	case backRowArea:		//后军n排					109
		{
			int grid = FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget,true);
			pVector = RowType(CountInfo.Enemy,CountInfo.pTarget,grid/C_GRID_ROW,BackArea,range);
		}break;
	case anyFixGrid:		//随机固定格子				110
		{
			if (!tAlive->AtkArea.size())
				FixGrid(CountInfo);
			pVector = tAlive->AtkArea;		//为什么这里还需要复制传出去？
		}
	case anyFixGridArea:	//随机格子区域				111
		{
			if (!tAlive->AtkArea.size())
				FixGrid(CountInfo,true);
			pVector = tAlive->AtkArea;
		}break;
	case anyFixAlive:		//任意武将目标(range个)		112
		{
			if (!tAlive->AliveS.size())
				FixAlive(CountInfo);
			if (tAlive->AliveS.size())			//仍然是把存储固定武将的格子取出来传了出去，为什么？
			{
				for (int i=0;i<tAlive->AliveS.size();i++)
					pVector.push_back(tAlive->AliveS.at(i)->getGridIndex());
			}
		}break;
	case anyFixAliveArea:	//任意武将区域				113
		{
			if (!tAlive->AliveS.size())
				FixAlive(CountInfo);
			if (!tAlive->AliveS.size())
				break;
			for (int i=0;i<tAlive->AliveS.size();i++)
			{
				vector<int>Vec = DisperseDispose(tAlive->AliveS.at(i)->getGridIndex());
				vector<int>::iterator iter = Vec.begin();
				for (;iter != Vec.end();iter ++)
					pVector.push_back(*iter);
			}
		}break;
	case allTypeLowestHp://血量最少单位					114
		{
			if ((CountInfo.Enemy&&CountInfo.pTarget==ustarget) || (!CountInfo.Enemy&&CountInfo.pTarget==enemyTyep))
			{
				pVector = lowestAlive(enemyTyep,range);
			}else if ((!CountInfo.Enemy&&CountInfo.pTarget==ustarget) || (CountInfo.Enemy&&CountInfo.pTarget==enemyTyep))
			{
				pVector = lowestAlive(ustarget,range);
			}else{
				CCLOG("[ ERROR ] SkillRange::getAtkGrid Type allTypeLowestHp");
			}
		}break;
	case frontTwoType:		//地图中心两格区域			115
		{		
			if (CountInfo.Enemy)
			{
				int col = CountInfo.attackDistance+C_BEGINGRID/C_GRID_ROW;
				for (int i=col;i<col+range;i++)
				{
					pVector.push_back(i*C_GRID_ROW+1);
					pVector.push_back(i*C_GRID_ROW+2);
				}
			}else{
				int col = C_ENDGRID/C_GRID_ROW - CountInfo.attackDistance;
				for (int i=col;i>col-range;i--)
				{
					pVector.push_back(i*C_GRID_ROW+1);
					pVector.push_back(i*C_GRID_ROW+2);
				}
			}
		}break;
	case frontFourType:		//前方四个区域				116
		{
			if (CountInfo.Enemy)
			{
				int col = CountInfo.attackDistance+C_BEGINGRID/C_GRID_ROW;
				for (int i=col;i<col+range;i++)
				{
					for (int j=0;j<4;j++)
						pVector.push_back(i*C_GRID_ROW+j);
				}
			}else{
				int col = C_ENDGRID/C_GRID_ROW - CountInfo.attackDistance;
				for (int i=col;i>col-range;i--)
				{
					for (int j=0;j<4;j++)
						pVector.push_back(i*C_GRID_ROW+j);
				}
			}
		}break;
	case TypeLowestHp:	//除主帅外血量最低对象		117
		{
			if ((CountInfo.Enemy&&CountInfo.pTarget==ustarget) || (!CountInfo.Enemy&&CountInfo.pTarget==enemyTyep))
			{
				pVector = lowestAlive(enemyTyep,range,true);
			}else if ((!CountInfo.Enemy&&CountInfo.pTarget==ustarget) || (CountInfo.Enemy&&CountInfo.pTarget==enemyTyep))
			{
				pVector = lowestAlive(ustarget,range,true);
			}else{
				CCLOG("[ ERROR ] SkillRange::getAtkGrid Type allTypeLowestHp");
			}
			for (vector<int>::iterator iter = pVector.begin();iter != pVector.end();)
			{
				if (*iter >= C_CAPTAINGRID)
					iter = pVector.erase(iter);
				else
					iter++;
			}
		}break;
	case DoubleLine:
		{
			map<int,pair<int , int>> LineMap;
			LineMap[1] = make_pair(0,1);
			LineMap[2] = make_pair(1,2);
			LineMap[3] = make_pair(2,3);
			LineMap[4] = make_pair(0,3);
			LineMap[5] = make_pair(0,2);
			LineMap[6] = make_pair(1,3);
			pair<int , int> cpair = LineMap.find(CountInfo.attackDistance)->second;
			if (!CountInfo.Enemy&&!CountInfo.BackAtk)
			{
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
				{
					pVector.push_back(i*C_GRID_ROW+cpair.first);
					pVector.push_back(i*C_GRID_ROW+cpair.second);
				}		
			}else{
				int col = C_ENDGRID/C_GRID_ROW;
				for (int i=aliveCol+1;i<=aliveCol+range;i++)
				{
					pVector.push_back(i*C_GRID_ROW+cpair.first);
					pVector.push_back(i*C_GRID_ROW+cpair.second);
				}
			}
		}break;
	default:break;
	}
}