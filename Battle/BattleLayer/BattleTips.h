

/************************************************************* 
 *
 *
 *		Data : 2016.7.15
 *	
 *		Name : BattleTips
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __BattleTips_H_
#define __BattleTips_H_
#include "cocos2d.h"
#include "scene/layer/LayerManager.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class BattleTips : public BaseLayer
	{
	public:
		BattleTips();
		virtual ~BattleTips();
		virtual bool init();
		CREATE_FUNC(BattleTips);
		void showConfirmOnly();
		void setButtonLisener(CCObject* pListener, SEL_ClickHandler pHandler);
		bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
		//设置是否点击窗口外关闭窗口
		void setIsTouchOutClose(bool isClose);
		void addContentTip(const char *content);
		void removeSelf(CCObject* ob);
	private:
		CLayout* m_ui;
		CCNode* m_pBackgroundImage;
		CLabel *m_tipContent;
		CCObject *m_listener;
		SEL_ClickHandler m_handler;
		bool m_isOutClose;
		
	};

};
#endif