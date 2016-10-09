
#ifndef _BUF_EXPRESSION_H_
#define _BUF_EXPRESSION_H_
#include "AppUI.h"
/************************************************************* 
 *
 *
 *		Data : 2016.9.21
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 重构方向放在显示对象上每个人单独管理
 *
 *
 *************************************************************/

namespace BattleSpace
{
	class BattleBuff;
	class BufExp : public CCObject
	{
	public:
		BufExp();
		virtual ~BufExp();
		virtual bool init();
		CREATE_FUNC(BufExp);
		void AddBuffExcute(CCObject* ob);
		void updatePosition(CCObject* ob);
		void upBuffEffect(CCObject* ob);
		CCSprite* CreateSmallIcon(BattleBuff* info,vector<CCNode*>&Vec);
		void CreateBigIcon(BattleBuff* info,CCSprite* body);
		void ResetInterval(float dt);
	private:
		CCPoint m_hpSize;
		float m_interval;
	};
};

#endif