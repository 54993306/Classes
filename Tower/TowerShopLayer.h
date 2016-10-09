#ifndef _TOWERSHOPLAYER_
#define _TOWERSHOPLAYER_
/************************************************************************/
/* 
	�ļ�����:TowerShopLayer
	��д����:WYS
	��д����:16-9-18
	���ܱ���:ǧ�����̵�
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