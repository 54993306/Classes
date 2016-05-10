#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "CityData.h"

class CCompaseLayer:public BaseLayer
{
public:
	CCompaseLayer();
	~CCompaseLayer();

	CREATE_LAYER(CCompaseLayer);

	virtual bool init();

	void onEnter();
	void onExit();

	void touchRadioButton(int iTag);
	
	CC_SYNTHESIZE(CItem, m_pSearchItem, SearchItem);

private:
	void onClose(CCObject* pSender);
	void onGarrsion(CCObject* pSender);
	void onSelectItem(CCObject* pSender);
	void itemGetInfo(CCObject* pSender);
	void onTouchProp(CCObject* pSender);
	bool longClickItemListener(CCObject* pSender, CCTouch* pTouch);
	void onSwitchBtn(CCObject *pSender, bool bChecked);
	void onComposeButton(CCObject* pSender);

	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	void processNetMsg(int type, google::protobuf::Message *msg);

	void roleUpdate(const TMessage& tMsg);

	void callBackTouchSearchItem();

	void findSearchItem();

	void showComposeItem();
	void updateCompose(CComposeData *data);
	void showBuildInfo(const CCity& ct);
	void showSelectRadioImg(int selIndex);
	void removeAnimation(CCNode* node);

	void onSelectItemArmor(CItem* pItem);

	void showEffect();
	void shake();

	void callBackForReleaseTouchAfterCompase();
	void callBackForRequestNewDataAfterCompase();
private:
	CLayout *m_ui;
	CCity m_city;
	int  m_selectHero;
	CLayout *m_cell;
	CTableView *m_tableView;

	CLayout *m_poseLayer;

	CCSprite *m_selectCellImg;

	CItem *m_selectItem;

	vector<CComposeData> m_composeList;
	map<int,vector<CComposeData>> m_composeMap;
	CCPoint  m_tableOffset;

	int m_selectIndex;
	int m_switchType;
	int m_armorId;
};
