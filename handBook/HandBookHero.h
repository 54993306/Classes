/******************************************************
*文件名称:	HandBookHero.h
*编写日期:	2016-8-23-16:02
*编写作者:	YPF
*功能描述:	英雄图鉴
*******************************************************/

#pragma once
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

class CHandBookHeroLayer: public BaseLayer
{
public:
	CHandBookHeroLayer();
	~CHandBookHeroLayer();
	
	CREATE_LAYER(CHandBookHeroLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);


private:
	void updateUI( );

	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	void initListCellIcon( AtlasIns *pAtlasIns, CCPoint startPos, int iGapX, CTableViewCell * pCell );
	void initCellIconByData( AData *pAData, CLayout *pCell );

	void onClickBlank( CCObject *pSender );
	void onClickScoreIcon( CCObject *pSender );
	//弹进阶
	void onShowRankUp( CCObject *pSender );
	//获得途径
	void onShowWhereGet( CCObject *pSender );

private:
	CLayout												*m_ui;
	CLayout												*m_pCell;
	CLayout												*m_pCellBlank;
	CTableView											*m_pTableView;
	int														m_iSelectIndex;
	AtlasResponse										m_AtlasResponse;
};