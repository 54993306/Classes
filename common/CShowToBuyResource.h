#pragma once

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
