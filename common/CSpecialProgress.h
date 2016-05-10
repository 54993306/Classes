/******************************************************
*文件名称:	CSpecialProgress.h
*编写日期:	2016-4-27-11:20
*编写作者:	YPF
*功能描述:	有粒子光泽的进度条
*******************************************************/

#pragma once

#include "AppUI.h"

class CSpecialProgress:public CProgressBar
{
public:
	CSpecialProgress();
	~CSpecialProgress();

	virtual bool init();

	CREATE_FUNC(CSpecialProgress);
	static CProgressBar* Create(const char* pProgress);
	static CSpecialProgress* CreateWithProgressBar(CProgressBar* pBar);

	bool copyFromProgressBar(CProgressBar* pBar);

	void addHead(CCSprite* pSprite);

	void addParticleEffect(const char* pLz);

	virtual void changeValueAndExecuteEvent(float nValue, bool bExeEvent);	
	void updateForAdaptPos(float dt);
private:
	CProgressBar* m_pStencilBar;
	CCClippingNode* m_pClipNode;
	CCSprite* m_pHead;
	CCParticleSystemQuad* m_pLz;
};

void inline CopyProgressBar(CProgressBar* pAim, CProgressBar* pFrom);