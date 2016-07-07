#pragma once

/******************************************************
*文件名称:	SelectDefense.h
*编写日期:	2016-6-13-17:16
*编写作者:	YPF
*功能描述:	选择防守阵型
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "model/BattleData.h"

enum SelectDefenseLayerAction
{
	SelectDefenseLayerActionTouchHoldCheck = 1
};

#define SelectDefenseHoldTime 0.15f

class CSelectDefense:public BaseLayer
{
public:

	CSelectDefense();
	~CSelectDefense();
	CREATE_LAYER(CSelectDefense);
	virtual bool init();
	void onEnter();
	void onExit();


	virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent);

protected:

	void updateMainSkillDesc();

	void onSwitchBtn(CCObject *pSender, bool bChecked);
	void onSwitchStrategy(CCObject *pSender, bool bChecked);
	void updateHeroList();
	void updateCurrHeroList();
	void addGridCell(unsigned int uIdx, CGridViewCell* pCell);


	void onSelectHero(CCObject *pSender);
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void processMessage(int type, google::protobuf::Message *msg);
	void addHeroSelectCell(unsigned int uIdx, CGridViewCell* pCell);

private:
	void callBackForTouchHold();
	//初始化格子
	void initGreen();


private:
	CLayout *m_ui;

	//当前选择的英雄
	int  m_selectHero;
	
	//当前选择的策略
	int m_iStrategyIndex;

	//英雄信息cell
	CLayout *m_cell;

	//英雄列表
	CGridView *m_gridView;
	
	//英雄列表备份
	vector<CHero> m_heroList;
	//英雄地图备份
	map<int,vector<CHero*>> m_heroMap;
	//当前选中的英雄列表
	vector<CHero*> *m_currHeroList;
	
	//当前英雄数据
	CHero* m_pDragHeroData;

	//单触摸控制
	CCTouch* m_pTouch;
	bool m_bHold;
	bool m_bBaseTouch;

	//4X4格子
	CCSpriteBatchNode* m_pBatchGreen;
	//队长格子
	CImageView* m_pCaptainTile;
};