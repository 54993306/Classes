#ifndef _TOWERSHOPLAYER_
#define _TOWERSHOPLAYER_
/************************************************************************/
/* 
	文件名称:TowerShopLayer
	编写作者:WYS
	编写日期:16-9-18
	功能表述:千层塔商店
*/
/************************************************************************/
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"

class CTowerShopLayer : public BaseLayer
{
public:
	CTowerShopLayer(void);
	~CTowerShopLayer(void);
	CREATE_LAYER(CTowerShopLayer);
	virtual bool init();
	void onEnter();
	void onExit();
	//void onClose(CCObject* obj);

	//void processNetMsg();
	CCObject* gridviewDataSource(CCObject* pConvertCell,unsigned int uIdx);
	void addGridCell(CGridPageViewCell* pCell, unsigned int uIdx);

private:
	CLayout* m_ui;
	CGridPageView* m_gridView;
	CLayout* m_cell;

};

#endif