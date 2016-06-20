/************************************************************* 
 *
 *
 *		Data : 2016.6.6
 *	
 *		Name : HeroData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __HeroData_H_
#define __HeroData_H_
#include "Battle/BaseRoleData.h"
namespace BattleSpace
{
	class HeroData :public BaseRoleData
	{
	public:
		virtual ~HeroData(){};
		static HeroData* create(const google::protobuf::Message* pMessage = nullptr);
	public:
		CC_SYNTHESIZE(int,mAlertType,AlertType);						//警戒类型
		CC_SYNTHESIZE(int,mAlertRange,AlertRange);						//警戒范围
		CC_SYNTHESIZE(int,mColdDown,ColdDown);							//上阵CD
		CC_SYNTHESIZE(int,mInitCost,InitCost);							//上阵携带cost			（可直接计算出来，不需要再单独携带）
		CC_SYNTHESIZE(int,mExpendCost,ExpendCost);						//上阵消耗cost
		CC_SYNTHESIZE(int,mCostSpeed,CostSpeed);						//cost恢复速度(1/s)
		CC_SYNTHESIZE(int,mMaxCost,MaxCost);							//cost上限
		CC_SYNTHESIZE(bool,mCaptain,Captain);							//武将是否为上阵状态(战斗中标记队长)
	protected:
		virtual void parseData(const google::protobuf::Message* pMessage);
		HeroData(const google::protobuf::Message* pMessage = nullptr);
	};
};
#endif