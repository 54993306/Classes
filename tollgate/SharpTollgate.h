#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "tollgate/StageData.h"
#include "Chapter.h"

//精英关卡
class CSharpTollgate:public BaseLayer
{
public:
	CSharpTollgate();
	~CSharpTollgate();
	virtual bool init();
	CREATE_LAYER(CSharpTollgate);
	void onEnter();
	void onExit();
	void onClose( CCObject * pSender );
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	void setStageList(const vector<CStage>& stageList, int chapter, int openChapter);
private:

	
private:
	void onPhonePay( CCObject * pSender );
	void onCardPay( CCObject * pSender );
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void onBattle(CCObject* pSender);
	void heroCall(CCNode* pNode);
private:
	CLayout* m_ui;
	CTableView *m_tableView;
	CLayout *m_cell;
	vector<CStage> m_stageList;
	int m_chapter;
	int m_openChapter;
};