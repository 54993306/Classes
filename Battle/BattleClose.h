/************************************************************* 
 *
 *
 *		Data : 2016.7.6
 *	
 *		Name : BattleClose
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __BattleClose_H_
#define __BattleClose_H_
#include "cocos2d.h"
#include "update/CDownloadPackageDelegate.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	struct GameFinishData
	{
		int reason;
		bool res;
		int roundNum;
	};

	class BaseRole;
	class WarManager;
	class BattleClose : public CCNode, public DownloadPackageDelegate
	{
	public:
		BattleClose();
		virtual ~BattleClose();
		virtual bool init();
		CREATE_FUNC(BattleClose);
		void eventMonitor();
		void enentRemove();
		void roleDieMsg(CCObject* ob);
		void monsterDie(BaseRole* pRole);
		void battleResult(CCObject* ob);
		void wordBossFinish();
		void normalStage(CCObject* ob);
		void scheduleForRequesBossFinish();
		void downloadPackageEnd(bool bAnswer);
		void battleWin();
		void battleFail();
		void scheduleForRequestFinish();
		void roleObjectRemove(CCObject* ob);
		void NextBatch(float dt);
	private:
		WarManager* mManage;
		int	m_CurrBatchNum;				//��¼��ǰս������
		bool m_bRecvFinish;				//�Ƿ���յ�������������Ϣ
		GameFinishData m_finishData;
	};
};
#endif
