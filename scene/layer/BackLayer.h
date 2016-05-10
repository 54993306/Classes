#pragma once
#include "AppUI.h"
#include "GMessage.h"

/************************************************************************/
/*                      
						¼àÌýÊÖ»ú·µ»Ø¼ü  //  [9/29/2014 xc_lin]
*/
/************************************************************************/

class CPopTip;
class SetPanel;

class BackLayer :
	public cocos2d::CCLayer
{
public:
	
	BackLayer(void);
	~BackLayer(void);
	CREATE_FUNC(BackLayer);

	virtual void keyBackClicked();

	virtual bool init();
	void removePanel(cocos2d::CCObject* ob);

	SetPanel* m_setPanel;

protected:
	void onReturnLogin(CCObject* pSender);
	void onExitGame(CCObject* pSender);
private:
	CPopTip *m_backTip;

};

