/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : BattleTrap
 *
 *		Author : Lin_Xiancheng
 *
 *		Description ： 
 *
 *
*************************************************************/

#ifndef __BattleTrap__
#define __BattleTrap__

namespace protos{
	namespace common{
		class Trap;
	}
}

#include "cocos2d.h"
using namespace cocos2d;

namespace BattleSpace
{
	enum struct sTrapType
	{
		eNullType			= 0,//
		eFireObstacle		= 100,//火属性障碍 预留的可能存在类型，目前不存在
		eFireTrap			= 1,//火属性地形
		eWaterTrap			= 2,//水属性地形
		eWoodTrap			= 3,//木属性地形
		eObstacle			= 4,//障碍类地形(所有武将都不可通过)
		ePlayerObstacle		= 5,//玩家不可通过
		eMonsterObstacle	= 6,//怪物不可通过
	};

	class BuffData;
	class TrapData : public CCObject
	{
	public:
		TrapData();
		virtual ~TrapData();
		static TrapData* createTrap(const protos::common::Trap *pTrap);
		CC_SYNTHESIZE(int,mTrapID,TrapID);
		CC_SYNTHESIZE(int,mTrapModel,TrapModel);			//地形ID，也是地形效果ID，每个地形的效果是固定的
		CC_SYNTHESIZE(sTrapType,mTrapType,TrapType);		//地形类型
		CC_SYNTHESIZE(int,mPosition,Position);				//位置
		CC_SYNTHESIZE(int,mAttribute,AtbType);				//属性类型
		CC_SYNTHESIZE(float,mRate,Rate);					//影响比率
		CC_SYNTHESIZE(int,mDmage,Dmage);					//造成的伤害
		CC_SYNTHESIZE(int,mTouch,TouchNum);					//地形效果ID
		CC_SYNTHESIZE(int,mRound,RoundNum);					//地形命中效果ID
		CC_SYNTHESIZE(int,mRotatione,Rotatione);			//地形旋转角度
		const BuffData* getTrapBuff() const;
	protected:
		void readData(const protos::common::Trap *pTrap);
	private:
		BuffData* mTrapBuff;								//地形附带buff 有可能为一个列表
	};
	//
}


#endif