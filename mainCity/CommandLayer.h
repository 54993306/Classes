#ifndef __COMMAND_LAYER_
#define __COMMAND_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
// 调和塔类
#include "CityData.h"

struct CommandData
{
	CCity cityInfo;
	vector<CCity> cityList;
};

class CCommandLayer:public BaseLayer
{
public:
	CCommandLayer():m_selectHero(0),m_selectCity(nullptr){}
	~CCommandLayer();
	CREATE_LAYER(CCommandLayer);
	virtual bool init();
	void onEnter();
	void onComposeButton(CCObject* pSender);
	void onExit();

	void updateHero(const TMessage& tMsg);
	void recBuildInfo(int type, google::protobuf::Message *msg);
protected:
	void onClose(CCObject* pSender);
	void onGarrsion(CCObject* pSender);
	void onSelectCity(CCObject* pSender);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);

	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	void onSwitchBtn(CCObject *pSender, bool bChecked);
	void processNetMsg(int type, google::protobuf::Message *msg);
	void showCommandList();
	void updateSelectCity(CCity *data);
	void showBuildInfo(const CCity& ct);
	void showSelectRadioImg(int selIndex);
	void removeAnimation(CCNode* node);
private:
	CLayout *m_ui;
	CCity m_city;
	int  m_selectHero;
	CLayout *m_cell;
	CTableView *m_tableView;

	CLayout *m_comLayer;
	CLayout *m_garrsionLayer;

	CCSprite *m_selectCellImg;

	CCity *m_selectCity;

	vector<CComposeData> m_composeList;

	CommandData m_commandData;
};



#endif // !__GOLDMINE_LAYER_
