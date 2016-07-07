#pragma once

/******************************************************
*文件名称:	CShowToBuyResource.h
*编写日期:	2016-6-14-11:40
*编写作者:	YPF
*功能描述:	提示购买资源的窗口
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

enum ShowBuyResourceType
{
	ShowBuyResourceAction,  //体力
	ShowBuyResourceGold,		//勾玉
	ShowBuyResourceCoin,		//铜钱
	ShowBuyResourceActionLimit  //体力上限

};

class CPopTip;

class CShowToBuyResource : public BaseLayer
{
public:
	CShowToBuyResource();
	~CShowToBuyResource();
	CREATE_LAYER(CShowToBuyResource);

	void onEnter();

	void onExit();

	void showToBuyResourceByType(ShowBuyResourceType type);

	void onClick(CCObject* pSender);
private:
	std::string getInfoText();
	void doConfirmAction();
private:
	CPopTip* m_pPopTip;
	ShowBuyResourceType m_iCurrentType;
};
