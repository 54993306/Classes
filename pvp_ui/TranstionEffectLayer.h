#pragma once

/******************************************************
*文件名称:	TranstionEffectLayer.h
*编写日期:	2016-6-13-17:16
*编写作者:	YPF
*功能描述:	过渡特效
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

enum TranstionEffectLineDirection
{
	TranstionEffectLineDirectionVertical,				//初始方向垂直
	TranstionEffectLineDirectionHorizontal			//初始方向水平
};

enum TranstionEffectGrowDirection
{
	TranstionEffectGrowDirectionAdd,				//方向递增
	TranstionEffectGrowDirectionMinus,			//方向递减
};

typedef void (CCObject::*transitionEffect_end)();
#define transitionEffect_end_callback(T_CALL) (transitionEffect_end)(&T_CALL)

class CTranstionEffectLayer: public BaseLayer
{
public:
	CTranstionEffectLayer();
	~CTranstionEffectLayer();
	
	CREATE_LAYER(CTranstionEffectLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	void onOpen(CCNode* pParent, int iZorder);
	void onClose();

	void setStartPos(CCPoint startPos);
	
	CC_SYNTHESIZE(TranstionEffectLineDirection, m_iLineDir, LineDir);
	CC_SYNTHESIZE(TranstionEffectGrowDirection, m_iGrowDir, GrowDir);

	CC_SYNTHESIZE(int, m_iLineCount, LineCount);
	CC_SYNTHESIZE(int, m_iIndexMax, LineMax);
	CC_SYNTHESIZE(int, m_iRotation, CellRotation);
	CC_SYNTHESIZE(float, m_fTimeMove, TimeMove);
	CC_SYNTHESIZE(float, m_fTimeFade, TimeFade);

	void showEffect();

	void bindCallback(CCObject* pObj, transitionEffect_end endCall);

private:
	void showLine(CCPoint pPos, float fTime, bool bHideFirst=false);
	void showLineElement(CCNode* pNode);
	CCSprite* createElement(CCPoint pPos, int iIndex, bool bIsHide=false);

	void initLineGapAndGrowGap();

	void callBackforRemove(CCNode* pNode);

private:
	CCSpriteBatchNode* m_pBatchNode;
	
	CCPoint m_pPosStart;
	CCPoint m_pLineGap;
	CCPoint m_pGrowGap;

	int m_iOverLineCount;
	int m_iShowIndex;

	CCObject* m_pTarget;
	transitionEffect_end m_endCallBack;
};