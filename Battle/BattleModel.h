/************************************************************* 
 *
 *
 *		Data : 2016.7.11
 *	
 *		Name : BattleModel
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __BattleModel_H_
#define __BattleModel_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	enum struct	sBattleType
	{
		eNormalBattle			= 0,//正常战斗模式
		eEliteBattle			= 1,//精英战斗模式
		eWordBossBattle			= 2,//世界boss战斗模式
		ePvEBattle				= 3,//异步PvE战斗模式
		ePvPBattle				= 4,//同步PvP战斗模式
	};
	enum struct sPvEStrategy
	{
		eNull					= 0,//未选取AI模式
		eBalance				= 1,//稳守突击
		eDefense				= 2,//防守模式
		eAttack					= 3,//攻击模式
		
	};
	//后续需要拆分出一个继承体系出来
	class BaseRole;
	class WarManager;
	class BattleModel : public CCObject
	{
	protected:
		BattleModel();
	public:
		virtual ~BattleModel();
		static BattleModel*CreateBattleModel(sBattleType pBattleType);
		virtual bool init();
		bool isPvEBattle();
		bool isBattleType(sBattleType pType);
		bool moveJudge(BaseRole* pRole,int pGrid);
		bool heroMove(BaseRole* pRole);
		bool layerMove();
		bool battlePause();
		CC_SYNTHESIZE(sBattleType,mBattleType,BattleType);
		CC_SYNTHESIZE(sPvEStrategy,mStrategyType,StrategyType);
	private:
		WarManager* mManage;
	};
};
#endif