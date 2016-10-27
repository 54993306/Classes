/************************************************************* 
 *
 *
 *		Data : 2016.6.1
 *	
 *		Name : AreaCountInfo
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __AreaCountInfo_H_
#define __AreaCountInfo_H_

#include <vector>
#include "cocos2d.h"
namespace BattleSpace
{
	class BaseRole;
	class AreaCountInfo
	{
	public:
		AreaCountInfo(std::vector<int>& pVector,BaseRole* pAlive);
		CC_SYNTHESIZE(BaseRole*,mAlive,Alive);		
		CC_SYNTHESIZE(int,mGrid,Grid);						//范围计算参照点
		CC_SYNTHESIZE(bool,mEnemy,Enemy);					//武将阵营
		CC_SYNTHESIZE(bool,mBackAttack,BackAttack);			//反向攻击
		CC_SYNTHESIZE(int,mAreaType,AreaType);				//技能类型
		CC_SYNTHESIZE(int,mAreaRange,AreaRange);			//技能范围	
		CC_SYNTHESIZE(int,mDistance,Distance);				//范围距离
		CC_SYNTHESIZE(int,mTargetType,TargetType);			//效果目标类型
	public:
		void excludeCaptain();
		void excludeInvalid();
		void excludeStandGrid();
		int getRowByGrid(int pGrid);
		int getColByInfo(int pGrod);
		void addGrid(int pGrid);
		void removeByIndex(int pIndex);
		bool hasGrid(int pGrid);
		void DisperseDispose();
		void RowType(int Type);											//行类型处理
		void inserVector(std::vector<int>& pVector);
		const std::vector<int>& getVector()const {return mVector;}
	private:
		std::vector<int>& mVector;		//成员不能为引用,引用必须要初始化(尽量不要让其他人访问这个数组集合,提供添加和删除的函数被外部对象调用)
	};

	enum AreaMark
	{
		eFrontDirection			= 11,		//前
		eCentenDirection		= 12,		//中
		eBackDirection			= 13,		//后
	};
};
#endif