/** 
 *
 *		Data : 2016.4.12
 *
 *		Name : HeroRole
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 我方武将数据结构
 *
 */


#include "Battle/GameRoleData.h"
#include <protos/protocol.h>

class HeroRole : public GameRoleData
{
public:
	HeroRole();
	static HeroRole*Create(const protos::common::Hero &hero);
	bool initHeroData(const protos::common::Hero &hero);
public:
	CC_SYNTHESIZE(bool		,Captain			,Captain);						//是否为我方主帅
	CC_SYNTHESIZE(int		,AlertType			,AlertType);					//警戒区域类型
	CC_SYNTHESIZE(int		,AlertRange			,AlertRange);					//警戒区域范围
	CC_SYNTHESIZE(int		,Cooling			,Cooling);						//死亡重新上阵CD
	CC_SYNTHESIZE(int		,InitCost			,InitCost);						//初始COST
	CC_SYNTHESIZE(int		,MaxCost			,MaxCost);						//COST上限
	CC_SYNTHESIZE(int		,UseCost			,UseCost);						//上阵消耗cost
	CC_SYNTHESIZE(int		,CostRecover		,CostRecover);					//cost恢复速度(1/s)

private:

};