
#ifndef __TOLLGATEPREVIEW_
#define __TOLLGATEPREVIEW_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"

class CItemInfo;
class CMonsterInfo;
struct CStageInfoRes;

class CTollgatePreview: public BaseLayer
{
public:
	CREATE_LAYER(CTollgatePreview);
	CTollgatePreview();
    virtual bool init();
    void onEnter();
	void onExit();

	void onPressItem(CCObject* pSender, CTouchPressState iState);

	void onPressMonster(CCObject* pSender, CTouchPressState iState);

	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);

	void tableCell(CTableViewCell * pCell, unsigned int uIdx);
	void processNetMsg(int type, google::protobuf::Message *msg);

	void showStageInfo(CStageInfoRes &stageInfo);

	void setStage(int stageId, const char* name);
	void onCombat(CCObject* pSender);
	void onClose(CCObject* pSender);
	void setQuestId(int questId);
	// 	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);


	void showEffectNormal();
	void showEffectNormalCallBack();

	void showEffect();
	void showEffectCallBack();

	void onCombatEffect();
	void onCombatEffectCallBack();
	
	void runCloudAction();
	void relocation(CCNode* pSender);

	void exitMsg(CCObject* pObj);

	void roleUpdate(const TMessage& tMsg);

	void setIsLastStageInChapter(bool bLastStage);
protected:
	CC_SYNTHESIZE(bool, m_bIsNormal, Normal);

private:
	void callBackForSound425();
private:
	CLayout *m_ui;
	CLayout *m_cell;
	CTableView *m_tableView;
	vector<CMonster> m_monstList;
	vector<CPrize>  m_prizeList;
	int m_stageId;
	int m_questId;
	int m_iCostFood;
	CMonsterInfo* m_pMonsterInfo;
	CItemInfo* m_pItemInfo;
	CCSprite* m_pBody;

	CCPoint m_cloudPos;
	CCPoint m_cloudAPos;
	CLayout* m_fogLay;
	CLayout* m_fogLay1;

	MaskLayer* m_pMaskLayer;
	bool m_bLastStage;
};

#endif