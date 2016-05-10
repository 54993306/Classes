
#ifndef _TIP_ITEM_
#define _TIP_ITEM_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"

#define ITEM_INFO_HEIGHT 35

#define ITEM_INFO_START_POS ccp(1150, 17)

#define ITEM_MAX_LINE_COUNT 4

class CItemTip: public BaseLayer
{
public:
	CREATE_LAYER(CItemTip);
	CItemTip();
	~CItemTip();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	void showItemProperty(CItem *item);

	void updateArmorAttr(CItem * item);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void onResolve(CCObject* pSender);
	void onStrength(CCObject* pSender);
	void DoNotPopItem();
	void processNetMsg(int type, google::protobuf::Message *msg);
	void onConfirmDecompose(CCObject *pSender);
	void onDecomposeBtn(CCObject* pSender);
	void lockResolve();
	void callBackResolve();

	void hideHeroEquipHead();
	void hideButton();
	void hideBg();
	void updateItemData( const TMessage& tMsg );
private:
	void initItemInfo(CCPoint posStart, CLayout* pLayout[], int iCount);
	void updatePosOfLineAndDesc();
	void hideNoneValueCell(int iCount, CLayout* pLayout[]);
	int updateInfoToUIAndReturnCount(CLayout* pLayout[], int iNameId[], int iValue[], int iMax, const char* sTitle);
private:
	CLayout *m_ui;
	bool m_isOutClose;
	CItem *m_selectItem;
	int m_iMaxLineCount;
	CLayout* m_pCellInfo;
	CLayout* m_pBaseInfo[ITEM_MAX_LINE_COUNT];
	CLayout* m_pSpecialInfo[ITEM_MAX_LINE_COUNT];
};

void inline CopyBaseAttr(CItem* pAim, CItem* pFrom)
{
	pAim->armor.baseHp = pFrom->armor.baseHp;
	pAim->armor.baseDef = pFrom->armor.baseDef;
	pAim->armor.baseAtk = pFrom->armor.baseAtk;
	pAim->armor.baseHit = pFrom->armor.baseHit;
	pAim->armor.baseDex = pFrom->armor.baseDex;
	pAim->armor.baseCrit = pFrom->armor.baseCrit;
	pAim->armor.baseRenew =  pFrom->armor.baseRenew;
	pAim->armor.baseDodge = pFrom->armor.baseDodge;

}
void inline CopySpecialAttr(CItem* pAim, CItem* pFrom)
{
	pAim->armor.addHp = pFrom->armor.addHp;
	pAim->armor.addDef = pFrom->armor.addDef;
	pAim->armor.addAtk = pFrom->armor.addAtk;
	pAim->armor.addHit = pFrom->armor.addHit;
	pAim->armor.addDex = pFrom->armor.addDex;
	pAim->armor.addCrit = pFrom->armor.addCrit;
	pAim->armor.addRenew =  pFrom->armor.addRenew;
	pAim->armor.addDodge = pFrom->armor.addDodge;
}

#endif