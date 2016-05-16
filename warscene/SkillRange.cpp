#include "SkillRange.h"
#include "model/DataCenter.h"
#include "model/BattleData.h"
#include "common/CommonFunction.h"
#include "tools/commonDef.h"
#include "model/DataDefine.h"
#include "warscene/ConstNum.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include <functional>					//sort 的降序排序使用
#include "scene/alive/AliveDefine.h"
#include "warscene/BattleTools.h"
using namespace cocos2d;
SkillRange::SkillRange():m_Manage(nullptr){}
AreaCountInfo::AreaCountInfo()
:CountGrid(0),Enemy(false),type(0),range(1),distance(0)
,alive(nullptr),BackAtk(false),pTarget(enemyTyep){}
//目标格子、范围类型、范围大小、攻击距离(默认为0)

bool SkillRange::init()
{
	m_Manage = DataCenter::sharedData()->getWar();
	return true;
}

//返回受击区域(可以使用先获取敌方目标的方式然后判断敌方目标格子是否在攻击范围内)
CCArray* SkillRange::UnderAttackArea(WarAlive* alive)
{
	CCArray* Area = CCArray::create();
	CEffect* effect = m_Manage->getEffect(alive);				//对武将技能效果进行逻辑把关的第一位置
	if (!effect)
		return Area;
	vector<int> CountGrid = getAliveAtkGrid(alive);
	if (effect->pTarget > 1 )									//效果目标是敌方阵营
	{
		if (alive->getCaptain())
		{
			for (int i=C_CAPTAINGRID;i<=C_ENDGRID;i++)
				Area->addObject(CCInteger::create(i));
		}else{
			for (auto i:alive->grids)
				Area->addObject(CCInteger::create(i));				//最优先攻击相同格子怪物
		}
	}
	if (!effect->range)
	{
		Area->addObject(CCInteger::create(alive->getGridIndex()));
		return Area;
	}
	vector<int>VecAtkGrid;
	for (int i=0;i<CountGrid.size();i++)
	{
		AreaCountInfo CountInfo;
		CountInfo.CountGrid =CountGrid.at(i);
		CountInfo.alive = alive;
		CountInfo.pTarget = effect->pTarget;
		CountInfo.Enemy = alive->getEnemy();
		CountInfo.BackAtk = alive->getNegate();
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32												//技能范围测试
		if (!alive->getEnemy())
		{
			CountInfo.type = effect->mode;
			//CountInfo.type = EnemyArea;
			CountInfo.range = effect->range;
		}else{
			CountInfo.type = effect->mode;
			CountInfo.range = effect->range;
		}
#else
		CountInfo.type = effect->mode;
		CountInfo.range = effect->range;
#endif
		CountInfo.distance = effect->distance;
		getAtkGrid(CountInfo,VecAtkGrid);									//怪物从小往大,英雄由大往小(未反向情况)
	}
	RemoveVectorRepeat(VecAtkGrid);
	if (alive->getEnemy() || (!alive->getEnemy()&&alive->getNegate()))							//防止造成卡壳情况出现
	{
		vector<int>::iterator iter = VecAtkGrid.begin();
		for (;iter != VecAtkGrid.end();iter++)
			if (*iter>=C_BEGINGRID && *iter<=C_ENDGRID)
				Area->addObject(CCInteger::create(*iter));
	}else{
		vector<int>::reverse_iterator iter = VecAtkGrid.rbegin();
		for (;iter != VecAtkGrid.rend();iter++)
			if (*iter>=C_BEGINGRID && *iter<=C_ENDGRID)
				Area->addObject(CCInteger::create(*iter));
	}
	return Area;
}
//返回受击范围内受击武将
CCArray* SkillRange::UnderAttackAlive(WarAlive* self,CCArray* Area)
{
	CCArray* Alives = CCArray::create();
	CEffect* effect = m_Manage->getEffect(self);;
	vector<WarAlive*>* VecAlive;
	if ((self->getEnemy()&&effect->pTarget == enemyTyep)||(!self->getEnemy()&&effect->pTarget != enemyTyep))
	{
		VecAlive = m_Manage->getVecHeros();	
	}else{
		VecAlive = m_Manage->getVecMonsters();
	}																	//格子是排序好的,要使用格子来进行遍历
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(Area,obj)
	{
		CCInteger* grid = (CCInteger*)obj;
		if (effect->mode == frontAreaVia&&Alives->count())
		{
			WarAlive* alive = (WarAlive*)Alives->objectAtIndex(0);
			if (!alive->getcloaking())									//潜行类怪物处理
				break;
			else
				Alives->removeAllObjects();
		}
		for (WarAlive* pAlive: *VecAlive)
		{
			if ( Alives->containsObject(pAlive))
				continue;												//受击数组已存在该武将
			if (grid->getValue() >= C_CAPTAINGRID&&self->getEnemy())	//我方主帅特殊处理
			{
				WarAlive* alive = m_Manage->getAliveByGrid(C_CAPTAINGRID);
				if (alive)Alives->addObject(alive);
				break;
			}else{
				for (int i=0;i<pAlive->grids.size();i++)
				{
					if (pAlive->grids.at(i) == grid->getValue())
					{
						if (pAlive->getAliveStat() == INVINCIBLE || pAlive->getAliveStat() == UNHIT)
							break;
						Alives->addObject(pAlive);
						break;
					}
				}
			}
		}		
	}
	if (!self->getCriAtk()&&Alives->count())							//判断受击目标是否为潜行类怪物							
	{
		CCObject* pObj = nullptr;
		CCARRAY_FOREACH(Alives,pObj)
		{
			WarAlive* alive = (WarAlive*)pObj;
			if (!alive->getcloaking())									//攻击范围内存在非潜行类怪物
				return Alives;
		}
		Alives->removeAllObjects();
	}
	return Alives;
}

