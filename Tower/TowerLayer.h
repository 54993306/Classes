#ifndef _TOWERLAYER_
#define _TOWERLAYER_
/************************************************************************/
/*
	�ĵ����ƣ�TowerLayer.h
	��д���ߣ�WYS
	��д���ڣ�16-9-7
	����ʵ�֣�ǧ��������
*/
/************************************************************************/
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "common/CommonFunction.h"
#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"

//#include "CocoStudio/Reader/GUIReader.h"
//#include "CocoStudio/GUI/UIWidgets/Compatible/CompatibleClasses.h"
//#include "cocos2d.h"
//#include "cocos-ext.h"
//USING_NS_CC;
//USING_NS_CC_EXT;
//using namespace ui;

class CItemInfo;
class CMonsterInfo;
class CWShopLayer;

enum btnForKernel
{
	eRule = 1,
	eRank,
	eShop,
	eQuick,
	eKernelMax
};

class CTowerLayer : public BaseLayer
{
public:
	CTowerLayer(void);
	~CTowerLayer(void);
	CREATE_LAYER(CTowerLayer);
	virtual bool init();
	void onEnter();
	void onExit();
	void onClose(CCObject* obj);
	void onPress(CCObject* obj, CTouchPressState state);

	void btncallbackStartGame(CCObject* obj);
	void btncallbackFourKerbel(CCObject* obj);
	void btnRuleBombBox(CCObject* obj);
	//void btnRankBombBox(CCObject* obj);
	//void btnShopBombBox(CCObject* obj);
	//void btnquickBombBox(CCObject* obj);

	CCObject* addCellForScrollView(CCObject* cell,unsigned int uIdx);
	void initData();

private:
	CItemInfo* m_itemInfo;
	CMonsterInfo* m_monsterInfo;
	CWShopLayer* m_shopLayer;

	CLayout* m_ui;
	//CTableView* m_tableView;
	CGridView* m_gridView;
	CLayout* m_cell;

	int m_showLayerNum;//��ʾ�����ܲ���
	int m_currPos;//��ǰͨ��������

	int m_currTowerTimes;
};
#endif