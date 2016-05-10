#ifndef _SCOPECOUNT_H_
#define _SCOPECOUNT_H_
#include "AppUI.h"

class WarAlive;
/************************************************************************/
/*                  
		用于获取技能范围的区域得到范围内的对象		//  [9/9/2014 xc_lin]
*/
/************************************************************************/

enum AreaMark
{
	ustarget	= 1,		//我方目标
	enemyTyep,					//敌方目标
	allType,				//双方目标

	Grid		= 10,		//格子	(单个目标)
	Front		= 11,		//前
	Centen		= 12,		//中
	Back		= 13,		//后

	Row			= 20,		//行
	Disperse	= 30,		//扩散

	Hit_Alive	= 100,
	Atk_Area	= 200,
};
struct AreaCountInfo
{
	AreaCountInfo();
	WarAlive* alive;
	int CountGrid;		//武将当前格子
	bool Enemy;			//武将类型
	bool BackAtk;		//反向攻击
	int type;			//技能类型
	int range;			//技能范围
	int distance;		//范围距离
	int pTarget;		//效果目标类型
};
//技能施放区域类型
enum skillType{
	nullArea = 0,
	front,							//前方格子类型				1
	roundArea,						//周边范围					2
	lengthwaysArea,					//纵向格子类型				3(计算所有武将站位格子)
	crossArea,						//十字						4
	slantArea,						//前方两边					5
	roundExclude,					//周边范围不包含自身			6
	EnemyArea,						//敌方全体					7	
	usArea,							//我方全体					8
	allArea,						//全图						9
	frontAreaVia,					//前方格子类型不贯穿			10
	tTypeArea,						//T型区域					11
	//与武将站位格子无关类攻击范围		(从100开始进行特殊化处理)		
	RestArea = 100,
	frontOne,						//前军单体					101
	centerOne,						//中	军单体					102
	backOne,						//后	军单体					103
	frontArea,						//前军分散范围				104
	centerArea,						//中军分散范围				105
	backArea,						//后军分散范围				106
	frontRowArea,					//前军n排					107
	centerRowArea,					//中军n排					108
	backRowArea,					//后军n排					109
	anyFixGrid,						//随机固定格子				110
	anyFixGridArea,					//随机固定格子分散区域		111
	anyFixAlive,					//随机固定武将				112
	anyFixAliveArea,				//随机固定武将区域			113
	allTypeLowestHp,				//血量最低对象				114
	frontTwoType,					//地图中间两行区域			115
	frontFourType,					//地图四行区域				116		
	TypeLowestHp,					//除主帅外血量最低对象		117
	DoubleLine,						//地图中两行					118
};
//警戒区域
enum GuardType
{
	GuardFront =1,					//前方范围
	GuardBack,						//后方范围
	GuardFrontAndBack,				//前后范围
	GuardUpDown,					//警戒区域为上下范围
};
class WarManager;
class SkillRange : public CCObject
{
public:
	SkillRange();
	~SkillRange(){};
	virtual bool init();
	CREATE_FUNC(SkillRange);
	vector<int> getAliveAtkGrid(WarAlive* alive);											//得到武将有效攻击格子
	int FrontOrBackAreaDispose(bool Enemy,int pTarget,bool back = false);					//前方后方区域处理
	int CenterAreaDispose(bool Enemy,int pTarget);											//中心区域处理
	vector<int> DisperseDispose(int grid,int Val=1);										//分散处理
	void getAtkGrid(AreaCountInfo& CountInfo,vector<int>&VecAtkGrid);						//得到武将攻击判定格子(多格武将处理)
	vector<int> RowType(bool Enemy,int pTarget,int col,int Type,int val);					//行类型处理
	CCArray* RandomGrid(bool Enemy,int pTarget);											//随机格子方法
	void FixGrid(AreaCountInfo& CountInfo,bool disprese = false);							//固定格子
	void FixAlive(AreaCountInfo& CountInfo);												//固定武将
	vector<int> lowestAlive(int type = allType,int num = 1,bool unCaptain = false);			//血量最少单位处理
	CCArray* UnderAttackArea(WarAlive* self);												//受击区域
	CCArray* UnderAttackAlive(WarAlive* self,CCArray* Area);								//受击武将
	CCDictionary* UnderAttackInfo(WarAlive* self);											//受击信息(战斗处提升效率核心方法)
	CCDictionary* NextGroupAtkInfo(WarAlive* alive);										//技能下一效果组处理
	int GuardMoveArea(WarAlive* alive);														//
	vector<int>getAliveGuard(WarAlive* alive);												//
	CCDictionary* PlaySkillInfo(WarAlive* alive);
	int CaptainGuard(WarAlive* alive);
private:
	WarManager* m_Manage;
};
#endif