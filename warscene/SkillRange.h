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
	AreaCountInfo();
	WarAlive* alive;
	int CountGrid;		//范围计算参照点
	bool Enemy;			//武将类型
	bool BackAtk;		//反向攻击
	int type;			//技能类型
	int range;			//技能范围
	int attackDistance;	//范围距离
	int pTarget;		//效果目标类型
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
	int FrontOrBackAreaDispose(bool Enemy,int pTarget,bool back = false);					//前方后方区域处理
	int CenterAreaDispose(bool Enemy,int pTarget);											//中心区域处理
	vector<int> DisperseDispose(int grid,int Val=1);										//分散处理
	void initEffectTypeArea(AreaCountInfo& CountInfo,vector<int>&pVector);					//得到武将攻击判定格子(多格武将处理)
	vector<int> RowType(bool Enemy,int pTarget,int col,int Type,int val);					//行类型处理
	CCArray* RandomGrid(bool Enemy,int pTarget);											//随机格子方法
	void FixGrid(AreaCountInfo& CountInfo,bool disprese = false);							//固定格子
	void FixAlive(AreaCountInfo& CountInfo);												//固定武将
	vector<int> lowestAlive(int type,int num = 1,bool unCaptain = false);					//血量最少单位处理
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
private:
	WarManager* m_Manage;
};
#endif