CCDictionary* SkillRange::PlaySkillInfo( WarAlive* alive )
{
	CCDictionary* pDic = CCDictionary::create();
	CCArray* Area = CCArray::create();
	CCArray* Alives = CCArray::create();
	pDic->setObject(Alives,Hit_Alive);
	pDic->setObject(Area,Atk_Area);
	if (alive->getAliveStat() == UNATK	||
		alive->getAliveStat() == INVINCIBLE)					//超出边界的武将不再进行技能处理
		return pDic;
	switch (m_Manage->getSkill(alive)->skillType)
	{
	case NorAtk:
	case SpeAtk:
	case CriAtk:
		return UnderAttackInfo(alive);						//有受击目标类技能处理
	case CallAtk: 
		return pDic;										//无受击目标类型技能
	case CapAtk:
		break;
	default:{CCLOG("[ *ERROR ] SkillRange::PlaySkillInfo Skill Type UnKnow");}break;
	}
	return pDic;
}

//返回攻击区域受击武将信息
CCDictionary* SkillRange::UnderAttackInfo(WarAlive* alive)
{
	CCDictionary* pDic = CCDictionary::create();
	CCArray* Area = CCArray::create();
	CCArray* Alives = CCArray::create();
	Area = UnderAttackArea(alive);
	if (!Area->count())
	{
		pDic->setObject(Alives,Hit_Alive);
		pDic->setObject(Area,Atk_Area);
		return pDic;
	}
	Alives = UnderAttackAlive(alive,Area);
	if (!alive->getEnemy()&&!Alives->count()&&!alive->getCaptain())		//主帅的AI方式就是警戒区域为最后一排
	{
		alive->setNegate(true);
		CCArray* NegArea = UnderAttackArea(alive);
		Alives = UnderAttackAlive(alive,NegArea);
		if (!Alives->count())
		{
			alive->setNegate(false);
		}else{
			Area = NegArea;
		}
	}
	if (!Alives->count())
	{
		CCDictionary* pAttackInfo = NextGroupAtkInfo(alive);
		if (pAttackInfo)
			return pAttackInfo;
	}
	pDic->setObject(Alives,Hit_Alive);
	pDic->setObject(Area,Atk_Area);
	return pDic;
}
//判断武将当前状态是否存在第二组效果、第二组技能效果中是否存在受击目标
CCDictionary* SkillRange::NextGroupAtkInfo( WarAlive* alive )
{
	if (alive->getEffectGroup()!=1)
		return nullptr;
	CSkill*skill = m_Manage->getSkill(alive);
	if (skill->EffectList.size()<=1)											//判断是否存在多效果组
		return nullptr;
	alive->setEffectGroup(2);
	CCDictionary* AttackInfo = UnderAttackInfo(alive);
	if (!AttackInfo)return nullptr;
	CCArray* Alives = (CCArray*)AttackInfo->objectForKey(Hit_Alive);		//受击目标
	if (Alives->count())
		return AttackInfo;
	alive->setEffectGroup(1);
	return nullptr;
}
//主帅警戒区域处理,先判断所有位置效果组1，再判断所有位置效果组2
int SkillRange::CaptainGuard( WarAlive* alive )
{
	int currgrid = alive->getGridIndex();
	for (int i=1;i<3;i++)
	{
		CSkill*skill = m_Manage->getSkill(alive);
		if (skill->EffectList.size()<i)											//判断是否存在多效果组
			return INVALID_GRID;
		alive->setEffectGroup(i);
		for (int j=C_CAPTAINGRID;j<=C_ENDGRID;j++)
		{
			int row = j%C_GRID_ROW;												//最小格子的站位
			if (row+alive->role->row>C_GRID_ROW)								//武将所占格子,不能超出地图外
				continue;														
			alive->setGridIndex(j);
			CCArray*Area = UnderAttackArea(alive);
			if (!Area->count())
				continue;
			CCArray* Alives = UnderAttackAlive(alive,Area);
			if (Alives->count())
			{
				alive->setGridIndex(currgrid);
				return j;
			}
		}
	}
	alive->setEffectGroup(1);
	alive->setGridIndex(currgrid);
	return INVALID_GRID;
}

