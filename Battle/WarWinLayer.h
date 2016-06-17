#ifndef _WARWINLAYER_H_
#define _WARWINLAYER_H_

#include "scene/layer/LayerManager.h"
#include "google/protobuf/message.h"
#include "protos/protocol.h"
#include "bag/bagData.h"

using namespace protos;

class CMonsterInfo;
class CItemInfo;
class CNewHero;

struct CHero;
class WarWinLayer :
	public BaseLayer
{
public:
	WarWinLayer();
	~WarWinLayer();
	virtual bool init();
	CREATE_LAYER(WarWinLayer);

	void onEnter();
	void onExit();

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void callbackForScroll(CCNode* pSender);
public:
	void processBattleFinish(int type, google::protobuf::Message *msg);
	void upRoleMsg(const TMessage& tMsg);
	void touchDispose();

public:
	void afterTextAnime();
	void passTextAnime();
	void changeStar(CCNode *pSender);
	void genStar(int numOfStar);

	void openItem(CCNode *pSender);

	void onExitClick(CCObject *pSender);
	void onRestartClick(CCObject *pSender);
	void onBackClick(CCNode* ob);
	void stageRestart(CCNode *pSender);

	void callBackForProgressAction(CCNode* pSender);
	void callBackForProgressActionToAim(CCNode* pSender);

	void callBackShowRandomHero();

	void onPress(CCObject* pSender, CTouchPressState iState);

	void callBackForShowNewHero( );

public:
	static const char WAR_WIN_RESULT[];
	
private:
	CLayout* m_ui;
	vector<CPrize> m_prizeList;
	CCArray *itemBoxList;
	int _iLevelBefore;
	CMonsterInfo* m_pMonsterInfo;
	CItemInfo* m_pItemInfo;
	CTableView *m_tableView;
	CLayout *m_cell;
	CNewHero* m_pNewHeroEffect;
	int m_iIndexForFindNewHero;
};

enum TagDefine
{
	guankaName_tag = 1,
	zhushuai_tag,
	jingyanBar_tag,
	push_tag,
	jingyanNum_tag,
	jingyanPe_tag,
	qianNum_tag,
	likai_tag,
	daojudikuang1_tag = 11,
	daojudikuang2_tag,
	daojudikuang3_tag,
	daojudikuang4_tag,
	daojudikuang5_tag,
	linghundikuang1_tag,
	linghundikuang2_tag,
	linghundikuang3_tag,
	linghundikuang4_tag,
	linghundikuang5_tag,
	wupingaizi1_tag = 21,
	wupingaizi2_tag,
	wupingaizi3_tag,
	wupingaizi4_tag,
	wupingaizi5_tag,
	linghungaizi1_tag,
	linghungaizi2_tag,
	linghungaizi3_tag,
	linghungaizi4_tag,
	linghungaizi5_tag,
	xingxing1_tag = 31,
	xingxing2_tag,
	xingxing3_tag,
	xingxing4_tag,
	xingxing5_tag,

	TAG_NEW_BASE = 100,
	TAG_TITLE = 1000,
	TAG_TITLE_BLOOD = 1010,
	TAG_TITLE_LEFT,
	TAG_TITLE_RIGHT,

	TAG_TITLE_STAR,
	TAG_GRADE_STAR1,
	TAG_GRADE_STAR2,
	TAG_GRADE_STAR3,

	TAG_LEVEL_LABEL,
	TAG_LEVEL_BAR,

	TAG_MONEY_LABEL,

	TAG_ITEM_BOX, // reserve 20 slot id here

	TAG_CHAR_BAR = TAG_ITEM_BOX+20, // reserve 20 slot id here

	TAG_HEROS_LEVEL_LABEL = TAG_CHAR_BAR+20, // reserve 20 slot id here

	TAG_HEAD_ = TAG_HEROS_LEVEL_LABEL+20, // reserve 20 slot id here

	TAG_MAX = TAG_HEAD_ + 20,
};
#endif
