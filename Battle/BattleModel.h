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
		eNormalBattle			= 0,//����ս��ģʽ
		eEliteBattle			= 1,//��Ӣս��ģʽ
		eWordBossBattle			= 2,//����bossս��ģʽ
		ePvEBattle				= 3,//�첽PvEս��ģʽ
		ePvPBattle				= 4,//ͬ��PvPս��ģʽ
	};
	enum struct sPvEStrategy
	{
		eNull					= 0,//δѡȡAIģʽ
		eBalance				= 1,//����ͻ��
		eDefense				= 2,//����ģʽ
		eAttack					= 3,//����ģʽ
		
	};
	//������Ҫ��ֳ�һ���̳���ϵ����
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