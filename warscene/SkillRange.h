#ifndef _SCOPECOUNT_H_
#define _SCOPECOUNT_H_
#include "AppUI.h"

class WarAlive;
/************************************************************************/
/*                  
用于获取技能范围的区域得到范围内的对象		//  [9/9/2014 xc_lin]
*/
/************************************************************************/

class AreaCountInfo
{
public:
	AreaCountInfo(vector<int>& pVector);
	WarAlive* mAlive;
	int mGrid;					//范围计算参照点
	bool mEnemy;				//武将类型
	bool mBackAttack;			//反向攻击
	int mAreaType;				//技能类型
	int mRange;					//技能范围
	int mDistance;				//范围距离
	int mTargetType;			//效果目标类型
	vector<int>& mVector;		//成员不能为引用,引用必须要初始化
	void initData(WarAlive* pAlive);
};

class WarManager;
class SkillRange : public CCObject
{
public:
	SkillRange();
	~SkillRange(){};
	virtual bool init();
	CREATE_FUNC(SkillRange);
	CCDictionary* PlaySkillInfo(WarAlive* alive);
	void getSelfArea(WarAlive* pAlive);
	void initValidGrids(WarAlive* alive,vector<int>& pValids);								//得到武将有效攻击格子
	int FrontOrBackAreaDispose(WarAlive*pAlive,bool back = false);							//前方后方区域处理
	int CenterAreaDispose(WarAlive*pAlive);													//中心区域处理
	void DisperseDispose(vector<int>& tVector,int grid,int Val=1);							//分散处理
	void initEffectTypeArea(AreaCountInfo& pInfo);											//得到武将攻击判定格子(多格武将处理)
	vector<int> RowType(bool Enemy,int pTarget,int col,int Type,int val);					//行类型处理
	void FixGrid(AreaCountInfo& pInfo,bool disprese = false);								//固定格子
	void FixAlive(AreaCountInfo& pInfo);													//固定武将
	void lowestAlive(AreaCountInfo& pInfo);													//血量最少单位处理
	void initSkillArea(WarAlive* self,vector<int>& pVector);								//受击区域
	CCArray* initAreaTargets(WarAlive* self);												//受击武将
	void initAttackInfo(WarAlive* self);													//受击信息(战斗处提升效率核心方法)
	int aliveGuard(WarAlive* alive);													//
	void initAliveGuard(WarAlive* alive,vector<int>& pGuards);												//
	int CaptainGuard(WarAlive* alive);

	void initSkillEffectArea(WarAlive* pAlive,vector<int>& pVector);
	void initAliveCurrGrids(WarAlive* pAlive,vector<int>& pVector);
	void guradUpAndDown(WarAlive* pAlive,vector<int>& pGuards);
	void guardFront(WarAlive* pAlive,vector<int>& pGuards);
	void guradBack(WarAlive* pAlive,vector<int>& pGuards);
	void guradFrontAndBack(WarAlive* pAlive,vector<int>& pGuards);
protected:
	WarManager* m_Manage;
private:
	int getRowByGrid(int pGrid);
	int getColByInfo(AreaCountInfo& pInfo);
	void FrontArea(AreaCountInfo& pInfo);
	void NearbyArea(AreaCountInfo& pInfo);
	void VerticalArea(AreaCountInfo& pInfo);
	void CrossArea(AreaCountInfo& pInfo);
	void FrontTowArea(AreaCountInfo& pInfo);
	void NearbyUnselfArea(AreaCountInfo& pInfo);
	void Vertical(AreaCountInfo& pInfo);
	void CampArea(AreaCountInfo& pInfo);
	void TTypeArea(AreaCountInfo& pInfo);
	void Vertical(AreaCountInfo& pInfo);
	void MapDoubleLine(AreaCountInfo& pInfo);
	//void Vertical(AreaCountInfo& pInfo);
	//void Vertical(AreaCountInfo& pInfo);
	//void Vertical(AreaCountInfo& pInfo);
	//void Vertical(AreaCountInfo& pInfo);
	//void Vertical(AreaCountInfo& pInfo);
	//void Vertical(AreaCountInfo& pInfo);
	//void Vertical(AreaCountInfo& pInfo);
};
#endif