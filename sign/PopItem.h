
#ifndef _POP_ITEM_
#define _POP_ITEM_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
#include "SignData.h"
#include "tollgate/Chapter.h"
#include <map>
#include "model/CRoleData.h"

class CMonsterInfo;
class CItemInfo;
class CNewHero;

typedef void (CCObject::*PopItemCallBack)(void);
#define popitem_selector(FUN) (PopItemCallBack)(&FUN)

class CPopItem: public BaseLayer
{
public:
	CREATE_LAYER(CPopItem);
	CPopItem();
	~CPopItem();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();

	void signPrize(CPrize *prize);
	void callbackforsignPrizeEffectOne(CCNode* node);
	void callbackforsignPrizeEffectTwo(CCNode* node);

	void popItemList(const vector<CItem>& itemList);
	void popPrizeRes(CGetPrizeRes *prizeRes);
	void popPrizeResTwoLine(CGetPrizeRes* prizeRes);//双行显示奖励列表
	void callbackforShowTwoLinePrize(CCNode* node);
	void runEffectShake();

	void setBtnHandler( CCObject *pSender, SEL_ClickHandler pHandler);
	void setDescVisiable(bool isVisible);

	void bindTargetCallBack(CCObject* pObj, PopItemCallBack pFun);

private:
	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void onPress(CCObject* pSender, CTouchPressState iState);

	void doCallBack();

private:
	int getNewHeroIndex(int iStart);
	void callBackForShowNewHero( );
	void clearPrizeSave();
private:
	CLayout *m_ui;
	CTableView *m_tableView;
	CGetPrizeRes m_prizeRes;
	CLayout *m_cell;
	CMonsterInfo* m_pMonsterInfo;
	CItemInfo* m_pItemInfo;
	CNewHero* m_pNewHeroEffect;
	vector<CPrize*> m_prizeSave;
	int m_iIndexForFindNewHero;
	CCObject* m_pTargetCallBack;
	PopItemCallBack m_pTargetFun;

	int m_indexprizeRes;
};

#endif