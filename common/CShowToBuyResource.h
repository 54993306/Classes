#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

enum ShowBuyResourceType
{
	ShowBuyResourceAction,  //����
	ShowBuyResourceGold,		//����
	ShowBuyResourceCoin,		//ͭǮ
	ShowBuyResourceActionLimit  //��������

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