//武将警戒区域处理(多格子武将警戒区域处理)
int SkillRange::GuardMoveArea( WarAlive* alive )
{
	if (alive->getCaptain())
	{
		return CaptainGuard(alive);
	}else{
		if (!alive->role->alert)						//无警戒区域类型武将不进行处理
			return INVALID_GRID;
		//我方多格子武将警戒区域要做特殊处理
		vector<int> Vecguard = getAliveGuard(alive);
		sort(Vecguard.begin(),Vecguard.end(),greater<int>());//警戒区域优先判断后方与上方区域是否有人
		vector<WarAlive*>* AliveVec = m_Manage->getVecMonsters();
		for (auto i:Vecguard)
		{
			for (WarAlive* pAlive:*AliveVec)
			{
				for (auto j:pAlive->grids)
				{
					if (i==j)											//当前警戒区域范围内存在敌方目标
					{
						int movegrid = i;
						int row = i%C_GRID_ROW;
						if ( row+alive->role->row > C_GRID_ROW )		//我方武将占3行,地方怪物在row=3的位置，超出地图范围了
							movegrid = i-((alive->role->row+row)-C_GRID_ROW);	//把超出的部分减掉
						if (C_BEGINGRID>movegrid||movegrid>C_ENDGRID)
							return INVALID_GRID;
						return movegrid;
					}
				}
			}
		}
	}
	return INVALID_GRID;
}
//警戒区域可以参照之前5方向移动方式进行处理
vector<int> SkillRange::getAliveGuard( WarAlive* alive )
{
	vector<int>grids;
	if (alive->getTouchState())
	{
		if (alive->TouchGrids.size())
		{
			grids = alive->TouchGrids;
		}else{
			grids.push_back(alive->getTouchGrid());
		}
	}else{
		if (alive->grids.size()>1)
		{
			grids = alive->grids;
		}else{
			grids.push_back(alive->getGridIndex());
		}
	}
	vector<int> VecGuard;
	for (auto j:grids)
	{
		int aliveCol = j / C_GRID_ROW;
		int aliveRow = j % C_GRID_ROW;
		switch (alive->role->alert)
		{
		case GuardFront:
		case GuardBack:
		case GuardFrontAndBack:
			{
				//int minRow = aliveRow - 1;						//周边范围的警戒区域暂时取消
				//int maxRow = aliveRow + 1;
				//int minCol = aliveCol - 1;
				//int maxCol = aliveCol + 1;
				//minRow = max(0,minRow);
				//maxRow = min(3,maxRow);
				//minCol = max(16,minCol);
				//maxCol = min(31,maxCol);
				//for (int i=maxRow;i>=minRow;i--)
				//{
				//	for (int j=maxCol;j>=minCol;j--)
				//		VecGuard.push_back(j*C_GRID_ROW+i);
				//}
				for (int i=1;i<=alive->role->alertRange;i++)		//从2开始的原因是因为1是外层的圈(现在外层的圈去掉了从1开始)
				{
					switch (alive->role->alert)
					{
					case GuardFront:
						{
							VecGuard.push_back(j - i*C_GRID_ROW);
						}break;
					case GuardBack:
						{
							VecGuard.push_back(j + i*C_GRID_ROW);
						}break;
					case GuardFrontAndBack:
						{
							VecGuard.push_back(j - i*C_GRID_ROW);
							VecGuard.push_back(j + i*C_GRID_ROW);
						}break;
					default:
						break;
					}
				}
			}break;
		case GuardUpDown:
			{
				for (int i=aliveRow-1;i<=aliveRow+1;i++)
				{
					if (i>=C_GRID_ROW||i<0)continue;
					VecGuard.push_back(aliveCol*C_GRID_ROW+i);						
				}
			}
			break;
		default:
			break;
		}
	}
	sort(VecGuard.begin(),VecGuard.end());
	VecGuard.erase(unique(VecGuard.begin(),VecGuard.end()),VecGuard.end());
	return VecGuard;
}
//前后方区域处理,找出最前或最后的位置
int SkillRange::FrontOrBackAreaDispose(bool Enemy,int pTarget,bool back/*false*/)
{
	CCObject* obj = nullptr;
	int pgrid = -1;
	if ((Enemy&&pTarget == ustarget)||( !Enemy&&pTarget == enemyTyep))			/*敌方自己，我方敌人*/
	{
		if (back)pgrid = 1000;
		vector<WarAlive*>*Vec = m_Manage->getVecMonsters();
		for (auto i:*Vec)
		{
			if (back)
			{
				pgrid = min(pgrid,i->getGridIndex());//后方区域,找出最小的位置
			}else{
				pgrid = max(pgrid,i->getGridIndex());//前方区域,找出最大的位置
			}
		}
	}else if ((Enemy&&pTarget == enemyTyep) || ( !Enemy&&pTarget == ustarget))	/*敌方敌人，我方自己*/
	{
		if(!back)pgrid = 1000;
		vector<WarAlive*>*Vec = m_Manage->getVecHeros();
		for (auto i:*Vec)
		{
			if (i->getCaptain())continue;			//boss不算在内
			if (back)
			{
				pgrid = max(pgrid,i->getGridIndex());//后方区域
			}else{
				pgrid = min(pgrid,i->getGridIndex());//前方区域
			}

		}
	}else{
		CCLOG("[ ERROR ] SkillRange::FrontAreaDispose 1 target =%d",pTarget);
	}
	if (pgrid<0)CCLOG("[ ERROR ] SkillRange::FrontAreaDispose 2");
	return pgrid;
}
//中心区域处理
int SkillRange::CenterAreaDispose(bool Enemy,int pTarget)
{
	int grid = -1;
	CCObject* obj = nullptr;
	if ((Enemy&&ustarget == pTarget) || ( !Enemy&&enemyTyep == pTarget))
	{
		CCArray*arr = m_Manage->getMonsts(true);	//默认排序好的
		WarAlive*alive = (WarAlive*)arr->objectAtIndex(arr->count()/2);
		return alive->getGridIndex();
	}else if ((Enemy&&enemyTyep == pTarget) || (!Enemy&&ustarget == pTarget))
	{
		CCArray*arr = m_Manage->getHeros(true);
		WarAlive* alive = (WarAlive*)arr->objectAtIndex(arr->count()/2);
		return alive->getGridIndex();
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
		case Front:
		case Centen:
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
		case Back:
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
vector<int> SkillRange::getAliveAtkGrid( WarAlive* alive )
{
	int aliveGrid = 0;
	vector<int>VecAtkGrid;
	vector<int> CountGrid;
	CEffect* effect = m_Manage->getEffect(alive);
	if (alive->getTouchState())								//单格子武将触摸状态下处理(此处应进行移动状态下攻击格子特殊处理)
	{
		aliveGrid = alive->getTouchGrid();
		CountGrid = alive->TouchGrids;
	}else{
		CountGrid = alive->grids;
		aliveGrid = alive->getGridIndex();
	}
	if (alive->role->row==1&&alive->role->col==1)
	{
		VecAtkGrid.push_back(aliveGrid);
		return VecAtkGrid;
	}else if (effect->mode == lengthwaysArea || roundArea == effect->mode)
	{
		VecAtkGrid = alive->grids;
		return VecAtkGrid;
	}
	if (!CountGrid.size())
	{
		VecAtkGrid.push_back(INVALID_GRID);
		return VecAtkGrid;
	}
	if (alive->getEnemy()|| (!alive->getEnemy()&&alive->getNegate()))//武将反向攻击
	{
		for (int i=0;i<alive->role->row;i++)							//获取向后攻击的判断格子(我方武将反向)
			VecAtkGrid.push_back(CountGrid.at(CountGrid.size()-i-1));
	}else{
		for (int i=0;i<alive->role->row;i++)							//获取向后攻击的判断格子(我方武将反向)
			VecAtkGrid.push_back(CountGrid.at(i));
	}
	return VecAtkGrid;
}

void SkillRange::getAtkGrid(AreaCountInfo& CountInfo,vector<int>&VecAtkGrid)
{
	int aliveCol = CountInfo.CountGrid / C_GRID_ROW;
	int aliveRow = CountInfo.CountGrid % C_GRID_ROW;
	int skilltype = CountInfo.type;							//技能类型
	int range = CountInfo.range;							//得到当前对象的伤害范围，该值由服务器发送	
	if (CountInfo.type != DoubleLine)
	{
		if (CountInfo.Enemy && !CountInfo.BackAtk || 
			!CountInfo.Enemy && CountInfo.BackAtk)			//让技能区域整体移动的情况就类似于武将位置变动了的情况
		{
			aliveCol = aliveCol + CountInfo.distance;
		}else{
			aliveCol = aliveCol - CountInfo.distance;
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
					VecAtkGrid.push_back(i*C_GRID_ROW+aliveRow);
			}else{
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
					VecAtkGrid.push_back(i*C_GRID_ROW+aliveRow);
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
					VecAtkGrid.push_back(j*C_GRID_ROW+i);
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
						VecAtkGrid.push_back(j*C_GRID_ROW+i);						
				}
			}else{
				for (int i=aliveRow-1;i<=aliveRow+1;i++)
				{
					if (i>=C_GRID_ROW||i<0)continue;
					for (int j=aliveCol-1;j>=aliveCol-range;j--)
						VecAtkGrid.push_back(j*C_GRID_ROW+i);
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
				VecAtkGrid.push_back(i*C_GRID_ROW+center_row);
			for (int j=center_row - range;j<=center_row + range;j++)
			{
				if (j>=C_GRID_ROW||j<0)continue;
				VecAtkGrid.push_back(center_col*C_GRID_ROW+j);
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
					VecAtkGrid.push_back(i*C_GRID_ROW+aliveRow-1);
				}
				for (int i=aliveCol+1;i<=aliveCol+range;i++)
				{
					if (aliveRow+1>=C_GRID_ROW)continue;
					VecAtkGrid.push_back(i*C_GRID_ROW+aliveRow+1);
				}
			}else{
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
				{
					if (aliveRow-1<0)continue;
					VecAtkGrid.push_back(i*C_GRID_ROW+aliveRow-1);
				}					
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
				{
					if (aliveRow+1>=C_GRID_ROW)continue;
					VecAtkGrid.push_back(i*C_GRID_ROW+aliveRow+1);
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
						VecAtkGrid.push_back(j*C_GRID_ROW+i);
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
					VecAtkGrid.push_back(i->getGridIndex());
			}else{
				vector<WarAlive*>* vec = m_Manage->getVecMonsters();
				for(auto i:*vec)
					VecAtkGrid.push_back(i->getGridIndex());				
			}
		}break;
	case allArea: 
		{
			for (int i = C_BEGINGRID; i<C_GRID_ROW*C_GRID_COL ; i++ )
				VecAtkGrid.push_back(i);
		} break;
	case tTypeArea:			//T型区域处理				11
		{	
			if (CountInfo.Enemy && !CountInfo.BackAtk || 
				!CountInfo.Enemy && CountInfo.BackAtk)
			{
				for (int i=aliveCol+1;i<=aliveCol+range;i++)
					VecAtkGrid.push_back(i*C_GRID_ROW+aliveRow);
				for (int j=aliveCol+2;j<aliveCol+2+range;j++)
				{
					for (int k=aliveRow-1;k<=aliveRow+1;k++)
					{
						if (k>=C_GRID_ROW||k<0)continue;
						VecAtkGrid.push_back(j*C_GRID_ROW+k);
					}

				}
			}else{
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
					VecAtkGrid.push_back(i*C_GRID_ROW+aliveRow);
				for (int j=aliveCol-2;j>aliveCol-2-range;j--)
				{
					for (int k=aliveRow-1;k<=aliveRow+1;k++)
					{
						if (k>=C_GRID_ROW||k<0)continue;
						VecAtkGrid.push_back(j*C_GRID_ROW+k);
					}
				}
			}
		}break;
	case frontOne://前方单体								101
		{
			VecAtkGrid.push_back(FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget));
		}break;
	case centerOne://中间单体							102
		{
			VecAtkGrid.push_back(CenterAreaDispose(CountInfo.Enemy,CountInfo.pTarget));
		}break;
	case backOne:	//后方单体							103
		{
			VecAtkGrid.push_back(FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget,true));
		}break;
	case frontArea://前方分散范围						104
		{
			int grid = FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget);
			VecAtkGrid = DisperseDispose(grid,range);
		}break;
	case centerArea://中间分散范围						105
		{
			int grid = CenterAreaDispose(CountInfo.Enemy,CountInfo.pTarget);
			VecAtkGrid = DisperseDispose(grid,range);
		}break;
	case backArea://后方分散范围							106
		{
			int grid = FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget,true);
			VecAtkGrid = DisperseDispose(grid,range);
		}break;
	case frontRowArea:		//前军n排					107
		{
			int grid = FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget);
			VecAtkGrid =  RowType(CountInfo.Enemy,CountInfo.pTarget,grid/C_GRID_ROW,Front,range);
		}break;
	case centerRowArea:		//中军n排					108
		{
			int grid = CenterAreaDispose(CountInfo.Enemy,CountInfo.pTarget);
			VecAtkGrid = RowType(CountInfo.Enemy,CountInfo.pTarget,grid/C_GRID_ROW,Centen,range);
		}break;
	case backRowArea:		//后军n排					109
		{
			int grid = FrontOrBackAreaDispose(CountInfo.Enemy,CountInfo.pTarget,true);
			VecAtkGrid = RowType(CountInfo.Enemy,CountInfo.pTarget,grid/C_GRID_ROW,Back,range);
		}break;
	case anyFixGrid:		//随机固定格子				110
		{
			if (!CountInfo.alive->AtkArea.size())
				FixGrid(CountInfo);
			VecAtkGrid = CountInfo.alive->AtkArea;		//为什么这里还需要复制传出去？
		}
	case anyFixGridArea:	//随机格子区域				111
		{
			if (!CountInfo.alive->AtkArea.size())
				FixGrid(CountInfo,true);
			VecAtkGrid = CountInfo.alive->AtkArea;
		}break;
	case anyFixAlive:		//任意武将目标(range个)		112
		{
			if (!CountInfo.alive->AliveS.size())
				FixAlive(CountInfo);
			if (CountInfo.alive->AliveS.size())			//仍然是把存储固定武将的格子取出来传了出去，为什么？
			{
				for (int i=0;i<CountInfo.alive->AliveS.size();i++)
					VecAtkGrid.push_back(CountInfo.alive->AliveS.at(i)->getGridIndex());
			}
		}break;
	case anyFixAliveArea:	//任意武将区域				113
		{
			if (!CountInfo.alive->AliveS.size())
				FixAlive(CountInfo);
			if (!CountInfo.alive->AliveS.size())
				break;
			for (int i=0;i<CountInfo.alive->AliveS.size();i++)
			{
				vector<int>Vec = DisperseDispose(CountInfo.alive->AliveS.at(i)->getGridIndex());
				vector<int>::iterator iter = Vec.begin();
				for (;iter != Vec.end();iter ++)
					VecAtkGrid.push_back(*iter);
			}
		}break;
	case allTypeLowestHp://血量最少单位					114
		{
			if ((CountInfo.Enemy&&CountInfo.pTarget==ustarget) || (!CountInfo.Enemy&&CountInfo.pTarget==enemyTyep))
			{
				VecAtkGrid = lowestAlive(enemyTyep,range);
			}else if ((!CountInfo.Enemy&&CountInfo.pTarget==ustarget) || (CountInfo.Enemy&&CountInfo.pTarget==enemyTyep))
			{
				VecAtkGrid = lowestAlive(ustarget,range);
			}else{
				CCLOG("[ ERROR ] SkillRange::getAtkGrid Type allTypeLowestHp");
			}
		}break;
	case frontTwoType:		//地图中心两格区域			115
		{		
			if (CountInfo.Enemy)
			{
				int col = CountInfo.distance+C_BEGINGRID/C_GRID_ROW;
				for (int i=col;i<col+range;i++)
				{
					VecAtkGrid.push_back(i*C_GRID_ROW+1);
					VecAtkGrid.push_back(i*C_GRID_ROW+2);
				}
			}else{
				int col = C_ENDGRID/C_GRID_ROW - CountInfo.distance;
				for (int i=col;i>col-range;i--)
				{
					VecAtkGrid.push_back(i*C_GRID_ROW+1);
					VecAtkGrid.push_back(i*C_GRID_ROW+2);
				}
			}
		}break;
	case frontFourType:		//前方四个区域				116
		{
			if (CountInfo.Enemy)
			{
				int col = CountInfo.distance+C_BEGINGRID/C_GRID_ROW;
				for (int i=col;i<col+range;i++)
				{
					for (int j=0;j<4;j++)
						VecAtkGrid.push_back(i*C_GRID_ROW+j);
				}
			}else{
				int col = C_ENDGRID/C_GRID_ROW - CountInfo.distance;
				for (int i=col;i>col-range;i--)
				{
					for (int j=0;j<4;j++)
						VecAtkGrid.push_back(i*C_GRID_ROW+j);
				}
			}
		}break;
	case TypeLowestHp:	//除主帅外血量最低对象		117
		{
			if ((CountInfo.Enemy&&CountInfo.pTarget==ustarget) || (!CountInfo.Enemy&&CountInfo.pTarget==enemyTyep))
			{
				VecAtkGrid = lowestAlive(enemyTyep,range,true);
			}else if ((!CountInfo.Enemy&&CountInfo.pTarget==ustarget) || (CountInfo.Enemy&&CountInfo.pTarget==enemyTyep))
			{
				VecAtkGrid = lowestAlive(ustarget,range,true);
			}else{
				CCLOG("[ ERROR ] SkillRange::getAtkGrid Type allTypeLowestHp");
			}
			for (vector<int>::iterator iter = VecAtkGrid.begin();iter != VecAtkGrid.end();)
			{
				if (*iter >= C_CAPTAINGRID)
					iter = VecAtkGrid.erase(iter);
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
			pair<int , int> cpair = LineMap.find(CountInfo.distance)->second;
			if (!CountInfo.Enemy&&!CountInfo.BackAtk)
			{
				for (int i=aliveCol-1;i>=aliveCol-range;i--)
				{
					VecAtkGrid.push_back(i*C_GRID_ROW+cpair.first);
					VecAtkGrid.push_back(i*C_GRID_ROW+cpair.second);
				}		
			}else{
				int col = C_ENDGRID/C_GRID_ROW;
				for (int i=aliveCol+1;i<=aliveCol+range;i++)
				{
					VecAtkGrid.push_back(i*C_GRID_ROW+cpair.first);
					VecAtkGrid.push_back(i*C_GRID_ROW+cpair.second);
				}
			}
		}break;
	default:break;
	}
}