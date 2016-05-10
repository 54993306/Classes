
#ifndef _BUF_EXPRESSION_H_
#define _BUF_EXPRESSION_H_
#include "AppUI.h"

class BuffInfo;
/************************************************************************/
/*                         
				用于buf效果显示BuffExpression//  [9/9/2014 xc_lin]
*/
/************************************************************************/

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
	CCSprite* CreateSmallIcon(BuffInfo* info,vector<CCNode*>&Vec);
	void CreateBigIcon(BuffInfo* info,CCSprite* body);
	void ResetInterval(float dt);
private:
	CCPoint m_hpSize;
	float m_interval;
};
#endif